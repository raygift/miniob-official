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

#include "storage/index/bplus_tree_multi.h"
#include "storage/index/index.h"
#include "storage/index/index_multi.h"
#include "storage/index/bplus_tree.h"


class BplusTreeMultiIndex : public IndexMulti {
public:
  BplusTreeMultiIndex() = default;
  virtual ~BplusTreeMultiIndex() noexcept;

  RC create(const char *file_name, const IndexMultiMeta &index_multi_meta, std::vector<const FieldMeta*>fields_meta);
  RC open(const char *file_name, const IndexMeta &index_meta, const FieldMeta &field_meta);
  RC close();

  RC insert_entry(const char *record, const RID *rid) override;
  RC delete_entry(const char *record, const RID *rid) override;

  /**
   * 扫描指定范围的数据
   */
  MultiIndexScanner *create_scanner(const char *left_key, int left_len, bool left_inclusive,
			       const char *right_key, int right_len, bool right_inclusive) override;

  RC sync() override;

private:
  const int is_unique()
  {
    return index_multi_meta_.is_unique();
  }

private:
  bool inited_ = false;
  // BplusTreeHandler 用于提供 B+ 树相关操作，针对 index 的 create、insert_entry 等操作都需要 B+树 执行对应操作
  // BplusTreeHandler index_handler_;
  BplusTreeMultiHandler m_index_handler_;
};

class MultiBplusTreeIndexScanner : public MultiIndexScanner {
public:
  MultiBplusTreeIndexScanner(BplusTreeMultiHandler &m_tree_handler);
  ~MultiBplusTreeIndexScanner() noexcept override;

  RC next_entry(RID *rid) override;
  RC destroy() override;

  RC open(const char *left_key, int left_len, bool left_inclusive,
          const char *right_key, int right_len, bool right_inclusive);
private:
  BplusTreeMultiScanner m_tree_scanner_;
};

#endif  //__OBSERVER_STORAGE_COMMON_BPLUS_TREE_MULTI_INDEX_H_
