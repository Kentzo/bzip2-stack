#include "rle.h"

#include <functional>
#include <algorithm>

void compression::algorithm::RLE(block_base& blck) {
  // TODO: It is possible to process block in several parallel processes
  // In the worst case expense will be 1.25
  using namespace std;
  
  block_base compressed_data;
  compressed_data.reserve(blck.size() * 0.7);

  block_base::iterator first = blck.begin(), end = blck.end();
  while (first != end) {
    block_base::iterator last = find_if(first, end, bind2nd(not_equal_to<block_base::value_type>(), *first));
    size_t size = distance(first, last);
    // If size more than 255 divide sequence into blocks 
    for (size_t i=0, blocks_number=size/255; i<blocks_number; ++i) {
      compressed_data.insert(compressed_data.end(), 4, *first);
      compressed_data.push_back(251);
    }
    // If size less than 4 just print sequence
    if (size < 4)
      compressed_data.insert(compressed_data.end(), size, *first);
    // Otherwise print 4 *first symbols and one (size - 4) symbol
    else {
      compressed_data.insert(compressed_data.end(), 4, *first);
      compressed_data.push_back(static_cast<char>(size) - 4);
    }
    first = last;
  }
  blck.swap(compressed_data);
}
void compression::algorithm::Reverse_RLE(block_base& blck) {
  using namespace std;
  
  block_base decompressed_data;
  decompressed_data.reserve(blck.size() * 1.5);
  
  block_base::iterator first = blck.begin(), end = blck.end();
  while (first < end) {
    block_base::iterator last = find_if(first, end, bind2nd(not_equal_to<block_base::value_type>(), *first));
    size_t size = distance(first, last);
    // If size less than 4 just print sequence
    if (size < 4)
      decompressed_data.insert(decompressed_data.end(), size, *first);
    // Otherwise number of symbols is equal to *last + 4
    else {
      decompressed_data.insert(decompressed_data.end(), 4 + *last, *first);
      ++first; // *last symbol was not in original sequence, it was append by RLE algorithm
    }
    advance(first, size);
  }
  blck.swap(decompressed_data);
}
