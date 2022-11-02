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
// Created by Meiyi & Wangyunlai on 2021/5/11.
//

#ifndef __OBSERVER_STORAGE_COMMON_INDEX_MULTI_H_
#define __OBSERVER_STORAGE_COMMON_INDEX_MULTI_H_

#include <stddef.h>
#include <vector>

#include "rc.h"
#include "storage/common/index_multi_meta.h"
#include "storage/common/field_meta.h"
#include "storage/record/record_manager.h"

class MultiIndexDataOperator {
public:
  virtual ~MultiIndexDataOperator() = default;
  virtual int compare(const void *data1, const void *data2) const = 0;
  virtual size_t hash(const void *data) const = 0;
};

class MultiIndexScanner;

class IndexMulti {

public:
  IndexMulti() = default;
  virtual ~IndexMulti() = default;

  const IndexMultiMeta &index_multi_meta() const
  {
    return index_multi_meta_;
  }

  virtual RC insert_entry(const char *record, const RID *rid) = 0;
  virtual RC delete_entry(const char *record, const RID *rid) = 0;

  virtual MultiIndexScanner *create_scanner(const char *left_key, int left_len, bool left_inclusive,
				       const char *right_key, int right_len, bool right_inclusive) = 0;

  virtual RC sync() = 0;

protected:
  RC init(const IndexMultiMeta &index_meta, std::vector<const FieldMeta*> multi_fields_meta);

protected:
  IndexMultiMeta index_multi_meta_;
  std::vector<const FieldMeta*> multi_fields_meta_;  /// 多个字段的索引
};

class MultiIndexScanner {
public:
  MultiIndexScanner() = default;
  virtual ~MultiIndexScanner() = default;

  /**
   * 遍历元素数据
   * 如果没有更多的元素，返回RECORD_EOF
   */
  virtual RC next_entry(RID *rid) = 0;
  virtual RC destroy() = 0;
};

#endif  // __OBSERVER_STORAGE_COMMON_INDEX_MULTI_H_
