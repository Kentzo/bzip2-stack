#pragma once

#include <string>

namespace bzip2 {
  typedef std::string block;
  /*class block {
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
    std::vector<char>::iterator begin();
    std::vector<char>::const_iterator begin() const;
    // get end of the block
    std::vector<char>::iterator end();
    std::vector<char>::const_iterator end() const;
    // get size of the block
    size_t block_size() const;
    // append data to the block
    
  private:
    void release() throw();
    
    std::vector<char> _data;
  };*/
}
