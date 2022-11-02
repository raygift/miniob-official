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

#include "storage/index/bplus_tree_multi_index.h"
#include "common/log/log.h"

BplusTreeMultiIndex::~BplusTreeMultiIndex() noexcept
{
  close();
}

RC BplusTreeMultiIndex::create(const char *file_name, const IndexMultiMeta &index_multi_meta, std::vector<const FieldMeta*>fields_meta)
{
  // if (inited_) {
  //   LOG_WARN("Failed to create index due to the index has been created before. file_name:%s, index:%s, field:%s",
  //       file_name,
  //       index_meta.name(),
  //       index_meta.field());
  //   return RC::RECORD_OPENNED;
  // }

  // IndexMulti::init(index_meta, fields_meta);
  // for (size_t i = 0; i != index_handlers_.size(); i++) {

  //   RC rc = index_handlers_[i].create(file_name, fields_meta[i].type(), field_meta.len());
  //   if (RC::SUCCESS != rc) {
  //     LOG_WARN("Failed to create index_handler, file_name:%s, index:%s, field:%s, rc:%s",
  //         file_name,
  //         index_meta.name(),
  //         index_meta.field(),
  //         strrc(rc));
  //     return rc;
  //   }
  // }

  // inited_ = true;
  // LOG_INFO(
  //     "Successfully create index, file_name:%s, index:%s, field:%s", file_name, index_meta.name(), index_meta.field());
  // return RC::SUCCESS;

  // TODO(multi-index)
  return RC::SUCCESS;
}

RC BplusTreeMultiIndex::open(const char *file_name, const IndexMeta &index_meta, const FieldMeta &field_meta)
{
  // if (inited_) {
  //   LOG_WARN("Failed to open index due to the index has been initedd before. file_name:%s, index:%s, field:%s",
  //       file_name,
  //       index_meta.name(),
  //       index_meta.field());
  //   return RC::RECORD_OPENNED;
  // }

  // Index::init(index_meta, field_meta);

  // RC rc = index_handler_.open(file_name);
  // if (RC::SUCCESS != rc) {
  //   LOG_WARN("Failed to open index_handler, file_name:%s, index:%s, field:%s, rc:%s",
  //       file_name,
  //       index_meta.name(),
  //       index_meta.field(),
  //       strrc(rc));
  //   return rc;
  // }

  // inited_ = true;
  // LOG_INFO(
  //     "Successfully open index, file_name:%s, index:%s, field:%s", file_name, index_meta.name(), index_meta.field());
  // return RC::SUCCESS;
  // TODO(multi-index)
  return RC::SUCCESS;
}

RC BplusTreeMultiIndex::close()
{
  // if (inited_) {
  //   LOG_INFO("Begin to close index, index:%s, field:%s",
  //       index_meta_.name(), index_meta_.field());
  //   index_handler_.close();
  //   inited_ = false;
  // }
  // LOG_INFO("Successfully close index.");
  return RC::SUCCESS;
}

RC BplusTreeMultiIndex::insert_entry(const char *record, const RID *rid)
{
  size_t field_num = multi_fields_meta_.size();
  int key_len = 0;
  for (size_t i = 0; i < field_num; i++) {
    key_len += multi_fields_meta_[i]->len();
  }

  char *user_key = new char(key_len + 1);
  int offset = 0;
  // 将多列属性的值取出，组合为 user_key ， 传递给 IndexHandler 用于创建 B+Tree 中的 key
  for (size_t i = 0; i < field_num; i++) {
    memcpy(user_key + offset, record + multi_fields_meta_[i]->offset(), multi_fields_meta_[i]->len());
    offset += multi_fields_meta_[i]->len();
  }

  return m_index_handler_.insert_entry(user_key, rid);
  // return RC::SUCCESS;
}

RC BplusTreeMultiIndex::delete_entry(const char *record, const RID *rid)
{
  // TODO(multi-index)
  return RC::SUCCESS;
  // return index_handler_.delete_entry(record + field_meta_.offset(), rid);
    // return RC::SUCCESS;
}

MultiIndexScanner *BplusTreeMultiIndex::create_scanner(const char *left_key, int left_len, bool left_inclusive,
					     const char *right_key, int right_len, bool right_inclusive)
{
  MultiBplusTreeIndexScanner *index_scanner = new MultiBplusTreeIndexScanner(m_index_handler_);
  RC rc = index_scanner->open(left_key, left_len, left_inclusive, right_key, right_len, right_inclusive);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open index scanner. rc=%d:%s", rc, strrc(rc));
    delete index_scanner;
    return nullptr;
  }
  return index_scanner;
}

RC BplusTreeMultiIndex::sync()
{
  // return index_handler_.sync();
  // TODO(multi-index)
  return RC::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
MultiBplusTreeIndexScanner::MultiBplusTreeIndexScanner(BplusTreeMultiHandler &m_tree_handler) : m_tree_scanner_(m_tree_handler)
{}

MultiBplusTreeIndexScanner::~MultiBplusTreeIndexScanner() noexcept
{
  m_tree_scanner_.close();
}

RC MultiBplusTreeIndexScanner::open(const char *left_key, int left_len, bool left_inclusive,
                               const char *right_key, int right_len, bool right_inclusive)
{
  // return tree_scanner_.open(left_key, left_len, left_inclusive, right_key, right_len, right_inclusive);
  // TODO(multi-index)
  return RC::SUCCESS;
}

RC MultiBplusTreeIndexScanner::next_entry(RID *rid)
{
  // return tree_scanner_.next_entry(rid);
  // TODO(multi-index)
  return RC::SUCCESS;
}

RC MultiBplusTreeIndexScanner::destroy()
{
  delete this;
  return RC::SUCCESS;
}
