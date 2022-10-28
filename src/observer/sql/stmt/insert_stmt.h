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

#pragma once

#include "rc.h"
#include "sql/stmt/stmt.h"

class Table;
class Db;

class InsertStmt : public Stmt
{
public:

  InsertStmt() = default;
  InsertStmt(Table *table, const Value *values, int value_amount);
  InsertStmt(Table *table, const Value *values, int value_amount, const InsertValue *value_array, int array_length);
  
  StmtType type() const override {
    return StmtType::INSERT;
  }
public:
  static RC create(Db *db, const Inserts &insert_sql, Stmt *&stmt);

public:
  Table *table() const {return table_;}
  const Value *values() const { return values_; }
  int value_amount() const { return value_amount_; }
  const InsertValue *value_array() const { return value_array_; }
  int array_length() const { return array_length_; }
  void set_values(const Value * values) { values_ = values; }
  void set_value_amount(int value_amout) { value_amount_ = value_amout; }

private:
  Table *table_ = nullptr;
  const Value *values_ = nullptr;
  int value_amount_ = 0;
  const InsertValue *value_array_ = nullptr;
  int array_length_ = 0;
};

