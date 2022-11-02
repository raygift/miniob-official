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
// Created by Meiyi & Wangyunlai.wyl on 2021/5/18.
//

#include "storage/common/index_multi_meta.h"
#include "storage/common/field_meta.h"
#include "storage/common/table_meta.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "rc.h"
// #include "json/json.h"

// const static Json::StaticString FIELD_NAME("name");
// const static Json::StaticString FIELD_FIELD_NAME("field_name");

RC IndexMultiMeta::init(const char *name, std::vector<const FieldMeta *> fields_meta, bool is_unique)
{
  if (common::is_blank(name)) {
    LOG_ERROR("Failed to init index, name is empty.");
    return RC::INVALID_ARGUMENT;
  }

  name_ = name;
  for(size_t i = 0;i< fields_meta.size();i++){
    const FieldMeta *field = fields_meta[i];
    fields_->push_back(field->name());
  }
  is_unique_ = is_unique;
  return RC::SUCCESS;
}

const char *IndexMultiMeta::name() const
{
  return name_.c_str();
}

const char *IndexMultiMeta::field() const
{
  // TODO(multi-index)
  char *new_char = "todo(multi-index)";
  return new_char;
}


const std::vector<std::string> *IndexMultiMeta::fields() const
{
  // TODO(multi-index)
  return fields_;
}

const int IndexMultiMeta::is_unique()
{
  return is_unique_;
}

void IndexMultiMeta::desc(std::ostream &os) const
{
  // TODO(multi-index)
  os << "index name=" << name_ << ", field= todo(multi-index)";
}