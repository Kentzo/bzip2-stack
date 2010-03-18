#include "algorithm_blockbased.h"

#include <algorithm>
#include <functional>
#include <cassert>
#include <list>
#include <iterator>

bzip2::block& bzip2::algorithm::rle(bzip2::block& blck) {
  // TODO: It is possible to process block in several parallel processes
  // In the worst case expense will be 1.25
  using namespace std;
  
  block compressed_data;
  compressed_data.reserve(blck.size() * 0.7);

  block::iterator first = blck.begin(), end = blck.end();
  while (first < end) {
    // TODO: Is there a way to replace char with template parameter?
    block::iterator last = find_if(first, end, bind2nd(not_equal_to<char>(), *first));
    size_t size = last - first;
    // If size more than 255 divide sequence into blocks 
    for (size_t i=0, blocks_count=size/255; i<blocks_count; ++i) {
      compressed_data.append(4, *first);
      compressed_data.push_back(251);
    }
    // If size less than 4 just print sequence
    if (size < 4)
      compressed_data.append(size, *first);
    // Otherwise print 4 symbols and (size - 4)
    else {
      compressed_data.append(4, *first);
      compressed_data.push_back(char(size) - 4);
    }
    first = last;
  }
  compressed_data.swap(blck);
  return blck;
}

bzip2::block& bzip2::algorithm::reverse_rle(block& blck) {
  using namespace std;
  
  block decompressed_data;
  decompressed_data.reserve(blck.size() * 1.5);
  
  block::iterator first = blck.begin(), end = blck.end();
  while (first < end) {
    block::iterator last = find_if(first, end, bind2nd(not_equal_to<char>(), *first));
    size_t size = last - first;
    // If size less than 4 just print sequence
    if (size < 4)
      decompressed_data.append(size, *first);
    // Otherwise count of symbols is equal to *last + 4
    else {
      decompressed_data.append(4 + *last, *first);
      ++first; // *last symbol isn't in original sequence
    }
    first += size;
  }
  blck.swap(decompressed_data);
  return blck;
}

bzip2::block& bzip2::algorithm::mtf(block& blck) {
  using namespace std;
  // alphabet contains all possible symbols for 1byte
  list<char> alphabet(256);
  list<char>::iterator pos = alphabet.begin(), end = alphabet.end();
  for (char i=0; pos != end; ++i, ++pos) {
    *pos = i;
  }

  // Map symbols to their indexes in alphabet
  for (size_t i=0, size=blck.size(); i<size; ++i) {
    // Locate position of i-th symbol and replace it with index of this symbol in the alphabet
    list<char>::iterator alphabet_pos = find(alphabet.begin(), alphabet.end(), blck.at(i));
    size_t index = distance(alphabet.begin(), alphabet_pos);
    blck.at(i) = index;
    // Move this symbol to the front of the alphabet
    alphabet.push_front(*alphabet_pos);
    alphabet.erase(alphabet_pos);
  }
  
  return blck;
}

bzip2::block& bzip2::algorithm::reverse_mtf(block& blck) {
  using namespace std;
  // alphabet contains all possible symbols for 1byte
  list<char> alphabet(256);
  list<char>::iterator pos = alphabet.begin(), end = alphabet.end();
  for (char i=0; pos != end; ++i, ++pos) {
    *pos = i;
  }

  for (size_t i=0, size=blck.size(); i<size; ++i) {
    // Every blck[i] symbol has a position of real symbol in the alphabet
    size_t index = static_cast<unsigned char>(blck.at(i)); // Convert char to unsigned char in order to correctly convert it to size_t
    list<char>::iterator alphabet_pos = alphabet.begin();
    advance(alphabet_pos, index);
    blck.at(i) = *alphabet_pos;
    // Move this symbol to the front of the alphabet
    alphabet.push_front(*alphabet_pos);
    alphabet.erase(alphabet_pos);
  }
  
  return blck;
}

bzip2::block& bzip2::algorithm::bwt(block& blck) {
  
}
bzip2::block& bzip2::algorithm::reverse_bwt(block& blck) {
}
