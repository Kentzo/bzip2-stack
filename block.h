#pragma once

#include <string>
#include <utility>

namespace bzip2 {
  typedef std::string block;
  typedef std::pair<block, size_t> bwt_block;
}
