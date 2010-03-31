#include "mtf.h"

#include <list>
#include <algorithm>

void compression::algorithm::MTF(block_base& blck) {
  using namespace std;
  // Generate alphabet
  list<block_base::value_type> alphabet(256);
  list<block_base::value_type>::iterator pos = alphabet.begin(), end = alphabet.end();
  for (block_base::value_type i=0; pos != end; ++i, ++pos) {
    *pos = i;
  }

  // Map symbols to their indexes in alphabet
  for (size_t i=0, size=blck.size(); i<size; ++i) {
    // Locate position of i-th symbol and replace it with index of this symbol in the alphabet
    list<block_base::value_type>::iterator alphabet_pos = find(alphabet.begin(), alphabet.end(), blck.at(i));
    size_t index = distance(alphabet.begin(), alphabet_pos);
    blck.at(i) = index;
    // Move this symbol to the front of the alphabet
    alphabet.push_front(*alphabet_pos);
    alphabet.erase(alphabet_pos);
  }
}

void compression::algorithm::Reverse_MTF(block_base& blck) {
  using namespace std;
  // alphabet contains all possible symbols for 1byte
  list<block_base::value_type> alphabet(256);
  list<block_base::value_type>::iterator pos = alphabet.begin(), end = alphabet.end();
  for (block_base::value_type i=0; pos != end; ++i, ++pos) {
    *pos = i;
  }

  for (size_t i=0, size=blck.size(); i<size; ++i) {
    // Every blck[i] symbol has a position of real symbol in the alphabet
    size_t index = static_cast<unsigned char>(blck.at(i)); // Convert char to unsigned char in order to correctly convert it to size_t
    list<block_base::value_type>::iterator alphabet_pos = alphabet.begin();
    advance(alphabet_pos, index);
    blck.at(i) = *alphabet_pos;
    // Move this symbol to the front of the alphabet
    alphabet.push_front(*alphabet_pos);
    alphabet.erase(alphabet_pos);
  }
}
