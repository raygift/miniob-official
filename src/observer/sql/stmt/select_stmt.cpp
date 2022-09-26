/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/6/6.
//

#include "sql/stmt/select_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/common/db.h"
#include "storage/common/table.h"

SelectStmt::~SelectStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
}

static void wildcard_fields(Table *table, std::vector<Field> &field_metas)
{
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num();
  for (int i = table_meta.sys_field_num(); i < field_num; i++) {
    field_metas.push_back(Field(table, table_meta.field(i)));
  }
}

RC SelectStmt::create(Db *db, const Selects &select_sql, Stmt *&stmt)
{
  if (nullptr == db) {
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  }

  // collect tables in `from` statement
  std::vector<Table *> tables;
  std::unordered_map<std::string, Table *> table_map;
  for (int i = 0; i < select_sql.relation_num; i++) {
    const char *table_name = select_sql.relations[i];
    if (nullptr == table_name) {
      LOG_WARN("invalid argument. relation name is null. index=%d", i);
      return RC::INVALID_ARGUMENT;
    }

    Table *table = db->find_table(table_name);
    if (nullptr == table) {
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    tables.push_back(table);// sql 语句from 指示的表 都被收集到 tables 容器中
    table_map.insert(std::pair<std::string, Table*>(table_name, table));// 同时创建一个无序容器，用于记录表名 和 表 类对象的对应
  }
  
  // collect query fields in `select` statement
  std::vector<Field> query_fields;
  for (int i = select_sql.attr_num - 1; i >= 0; i--) {// 遍历 sql 语句  select 指示的列
    const RelAttr &relation_attr = select_sql.attributes[i];
    
    // sql 语句 select 指示的列未指明表名，且列名使用“*”，
    // select * from
    // 则将所有表中所有列收集到容器 query_fields中
    if (common::is_blank(relation_attr.relation_name) && 0 == strcmp(relation_attr.attribute_name, "*")) {
      for (Table *table : tables) {
        wildcard_fields(table, query_fields);
      }

    } else if (!common::is_blank(relation_attr.relation_name)) { // TODO // 否则检查是否指定了表名，若指定了表名，select t.
      const char *table_name = relation_attr.relation_name;
      const char *field_name = relation_attr.attribute_name;

      if (0 == strcmp(table_name, "*")) {// 指定了表名，且表名为“*”，select *.
        if (0 != strcmp(field_name, "*")) {// 列名不是“*”，非法；不允许出现 select *.name from 、select *.id from 这种投影表示
          LOG_WARN("invalid field name while table is *. attr=%s", field_name);
          return RC::SCHEMA_FIELD_MISSING;
        }
        for (Table *table : tables) {// 表名为*，且列名为*，select *.* from ，将每个表的所有列名收集到 query_fields 容器中
          wildcard_fields(table, query_fields);
        }
      } else {// 指定了表名，且表名不为“*”，select t.age from ，则遍历 table_map 中收集的表，根据表名得到对应的 table 对象
        auto iter = table_map.find(table_name);
        if (iter == table_map.end()) {
          LOG_WARN("no such table in from list: %s", table_name);
          return RC::SCHEMA_FIELD_MISSING;
        }

        Table *table = iter->second;
        if (0 == strcmp(field_name, "*")) {// 若列名指定为“*”, select t.* from
          wildcard_fields(table, query_fields);// 将该表所有列收集到容器 query_fields 中
        } else {// 否则，根据表记录的元信息，找到列名对应的列元信息，创建 Field 对象并收集到容器 query_fields 中
          const FieldMeta *field_meta = table->table_meta().field(field_name);
          if (nullptr == field_meta) {
            LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), field_name);
            return RC::SCHEMA_FIELD_MISSING;
          }

        query_fields.push_back(Field(table, field_meta));
        }
      }
    } else {// 否则，未指定表名，但指定了列名,  select age,name from 
      if (tables.size() != 1) {// TODO(zhangpc) 不支持 select 指定列名而不指定表名？
        LOG_WARN("invalid. I do not know the attr's table. attr=%s", relation_attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }

      Table *table = tables[0];
      const FieldMeta *field_meta = table->table_meta().field(relation_attr.attribute_name);
      if (nullptr == field_meta) {
        LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), relation_attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }

      query_fields.push_back(Field(table, field_meta));
    }
  }

  LOG_INFO("got %d tables in from stmt and %d fields in query stmt", tables.size(), query_fields.size());

  Table *default_table = nullptr;
  if (tables.size() == 1) {// sql 语句 from 之后只有一个表，则使用此表以及condition 构造 filter_stmt
    default_table = tables[0];
  }

  // create filter statement in `where` statement
  FilterStmt *filter_stmt = nullptr;
  RC rc = FilterStmt::create(db, default_table, &table_map,
           select_sql.conditions, select_sql.condition_num, filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  // 为 select-tables 多表查询新增针对多张表分别创建适用的 filter_stmt
      std::unordered_map<std::string, FilterStmt *> *table_filter_stmt =
          new std::unordered_map<std::string, FilterStmt *>;
  if (tables.size()> 1){
    int nSize = (int)tables.size();
    for (int i = nSize -1 ; i >=0; i--) {  // 针对 sql 语句 from 之后的每一张表，使用表以及合适的 condition 构造 filter_stmt
        create_table_filter(
            db, tables[i], &table_map, select_sql.conditions, select_sql.condition_num, table_filter_stmt);

        // Table *table = tables[i];
        // FilterStmt *filter_stmt = nullptr;

        // RC rc = FilterStmt::create(db, table, &table_map, select_sql.conditions, select_sql.condition_num,
        // filter_stmt);
        // // 这里应该不会失败，因为已经检查过表中确实存在con 指示的列
        // if (rc != RC::SUCCESS) {
        //   LOG_ERROR("cannot construct filter stmt");
        //   return rc;
        // }
        // std::string table_name = table->name();
        // table_filter_stmt->insert(std::pair<std::string, FilterStmt *>(table_name, filter_stmt));
      }
      // 保存 为 select-tables 多表查询新增针对多张表分别创建适用的 filter_stmt
    }

  // everything alright
  SelectStmt *select_stmt = new SelectStmt();
  select_stmt->tables_.swap(tables);
  select_stmt->query_fields_.swap(query_fields);
  select_stmt->filter_stmt_ = filter_stmt;
  select_stmt->table_filter_stmt_ = *table_filter_stmt;
  stmt = select_stmt;
  return RC::SUCCESS;
}

// 仍然使用 FilterStmt::create 创建
// 只是创建前先进行一次 conditions 中列名是否存在于 table 的检查
// （FilterStmt::create() 中也会执行此检查，这里因为复用了原来逻辑导致有一些冗余的检查逻辑）
RC create_table_filter(Db *db, Table *table, std::unordered_map<std::string, Table *> *tables, 
                    const Condition *conditions, int condition_num, std::unordered_map<std::string, FilterStmt *> *&table_filter){

  // std::unordered_map<char *, FilterStmt *> *tmp_filters = new std::unordered_map<char *, FilterStmt *> ;

  // 遍历所有 conditions，判断是否适用于当前table
  int nSize = (int)condition_num;
    for (int i = nSize -1 ; i >=0; i--){
    const Condition condition = conditions[i];

    if (!condition.left_is_attr) {  // where 条件左侧为 列名时才可以判断
      continue;
    }
    // 跳过 condition 中不是针对当前表的查询条件
    if ((condition.left_is_attr && condition.left_attr.relation_name == nullptr) ||
        (condition.right_is_attr && condition.right_attr.relation_name == nullptr)) {
      continue;
    }
    if (condition.left_is_attr && strcmp(condition.left_attr.relation_name, table->name()) != 0) {
      continue;
      ;
    }
    if (condition.right_is_attr && strcmp(condition.right_attr.relation_name, table->name()) != 0) {
      continue;
    }

    // 当前condition 使用的 列名
    const RelAttr attr = condition.left_attr;
    // 检查当前 table 是否有此列
    const FieldMeta *field = table->table_meta().field(attr.attribute_name);// gug: 不能通过where 查找条件去表中寻找列，因为多个表可能存在相同的列名
    if (nullptr == field) {
      continue;  // table 不包含该列，跳过
    }

    FilterStmt *filter_stmt = nullptr;

    RC rc = FilterStmt::create(db, table, tables, &condition, 1, filter_stmt);
    // 这里应该不会失败，因为已经检查过表中确实存在con 指示的列
    if (rc != RC::SUCCESS) {
      LOG_ERROR("cannot construct filter stmt");
      return rc;
    }
    std::string table_name = table->name();
    
    table_filter->insert(std::pair<std::string, FilterStmt *>(table_name, filter_stmt));
  }
}
