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
// Created by Meiyi & Longda on 2021/4/13.
//

#include <string>
#include <sstream>

#include "execute_stage.h"

#include "common/io/io.h"
#include "common/log/log.h"
#include "common/lang/defer.h"
#include "common/seda/timer_stage.h"
#include "common/lang/string.h"
#include "session/session.h"
#include "event/storage_event.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "sql/expr/tuple.h"
#include "sql/operator/table_scan_operator.h"
#include "sql/operator/index_scan_operator.h"
#include "sql/operator/predicate_operator.h"
#include "sql/operator/delete_operator.h"
#include "sql/operator/project_operator.h"
#include "sql/stmt/stmt.h"
#include "sql/stmt/select_stmt.h"
#include "sql/stmt/update_stmt.h"
#include "sql/stmt/delete_stmt.h"
#include "sql/stmt/insert_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/common/table.h"
#include "storage/common/field.h"
#include "storage/index/index.h"
#include "storage/default/default_handler.h"
#include "storage/common/condition_filter.h"
#include "storage/trx/trx.h"

using namespace common;

// RC create_selection_executor(
//    Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectExeNode &select_node);

//! Constructor
ExecuteStage::ExecuteStage(const char *tag) : Stage(tag)
{}

//! Destructor
ExecuteStage::~ExecuteStage()
{}

//! Parse properties, instantiate a stage object
Stage *ExecuteStage::make_stage(const std::string &tag)
{
  ExecuteStage *stage = new (std::nothrow) ExecuteStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new ExecuteStage failed");
    return nullptr;
  }
  stage->set_properties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool ExecuteStage::set_properties()
{
  //  std::string stageNameStr(stageName);
  //  std::map<std::string, std::string> section = theGlobalProperties()->get(
  //    stageNameStr);
  //
  //  std::map<std::string, std::string>::iterator it;
  //
  //  std::string key;

  return true;
}

//! Initialize stage params and validate outputs
bool ExecuteStage::initialize()
{
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = next_stage_list_.begin();
  default_storage_stage_ = *(stgp++);
  mem_storage_stage_ = *(stgp++);

  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void ExecuteStage::cleanup()
{
  LOG_TRACE("Enter");

  LOG_TRACE("Exit");
}

void ExecuteStage::handle_event(StageEvent *event)
{
  LOG_TRACE("Enter\n");

  handle_request(event);

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::callback_event(StageEvent *event, CallbackContext *context)
{
  LOG_TRACE("Enter\n");

  // here finish read all data from disk or network, but do nothing here.

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::handle_request(common::StageEvent *event)
{
  SQLStageEvent *sql_event = static_cast<SQLStageEvent *>(event);
  SessionEvent *session_event = sql_event->session_event();
  Stmt *stmt = sql_event->stmt();
  Session *session = session_event->session();
  Query *sql = sql_event->query();

  if (stmt != nullptr) {
    switch (stmt->type()) {
      case StmtType::SELECT: {
        // fixed(zhangpc) 原 miniob 不支持多表查询
        SelectStmt *select_stmt = (SelectStmt *)(stmt);
        if (select_stmt->tables().size() == 1) {
          do_select(sql_event);
        }else{
          do_select_multi(sql_event);
        }
      } break;
      case StmtType::INSERT: {
        do_insert(sql_event);
      } break;
      case StmtType::UPDATE: {
        do_update(sql_event);// 处理 update
      } break;
      case StmtType::DELETE: {
        do_delete(sql_event);
      } break;
    }
  } else {
    switch (sql->flag) {
      case SCF_HELP: {
        do_help(sql_event);
      } break;
      case SCF_CREATE_TABLE: {
        do_create_table(sql_event);
      } break;
      case SCF_CREATE_INDEX: {
        do_create_index(sql_event);
      } break;
      case SCF_SHOW_TABLES: {
        do_show_tables(sql_event);
      } break;
      case SCF_DESC_TABLE: {
        do_desc_table(sql_event);
      } break;

      case SCF_DROP_TABLE:
      case SCF_DROP_INDEX:
      case SCF_LOAD_DATA: {
        default_storage_stage_->handle_event(event);
      } break;
      case SCF_SYNC: {
        RC rc = DefaultHandler::get_default().sync();
        session_event->set_response(strrc(rc));
      } break;
      case SCF_BEGIN: {
        session_event->set_response("SUCCESS\n");
      } break;
      case SCF_COMMIT: {
        Trx *trx = session->current_trx();
        RC rc = trx->commit();
        session->set_trx_multi_operation_mode(false);
        session_event->set_response(strrc(rc));
      } break;
      case SCF_ROLLBACK: {
        Trx *trx = session_event->get_client()->session->current_trx();
        RC rc = trx->rollback();
        session->set_trx_multi_operation_mode(false);
        session_event->set_response(strrc(rc));
      } break;
      case SCF_EXIT: {
        // do nothing
        const char *response = "Unsupported\n";
        session_event->set_response(response);
      } break;
      default: {
        LOG_ERROR("Unsupported command=%d\n", sql->flag);
      }
    }
  }
}

void end_trx_if_need(Session *session, Trx *trx, bool all_right)
{
  if (!session->is_trx_multi_operation_mode()) {
    if (all_right) {
      trx->commit();
    } else {
      trx->rollback();
    }
  }
}

void print_tuple_header(std::ostream &os, const ProjectOperator &oper)
{
  const int cell_num = oper.tuple_cell_num();
  const TupleCellSpec *cell_spec = nullptr;
  for (int i = 0; i < cell_num; i++) {
    oper.tuple_cell_spec_at(i, cell_spec);
    if (i != 0) {
      os << " | ";
    }

    if (cell_spec->alias()) {
      os << cell_spec->alias();
    }
  }

  if (cell_num > 0) {
    os << '\n';
  }
}
void tuple_to_string(std::ostream &os, const Tuple &tuple)
{
  TupleCell cell;
  RC rc = RC::SUCCESS;
  bool first_field = true;
  for (int i = 0; i < tuple.cell_num(); i++) {// 遍历 tuple 的所有列
    rc = tuple.cell_at(i, cell);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to fetch field of cell. index=%d, rc=%s", i, strrc(rc));
      break;
    }

    if (!first_field) {
      os << " | ";
    } else {
      first_field = false;
    }
    cell.to_string(os);// 将值转为查询结果的字符串格式，并放入输出流
  }
}

IndexScanOperator *try_to_create_index_scan_operator(FilterStmt *filter_stmt)
{
  const std::vector<FilterUnit *> &filter_units = filter_stmt->filter_units();
  if (filter_units.empty()) {
    return nullptr;
  }

  // 在所有过滤条件中，找到字段与值做比较的条件，然后判断字段是否可以使用索引
  // 如果是多列索引，这里的处理需要更复杂。
  // 这里的查找规则是比较简单的，就是尽量找到使用相等比较的索引
  // 如果没有就找范围比较的，但是直接排除不等比较的索引查询. (你知道为什么?)
  const FilterUnit *better_filter = nullptr;
  for (const FilterUnit *filter_unit : filter_units) {
    if (filter_unit->comp() == NOT_EQUAL) {
      continue;
    }

    Expression *left = filter_unit->left();
    Expression *right = filter_unit->right();
    if (left->type() == ExprType::FIELD && right->type() == ExprType::VALUE) {
    } else if (left->type() == ExprType::VALUE && right->type() == ExprType::FIELD) {
      std::swap(left, right);
    }
    FieldExpr &left_field_expr = *(FieldExpr *)left;
    const Field &field = left_field_expr.field();
    const Table *table = field.table();
    Index *index = table->find_index_by_field(field.field_name());
    if (index != nullptr) {
      if (better_filter == nullptr) {
        better_filter = filter_unit;
      } else if (filter_unit->comp() == EQUAL_TO) {
        better_filter = filter_unit;
        break;
      }
    }
  }

  if (better_filter == nullptr) {
    return nullptr;
  }

  Expression *left = better_filter->left();
  Expression *right = better_filter->right();
  CompOp comp = better_filter->comp();
  if (left->type() == ExprType::VALUE && right->type() == ExprType::FIELD) {
    std::swap(left, right);
    switch (comp) {
      case EQUAL_TO: {
        comp = EQUAL_TO;
      } break;
      case LESS_EQUAL: {
        comp = GREAT_THAN;
      } break;
      case NOT_EQUAL: {
        comp = NOT_EQUAL;
      } break;
      case LESS_THAN: {
        comp = GREAT_EQUAL;
      } break;
      case GREAT_EQUAL: {
        comp = LESS_THAN;
      } break;
      case GREAT_THAN: {
        comp = LESS_EQUAL;
      } break;
      default: {
        LOG_WARN("should not happen");
      }
    }
  }

  FieldExpr &left_field_expr = *(FieldExpr *)left;
  const Field &field = left_field_expr.field();
  const Table *table = field.table();
  Index *index = table->find_index_by_field(field.field_name());
  assert(index != nullptr);

  ValueExpr &right_value_expr = *(ValueExpr *)right;
  TupleCell value;
  right_value_expr.get_tuple_cell(value);

  const TupleCell *left_cell = nullptr;
  const TupleCell *right_cell = nullptr;
  bool left_inclusive = false;
  bool right_inclusive = false;

  switch (comp) {
    case EQUAL_TO: {
      left_cell = &value;
      right_cell = &value;
      left_inclusive = true;
      right_inclusive = true;
    } break;

    case LESS_EQUAL: {
      left_cell = nullptr;
      left_inclusive = false;
      right_cell = &value;
      right_inclusive = true;
    } break;

    case LESS_THAN: {
      left_cell = nullptr;
      left_inclusive = false;
      right_cell = &value;
      right_inclusive = false;
    } break;

    case GREAT_EQUAL: {
      left_cell = &value;
      left_inclusive = true;
      right_cell = nullptr;
      right_inclusive = false;
    } break;

    case GREAT_THAN: {
      left_cell = &value;
      left_inclusive = false;
      right_cell = nullptr;
      right_inclusive = false;
    } break;

    default: {
      LOG_WARN("should not happen. comp=%d", comp);
    } break;
  }

  IndexScanOperator *oper = new IndexScanOperator(table, index, left_cell, left_inclusive, right_cell, right_inclusive);

  LOG_INFO("use index for scan: %s in table %s", index->index_meta().name(), table->name());
  return oper;
}

RC ExecuteStage::do_select(SQLStageEvent *sql_event)
{
  SelectStmt *select_stmt = (SelectStmt *)(sql_event->stmt());
  SessionEvent *session_event = sql_event->session_event();
  RC rc = RC::SUCCESS;
  if (select_stmt->tables().size() != 1) {
    LOG_WARN("select more than 1 tables is not supported");
    rc = RC::UNIMPLENMENT;
    return rc;
  }

  Operator *scan_oper = try_to_create_index_scan_operator(select_stmt->filter_stmt());
  if (nullptr == scan_oper) {// 如果filter_stmt 为空，或者其他原因没有得到 索引扫描操作器
      scan_oper = new TableScanOperator(select_stmt->tables()[0]);// 支持单表查询
  }

  DEFER([&]() { delete scan_oper; });

  PredicateOperator pred_oper(select_stmt->filter_stmt());
  pred_oper.add_child(scan_oper);
  ProjectOperator project_oper;
  project_oper.add_child(&pred_oper);
  for (const Field &field : select_stmt->query_fields()) {
    project_oper.add_projection(field.table(), field.meta());// 将所有 select 要查询的列名称 记录在 ProjectOperator 的 speces_ 容器中
  }
  rc = project_oper.open();// project_oper.open() -> pred_oper.open() -> scan_oper.open()
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open operator");
    return rc;
  }

  std::stringstream ss;
  print_tuple_header(ss, project_oper);// 打印select 要查询的列记录的列名
  while ((rc = project_oper.next()) == RC::SUCCESS) {// 迭代获取下一个元组，project_oper.next() -> pred_oper.next() -> scan_oper.next()
    // get current record
    // write to response
    Tuple *tuple = project_oper.current_tuple();// 获取当前元组
    if (nullptr == tuple) {
      rc = RC::INTERNAL;
      LOG_WARN("failed to get current record. rc=%s", strrc(rc));
      break;
    }

    tuple_to_string(ss, *tuple);// 元组转为字符串
    ss << std::endl;
  }

  if (rc != RC::RECORD_EOF) {
    LOG_WARN("something wrong while iterate operator. rc=%s", strrc(rc));
    project_oper.close();
  } else {
    rc = project_oper.close();
  }
  session_event->set_response(ss.str());// 将结果返回
  return rc;
}

// void cartesian_product_dfs(std::ostream &os, std::string pre_tuple_str, std::unordered_map<char *, std::vector<std::string>>::iterator table_iter_cur, 
//                                std::unordered_map<char *,std::vector<std::string>>::iterator table_iter_end)

void cartesian_product_dfs(std::ostream &os, std::string &pre_tuple_str, std::vector<std::vector<std::string>> &tables_tuple,long unsigned int * table_idx, 
                              long unsigned int end)
{
  if (*table_idx >= end){
    return;
  }
  int table_idx_next = (*table_idx)+1;
  if (table_idx_next >= end) {// 下一个元素是end，说明当前是最后一个存有值的元素
  std::vector<std::string> cur_tuples = tables_tuple[*table_idx];
    if (pre_tuple_str.length() == 0) {// 不需要与前序字符串拼接
      // 遍历当前 table
      for (long unsigned int i = 0; i< cur_tuples.size();i++) {
        std::string tuple_str = cur_tuples[i];
        os << tuple_str;// 直接将当前tuple 的字符串加入到输出流
      }
    } else {// 需要与前序字符创拼接
      // 与传入的 pre_tuple_str 拼接
      // for (std::string tuple_str : cur_tuples) {
      for (long unsigned int i = 0; i< cur_tuples.size();i++) {
        std::string tuple_str = cur_tuples[i];
        os << pre_tuple_str;
        os << "|";
        os << tuple_str;
        os << "\n";
      }
    }
  } else {  // 不是最后一个 table，遍历当前table，table 的每个 tuple 与前序字符串拼接，并递归的完成单个 tuple 与下一张表的所有 tuple 的拼接
            //
    std::vector<std::string> cur_tuples = tables_tuple[*table_idx];
    // for (std::string tuple_str : cur_table_tuple) {
    (*table_idx)++;
    for (long unsigned int i = 0; i< cur_tuples.size();i++) {
      std::string tuple_str = cur_tuples[i];
      // auto next_iter = table_iter++;
      if (pre_tuple_str.length() != 0) {
        tuple_str = pre_tuple_str + "|" + tuple_str;
      }
      cartesian_product_dfs(os, tuple_str, tables_tuple, table_idx, end);
    }
    (*table_idx)--;
  }
}

  // 支持多表查询
  RC ExecuteStage::do_select_multi(SQLStageEvent * sql_event)
  {
    SelectStmt *select_stmt = (SelectStmt *)(sql_event->stmt());
    SessionEvent *session_event = sql_event->session_event();
    RC rc = RC::SUCCESS;

    if (select_stmt->tables().size() == 1) {
      LOG_WARN("select  1 tables");
      rc = RC::UNIMPLENMENT;
      return rc;
    }
    // TODO(zhangpc) 多表查询时是否使用索引，如有多个索引如何选择？

    // std::unordered_map<char *, std::vector<std::string>> table_tuples;  // 记录表对象及存储其tuple 的vector 的对应关系
    std::vector<std::vector<std::string>> table_tuples;

    // 对所有表执行 scan_record 操作，保存每一张表的扫描结果与表名对应关系 table_name : records
    int nSize = (int)select_stmt->tables().size();
    for (int i = nSize -1 ; i >=0; i--) {
      Table *table = select_stmt->tables()[i];
      // 构造表扫描器

      Operator *scan_oper = new TableScanOperator(table);

      DEFER([&]() { delete scan_oper; });

      FilterStmt *my_filter_stmt = nullptr;

      // 挑选出 where 语句中适用于本表的查询条件
      std::unordered_map<std::string, FilterStmt *> table_filter_stmt = select_stmt->filter_stmts();

      std::string table_name =  table->name();
      auto iter = table_filter_stmt.find(table_name);  // 通过 table_name 找到保存在 select_stmt->table_filter_stmt_ 中的 filter_stmt
      if (iter != table_filter_stmt.end()) {  // 若不存在 table_name 对应的 filter_stmt，跳过
        my_filter_stmt = iter->second;        // 记录table_name 对应的 filter_stmt
      }

// 创建谓词操作器，    
// 对得到的乘积结果执行过滤
      PredicateOperator pred_oper(my_filter_stmt);  // 若不存在 table_name 对应的 filter_stmt，此处传入参数为空
      pred_oper.add_child(scan_oper);

      ProjectOperator project_oper;
      project_oper.add_child(&pred_oper);
      for (const Field &field : select_stmt->query_fields()) {
        const char *filed_belong = field.table()->name();
        if (strcmp(filed_belong, table->name()) == 0) {
          project_oper.add_projection(true, field.table(), field.meta());  // 将所有 select 要查询的列名称 记录在 ProjectOperator 的 speces_ 容器中
        }
      }

      //  vector 保存每张表的扫描结果
      std::vector<std::string> tuple_data;

      rc = project_oper.open();  // project_oper.open() -> pred_oper.open() -> scan_oper.open()
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to open operator");
        return rc;
      }

      // 扫描表
      while ((rc = project_oper.next()) == RC::SUCCESS) {
        std::stringstream ss;
        Tuple *tuple = project_oper.current_tuple();  // 获取当前元组

        if (nullptr == tuple) {
          rc = RC::INTERNAL;
          LOG_WARN("failed to get current record. rc=%s", strrc(rc));
          break;
        }

        tuple_to_string(ss, *tuple);  // 元组转为字符串
        tuple_data.push_back(ss.str());
      }
      // table_tuples.insert(std::pair<char *, std::vector<std::string>>(table_name, tuple_data));
      table_tuples.push_back(tuple_data);

      project_oper.close();

    }
    // 至此，所有table 中的符合条件的行都已经被收集到 table_tuples 中

    // 自定义投影操作器，下面会针对 vector<string> 根据 query 的f ields 进行投影
    ProjectOperator project_str_oper;
    // 记录每所有表的所有投影列，由于 query_fields 是根据 from 后表的逆序存储表的投影的
    // 因此需要反向遍历 tables，找到from 后多表的正向顺序，并得到其投影field
    int tSize = (int)select_stmt->tables().size();
    for (int i = tSize - 1; i >= 0; i--) {
      Table *t = select_stmt->tables()[i];

      for (const Field &field : select_stmt->query_fields()) {
        if (strcmp(field.table()->name(), t->name()) == 0) {
          project_str_oper.add_projection(true, field.table(), field.meta());  // 将所有 select 要查询的列名称 记录在 ProjectOperator 的 speces_ 容器中
        }
      }
    }

    // 对所有组结果执行笛卡尔积操作，深度优先遍历搜索或者回溯算法
    std::stringstream c_p_os;

    print_tuple_header(c_p_os, project_str_oper);// 打印select 要查询的列记录的列名
    // 遍历所有表的所有行，过程中执行拼接和投影操作
    long unsigned int table_i = 0;
    std::string empty_pre = "";
    cartesian_product_dfs(c_p_os, empty_pre, table_tuples, &table_i, table_tuples.size());


// close()


    // 将最终结果返回
    session_event->set_response(c_p_os.str());  // 将结果返回
    return rc;
  }

  RC ExecuteStage::do_help(SQLStageEvent * sql_event)
  {
    SessionEvent *session_event = sql_event->session_event();
    const char *response = "show tables;\n"
                           "desc `table name`;\n"
                           "create table `table name` (`column name` `column type`, ...);\n"
                           "create index `index name` on `table` (`column`);\n"
                           "insert into `table` values(`value1`,`value2`);\n"
                           "update `table` set column=value [where `column`=`value`];\n"
                           "delete from `table` [where `column`=`value`];\n"
                           "select [ * | `columns` ] from `table`;\n";
    session_event->set_response(response);
    return RC::SUCCESS;
  }

  RC ExecuteStage::do_create_table(SQLStageEvent * sql_event)
  {
    const CreateTable &create_table = sql_event->query()->sstr.create_table;
    SessionEvent *session_event = sql_event->session_event();
    Db *db = session_event->session()->get_current_db();
    RC rc = db->create_table(create_table.relation_name,  // 在当前数据库中创建 table
        create_table.attribute_count,
        create_table.attributes);
    if (rc == RC::SUCCESS) {
      session_event->set_response("SUCCESS\n");
    } else {
      session_event->set_response("FAILURE\n");
    }
    return rc;
  }
  RC ExecuteStage::do_create_index(SQLStageEvent * sql_event)
  {
    SessionEvent *session_event = sql_event->session_event();
    Db *db = session_event->session()->get_current_db();  // 确定目标数据库
    const CreateIndex &create_index = sql_event->query()->sstr.create_index;
    Table *table = db->find_table(create_index.relation_name);  // 确定目标表
    if (nullptr == table) {
      session_event->set_response("FAILURE\n");
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    RC rc = table->create_index(nullptr, create_index.index_name, create_index.attribute_name);  // 执行索引创建
    sql_event->session_event()->set_response(rc == RC::SUCCESS ? "SUCCESS\n" : "FAILURE\n");
    return rc;
  }

  RC ExecuteStage::do_show_tables(SQLStageEvent * sql_event)
  {
    SessionEvent *session_event = sql_event->session_event();
    Db *db = session_event->session()->get_current_db();
    std::vector<std::string> all_tables;
    db->all_tables(all_tables);
    if (all_tables.empty()) {
      session_event->set_response("No table\n");
    } else {
      std::stringstream ss;
      for (const auto &table : all_tables) {
        ss << table << std::endl;
      }
      session_event->set_response(ss.str().c_str());
    }
    return RC::SUCCESS;
  }

  RC ExecuteStage::do_desc_table(SQLStageEvent * sql_event)
  {
    Query *query = sql_event->query();
    Db *db = sql_event->session_event()->session()->get_current_db();
    const char *table_name = query->sstr.desc_table.relation_name;
    Table *table = db->find_table(table_name);
    std::stringstream ss;
    if (table != nullptr) {
      table->table_meta().desc(ss);
    } else {
      ss << "No such table: " << table_name << std::endl;
    }
    sql_event->session_event()->set_response(ss.str().c_str());
    return RC::SUCCESS;
  }

  RC ExecuteStage::do_insert(SQLStageEvent * sql_event)
  {
    Stmt *stmt = sql_event->stmt();
    SessionEvent *session_event = sql_event->session_event();

    if (stmt == nullptr) {
      LOG_WARN("cannot find statement");
      return RC::GENERIC_ERROR;
    }

    InsertStmt *insert_stmt = (InsertStmt *)stmt;

    Table *table = insert_stmt->table();
    RC rc = table->insert_record(nullptr, insert_stmt->value_amount(), insert_stmt->values());
    if (rc == RC::SUCCESS) {
      session_event->set_response("SUCCESS\n");
    } else {
      session_event->set_response("FAILURE\n");
    }
    return rc;
  }

  RC ExecuteStage::do_delete(SQLStageEvent * sql_event)
  {
    Stmt *stmt = sql_event->stmt();
    SessionEvent *session_event = sql_event->session_event();

    if (stmt == nullptr) {
      LOG_WARN("cannot find statement");
      return RC::GENERIC_ERROR;
    }

    DeleteStmt *delete_stmt = (DeleteStmt *)stmt;
    TableScanOperator scan_oper(delete_stmt->table());
    PredicateOperator pred_oper(delete_stmt->filter_stmt());
    pred_oper.add_child(&scan_oper);
    DeleteOperator delete_oper(delete_stmt);
    delete_oper.add_child(&pred_oper);

    RC rc = delete_oper.open();
    if (rc != RC::SUCCESS) {
      session_event->set_response("FAILURE\n");
    } else {
      session_event->set_response("SUCCESS\n");
    }
    return rc;
  }

  RC ExecuteStage::do_update(SQLStageEvent * sql_event)
  {
    Stmt *stmt = sql_event->stmt();
    SessionEvent *session_event = sql_event->session_event();

    if (stmt == nullptr) {
      LOG_WARN("cannot find statement");
      return RC::GENERIC_ERROR;
    }

    UpdateStmt *update_stmt = (UpdateStmt *)stmt;

    const Updates &updates = sql_event->query()->sstr.update;

    Table *table = update_stmt->table();
    int *updated_rows = 0;
    RC rc = table->update_record(
        nullptr, updates.attribute_name, &(updates.value), updates.condition_num, updates.conditions, updated_rows);
    if (rc == RC::SUCCESS) {
      session_event->set_response("SUCCESS\n");
    } else {
      session_event->set_response("FAILURE\n");
    }
    return rc;
  }