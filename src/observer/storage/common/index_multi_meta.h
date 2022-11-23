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
// Created by Meiyi & Wangyunlai on 2021/5/12.
//

#ifndef __OBSERVER_STORAGE_COMMON_INDEX_MULTI_META_H__
#define __OBSERVER_STORAGE_COMMON_INDEX_MULTI_META_H__

#include <string>
#include <vector>
#include "rc.h"

class TableMeta;
class FieldMeta;

// namespace Json {
// class Value;
// }  // namespace Json

class IndexMultiMeta {
public:
  IndexMultiMeta() = default;

  RC init(const char *name, std::vector<const FieldMeta *> fields_meta, int is_unique);

public:
  const char *name() const;
  const char *field() const;
  const std::vector<std::string> fields() const;

  void desc(std::ostream &os) const;
  const int is_unique();

// public:
//   void to_json(Json::Value &json_value) const;
//   static RC from_json(const IndexMultiMeta &table, const Json::Value &json_value, IndexMultiMeta &index);

protected:
  std::string name_;   // index's name
  std::vector<std::string> fields_;  // field's name
  int is_unique_;
};
#endif  // __OBSERVER_STORAGE_COMMON_INDEX_MULTI_META_H__