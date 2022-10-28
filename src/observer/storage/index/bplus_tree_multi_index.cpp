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

RC BplusTreeMultiIndex::create(const char *file_name, const IndexMultiMeta &index_meta, std::vector<FieldMeta*> fields_meta)
{
  // if (inited_) {
  //   LOG_WARN("Failed to create index due to the index has been created before. file_name:%s, index:%s, field:%s",
  //       file_name,
  //       index_meta.name(),
  //       index_meta.field());
  //   return RC::RECORD_OPENNED;
  // }

  IndexMulti::init(index_meta, fields_meta);
  for (size_t i = 0; i != index_handlers_.size(); i++) {

    RC rc = index_handlers_[i].create(file_name, fields_meta[i].type(), field_meta.len());
    if (RC::SUCCESS != rc) {
      LOG_WARN("Failed to create index_handler, file_name:%s, index:%s, field:%s, rc:%s",
          file_name,
          index_meta.name(),
          index_meta.field(),
          strrc(rc));
      return rc;
    }
  }

  inited_ = true;
  LOG_INFO(
      "Successfully create index, file_name:%s, index:%s, field:%s", file_name, index_meta.name(), index_meta.field());
  return RC::SUCCESS;
}

RC BplusTreeMultiIndex::open(const char *file_name, const IndexMeta &index_meta, const FieldMeta &field_meta)
{
  if (inited_) {
    LOG_WARN("Failed to open index due to the index has been initedd before. file_name:%s, index:%s, field:%s",
        file_name,
        index_meta.name(),
        index_meta.field());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_meta);

  RC rc = index_handler_.open(file_name);
  if (RC::SUCCESS != rc) {
    LOG_WARN("Failed to open index_handler, file_name:%s, index:%s, field:%s, rc:%s",
        file_name,
        index_meta.name(),
        index_meta.field(),
        strrc(rc));
    return rc;
  }

  inited_ = true;
  LOG_INFO(
      "Successfully open index, file_name:%s, index:%s, field:%s", file_name, index_meta.name(), index_meta.field());
  return RC::SUCCESS;
}

RC BplusTreeMultiIndex::close()
{
  if (inited_) {
    LOG_INFO("Begin to close index, index:%s, field:%s",
        index_meta_.name(), index_meta_.field());
    index_handler_.close();
    inited_ = false;
  }
  LOG_INFO("Successfully close index.");
  return RC::SUCCESS;
}

RC BplusTreeMultiIndex::insert_entry(const char *record, const RID *rid)
{
  return index_handler_.insert_entry(record + field_meta_.offset(), rid);
}

RC BplusTreeMultiIndex::delete_entry(const char *record, const RID *rid)
{
  return index_handler_.delete_entry(record + field_meta_.offset(), rid);
}

IndexScanner *BplusTreeMultiIndex::create_scanner(const char *left_key, int left_len, bool left_inclusive,
					     const char *right_key, int right_len, bool right_inclusive)
{
  BplusTreeMultiIndexScanner *index_scanner = new BplusTreeMultiIndexScanner(index_handler_);
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
  return index_handler_.sync();
}

////////////////////////////////////////////////////////////////////////////////
BplusTreeMultiIndexScanner::BplusTreeMultiIndexScanner(BplusTreeHandler &tree_handler) : tree_scanner_(tree_handler)
{}

BplusTreeMultiIndexScanner::~BplusTreeMultiIndexScanner() noexcept
{
  tree_scanner_.close();
}

RC BplusTreeMultiIndexScanner::open(const char *left_key, int left_len, bool left_inclusive,
                               const char *right_key, int right_len, bool right_inclusive)
{
  return tree_scanner_.open(left_key, left_len, left_inclusive, right_key, right_len, right_inclusive);
}

RC BplusTreeMultiIndexScanner::next_entry(RID *rid)
{
  return tree_scanner_.next_entry(rid);
}

RC BplusTreeMultiIndexScanner::destroy()
{
  delete this;
  return RC::SUCCESS;
}
