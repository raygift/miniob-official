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
// Created by Xie Meiyi
// Rewritten by Longda & Wangyunlai
//
// #include "storage/index/bplus_tree.h"
#include "storage/index/bplus_tree_multi.h"

#include "storage/default/disk_buffer_pool.h"
#include "rc.h"
#include "common/log/log.h"
#include "sql/parser/parse_defs.h"
#include "common/lang/lower_bound.h"

#define FIRST_INDEX_PAGE 1

// 计算 B+Tree 内部节点的索引项容量
int multi_index_calc_internal_page_capacity(int attrs_length)
{
  // 内部节点除了 common header 部分，剩余部分存储 key 和 page num
  // 这里计算得到 key 和 page num 所占空间大小
  // 其中 key 为所有索引列的属性值+RID 构成
  int item_size = attrs_length + sizeof(RID) + sizeof(PageNum);

  // 页面大小 - 内部节点的 comman header 部分，得到的是存储 key:page_num 部分的总空间
  // 再除以每个索引项所占空间，得到的是共可以存储多少个索引项
  int capacity =
    ((int)BP_PAGE_DATA_SIZE - MultiInternalIndexNode::HEADER_SIZE) / item_size;
  return capacity;
}

// 计算 B+Tree 叶子节点的索引项容量
int multi_index_calc_leaf_page_capacity(int attrs_length)
{
  int item_size = attrs_length + sizeof(RID) + sizeof(RID);
  int capacity =
    ((int)BP_PAGE_DATA_SIZE - MultiLeafIndexNode::HEADER_SIZE) / item_size;
  return capacity;
}

/////////////////////////////////////////////////////////////////////////////////
MultiIndexNodeHandler::MultiIndexNodeHandler(const MultiIndexFileHeader &header, Frame *frame)
  : header_(header), page_num_(frame->page_num()), node_((MultiIndexNode *)frame->data())
{}

bool MultiIndexNodeHandler::is_leaf() const
{
  return node_->is_leaf;
}
void MultiIndexNodeHandler::init_empty(bool leaf)
{
  node_->is_leaf = leaf;
  node_->key_num = 0;
  node_->parent = BP_INVALID_PAGE_NUM;
}
PageNum MultiIndexNodeHandler::page_num() const
{
  return page_num_;
}

int MultiIndexNodeHandler::key_size() const
{
  return header_.key_length;
}

int MultiIndexNodeHandler::value_size() const
{
  // return header_.value_size;
  return sizeof(RID);
}

int MultiIndexNodeHandler::item_size() const
{
  return key_size() + value_size();
}

int MultiIndexNodeHandler::size() const
{
  return node_->key_num;
}

void MultiIndexNodeHandler::increase_size(int n)
{
  node_->key_num += n;
}

PageNum MultiIndexNodeHandler::parent_page_num() const
{
  return node_->parent;
}

void MultiIndexNodeHandler::set_parent_page_num(PageNum page_num)
{
  this->node_->parent = page_num;
}
std::string to_string(const MultiIndexNodeHandler &handler)
{
  std::stringstream ss;

  ss << "PageNum:" << handler.page_num()
     << ",is_leaf:" << handler.is_leaf() << ","
     << "key_num:" << handler.size() << ","
     << "parent:" << handler.parent_page_num() << ",";

  return ss.str();
}

bool MultiIndexNodeHandler::validate() const
{
  if (parent_page_num() == BP_INVALID_PAGE_NUM) {
    // this is a root page
    if (size() < 1) {
      LOG_WARN("root page has no item");
      return false;
    }

    if (!is_leaf() && size() < 2) {
      LOG_WARN("root page internal node has less than 2 child. size=%d", size());
      return false;
    }
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////
MultiLeafIndexNodeHandler::MultiLeafIndexNodeHandler(const MultiIndexFileHeader &header, Frame *frame)
  : MultiIndexNodeHandler(header, frame), leaf_node_((MultiLeafIndexNode *)frame->data())
{}

void MultiLeafIndexNodeHandler::init_empty()
{
  MultiIndexNodeHandler::init_empty(true);
  leaf_node_->prev_brother = BP_INVALID_PAGE_NUM;
  leaf_node_->next_brother = BP_INVALID_PAGE_NUM;
}

void MultiLeafIndexNodeHandler::set_next_page(PageNum page_num)
{
  leaf_node_->next_brother = page_num;
}

void MultiLeafIndexNodeHandler::set_prev_page(PageNum page_num)
{
  leaf_node_->prev_brother = page_num;
}
PageNum MultiLeafIndexNodeHandler::next_page() const
{
  return leaf_node_->next_brother;
}
PageNum MultiLeafIndexNodeHandler::prev_page() const
{
  return leaf_node_->prev_brother;
}

char *MultiLeafIndexNodeHandler::key_at(int index)
{
  assert(index >= 0 && index < size());
  return __key_at(index);
}

char *MultiLeafIndexNodeHandler::value_at(int index)
{
  assert(index >= 0 && index < size());
  return __value_at(index);
}

int MultiLeafIndexNodeHandler::max_size() const
{
  return header_.leaf_max_size;
}

int MultiLeafIndexNodeHandler::min_size() const
{
  return header_.leaf_max_size - header_.leaf_max_size / 2;
}

int MultiLeafIndexNodeHandler::lookup(const MultiKeyComparator &comparator, const char *key, bool *found /* = nullptr */) const
{
  const int size = this->size();
  common::BinaryIterator<char> iter_begin(item_size(), __key_at(0));
  common::BinaryIterator<char> iter_end(item_size(), __key_at(size));
  common::BinaryIterator<char> iter = lower_bound(iter_begin, iter_end, key, comparator, found);
  return iter - iter_begin;
}

void MultiLeafIndexNodeHandler::insert(int index, const char *key, const char *value)
{
  if (index < size()) {
    memmove(__item_at(index + 1), __item_at(index), (size() - index) * item_size());
  }
  memcpy(__item_at(index), key, key_size());
  memcpy(__item_at(index) + key_size(), value, value_size());
  increase_size(1);
}
void MultiLeafIndexNodeHandler::remove(int index)
{
  assert(index >= 0 && index < size());
  if (index < size() - 1) {
    memmove(__item_at(index), __item_at(index + 1), (size() - index - 1) * item_size());
  }
  increase_size(-1);
}

int MultiLeafIndexNodeHandler::remove(const char *key, const MultiKeyComparator &comparator)
{
  bool found = false;
  int index = lookup(comparator, key, &found);
  if (found) {
    this->remove(index);
    return 1;
  }
  return 0;
}

RC MultiLeafIndexNodeHandler::move_half_to(MultiLeafIndexNodeHandler &other, DiskBufferPool *bp)
{
  const int size = this->size();
  const int move_index = size / 2;

  memcpy(other.__item_at(0), this->__item_at(move_index), item_size() * (size - move_index));
  other.increase_size(size - move_index);
  this->increase_size(- ( size - move_index));
  return RC::SUCCESS;
}
RC MultiLeafIndexNodeHandler::move_first_to_end(MultiLeafIndexNodeHandler &other, DiskBufferPool *disk_buffer_pool)
{
  other.append(__item_at(0));

  if (size() >= 1) {
    memmove(__item_at(0), __item_at(1), (size() - 1) * item_size() );
  }
  increase_size(-1);
  return RC::SUCCESS;
}

RC MultiLeafIndexNodeHandler::move_last_to_front(MultiLeafIndexNodeHandler &other, DiskBufferPool *bp)
{
  other.preappend(__item_at(size() - 1));

  increase_size(-1);
  return RC::SUCCESS;
}

/**
 * move all items to left page
 */
RC MultiLeafIndexNodeHandler::move_to(MultiLeafIndexNodeHandler &other, DiskBufferPool *bp)
{
  memcpy(other.__item_at(other.size()), this->__item_at(0), this->size() * item_size());
  other.increase_size(this->size());
  this->increase_size(- this->size());

  other.set_next_page(this->next_page());

  PageNum next_right_page_num = this->next_page();
  if (next_right_page_num != BP_INVALID_PAGE_NUM) {
    Frame *next_right_frame;
    RC rc = bp->get_this_page(next_right_page_num, &next_right_frame);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to fetch next right page. page number:%d. rc=%d:%s", next_right_page_num, rc, strrc(rc));
      return rc;
    }

    MultiLeafIndexNodeHandler next_right_node(header_, next_right_frame);
    next_right_node.set_prev_page(other.page_num());
    next_right_frame->mark_dirty();
    bp->unpin_page(next_right_frame);
  }
  return RC::SUCCESS;
}

void MultiLeafIndexNodeHandler::append(const char *item)
{
  memcpy(__item_at(size()), item, item_size());
  increase_size(1);
}

void MultiLeafIndexNodeHandler::preappend(const char *item)
{
  if (size() > 0) {
    memmove(__item_at(1), __item_at(0), size() * item_size());
  }
  memcpy(__item_at(0), item, item_size());
  increase_size(1);
}

char *MultiLeafIndexNodeHandler::__item_at(int index) const
{
  return leaf_node_->array + (index * item_size());
}
char *MultiLeafIndexNodeHandler::__key_at(int index) const
{
  return __item_at(index);
}
char *MultiLeafIndexNodeHandler::__value_at(int index) const
{
  return __item_at(index) + key_size();
}

std::string to_string(const MultiLeafIndexNodeHandler &handler, const MultiKeyPrinter &printer)
{
  std::stringstream ss;
  // ss << to_string((const MultiIndexNodeHandler &)handler)
  //    << ",prev page:" << handler.prev_page()
  //    << ",next page:" << handler.next_page();
  // ss << ",values=[" << printer(handler.__key_at(0)) ;
  // for (int i = 1; i < handler.size(); i++) {
  //   ss << "," << printer(handler.__key_at(i));
  // }
  ss << "]";
  return ss.str();
}


bool MultiLeafIndexNodeHandler::validate(const MultiKeyComparator &comparator, DiskBufferPool *bp) const
{
  bool result = MultiIndexNodeHandler::validate();
  if (false == result) {
    return false;
  }

  const int node_size = size();
  for (int i = 1; i < node_size; i++) {
    if (comparator(__key_at(i - 1), __key_at(i)) >= 0) {
      LOG_WARN("page number = %d, invalid key order. id1=%d,id2=%d, this=%s",
	       page_num(), i-1, i, to_string(*this).c_str());
      return false;
    }
  }

  PageNum parent_page_num = this->parent_page_num();
  if (parent_page_num == BP_INVALID_PAGE_NUM) {
    return true;
  }

  Frame *parent_frame;
  RC rc = bp->get_this_page(parent_page_num, &parent_frame);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to fetch parent page. page num=%d, rc=%d:%s",
	     parent_page_num, rc, strrc(rc));
    return false;
  }

  MultiInternalIndexNodeHandler parent_node(header_, parent_frame);
  int index_in_parent = parent_node.value_index(this->page_num());
  if (index_in_parent < 0) {
    LOG_WARN("invalid leaf node. cannot find index in parent. this page num=%d, parent page num=%d",
	     this->page_num(), parent_page_num);
    bp->unpin_page(parent_frame);
    return false;
  }

  if (0 != index_in_parent) {
    int cmp_result = comparator(__key_at(0), parent_node.key_at(index_in_parent));
    if (cmp_result < 0) {
      LOG_WARN("invalid leaf node. first item should be greate than or equal to parent item. " \
	       "this page num=%d, parent page num=%d, index in parent=%d",
	       this->page_num(), parent_node.page_num(), index_in_parent);
      bp->unpin_page(parent_frame);
      return false;
    }
  }

  if (index_in_parent < parent_node.size() - 1) {
    int cmp_result = comparator(__key_at(size() - 1), parent_node.key_at(index_in_parent + 1));
    if (cmp_result >= 0) {
      LOG_WARN("invalid leaf node. last item should be less than the item at the first after item in parent." \
	       "this page num=%d, parent page num=%d, parent item to compare=%d",
	       this->page_num(), parent_node.page_num(), index_in_parent + 1);
      bp->unpin_page(parent_frame);
      return false;
    }
  }
  bp->unpin_page(parent_frame);
  return true;
}


/////////////////////////////////////////////////////////////////////////////////
MultiInternalIndexNodeHandler::MultiInternalIndexNodeHandler(const MultiIndexFileHeader &header, Frame *frame)
  : MultiIndexNodeHandler(header, frame), internal_node_((MultiInternalIndexNode *)frame->data())
{}


std::string to_string(const MultiInternalIndexNodeHandler &node, const MultiKeyPrinter &printer)
{
  std::stringstream ss;
  ss << to_string((const MultiIndexNodeHandler &)node);
  // ss << ",children:["
  //    << "{key:" << printer(node.__key_at(0)) << ","
  //    << "value:" << *(PageNum *)node.__value_at(0) << "}";

  // for (int i = 1; i < node.size(); i++) {
  //   ss << ",{key:" << printer(node.__key_at(i))
  //      << ",value:"<< *(PageNum *)node.__value_at(i) << "}";
  // }
  ss << "]";
  return ss.str();
}

void MultiInternalIndexNodeHandler::init_empty()
{
  MultiIndexNodeHandler::init_empty(false);
}
void MultiInternalIndexNodeHandler::create_new_root(PageNum first_page_num, const char *key, PageNum page_num)
{
  memset(__key_at(0), 0, key_size());
  memcpy(__value_at(0), &first_page_num, value_size());
  memcpy(__item_at(1), key, key_size());
  memcpy(__value_at(1), &page_num, value_size());
  increase_size(2);
}

/**
 * insert one entry
 * the entry to be inserted will never at the first slot.
 * the right child page after split will always have bigger keys.
 */
void MultiInternalIndexNodeHandler::insert(const char *key, PageNum page_num, const MultiKeyComparator &comparator)
{
  int insert_position = -1;
  lookup(comparator, key, nullptr, &insert_position);
  if (insert_position < size()) {
    memmove(__item_at(insert_position + 1), __item_at(insert_position), (size() - insert_position) * item_size());
  }
  memcpy(__item_at(insert_position), key, key_size());
  memcpy(__value_at(insert_position), &page_num, value_size());
  increase_size(1);
}

RC MultiInternalIndexNodeHandler::move_half_to(MultiInternalIndexNodeHandler &other, DiskBufferPool *bp)
{
  const int size = this->size();
  const int move_index = size / 2;
  RC rc = other.copy_from(this->__item_at(move_index), size - move_index, bp);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to copy item to new node. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  increase_size(- (size - move_index));
  return rc;
}

int MultiInternalIndexNodeHandler::max_size() const
{
  return header_.internal_max_size;
}

int MultiInternalIndexNodeHandler::min_size() const
{
  return header_.internal_max_size - header_.internal_max_size / 2;
}

/**
 * lookup the first item which key <= item
 * @return unlike the leafNode, the return value is not the insert position,
 * but only the index of child to find. 
 */
int MultiInternalIndexNodeHandler::lookup(const MultiKeyComparator &comparator, const char *key,
				     bool *found /* = nullptr */, int *insert_position /*= nullptr */) const
{
  const int size = this->size();
  if (size == 0) {
    if (insert_position) {
      *insert_position = 1;
    }
    if (found) {
      *found = false;
    }
    return 0;
  }

  common::BinaryIterator<char> iter_begin(item_size(), __key_at(1));
  common::BinaryIterator<char> iter_end(item_size(), __key_at(size));
  common::BinaryIterator<char> iter = lower_bound(iter_begin, iter_end, key, comparator, found);
  int ret = static_cast<int>(iter - iter_begin) + 1;
  if (insert_position) {
    *insert_position = ret;
  }

  if (ret >= size || comparator(key, __key_at(ret)) < 0) {
    return ret - 1;
  }
  return ret;
}

char *MultiInternalIndexNodeHandler::key_at(int index)
{
  assert(index >= 0 && index < size());
  return __key_at(index);
}

void MultiInternalIndexNodeHandler::set_key_at(int index, const char *key)
{
  assert(index >= 0 && index < size());
  memcpy(__key_at(index), key, key_size());
}

PageNum MultiInternalIndexNodeHandler::value_at(int index)
{
  assert(index >= 0 && index < size());
  return *(PageNum *)__value_at(index);
}

int MultiInternalIndexNodeHandler::value_index(PageNum page_num)
{
  for (int i = 0; i < size(); i++) {
    if (page_num == *(PageNum*)__value_at(i)) {
      return i;
    }
  }
  return -1;
}

void MultiInternalIndexNodeHandler::remove(int index)
{
  assert(index >= 0 && index < size());
  if (index < size() - 1) {
    memmove(__item_at(index), __item_at(index + 1), (size() - index - 1) * item_size());
  }
  increase_size(-1);
}

RC MultiInternalIndexNodeHandler::move_to(MultiInternalIndexNodeHandler &other, DiskBufferPool *disk_buffer_pool)
{
  RC rc = other.copy_from(__item_at(0), size(), disk_buffer_pool);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to copy items to other node. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  increase_size(- this->size());
  return RC::SUCCESS;
}

RC MultiInternalIndexNodeHandler::move_first_to_end(MultiInternalIndexNodeHandler &other, DiskBufferPool *disk_buffer_pool)
{
  RC rc = other.append(__item_at(0), disk_buffer_pool);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to append item to others.");
    return rc;
  }

  if (size() >= 1) {
    memmove(__item_at(0), __item_at(1), (size() - 1) * item_size() );
  }
  increase_size(-1);
  return rc;
}

RC MultiInternalIndexNodeHandler::move_last_to_front(MultiInternalIndexNodeHandler &other, DiskBufferPool *bp)
{
  RC rc = other.preappend(__item_at(size() - 1), bp);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to preappend to others");
    return rc;
  }

  increase_size(-1);
  return rc;
}
/**
 * copy items from other node to self's right
 */
RC MultiInternalIndexNodeHandler::copy_from(const char *items, int num, DiskBufferPool *disk_buffer_pool)
{
  memcpy(__item_at(this->size()), items, num * item_size());

  RC rc = RC::SUCCESS;
  PageNum this_page_num = this->page_num();
  Frame *frame = nullptr;
  for (int i = 0; i < num; i++) {
    const PageNum page_num = *(const PageNum *)((items + i * item_size()) + key_size());
    rc = disk_buffer_pool->get_this_page(page_num, &frame);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to set child's page num. child page num:%d, this page num=%d, rc=%d:%s",
	       page_num, this_page_num, rc, strrc(rc));
      return rc;
    }
    MultiIndexNodeHandler child_node(header_, frame);
    child_node.set_parent_page_num(this_page_num);
    frame->mark_dirty();
    disk_buffer_pool->unpin_page(frame);
  }
  increase_size(num);
  return rc;
}

RC MultiInternalIndexNodeHandler::append(const char *item, DiskBufferPool *bp)
{
  return this->copy_from(item, 1, bp);
}

RC MultiInternalIndexNodeHandler::preappend(const char *item, DiskBufferPool *bp)
{
  PageNum child_page_num = *(PageNum *)(item + key_size());
  Frame *frame = nullptr;
  RC rc = bp->get_this_page(child_page_num, &frame);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to fetch child page. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  MultiIndexNodeHandler child_node(header_, frame);
  child_node.set_parent_page_num(this->page_num());

  frame->mark_dirty();
  bp->unpin_page(frame);

  if (this->size() > 0) {
    memmove(__item_at(1), __item_at(0), this->size() * item_size());
  }

  memcpy(__item_at(0), item, item_size());
  increase_size(1);
  return RC::SUCCESS;
}

char *MultiInternalIndexNodeHandler::__item_at(int index) const
{
  return internal_node_->array + (index * item_size());
}

char *MultiInternalIndexNodeHandler::__key_at(int index) const
{
  return __item_at(index);
}

char *MultiInternalIndexNodeHandler::__value_at(int index) const
{
  return __item_at(index) + key_size();
}

int MultiInternalIndexNodeHandler::value_size() const
{
  return sizeof(PageNum);
}

int MultiInternalIndexNodeHandler::item_size() const
{
  return key_size() + this->value_size();
}

bool MultiInternalIndexNodeHandler::validate(const MultiKeyComparator &comparator, DiskBufferPool *bp) const
{
  bool result = MultiIndexNodeHandler::validate();
  if (false == result) {
    return false;
  }

  const int node_size = size();
  for (int i = 2; i < node_size; i++) {
    if (comparator(__key_at(i - 1), __key_at(i)) >= 0) {
      LOG_WARN("page number = %d, invalid key order. id1=%d,id2=%d, this=%s",
	       page_num(), i-1, i, to_string(*this).c_str());
      return false;
    }
  }

  for (int i = 0; result && i < node_size; i++) {
    PageNum page_num = *(PageNum *)__value_at(i);
    if (page_num < 0) {
      LOG_WARN("this page num=%d, got invalid child page. page num=%d", this->page_num(), page_num);
    } else {
      Frame *child_frame;
      RC rc = bp->get_this_page(page_num, &child_frame);
      if (rc != RC::SUCCESS) {
	LOG_WARN("failed to fetch child page while validate internal page. page num=%d, rc=%d:%s",
		 page_num, rc, strrc(rc));
      } else {
	MultiIndexNodeHandler child_node(header_, child_frame);
	if (child_node.parent_page_num() != this->page_num()) {
	  LOG_WARN("child's parent page num is invalid. child page num=%d, parent page num=%d, this page num=%d",
		   child_node.page_num(), child_node.parent_page_num(), this->page_num());
	  result = false;
	}
	bp->unpin_page(child_frame);
      }
    }
  }

  if (!result) {
    return result;
  }

  const PageNum parent_page_num = this->parent_page_num();
  if (parent_page_num == BP_INVALID_PAGE_NUM) {
    return result;
  }

  Frame *parent_frame;
  RC rc = bp->get_this_page(parent_page_num, &parent_frame);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to fetch parent page. page num=%d, rc=%d:%s", parent_page_num, rc, strrc(rc));
    return false;
  }

  MultiInternalIndexNodeHandler parent_node(header_, parent_frame);
  int index_in_parent = parent_node.value_index(this->page_num());
  if (index_in_parent < 0) {
    LOG_WARN("invalid internal node. cannot find index in parent. this page num=%d, parent page num=%d",
	     this->page_num(), parent_page_num);
    bp->unpin_page(parent_frame);
    return false;
  }

  if (0 != index_in_parent) {
    int cmp_result = comparator(__key_at(1), parent_node.key_at(index_in_parent));
    if (cmp_result < 0) {
      LOG_WARN("invalid internal node. the second item should be greate than or equal to parent item. " \
	       "this page num=%d, parent page num=%d, index in parent=%d",
	       this->page_num(), parent_node.page_num(), index_in_parent);
      bp->unpin_page(parent_frame);
      return false;
    }
  }

  if (index_in_parent < parent_node.size() - 1) {
    int cmp_result = comparator(__key_at(size() - 1), parent_node.key_at(index_in_parent + 1));
    if (cmp_result >= 0) {
      LOG_WARN("invalid internal node. last item should be less than the item at the first after item in parent." \
	       "this page num=%d, parent page num=%d, parent item to compare=%d",
	       this->page_num(), parent_node.page_num(), index_in_parent + 1);
      bp->unpin_page(parent_frame);
      return false;
    }
  }
  bp->unpin_page(parent_frame);

  return result;
}

/////////////////////////////////////////////////////////////////////////////////

RC BplusTreeMultiHandler::sync()
{
  return disk_buffer_pool_->flush_all_pages();
}

// 创建 B+Tree 对应的文件
// 并记录index 对应的多列属性类型、属性值长度
// 确定树中内部节点和叶子节点的索引项数量
// 并初始化 key 的比较器和打印器
RC BplusTreeMultiHandler::create(const char *file_name, std::vector<AttrType> attrs, std::vector<int> lens,
    int internal_max_size /* = -1*/, int leaf_max_size /* = -1 */)
{
  // TODO(multi-index)

  // 创建 B+Tree 对应的文件
  BufferPoolManager &bpm = BufferPoolManager::instance();
  RC rc = bpm.create_file(file_name);
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to create file. file name=%s, rc=%d:%s", file_name, rc, strrc(rc));
    return rc;
  }
  LOG_INFO("Successfully create index file:%s", file_name);

  // 得到 B+Tree 文件对应的缓冲池
  DiskBufferPool *bp = nullptr;
  rc = bpm.open_file(file_name, bp);
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to open file. file name=%s, rc=%d:%s", file_name, rc, strrc(rc));
    return rc;
  }
  LOG_INFO("Successfully open index file %s.", file_name);

  // 获得 Page 中的第一个 Frame，用于记录文件头信息
  Frame *header_frame;
  rc = bp->allocate_page(&header_frame);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to allocate header page for bplus tree. rc=%d:%s", rc, strrc(rc));
    bpm.close_file(file_name);
    return rc;
  }
  if (header_frame->page_num() != FIRST_INDEX_PAGE) {
    LOG_WARN("header page num should be %d but got %d. is it a new file : %s",
	     FIRST_INDEX_PAGE, header_frame->page_num(), file_name);
    bpm.close_file(file_name);
    return RC::INTERNAL;
  }

  // 计算并记录 B+Tree 内部节点和叶子节点可以容纳的索引项数量
  int attrs_length = 0;
  for (size_t i = 0; i < lens.size(); i++) {
    attrs_length += lens[i];
  }
  if (internal_max_size < 0) {
    internal_max_size = multi_index_calc_internal_page_capacity(attrs_length);
  }
  if (leaf_max_size < 0) {
    leaf_max_size = multi_index_calc_leaf_page_capacity(attrs_length);
  }

  // 填充文件头的 data 部分，用于记录 索引相关参数
  char *pdata = header_frame->data();
  MultiIndexFileHeader *file_header = (MultiIndexFileHeader *)pdata;
  file_header->internal_max_size = internal_max_size;
  file_header->leaf_max_size = leaf_max_size;
  file_header->root_page = BP_INVALID_PAGE_NUM;
    // 多列索引需要记录多列属性的类型，值长度，以及创建的树节点中 key 的长度
  file_header->attrs_type.swap(attrs);
  file_header->attrs_len.swap(lens);
  // file_header->attr_length = attrs_length;
  file_header->key_length = attrs_length + sizeof(RID);
  
  // 标记 frame 有脏数据
  header_frame->mark_dirty();
  // 私有变量记录 bufferpool
  disk_buffer_pool_ = bp;
  // 私有变量记录 file_header
  memcpy(&file_header_, pdata, sizeof(file_header_));

  header_dirty_ = false;
  bp->unpin_page(header_frame);

  // 新分配一个内存池数据项
  // 大小为 key_length
  mem_pool_item_ = new common::MemPoolItem(file_name);
  if (mem_pool_item_->init(file_header->key_length) < 0) {
    LOG_WARN("Failed to init memory pool for index %s", file_name);
    close();
    return RC::NOMEM;
  }

// 初始化 key_comparator_ ，用于进行 key 值的比较，完成在 B+Tree 中查找和插入操作
// 初始化 key_printer_，用于？ TODO(multi-index)
  key_comparator_.init(file_header->attrs_type, file_header->attrs_len);
  // key_printer_.init(file_header->attrs_type, file_header->attrs_len);
  LOG_INFO("Successfully create index %s", file_name);
  return RC::SUCCESS;
}

RC BplusTreeMultiHandler::open(const char *file_name){
  // disk_buffer_pool_ 非空，说明已经执行过 open 或 create
  if (disk_buffer_pool_ != nullptr) {
    LOG_WARN("%s has been opened before index.open.", file_name);
    return RC::RECORD_OPENNED;
  }

  // disk_buffer_pool_ 为空，打开文件
  BufferPoolManager &bpm = BufferPoolManager::instance();
  DiskBufferPool *disk_buffer_pool;
  RC rc = bpm.open_file(file_name, disk_buffer_pool);
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to open file name=%s, rc=%d:%s", file_name, rc, strrc(rc));
    return rc;
  }

  // 从文件中读取第一个 frame，此 frame 中记录的是文件头信息 header_frame
  Frame *frame;
  rc = disk_buffer_pool->get_this_page(FIRST_INDEX_PAGE, &frame);
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to get first page file name=%s, rc=%d:%s", file_name, rc, strrc(rc));
    bpm.close_file(file_name);
    return rc;
  }

  // header_frame->data 部分记录的是索引文件的特定头信息 MultiIndexFileHeader
  char *pdata = frame->data();
  memcpy(&file_header_, pdata, sizeof(MultiIndexFileHeader));
  header_dirty_ = false;
  disk_buffer_pool_ = disk_buffer_pool;

  // 初始化内存池的内存项
  mem_pool_item_ = new common::MemPoolItem(file_name);
  if (mem_pool_item_->init(file_header_.key_length) < 0) {
    LOG_WARN("Failed to init memory pool for index %s", file_name);
    close();
    return RC::NOMEM;
  }

  // close old page_handle
  disk_buffer_pool->unpin_page(frame);

  key_comparator_.init(file_header_.attrs_type, file_header_.attrs_len);
  // TODO(multi-index)
  // key_printer_.init(file_header_.attr_type, file_header_.attr_length);
  LOG_INFO("Successfully open index %s", file_name);
  return RC::SUCCESS;
}

RC BplusTreeMultiHandler::close()
{
  if (disk_buffer_pool_ != nullptr) {

    disk_buffer_pool_->close_file(); // TODO

    delete mem_pool_item_;
    mem_pool_item_ = nullptr;
  }

  disk_buffer_pool_ = nullptr;
  return RC::SUCCESS;
}

RC BplusTreeMultiHandler::print_leaf(Frame *frame)
{
  // MultiLeafIndexNodeHandler leaf_node(file_header_, frame);
  // LOG_INFO("leaf node: %s", to_string(leaf_node, key_printer_).c_str());
  // disk_buffer_pool_->unpin_page(frame);
  // TODO(multi-index)
  return RC::SUCCESS;
}

RC BplusTreeMultiHandler::print_internal_node_recursive(Frame *frame)
{
  // TODO(multi-index)
  return RC::SUCCESS;
}

RC BplusTreeMultiHandler::print_tree()
{
  // TODO(multi-index)
  return RC::SUCCESS;
}

RC BplusTreeMultiHandler::print_leafs()
{
  // TODO(multi-index)
  return RC::SUCCESS;
}

bool BplusTreeMultiHandler::validate_node_recursive(Frame *frame)
{  
  // TODO(multi-index)
  return true;
}

bool BplusTreeMultiHandler::validate_leaf_link()
{
// TODO(multi-index)
  return true;
}

bool BplusTreeMultiHandler::validate_tree()
{
// TODO(multi-index)
  return true;
}

bool BplusTreeMultiHandler::is_empty() const
{
  return file_header_.root_page == BP_INVALID_PAGE_NUM;
}

RC BplusTreeMultiHandler::find_leaf(const char *key, Frame *&frame)
{
  return find_leaf_internal(
			    [&](MultiInternalIndexNodeHandler &internal_node) {
			      return internal_node.value_at(internal_node.lookup(key_comparator_, key));// 传入key 比较器和key 本身，从内部节点中找到不小于 key 的key值，并返回该值存在于节点的编号
			    },
			    frame);
  // // TODO(multi-index)
  // return RC::SUCCESS;
}

RC BplusTreeMultiHandler::left_most_page(Frame *&frame)
{
  // TODO(multi-index)
  return RC::SUCCESS;
}

RC BplusTreeMultiHandler::right_most_page(Frame *&frame)
{
  return find_leaf_internal(
			    [&](MultiInternalIndexNodeHandler &internal_node) {
			      return internal_node.value_at(internal_node.size() - 1);
			    },
			    frame
			    );
}

RC BplusTreeMultiHandler::find_leaf_internal(const std::function<PageNum(MultiInternalIndexNodeHandler &)> &child_page_getter,
					Frame *&frame)
{
  if (is_empty()) {
    return RC::EMPTY;
  }

  RC rc = disk_buffer_pool_->get_this_page(file_header_.root_page, &frame);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to fetch root page. page id=%d, rc=%d:%s", file_header_.root_page, rc, strrc(rc));
    return rc;
  }

  MultiIndexNode *node = (MultiIndexNode *)frame->data();
  while (false == node->is_leaf) {// 从根节点开始循环遍历，检查当前节点是否为叶子节点，若不是则继续遍历其子节点
    MultiInternalIndexNodeHandler internal_node(file_header_, frame);// 当前节点为内部节点
    // 在当前内部节点中，
    // 遍历找到不小于 find_leaf(key, ...) 传入 key 值的 key，
    // 并得到该 key 所对应的value，
    // 也即下一层被查找子节点的page_num
    PageNum page_num = child_page_getter(internal_node);

    disk_buffer_pool_->unpin_page(frame);

    rc = disk_buffer_pool_->get_this_page(page_num, &frame);// 再根据子节点的 page_num 读取到子节点
    if (rc != RC::SUCCESS) {
      LOG_WARN("Failed to load page page_num:%d", page_num);
      return rc;
    }
    node = (MultiIndexNode *)frame->data();// 并将子节点作为当前节点，返回到 while 循环开始处再次检查是否查找到了叶子结点
  }

  return RC::SUCCESS;
}
RC BplusTreeMultiHandler::check_entry_duplication(Frame *frame, const char *key, const RID *rid, const int is_unique)
{
  if (!is_unique) {  // 索引不是唯一约束
    return RC::SUCCESS;
  }
  // 索引是唯一约束，比较叶子节点中 rid 之前的 key 部分是否重复
  MultiLeafIndexNodeHandler leaf_node(file_header_, frame);
  bool exists = false;
  key_comparator_.ignore_rid();
  leaf_node.lookup(key_comparator_, key, &exists);
  key_comparator_.ignore_rid_reset();
  if (exists) {
    LOG_TRACE("unique key already exists");
    return RC::RECORD_DUPLICATE_KEY;
  }
  return RC::SUCCESS;
}
RC BplusTreeMultiHandler::insert_entry_into_leaf_node(Frame *frame, const char *key, const RID *rid,const int is_unique)
{
  RC rc1 = check_entry_duplication(frame, key, rid, is_unique);
  if (rc1 != RC::SUCCESS) {
    LOG_TRACE("entry exists");
    return RC::RECORD_DUPLICATE_KEY;
  }
  rc1 = insert_entry_into_leaf_node(frame, key, rid);
  return rc1;
}
RC BplusTreeMultiHandler::insert_entry_into_leaf_node(Frame *frame, const char *key, const RID *rid)
{

  MultiLeafIndexNodeHandler leaf_node(file_header_, frame);
  bool exists = false;
  int insert_position = leaf_node.lookup(key_comparator_, key, &exists);
  if (exists) {
    LOG_TRACE("entry exists");
    return RC::RECORD_DUPLICATE_KEY;
  }

  if (leaf_node.size() < leaf_node.max_size()) {
    leaf_node.insert(insert_position, key, (const char *)rid);
    frame->mark_dirty();
    disk_buffer_pool_->unpin_page(frame);
    return RC::SUCCESS;
  }

  Frame * new_frame = nullptr;
  RC rc = split<MultiLeafIndexNodeHandler>(frame, new_frame);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to split leaf node. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  MultiLeafIndexNodeHandler new_index_node(file_header_, new_frame);
  new_index_node.set_prev_page(frame->page_num());
  new_index_node.set_next_page(leaf_node.next_page());
  new_index_node.set_parent_page_num(leaf_node.parent_page_num());
  leaf_node.set_next_page(new_frame->page_num());

  PageNum next_page_num = new_index_node.next_page();
  if (next_page_num != BP_INVALID_PAGE_NUM) {
    Frame * next_frame;
    rc = disk_buffer_pool_->get_this_page(next_page_num, &next_frame);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to fetch next page. page num=%d, rc=%d:%s", next_page_num, rc, strrc(rc));
      return rc;
    }

    MultiLeafIndexNodeHandler next_node(file_header_, next_frame);
    next_node.set_prev_page(new_frame->page_num());
    disk_buffer_pool_->unpin_page(next_frame);
  }

  if (insert_position < leaf_node.size()) {
    leaf_node.insert(insert_position, key, (const char *)rid);
  } else {
    new_index_node.insert(insert_position - leaf_node.size(), key, (const char *)rid);
  }

  return insert_entry_into_parent(frame, new_frame, new_index_node.key_at(0));

}

RC BplusTreeMultiHandler::insert_entry_into_parent(Frame *frame, Frame *new_frame, const char *key)
{
  RC rc = RC::SUCCESS;

  MultiIndexNodeHandler node_handler(file_header_, frame);
  MultiIndexNodeHandler new_node_handler(file_header_, new_frame);
  PageNum parent_page_num = node_handler.parent_page_num();

  if (parent_page_num == BP_INVALID_PAGE_NUM) {

    // create new root page
    Frame *root_frame;
    rc = disk_buffer_pool_->allocate_page(&root_frame);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to allocate new root page. rc=%d:%s", rc, strrc(rc));
      return rc;
    }

    MultiInternalIndexNodeHandler root_node(file_header_, root_frame);
    root_node.init_empty();
    root_node.create_new_root(frame->page_num(), key, new_frame->page_num());
    node_handler.set_parent_page_num(root_frame->page_num());
    new_node_handler.set_parent_page_num(root_frame->page_num());

    frame->mark_dirty();
    new_frame->mark_dirty();
    disk_buffer_pool_->unpin_page(frame);
    disk_buffer_pool_->unpin_page(new_frame);

    file_header_.root_page = root_frame->page_num();
    update_root_page_num(); // TODO
    root_frame->mark_dirty();
    disk_buffer_pool_->unpin_page(root_frame);

    return RC::SUCCESS;

  } else {

    Frame *parent_frame;
    rc = disk_buffer_pool_->get_this_page(parent_page_num, &parent_frame);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to insert entry into leaf. rc=%d:%s", rc, strrc(rc));
      // should do more things to recover
      return rc;
    }

    MultiInternalIndexNodeHandler node(file_header_, parent_frame);

    /// current node is not in full mode, insert the entry and return
    if (node.size() < node.max_size()) {
      node.insert(key, new_frame->page_num(), key_comparator_);
      new_node_handler.set_parent_page_num(parent_page_num);

      frame->mark_dirty();
      new_frame->mark_dirty();
      parent_frame->mark_dirty();
      disk_buffer_pool_->unpin_page(frame);
      disk_buffer_pool_->unpin_page(new_frame);
      disk_buffer_pool_->unpin_page(parent_frame);

    } else {

      // we should split the node and insert the entry and then insert new entry to current node's parent
      Frame *new_parent_frame;
      rc = split<MultiInternalIndexNodeHandler>(parent_frame, new_parent_frame);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to split internal node. rc=%d:%s", rc, strrc(rc));
        disk_buffer_pool_->unpin_page(frame);
        disk_buffer_pool_->unpin_page(new_frame);
        disk_buffer_pool_->unpin_page(parent_frame);
      } else {
        // insert into left or right ? decide by key compare result
        MultiInternalIndexNodeHandler new_node(file_header_, new_parent_frame);
        if (key_comparator_(key, new_node.key_at(0)) > 0) {
          new_node.insert(key, new_frame->page_num(), key_comparator_);
          new_node_handler.set_parent_page_num(new_node.page_num());
        } else {
          node.insert(key, new_frame->page_num(), key_comparator_);
          new_node_handler.set_parent_page_num(node.page_num());
        }

        disk_buffer_pool_->unpin_page(frame);
        disk_buffer_pool_->unpin_page(new_frame);

        rc = insert_entry_into_parent(parent_frame, new_parent_frame, new_node.key_at(0));
      }
    }
  }
  return rc;
}

/**
 * split one full node into two
 * @param page_handle[inout] the node to split
 * @param new_page_handle[out] the new node after split
 * @param intert_position the intert position of new key
 */
template <typename IndexNodeHandlerType>
RC BplusTreeMultiHandler::split(Frame *frame, Frame *&new_frame)
{
  IndexNodeHandlerType old_node(file_header_, frame);

  // add a new node
  RC rc = disk_buffer_pool_->allocate_page(&new_frame);
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to split index page due to failed to allocate page, rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  IndexNodeHandlerType new_node(file_header_, new_frame);
  new_node.init_empty();
  new_node.set_parent_page_num(old_node.parent_page_num());

  old_node.move_half_to(new_node, disk_buffer_pool_);

  frame->mark_dirty();
  new_frame->mark_dirty();
  return RC::SUCCESS;
}

// .index 文件的 header 部分记录了 B+Tree 的root_page
// 更新为当前 file_header_ 所记录的 B+Tree 的 root_page
RC BplusTreeMultiHandler::update_root_page_num()
{
  Frame * header_frame;
  RC rc = disk_buffer_pool_->get_this_page(FIRST_INDEX_PAGE, &header_frame);// 读取.index 文件的首个 page
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to fetch header page. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  MultiIndexFileHeader *header = (MultiIndexFileHeader *)header_frame->data();// 获得.index 文件头的数据部分
  header->root_page = file_header_.root_page;// 文件头的数据部分记录了 B+Tree 的 root_page，更新为新创建的 B+Tree 的 root_page
  header_frame->mark_dirty();
  disk_buffer_pool_->unpin_page(header_frame);
  return rc;
}



// 创建一棵新的 B+ Tree
// 从 disk_buffer_pool_ 对应的文件中获得空闲页 page，并将页加载到内存 frame
// 创建 B+Tree 的叶子节点，并将 key,rid 存入叶子节点
// 并更新 .index 文件 header 部分记录的 B+Tree 的 root_page
RC BplusTreeMultiHandler::create_new_tree(const char *key, const RID *rid)
{
  RC rc = RC::SUCCESS;
  if (file_header_.root_page != BP_INVALID_PAGE_NUM) {
    rc = RC::INTERNAL;
    LOG_WARN("cannot create new tree while root page is valid. root page id=%d", file_header_.root_page);
    return rc;
  }

  Frame *frame;
  rc = disk_buffer_pool_->allocate_page(&frame);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to allocate root page. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  MultiLeafIndexNodeHandler leaf_node(file_header_, frame);
  leaf_node.init_empty();
  leaf_node.insert(0, key, (const char *)rid);  // 将 key,rid 存入叶子节点中 key0,rid0 位置
  file_header_.root_page = frame->page_num();  // 当前 frame 对应的 page_num 被即为 root_page
  frame->mark_dirty();                       // 标记frame 写入了数据需要后续刷盘
  disk_buffer_pool_->unpin_page(frame);

  rc = update_root_page_num(); // 将.index 文件的 header 部分记录的 root_page 更新为 file_header_.root_page 
  // disk_buffer_pool_->check_all_pages_unpinned(file_id_);
  return rc;
}

// 从 file_header_ 中取得多列属性的所有长度，得到 user_key 的长度
char *BplusTreeMultiHandler::make_key(const char *user_key, const RID &rid)
{
  char *key = (char *)mem_pool_item_->alloc();
  if (key == nullptr) {
    LOG_WARN("Failed to alloc memory for key.");
    return nullptr;
  }

  int32_t attr_length = 0;
  for (size_t i = 0; i < file_header_.attrs_len.size(); i++) {
    attr_length += file_header_.attrs_len[i];
  }

  memcpy(key, user_key, attr_length);
  memcpy(key + attr_length, &rid, sizeof(rid));
  return key;
}

void BplusTreeMultiHandler::free_key(char *key)
{
  mem_pool_item_->free(key);
}

// 接收的 user_key 参数指向的字符是创建索引的多列值的全部值按序拼接
RC BplusTreeMultiHandler::insert_entry(const char *user_key, const RID *rid)
{
  if (user_key == nullptr || rid == nullptr) {
    LOG_WARN("Invalid arguments, key is empty or rid is empty");
    return RC::INVALID_ARGUMENT;
  }

  char *key = make_key(user_key, *rid);
  if (key == nullptr) {
    LOG_WARN("Failed to alloc memory for key.");
    return RC::NOMEM;
  }

  if (is_empty()) {// 若 file_header_.root_page 没有记录合法 page_num，则认为 B+Tree 不存在
    RC rc = create_new_tree(key, rid);// 创建 B+Tree 并将 key,rid 存入树中唯一叶子节点
    mem_pool_item_->free(key);
    return rc;
  }
// 若已经存在一棵 B+Tree
  Frame *frame;
  RC rc = find_leaf(key, frame);// 找到 key 可能存在的叶子节点
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to find leaf %s. rc=%d:%s", rid->to_string().c_str(), rc, strrc(rc));
    mem_pool_item_->free(key);
    return rc;
  }

  rc = insert_entry_into_leaf_node(frame, key, rid);// 将 key 插入该叶子节点中，插入时可能存在节点分裂等情况
  if (rc != RC::SUCCESS) {
    LOG_TRACE("Failed to insert into leaf of index, rid:%s", rid->to_string().c_str());
    disk_buffer_pool_->unpin_page(frame);
    mem_pool_item_->free(key);
    // disk_buffer_pool_->check_all_pages_unpinned(file_id_);
    return rc;
  }

  mem_pool_item_->free(key);
  LOG_TRACE("insert entry success");
  // disk_buffer_pool_->check_all_pages_unpinned(file_id_);
  return RC::SUCCESS;
}

// 接收的 user_key 参数指向的字符是创建索引的多列值的全部值按序拼接
RC BplusTreeMultiHandler::insert_entry(const char *user_key, const RID *rid, const int is_unique)
{
  if (user_key == nullptr || rid == nullptr) {
    LOG_WARN("Invalid arguments, key is empty or rid is empty");
    return RC::INVALID_ARGUMENT;
  }

  char *key = make_key(user_key, *rid);
  if (key == nullptr) {
    LOG_WARN("Failed to alloc memory for key.");
    return RC::NOMEM;
  }

  if (is_empty()) {// 若 file_header_.root_page 没有记录合法 page_num，则认为 B+Tree 不存在
    RC rc = create_new_tree(key, rid);// 创建 B+Tree 并将 key,rid 存入树中唯一叶子节点
    mem_pool_item_->free(key);
    return rc;
  }
// 若已经存在一棵 B+Tree
  Frame *frame;
  RC rc = find_leaf(key, frame);// 找到 key 可能存在的叶子节点
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to find leaf %s. rc=%d:%s", rid->to_string().c_str(), rc, strrc(rc));
    mem_pool_item_->free(key);
    return rc;
  }

  rc =
      insert_entry_into_leaf_node(frame, key, rid, is_unique);  // 将 key 插入该叶子节点中，插入时可能存在节点分裂等情况
  if (rc != RC::SUCCESS) {
    LOG_TRACE("Failed to insert into leaf of index, rid:%s", rid->to_string().c_str());
    disk_buffer_pool_->unpin_page(frame);
    mem_pool_item_->free(key);
    // disk_buffer_pool_->check_all_pages_unpinned(file_id_);
    return rc;
  }

  mem_pool_item_->free(key);
  LOG_TRACE("insert entry success");
  // disk_buffer_pool_->check_all_pages_unpinned(file_id_);
  return RC::SUCCESS;
}

RC BplusTreeMultiHandler::get_entry(const char *user_key, int key_len, std::list<RID> &rids)
{
  BplusTreeMultiScanner scanner(*this);
  RC rc = scanner.open(user_key, key_len, true/*left_inclusive*/, user_key, key_len, true/*right_inclusive*/);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open scanner. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  RID rid;
  while ((rc = scanner.next_entry(&rid)) == RC::SUCCESS) {
    rids.push_back(rid);
  }

  scanner.close();
  if (rc != RC::RECORD_EOF) {
    LOG_WARN("scanner return error. rc=%d:%s", rc, strrc(rc));
  } else {
    rc = RC::SUCCESS;
  }
  return rc;
}

RC BplusTreeMultiHandler::adjust_root(Frame *root_frame)
{
  MultiIndexNodeHandler root_node(file_header_, root_frame);
  if (root_node.is_leaf() && root_node.size() > 0) {
    root_frame->mark_dirty();
    disk_buffer_pool_->unpin_page(root_frame);
    return RC::SUCCESS;
  }

  if (root_node.is_leaf()) {
    // this is a leaf and an empty node
    file_header_.root_page = BP_INVALID_PAGE_NUM;
  } else {
    // this is an internal node and has only one child node
    MultiInternalIndexNodeHandler internal_node(file_header_, root_frame);

    const PageNum child_page_num = internal_node.value_at(0);
    Frame * child_frame;
    RC rc = disk_buffer_pool_->get_this_page(child_page_num, &child_frame);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to fetch child page. page num=%d, rc=%d:%s", child_page_num, rc, strrc(rc));
      return rc;
    }

    MultiIndexNodeHandler child_node(file_header_, child_frame);
    child_node.set_parent_page_num(BP_INVALID_PAGE_NUM);
    disk_buffer_pool_->unpin_page(child_frame);
    
    file_header_.root_page = child_page_num;
  }

  update_root_page_num();

  PageNum old_root_page_num = root_frame->page_num();
  disk_buffer_pool_->unpin_page(root_frame);
  disk_buffer_pool_->dispose_page(old_root_page_num);
  return RC::SUCCESS;
}

template <typename IndexNodeHandlerType>
RC BplusTreeMultiHandler::coalesce_or_redistribute(Frame *frame)
{
  IndexNodeHandlerType index_node(file_header_, frame);
  if (index_node.size() >= index_node.min_size()) {
    disk_buffer_pool_->unpin_page(frame);
    return RC::SUCCESS;
  }

  const PageNum parent_page_num = index_node.parent_page_num();
  if (BP_INVALID_PAGE_NUM == parent_page_num) {
    // this is the root page
    if (index_node.size() > 1) {
      disk_buffer_pool_->unpin_page(frame);
    } else {
      // adjust the root node
      adjust_root(frame);
    }
    return RC::SUCCESS;
  }

  Frame * parent_frame;
  RC rc = disk_buffer_pool_->get_this_page(parent_page_num, &parent_frame);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to fetch parent page. page id=%d, rc=%d:%s", parent_page_num, rc, strrc(rc));
    disk_buffer_pool_->unpin_page(frame);
    return rc;
  }

  MultiInternalIndexNodeHandler parent_index_node(file_header_, parent_frame);
  int index = parent_index_node.lookup(key_comparator_, index_node.key_at(index_node.size() - 1));
  if (parent_index_node.value_at(index) != frame->page_num()) {
    LOG_ERROR("lookup return an invalid value. index=%d, this page num=%d, but got %d",
	      index, frame->page_num(), parent_index_node.value_at(index));
  }
  PageNum neighbor_page_num;
  if (index == 0) {
    neighbor_page_num = parent_index_node.value_at(1);
  } else {
    neighbor_page_num = parent_index_node.value_at(index - 1);
  }

  Frame * neighbor_frame;
  rc = disk_buffer_pool_->get_this_page(neighbor_page_num, &neighbor_frame);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to fetch neighbor page. page id=%d, rc=%d:%s", neighbor_page_num, rc, strrc(rc));
    // TODO do more thing to release resource
    disk_buffer_pool_->unpin_page(frame);
    disk_buffer_pool_->unpin_page(parent_frame);
    return rc;
  }

  IndexNodeHandlerType neighbor_node(file_header_, neighbor_frame);
  if (index_node.size() + neighbor_node.size() > index_node.max_size()) {
    rc = redistribute<IndexNodeHandlerType>(neighbor_frame, frame, parent_frame, index);
  } else {
    rc = coalesce<IndexNodeHandlerType>(neighbor_frame, frame, parent_frame, index);
  }
  return rc;
}



template <typename IndexNodeHandlerType>
RC BplusTreeMultiHandler::coalesce(Frame *neighbor_frame, Frame *frame, Frame *parent_frame, int index)
{
  IndexNodeHandlerType neighbor_node(file_header_, neighbor_frame);
  IndexNodeHandlerType node(file_header_, frame);

  MultiInternalIndexNodeHandler parent_node(file_header_, parent_frame);

  Frame *left_frame = nullptr;
  Frame *right_frame = nullptr;
  if (index == 0) {
    // neighbor node is at right
    left_frame  = frame;
    right_frame = neighbor_frame;
    index++;
  } else {
    left_frame  = neighbor_frame;
    right_frame = frame;
    // neighbor is at left
  }

  IndexNodeHandlerType left_node(file_header_, left_frame);
  IndexNodeHandlerType right_node(file_header_, right_frame);

  parent_node.remove(index);
  // parent_node.validate(key_comparator_, disk_buffer_pool_, file_id_);
  RC rc = right_node.move_to(left_node, disk_buffer_pool_);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to move right node to left. rc=%d:%s", rc, strrc(rc));
    return rc;
  }
  // left_node.validate(key_comparator_);

  if (left_node.is_leaf()) {
    MultiLeafIndexNodeHandler left_leaf_node(file_header_, left_frame);
    MultiLeafIndexNodeHandler right_leaf_node(file_header_, right_frame);
    left_leaf_node.set_next_page(right_leaf_node.next_page());

    PageNum next_right_page_num = right_leaf_node.next_page();
    if (next_right_page_num != BP_INVALID_PAGE_NUM) {
      Frame *next_right_frame;
      rc = disk_buffer_pool_->get_this_page(next_right_page_num, &next_right_frame);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to fetch next right page. page number:%d. rc=%d:%s", next_right_page_num, rc, strrc(rc));
        disk_buffer_pool_->unpin_page(frame);
        disk_buffer_pool_->unpin_page(neighbor_frame);
        disk_buffer_pool_->unpin_page(parent_frame);
        return rc;
      }

      MultiLeafIndexNodeHandler next_right_node(file_header_, next_right_frame);
      next_right_node.set_prev_page(left_node.page_num());
      disk_buffer_pool_->unpin_page(next_right_frame);
    }
  }

  PageNum right_page_num = right_frame->page_num();
  disk_buffer_pool_->unpin_page(left_frame);
  disk_buffer_pool_->unpin_page(right_frame);
  disk_buffer_pool_->dispose_page(right_page_num);
  return coalesce_or_redistribute<MultiInternalIndexNodeHandler>(parent_frame);
}


template <typename IndexNodeHandlerType>
RC BplusTreeMultiHandler::redistribute(Frame *neighbor_frame, Frame *frame, Frame *parent_frame, int index)
{
  MultiInternalIndexNodeHandler parent_node(file_header_, parent_frame);
  IndexNodeHandlerType neighbor_node(file_header_, neighbor_frame);
  IndexNodeHandlerType node(file_header_, frame);
  if (neighbor_node.size() < node.size()) {
    LOG_ERROR("got invalid nodes. neighbor node size %d, this node size %d",
	      neighbor_node.size(), node.size());
  }
  if (index == 0) {
    // the neighbor is at right
    neighbor_node.move_first_to_end(node, disk_buffer_pool_);
    // neighbor_node.validate(key_comparator_, disk_buffer_pool_, file_id_);
    // node.validate(key_comparator_, disk_buffer_pool_, file_id_);
    parent_node.set_key_at(index + 1, neighbor_node.key_at(0));
    // parent_node.validate(key_comparator_, disk_buffer_pool_, file_id_);
  } else {
    // the neighbor is at left
    neighbor_node.move_last_to_front(node, disk_buffer_pool_);
    // neighbor_node.validate(key_comparator_, disk_buffer_pool_, file_id_);
    // node.validate(key_comparator_, disk_buffer_pool_, file_id_);
    parent_node.set_key_at(index, node.key_at(0));
    // parent_node.validate(key_comparator_, disk_buffer_pool_, file_id_);
  }

  neighbor_frame->mark_dirty();
  frame->mark_dirty();
  parent_frame->mark_dirty();
  disk_buffer_pool_->unpin_page(parent_frame);
  disk_buffer_pool_->unpin_page(neighbor_frame);
  disk_buffer_pool_->unpin_page(frame);
  return RC::SUCCESS;
}


RC BplusTreeMultiHandler::delete_entry_internal(Frame *leaf_frame, const char *key)
{
  MultiLeafIndexNodeHandler leaf_index_node(file_header_, leaf_frame);

  const int remove_count = leaf_index_node.remove(key, key_comparator_);
  if (remove_count == 0) {
    LOG_TRACE("no data to remove");
    disk_buffer_pool_->unpin_page(leaf_frame);
    return RC::RECORD_RECORD_NOT_EXIST;
  }
  // leaf_index_node.validate(key_comparator_, disk_buffer_pool_, file_id_);

  leaf_frame->mark_dirty();

  if (leaf_index_node.size() >= leaf_index_node.min_size()) {
    disk_buffer_pool_->unpin_page(leaf_frame);
    return RC::SUCCESS;
  }

  return coalesce_or_redistribute<MultiLeafIndexNodeHandler>(leaf_frame);
}

RC BplusTreeMultiHandler::delete_entry(const char *user_key, const RID *rid)
{
  char *key = (char *)mem_pool_item_->alloc();
  if (nullptr == key) {
    LOG_WARN("Failed to alloc memory for key. size=%d", file_header_.key_length);
    return RC::NOMEM;
  }
  int attrs_length = 0;
  for (size_t i = 0; i < file_header_.attrs_len.size(); i++) {
    attrs_length += file_header_.attrs_len[i];
  }
  memcpy(key, user_key, attrs_length);
  memcpy(key + attrs_length, rid, sizeof(*rid));

  Frame *leaf_frame;
  RC rc = find_leaf(key, leaf_frame);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to find leaf page. rc =%d:%s", rc, strrc(rc));
    mem_pool_item_->free(key);
    return rc;
  }
  rc = delete_entry_internal(leaf_frame, key);
  if (rc != RC::SUCCESS) {
    LOG_WARN("Failed to delete index");
    mem_pool_item_->free(key);
    return rc;
  }
  mem_pool_item_->free(key);
  return RC::SUCCESS;
}


BplusTreeMultiScanner::BplusTreeMultiScanner(BplusTreeMultiHandler &tree_handler) : tree_handler_(tree_handler)
{}

BplusTreeMultiScanner::~BplusTreeMultiScanner()
{
  close();
}


RC BplusTreeMultiScanner::open(const char *left_user_key, int left_len, bool left_inclusive,
                          const char *right_user_key, int right_len, bool right_inclusive)
{
  RC rc = RC::SUCCESS;
  if (inited_) {
    LOG_WARN("tree scanner has been inited");
    return RC::INTERNAL;
  }

  inited_ = true;
  
  // 校验输入的键值是否是合法范围
  if (left_user_key && right_user_key) {
    const auto &attr_comparator = tree_handler_.key_comparator_.attrs_comparator();
    const int result = attr_comparator(left_user_key, right_user_key);
    if (result > 0 || // left < right
         // left == right but is (left,right)/[left,right) or (left,right]
	(result == 0 && (left_inclusive == false || right_inclusive == false))) { 
      return RC::INVALID_ARGUMENT;
    }
  }

  if (nullptr == left_user_key) {
    rc = tree_handler_.left_most_page(left_frame_);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to find left most page. rc=%d:%s", rc, strrc(rc));
      return rc;
    }

    iter_index_ = 0;
  } else {
    char *left_key = nullptr;

    char *fixed_left_key = const_cast<char *>(left_user_key);
    // TODO(multi-index)
    // if (tree_handler_.file_header_.attr_type == CHARS) {
    //   bool should_inclusive_after_fix = false;
    //   rc = fix_user_key(left_user_key, left_len, true/*greater*/, &fixed_left_key, &should_inclusive_after_fix);
    //   if (rc != RC::SUCCESS) {
    //     LOG_WARN("failed to fix left user key. rc=%s", strrc(rc));
    //     return rc;
    //   }

    //   if (should_inclusive_after_fix) {
	  //     left_inclusive = true;
    //   }
    // }

    if (left_inclusive) {
      left_key = tree_handler_.make_key(fixed_left_key, *RID::min());
    } else {
      left_key = tree_handler_.make_key(fixed_left_key, *RID::max());
    }

    if (fixed_left_key != left_user_key) {
      delete[] fixed_left_key;
      fixed_left_key = nullptr;
    }
    rc = tree_handler_.find_leaf(left_key, left_frame_);

    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to find left page. rc=%d:%s", rc, strrc(rc));
      tree_handler_.free_key(left_key);
      return rc;
    }
    MultiLeafIndexNodeHandler left_node(tree_handler_.file_header_, left_frame_);
    int left_index = left_node.lookup(tree_handler_.key_comparator_, left_key);
    tree_handler_.free_key(left_key);
    // lookup 返回的是适合插入的位置，还需要判断一下是否在合适的边界范围内
    if (left_index >= left_node.size()) { // 超出了当前页，就需要向后移动一个位置
      const PageNum next_page_num = left_node.next_page();
      if (next_page_num == BP_INVALID_PAGE_NUM) { // 这里已经是最后一页，说明当前扫描，没有数据
	      return RC::SUCCESS;
      }

      tree_handler_.disk_buffer_pool_->unpin_page(left_frame_);
      rc = tree_handler_.disk_buffer_pool_->get_this_page(next_page_num, &left_frame_);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to fetch next page. page num=%d, rc=%d:%s", next_page_num, rc, strrc(rc));
        return rc;
      }

      left_index = 0;
    }
    iter_index_ = left_index;
  }

  // 没有指定右边界范围，那么就返回右边界最大值
  if (nullptr == right_user_key) {
    rc = tree_handler_.right_most_page(right_frame_);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to fetch right most page. rc=%d:%s", rc, strrc(rc));
      return rc;
    }

    MultiLeafIndexNodeHandler node(tree_handler_.file_header_, right_frame_);
    end_index_ = node.size() - 1;
  } else {

    char *right_key = nullptr;
    char *fixed_right_key = const_cast<char *>(right_user_key);
    bool should_include_after_fix = false;
    // TODO(multi-index)
    // if (tree_handler_.file_header_.attr_type == CHARS) {
    //   rc = fix_user_key(right_user_key, right_len, false/*want_greater*/, &fixed_right_key, &should_include_after_fix);
    //   if (rc != RC::SUCCESS) {
    //     LOG_WARN("failed to fix right user key. rc=%s", strrc(rc));
    //     return rc;
    //   }

    //   if (should_include_after_fix) {
	  //     right_inclusive = true;
    //   }
    // }
    if (right_inclusive) {
      right_key = tree_handler_.make_key(fixed_right_key, *RID::max());
    } else {
      right_key = tree_handler_.make_key(fixed_right_key, *RID::min());
    }

    if (fixed_right_key != right_user_key) {
      delete[] fixed_right_key;
      fixed_right_key = nullptr;
    }

    rc = tree_handler_.find_leaf(right_key, right_frame_);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to find left page. rc=%d:%s", rc, strrc(rc));
      tree_handler_.free_key(right_key);
      return rc;
    }

    MultiLeafIndexNodeHandler right_node(tree_handler_.file_header_, right_frame_);
    int right_index = right_node.lookup(tree_handler_.key_comparator_, right_key);
    tree_handler_.free_key(right_key);
    // lookup 返回的是适合插入的位置，需要根据实际情况做调整
    // 通常情况下需要找到上一个位置
    if (right_index > 0) {
      right_index--;
    } else {
      // 实际上，只有最左边的叶子节点查找时，lookup 才可能返回0
      // 其它的叶子节点都不可能返回0，所以这段逻辑其实是可以简化的
      const PageNum prev_page_num = right_node.prev_page();
      if (prev_page_num == BP_INVALID_PAGE_NUM) {
        end_index_ = -1;
        return RC::SUCCESS;
      }

      tree_handler_.disk_buffer_pool_->unpin_page(right_frame_);
      rc = tree_handler_.disk_buffer_pool_->get_this_page(prev_page_num, &right_frame_);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to fetch prev page num. page num=%d, rc=%d:%s", prev_page_num, rc, strrc(rc));
        return rc;
      }

      MultiLeafIndexNodeHandler tmp_node(tree_handler_.file_header_, right_frame_);
      right_index = tmp_node.size() - 1;
    }
    end_index_ = right_index;
  }

  // 判断是否左边界比右边界要靠后
  // 两个边界最多会多一页
  // 查找不存在的元素，或者不存在的范围数据时，可能会存在这个问题
  if (left_frame_->page_num() == right_frame_->page_num() &&
      iter_index_ > end_index_) {
    end_index_ = -1;
  } else {
    MultiLeafIndexNodeHandler left_node(tree_handler_.file_header_, left_frame_);
    MultiLeafIndexNodeHandler right_node(tree_handler_.file_header_, right_frame_);
    if (left_node.prev_page() == right_node.page_num()) {
      end_index_ = -1;
    }
  }
  return RC::SUCCESS;
}



RC BplusTreeMultiScanner::next_entry(RID *rid)
{
  if (-1 == end_index_) {
    return RC::RECORD_EOF;
  }

  MultiLeafIndexNodeHandler node(tree_handler_.file_header_, left_frame_);
  memcpy(rid, node.value_at(iter_index_), sizeof(*rid));

  if (left_frame_->page_num() == right_frame_->page_num() &&
      iter_index_ == end_index_) {
    end_index_ = -1;
    return RC::SUCCESS;
  }

  if (iter_index_ < node.size() - 1) {
    ++iter_index_;
    return RC::SUCCESS;
  }

  RC rc = RC::SUCCESS;
  if (left_frame_->page_num() != right_frame_->page_num()) {
    PageNum page_num = node.next_page();
    tree_handler_.disk_buffer_pool_->unpin_page(left_frame_);
    if (page_num == BP_INVALID_PAGE_NUM) {
      left_frame_ = nullptr;
      LOG_WARN("got invalid next page. page num=%d", page_num);
      rc = RC::INTERNAL;
    } else {
      rc = tree_handler_.disk_buffer_pool_->get_this_page(page_num, &left_frame_);
      if (rc != RC::SUCCESS) {
	      left_frame_ = nullptr;
        LOG_WARN("failed to fetch next page. page num=%d, rc=%d:%s", page_num, rc, strrc(rc));
        return rc;
      }

      iter_index_ = 0;
    }
  } else if (end_index_ != -1) {
    LOG_WARN("should have more pages but not. left page=%d, right page=%d",
	     left_frame_->page_num(), right_frame_->page_num());
    rc = RC::INTERNAL;
  }
  return rc;
}

RC BplusTreeMultiScanner::close()
{
  if (left_frame_ != nullptr) {
    tree_handler_.disk_buffer_pool_->unpin_page(left_frame_);
    left_frame_ = nullptr;
  }
  if (right_frame_ != nullptr) {
    tree_handler_.disk_buffer_pool_->unpin_page(right_frame_);
    right_frame_ = nullptr;
  }
  end_index_ = -1;
  inited_ = false;
  LOG_INFO("bplus tree scanner closed");
  return RC::SUCCESS;
}

RC BplusTreeMultiScanner::fix_user_key(const char *user_key, int key_len, bool want_greater,
			      char **fixed_key, bool *should_inclusive)
{
  if (nullptr == fixed_key || nullptr == should_inclusive) {
    return RC::INVALID_ARGUMENT;
  }
  // TODO(multi-index)
  // // 这里很粗暴，变长字段才需要做调整，其它默认都不需要做调整
  // assert(tree_handler_.file_header_.attr_type == CHARS);
  // assert(strlen(user_key) >= static_cast<size_t>(key_len));

  // *should_inclusive = false;
  
  // int32_t attr_length = tree_handler_.file_header_.attr_length;
  // char *key_buf = new (std::nothrow)char [attr_length];
  // if (nullptr == key_buf) {
  //   return RC::NOMEM;
  // }

  // if (key_len <= attr_length) {
  //   memcpy(key_buf, user_key, key_len);
  //   memset(key_buf + key_len, 0, attr_length - key_len);

  //   *fixed_key = key_buf;
  //   return RC::SUCCESS;
  // }

  // // key_len > attr_length
  // memcpy(key_buf, user_key, attr_length);

  // char c = user_key[attr_length];
  // if (c == 0) {
  //   *fixed_key = key_buf;
  //   return RC::SUCCESS;
  // }

  // // 扫描 >=/> user_key 的数据
  // // 示例：>=/> ABCD1 的数据，attr_length=4,
  // //      等价于扫描 >= ABCE 的数据
  // // 如果是扫描 <=/< user_key的数据
  // // 示例：<=/< ABCD1  <==> <= ABCD  (attr_length=4)
  // *should_inclusive = true;
  // if (want_greater) {
  //   key_buf[attr_length - 1]++;
  // }
  
  // *fixed_key = key_buf;
  return RC::SUCCESS;
}
