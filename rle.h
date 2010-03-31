#pragma once

#include "block.h"

namespace compression {
  namespace algorithm {
    void RLE(block_base& blck);
    void Reverse_RLE(block_base& blck);
  }
}
