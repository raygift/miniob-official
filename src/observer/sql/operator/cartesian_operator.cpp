
//
// Created by Chow on 2022/09/16.
//

#include "common/log/log.h"
#include "common/lang/defer.h"
#include "sql/operator/cartesian_operator.h"
#include "sql/operator/predicate_operator.h"
#include "sql/operator/table_scan_operator.h"
#include "storage/record/record.h"
#include "storage/common/table.h"

RC CartesianOperator::open()
{

  // collect records from all chilren
  for (auto table : select_stmt_->tables()) {
    // TODO: 找到只属于单表的条件

    std::vector<Operator *> scan_opers;
    Operator *scan_oper = nullptr;
    // TODO: 使用单表的筛选条件进行索引扫描
    // Operator *scan_oper = try_to_create_index_scan_operator(select_stmt->filter_stmt());
    if (scan_oper == nullptr) {
      scan_oper = new TableScanOperator(table);
    }
    DEFER([&]() { delete scan_oper; });

    // TODO: 使用单表的筛选条件对单表的记录进行筛选
    // PredicateOperator pred_oper();

    RC rc = scan_oper->open();
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to open child operator: %s", strrc(rc));
      return rc;
    }
    std::vector<char *> table_data;

    // collect records of table
    while ((rc = scan_oper->next()) == RC::SUCCESS) {
      RowTuple *tuple = (RowTuple *)scan_oper->current_tuple();
      table_data.push_back(tuple->record().data());
    }

    scan_tables_.push_back(table_data);
    record_idx_.push_back(0);
    record_size_ += table->table_meta().record_size();
  }

  // record spec info of child
  current_tuple_.set_schema(select_stmt_->tables());

  return RC::SUCCESS;
}

RC CartesianOperator::next()
{
  // check if the first time call next()
  if (first_record_) {
    first_record_ = false;
    // check if there are empty records
    for (auto table_data : scan_tables_) {
      if (table_data.size() == 0) {
        return RC::RECORD_EOF;
      }
    }
  } else {  // push the index
    bool carry = true;
    for (int i = record_idx_.size() - 1; i >= 0; i--) {
      if (carry) {
        carry = false;
        record_idx_[i]++;
        if (record_idx_[i] == scan_tables_[i].size()) {
          carry = true;
          record_idx_[i] = 0;
        }
      }
    }
    if (carry) {
      return RC::RECORD_EOF;
    }
  }
  return RC::SUCCESS;
}

RC CartesianOperator::close()
{
  for (auto child : children_) {
    child->close();
  }
  return RC::SUCCESS;
}

Tuple *CartesianOperator::current_tuple()
{
  std::vector<char *> records;
  // concate the record result
  for (int i = 0; i < record_idx_.size(); i++) {
    int idx = record_idx_[i];
    // concat_record += scan_tables_[i][idx];
    records.push_back(scan_tables_[i][idx]);
  }
  current_tuple_.set_data(records);
  return &current_tuple_;
}
