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
  }
}
