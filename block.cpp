#include "block.h"
#include <cstring>
#include <cassert>
#include <algorithm>
#include <iostream>

static const size_t DEFAULT_BLOCK_SIZE = 921600; // 900kb
static const size_t MINIMAL_BLOCK_SIZE = 102400; // 100kb

block::block() {
  init((char*)0, (char*)0, DEFAULT_BLOCK_SIZE);
}

block::~block() {
  release();
}

template<typename InputIterator>
block::block(InputIterator begin, InputIterator end, size_t new_block_size) {
  init(begin, end, new_block_size);
}

block::block(const block& orig) {
  init(orig.begin(), orig.end(), orig._block_size);
}

block& block::operator=(const block& orig) {
  // do not check addresses in order to be efficient
  init(orig.begin(), orig.end(), orig._block_size);
  return *this;
}  

template<typename InputIterator>
void block::init(InputIterator begin, InputIterator end, size_t new_block_size) {
  assert(begin <= end);
  assert(new_block_size > MINIMAL_BLOCK_SIZE);
  release();
  _block_size = new_block_size;
  _data = new char[_block_size];
  memset(_data, 0, _block_size);
  std::copy(begin, end, _data);
}

char* block::begin() {
  return _data;
}
const char* block::begin() const {
  return _data;
}

char* block::end() {
  return (_data + _block_size);
}
const char* block::end() const {
  return (_data + _block_size);
}

size_t block::block_size() const {
  return _block_size;
}

void block::release() throw() {
  delete[] _data;
  _data = NULL;
}
