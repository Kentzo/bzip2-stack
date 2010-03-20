#pragma once

#include "block.h"

// Instead of iteratorbased algorithms applies to the sended block.
namespace bzip2 {
  namespace algorithm {
    block& rle(block& blck);
    block& reverse_rle(block& blck);

    block& mtf(block& blck);
    block& reverse_mtf(block& blck);
  }
}
