#include "block.h"
#include <cstring>
#include <cassert>
#include <algorithm>
#include <iostream>

/*
static const size_t DEFAULT_BLOCK_SIZE = 921600; // 900kb

bzip2::block::block() {
  init((char*)0, (char*)0, DEFAULT_BLOCK_SIZE);
}

bzip2::block::~block() {
  release();
}

template<typename InputIterator>
bzip2::block::block(InputIterator begin, InputIterator end, size_t new_block_size) {
  init(begin, end, new_block_size);
}

bzip2::block::block(const block& orig) {
  init(orig.begin(), orig.end(), orig._block_size);
}

block& bzip2::block::operator=(const block& orig) {
  // do not check addresses in order to be efficient
  init(orig.begin(), orig.end(), orig._block_size);
  return *this;
}  

template<typename InputIterator>
void bzip2::block::init(InputIterator begin, InputIterator end, size_t new_block_size) {
  assert(begin <= end);
  release();
  _data.reserve(new_block_size);
  _data.insert(_data.begin(), begin, end);
}

std::vector<char>::iterator bzip2::block::begin() {
  return _data.begin();
}
std::vector<char>::const_iterator bzip2::block::begin() const {
  return _data.begin();
}

std::vector<char>::iterator bzip2::block::end() {
  return _data.end();
}
std::vector<char>::const_iterator bzip2::block::end() const {
  return _data.end();
}

size_t bzip2::block::block_size() const {
  return _data.size();
}

void bzip2::block::release() throw() {
  std::vector<char>().swap(_data);
}
*/
