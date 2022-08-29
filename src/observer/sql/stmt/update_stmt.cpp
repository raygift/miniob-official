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
// Created by Wangyunlai on 2022/5/22.
//

#include "sql/stmt/update_stmt.h"
#include "common/log/log.h"
#include "storage/common/db.h"
#include "storage/common/table.h"


UpdateStmt::UpdateStmt(Table *table, Value *values, int value_amount)
  : table_ (table), values_(values), value_amount_(value_amount)
{}

UpdateStmt::~UpdateStmt()
{
  if (nullptr != values_) {
    delete values_;
    values_ = nullptr;
  }
}

RC UpdateStmt::create(Db *db, const Updates &update, Stmt *&stmt)
{
  // TODO
  const char *table_name = update.relation_name;
  if (nullptr == db || nullptr == table_name) {  // db 或 table 不存在
    LOG_WARN("invalid argument. db=%p, table_name=%p", db, table_name);
    return RC::INVALID_ARGUMENT;
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);// 找到要操作的 table
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  char *field_name = update.attribute_name;
  const TableMeta &table_meta = table->table_meta();
  const FieldMeta *field_meta = table_meta.field(field_name);  // 找到要更新的列
  // 检查要更新的列是否存在
  if (nullptr == field_meta) {
    LOG_WARN("no such field. db=%s, table_name=%s, field_name=%s", db->name(), table_name, field_name);
    return RC::SCHEMA_FIELD_NOT_EXIST;
  }

  // 找到满足条件的行
  // update.condition_num
  // update.conditions

  // 更新行对应的列值

  // 更新索引？

  // check the fields number
  Value *update_value = new Value();
  update_value->data=update.value.data;
  update_value->type=update.value.type;

  // const int field_num =
  //     table_meta.field_num() - table_meta.sys_field_num();

  // everything alright
  // stmt = new UpdateStmt(table, value, 1);
  UpdateStmt *update_stmt = new UpdateStmt(table,update_value,1);
  // update_stmt->table_ = table;
// 
  // update_stmt->values_=update_value;
  // update_stmt->value_amount_ = 1;
  stmt = update_stmt;
  return RC::SUCCESS;
  // stmt = nullptr;
  // return RC::INTERNAL;
}
