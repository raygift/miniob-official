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

#ifndef __OBSERVER_STORAGE_COMMON_INDEX_META_H__
#define __OBSERVER_STORAGE_COMMON_INDEX_META_H__

#include <string>
#include "rc.h"

class TableMeta;
class FieldMeta;

namespace Json {
class Value;
}  // namespace Json

class IndexMeta {
public:
  IndexMeta() = default;

  RC init(const char *name, const FieldMeta &field);
  RC init(const char *name, const FieldMeta &field, const AttrInfo attributes[]);
  RC init(const char *name, const FieldMeta &field, int is_unique);

public:
  const char *name() const;
  const char *field() const;
  const int is_unique();
  bool is_last_field();

  void desc(std::ostream &os) const;

public:
  void to_json(Json::Value &json_value) const;
  static RC from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index);

protected:
  std::string name_;   // index's name
  std::string field_;  // field's name
  int is_unique_;
  // 针对多列索引，
  // 根据 field_ 在 multi_fileds_ 中的顺序来判断该属性的叶节点所保存的内容；
  // 最后一个属性叶节点保存实际 record；
  // 除最后一个属性之外的其他属性对应的B+Tree 叶节点保存下一个属性对应的B+Tree
  std::vector<std::string> multi_fileds_;
};
#endif  // __OBSERVER_STORAGE_COMMON_INDEX_META_H__