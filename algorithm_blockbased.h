#pragma once

#include "block.h"

// Instead of iteratorbased algorithms applies to the sended block.
namespace bzip2 {
  namespace algorithm {
    // RLE
    block& rle(block& blck);
    block& reverse_rle(block& blck);

    // MTF
    block& mtf(block& blck);
    block& reverse_mtf(block& blck);

    // BWT
    block& bwt(block& blck);
    block& reverse_bwt(block& blck);

    namespace helpers {
      namespace bwt {
	void qsort(vector<block::iterator>::iterator first, vector<block::iterator>::iterator end, size_t word_size)
	struct chars_compare {
	  chars_compare(const char* chars, size_t size);
	  bool operator()(block::iterator it);
	private:
	  const char* _chars;
	  size_t _size;
	};
      }
    }
  }
}
