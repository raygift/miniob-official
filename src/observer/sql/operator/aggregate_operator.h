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
// Created by WangYunlai on 2022/07/01.
//

#pragma once

#include "sql/operator/operator.h"
#include "rc.h"

class AggregateOperator : public Operator
{
public:
  AggregateOperator()
  {}

  virtual ~AggregateOperator() = default;

  void add_aggregation(Field field);

  RC open() override;
  RC next() override;
  RC close() override;

  int tuple_cell_num() const
  {
    return tuple_.cell_num();
  }

  RC tuple_cell_spec_at(int index, const TupleCellSpec *&spec) const;

  Tuple * current_tuple() override;

private:
  SimpleTuple tuple_;
  std::vector<Field> aggre_fields_;
  std::vector<TupleCell> current_cell_;
  std::vector<float> statistics_;
  int total_row_count_ = 0;
  bool remain_result_ = true;
};
