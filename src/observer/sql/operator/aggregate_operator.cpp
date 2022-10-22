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
#include <cmath>

#include "common/log/log.h"
#include "sql/operator/aggregate_operator.h"
#include "storage/record/record.h"
#include "storage/common/table.h"

RC AggregateOperator::open()
{
  if (children_.size() != 1) {
    LOG_WARN("aggregate new_dataoperator must has 1 child");
    return RC::INTERNAL;
  }

  Operator *child = children_[0];
  RC rc = child->open();
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  // pre-process: init statistics and first cell
  for (int i = 0 ; i < aggre_fields_.size() ; i++) {
    current_cell_.push_back(TupleCell());
    statistics_.push_back(0);
  }
  
  //  scan all record and statistic
  while ((rc = child->next()) == RC::SUCCESS) {
    // get current record
    // write to response
    Tuple * tuple = child->current_tuple();
    if (nullptr == tuple) {
      rc = RC::INTERNAL;
      LOG_WARN("failed to get current record. rc=%s", strrc(rc));
      break;
    }

    for (int i = 0 ; i < aggre_fields_.size() ; i++) {
      TupleCell cell, zero_cell;
      char zero_data[sizeof(float)];
      sprintf(zero_data, "%f", 0);
      zero_cell.set_type(FLOATS);
      zero_cell.set_data(zero_data);
      zero_cell.set_length(sizeof(float));
      auto aggre_field = aggre_fields_[i];
      float new_data;
      // update statistic value
      switch (aggre_field.aggre_type())
      {
      case MAX: {
        tuple->find_cell(aggre_field, cell); // compare the statistic
        if (current_cell_[i].data() == nullptr || current_cell_[i].compare(cell) <= 0)
          current_cell_[i] = cell;
        break;
      }
      case MIN:
      {
        tuple->find_cell(aggre_field, cell); // compare the statistic
        if (current_cell_[i].data() == nullptr || current_cell_[i].compare(cell) >= 0)
          current_cell_[i] = cell;
        break;
      }
      case AVG:
      {
        tuple->find_cell(aggre_field, cell);
        switch (cell.attr_type())
        {
        case INTS: new_data = *(int *)cell.data(); break;
        case FLOATS: new_data = *(float *)cell.data(); break;
        case CHARS: new_data = std::atof((char *)cell.data()); break;
        default: new_data = 0; break; // TODO: 
        }
        statistics_[i] += new_data; 
        break;
      }
      case SUM:
      {
        tuple->find_cell(aggre_field, cell);
        switch (cell.attr_type())
        {
        case INTS: new_data = *(int *)cell.data(); break;
        case FLOATS: new_data = *(float *)cell.data(); break;
        case CHARS: new_data = std::atof((char *)cell.data()); break;
        default: new_data = 0; break; // TODO:
        }
        // new_data = cell.compare(zero_cell);
        statistics_[i] += new_data; 
        break;
      }
      case COUNT:
      {
        // tuple->find_cell(aggre_field, cell);
        // XXX: miniob doesn't have NULL values
        statistics_[i]++; 
        break;
      }
      default: break;
      }
      total_row_count_ ++;
    }
  }

  // post-process statistic value
  for (int i = 0 ; i < aggre_fields_.size() ; i++) {
    TupleCell cell;
    switch (aggre_fields_[i].aggre_type())
    {
    case AVG:
    {
      cell.set_type(FLOATS);
      float avg_data = statistics_[i] / total_row_count_;
      cell.set_data((char *) &avg_data);
      cell.set_length(sizeof(float));
      break;
    }
    case SUM: {
      cell.set_type(FLOATS);
      // char *sum_data = (char *)malloc(sizeof(int));
      // sprintf(sum_data, "%f", statistics_[i]);
      cell.set_data((char *) &statistics_[i]);
      cell.set_length(sizeof(float));
      break;
    }
    case COUNT:
    {
      cell.set_type(INTS);
      // char *count_data = (char *)malloc(sizeof(int));
      // sprintf(count_data, "%d", std::round(statistics_[i]));
      int count = std::round(statistics_[i]);
      cell.set_data((char *) &count);
      cell.set_length(sizeof(int));
      break;
    }
    default: // MIN / MAX
    {
      cell.set_type(aggre_fields_[i].attr_type());
      cell.set_data(current_cell_[i].data());
      cell.set_length(current_cell_[i].length());
      break;
    }
    }
    tuple_.push_cell(cell);
  }

  remain_result_ = true;
  if ( total_row_count_ == 0 ) {
    remain_result_= false;
    return RC::SUCCESS;
  }

  return RC::SUCCESS;
}

RC AggregateOperator::next()
{
  if (remain_result_) {
    remain_result_ = false;
    return RC::SUCCESS;
  } else {
    return RC::RECORD_EOF;
  }
} 

RC AggregateOperator::close()
{
  children_[0]->close();
  return RC::SUCCESS;
}

Tuple *AggregateOperator::current_tuple()
{
  // TODO: build a new statistic tuple

  // tuple_.set_tuple(children_[0]->current_tuple());
  return &tuple_;
}

void AggregateOperator::add_aggregation(Field field)
{
  TupleCellSpec *spec = new TupleCellSpec(new FieldExpr(field.table(), field.meta()));
  tuple_.push_cell_spec(spec);
  aggre_fields_.push_back(field);
}

RC AggregateOperator::tuple_cell_spec_at(int index, const TupleCellSpec *&spec) const
{
  return tuple_.cell_spec_at(index, spec);
}
