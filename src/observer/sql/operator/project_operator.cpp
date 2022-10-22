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

#include "common/log/log.h"
#include "sql/operator/project_operator.h"
#include "storage/record/record.h"
#include "storage/common/table.h"

RC ProjectOperator::open()
{
  if (children_.size() != 1) {
    LOG_WARN("project operator must has 1 child");
    return RC::INTERNAL;
  }

  Operator *child = children_[0];
  RC rc = child->open();
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  return RC::SUCCESS;
}

RC ProjectOperator::next()
{
  return children_[0]->next();
}

RC ProjectOperator::close()
{
  children_[0]->close();
  return RC::SUCCESS;
}
Tuple *ProjectOperator::current_tuple()
{
  tuple_.set_tuple(children_[0]->current_tuple());
  return &tuple_;
}

void ProjectOperator::add_projection(const Table *table, const FieldMeta *field_meta, AggreType aggre_type, bool multi_table)
{
  TupleCellSpec *spec = new TupleCellSpec(new FieldExpr(table, field_meta));
  char *basic_alias;
  int basic_length;
  const char *field_name = "*";
  if (field_meta != nullptr) {
    field_name = field_meta->name();
  }
  if (!multi_table) {
    // 对单表来说，展示的(alias) 字段总是字段名称，
    basic_length = strlen(field_name) + 1;
    basic_alias = (char *)malloc(basic_length);
    strcpy(basic_alias, field_name);
  } else {
    // 对多表查询来说，展示的alias 需要带表名字
    basic_length = strlen(table->name()) + 1 + strlen(field_name) + 1;
    basic_alias = (char *)malloc(basic_length);
    strcpy(basic_alias, table->name());
    strcat(basic_alias, ".");
    strcat(basic_alias, field_name);
  }
  char *aggre_alias = basic_alias;
  switch (aggre_type)
  {
  case MAX:
  {
    aggre_alias = (char *) malloc(basic_length + 5);
    strcpy(aggre_alias, "MAX(");
    strcat(aggre_alias, basic_alias);
    strcat(aggre_alias, ")");
    break;
  }
  case MIN:
  {
    aggre_alias = (char *) malloc(basic_length + 5);
    strcpy(aggre_alias, "MIN(");
    strcat(aggre_alias, basic_alias);
    strcat(aggre_alias, ")");
    break;
  }
  case SUM:
  {
    aggre_alias = (char *) malloc(basic_length + 5);
    strcpy(aggre_alias, "SUM(");
    strcat(aggre_alias, basic_alias);
    strcat(aggre_alias, ")");
    break;
  }
  case AVG:
  {
    aggre_alias = (char *) malloc(basic_length + 5);
    strcpy(aggre_alias, "AVG(");
    strcat(aggre_alias, basic_alias);
    strcat(aggre_alias, ")");
    break;
  }
  case COUNT:
  {
    aggre_alias = (char *) malloc(basic_length + 7);
    strcpy(aggre_alias, "COUNT(");
    strcat(aggre_alias, basic_alias);
    strcat(aggre_alias, ")");
    break;
  }
  }
  spec->set_alias(aggre_alias);
  tuple_.add_cell_spec(spec);
}

RC ProjectOperator::tuple_cell_spec_at(int index, const TupleCellSpec *&spec) const
{
  return tuple_.cell_spec_at(index, spec);
}
