#include "algorithm_blockbased.h"

#include <algorithm>
#include <functional>
#include <cassert>
#include <list>
#include <vector>
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

// This algorithm is described at "A Block-sorting Lossless Data Compression Algorithm" 
// original article from M. Burrows and D.J. Wheeler
bzip2::block& bzip2::algorithm::bwt(block& blck) {
  using namespace std;

  size_t word_size = sizeof(size_t)/sizeof(char);
  // Q1 
  // [form extended string]
  blck.append(word_size, block::traits_type::eof());

  // Q2 and Q3
  // [form array of words] and [form array of suffixes]
  vector<block::iterator> words;
  words.reserve(blck.size());
  for (size_t i=0, size=blck.size(); i<size; ++i) {
    block::iterator it = blck.begin();
    advance(it, i);
    words.push_back(it);
  }
  
  // Q4
  // [radix sort]
  for (size_t k=0; k<2; ++k) { // Radix sort based on first two characters of each suffix
    // Counting sort
    size_t C[256] = {0};
    for (size_t i=0, size=words.size(); i<size; ++i) {
      block::iterator character = words.at(i + k);
      size_t index = static_cast<unsigned char>(*character);
      C[index] += 1;
    }
    for (size_t i=1, size=sizeof(C)/sizeof(size_t); i<size; ++i) {
      C[i] = C[i-1];
    }
    vector<block::iterator> tmp_words(words.size());
    for (size_t i=0, size=words.size(); i<size; ++i) {
      block::iterator character = words.at(i + k);
      size_t index = static_cast<unsigned char>(*character);
      tmp_words.at(C[index]) = character;
      C[index] -= 1;
    }
    tmp_words.swap(words);
  }

  // Q5 
  // [iterate over each character in the alphabet]
  char* chars = new char[2];
  vector<block::iterator>::iterator last_pos = words.begin();
  for (char ch=0; ch<256; ++ch) {
    // Q6
    // [quicksort suffixes starting with ch]
    // For each character ch' in the alphabet: Apply quicksort to the elements of words starting with ch followed by ch'
    chars[0] = ch;
    for (char ch_=0; ch_<256; ++ch_) {
      chars[1] = ch_;
      pos = find_if(last_pos, words.end(), not1(helpers::chars_compare(two_char, 2)));
      helpers::qsort(last_pos, pos, word_size);
      last_pos = pos;
    }

    // Q7
    // [update sort keys]
    
    
  }
  delete[] two_char;
  /*
  vector<block> words; // words = W
  words.reserve(blck.size());
  for (size_t i=0, size=blck.size(); i<size; ++i)
    words.push_back(blck.substr(i, i + word_size - 1)); // W[i] contains the characters S'[i,...,i+k-1]
  // Q3 [form array of suffixes]
  list<size_t> suffixes; // suffixes = V
  suffixes.reserve(blck.size());
  for (size_t i=0, size=blck.size(); i<size; ++i)
    suffixes.push_back(i); // every j = suffixes[i] represents j-th suffix of extended string S'
  // Q4 [radix sort] Sort the elements of V, using the first two characters of each suffix as the sort key
  */
}

bzip2::block& bzip2::algorithm::reverse_bwt(block& blck) {
}

void bzip2::algorithm::helpers::bwt::qsort(vector<block::iterator>::iterator first, 
					  vector<block::iterator>::iterator end, size_t word_size) 
{
  assert(first <= end);
  vector<block::iterator>::iterator i = first, j = end - 1;
  block::iterator x = *i;
  do {
    while (std::lexicographical_compare(*i, i + word_size, x, x + word_size)) ++i;
    while (std::lexicographical_compare(x, x + word_size, *j, *j + word_size)) --j;
    if (j >= i) {
      swap(*i, *j);
      ++i;
      --j;
    }
  }
  while (j >= i);
  if (j > b) qsort(b, j + 1, word_size);
  if (e > i + 1) qsort(i, e, word_size);
}

bzip2::algorithm::helpers::bwt::chars_compare::chars_compare(const char* chars, size_t size) 
  : _chars(chars), _size(size) {}
bool bzip2::algorithm::helpers::bwt::chars_compare::operator()(block::iterator it) {
  return std::lexicographical_compare(it, it + size, chars, chars + size);
}
