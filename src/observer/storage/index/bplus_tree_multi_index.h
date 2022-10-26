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
// Created by Meiyi & wangyunlai.wyl on 2021/5/19.
//

#ifndef __OBSERVER_STORAGE_COMMON_BPLUS_TREE_MULTI_INDEX_H_
#define __OBSERVER_STORAGE_COMMON_BPLUS_TREE_MULTI_INDEX_H_

#include "storage/index/index.h"
#include "storage/index/index_multi.h"
#include "storage/index/bplus_tree.h"

class BplusTreeMultiIndex : public Index {
public:
  BplusTreeMultiIndex() = default;
  virtual ~BplusTreeMultiIndex() noexcept;

  RC create(const char *file_name, const IndexMultiMeta &index_meta, std::vector<FieldMeta*> fields_meta);
  RC open(const char *file_name, const IndexMeta &index_meta, const FieldMeta &field_meta);
  RC close();

  RC insert_entry(const char *record, const RID *rid) override;
  RC delete_entry(const char *record, const RID *rid) override;

  /**
   * 扫描指定范围的数据
   */
  IndexScanner *create_scanner(const char *left_key, int left_len, bool left_inclusive,
			       const char *right_key, int right_len, bool right_inclusive) override;

  RC sync() override;

private:
  bool inited_ = false;
  std::vector<BplusTreeHandler> index_handlers_;
};


#endif  //__OBSERVER_STORAGE_COMMON_BPLUS_TREE_MULTI_INDEX_H_
