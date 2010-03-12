#pragma once

#include <iosfwd>

class block {
 public:
  block();
  ~block();
  template<typename InputIterator>
    block(InputIterator begin, InputIterator end, size_t new_block_size);
  block(const block& orig);
  block& operator=(const block& orig);
// setup block data
  template<typename InputIterator>
    void init(InputIterator begin, InputIterator end, size_t new_block_size);
// get begin of the block
  char* begin();
  const char* begin() const;
// get end of the block
  char* end();
  const char* end() const;
// get size of the block
  size_t block_size() const;
 private:
  void release() throw();

  size_t _block_size;
  char* _data;
};

template<typename T>
std::basic_ostream<T>& operator<<(std::basic_ostream<T>& out, const block& blck) {
  out.write(blck.begin(), blck.block_size());
  return out;
}

template<typename T>
std::basic_istream<T>& operator>>(std::basic_istream<T>& in, block& blck) {
  in.read(blck.begin(), blck.block_size());
  return in;
}
