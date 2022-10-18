//
// Created by Chow on 2022/9/16.
//

#pragma once

#include "sql/operator/operator.h"
#include "sql/stmt/select_stmt.h"

class FilterStmt;

/**
 * CartesianOperator 用于单个表中的记录过滤
 * 如果是多个表数据过滤，比如join条件的过滤，需要设计新的predicate或者扩展
 */
class CartesianOperator : public Operator {
public:
  CartesianOperator()
  {}

  CartesianOperator(SelectStmt *select_stmt) : select_stmt_(select_stmt)
  {}

  virtual ~CartesianOperator() = default;

  RC open() override;
  RC next() override;
  RC close() override;

  Tuple *current_tuple() override;

private:
  SelectStmt *select_stmt_;

  std::vector<std::vector<char *>> scan_tables_;
  std::vector<int> record_idx_;
  int record_size_;
  bool first_record_ = true;

  CompositeTuple current_tuple_;
};
