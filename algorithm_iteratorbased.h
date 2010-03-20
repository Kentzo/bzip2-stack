#pragma once

#include "block.h"
#include <algorithm>
#include <functional>
#include <cassert>
#include <list>

namespace bzip2 {
  namespace algorithm {
    template<typename InputIterator>
      block rle(InputIterator first, InputIterator end) {
      // TODO: It is possible to process block in several parallel processes
      // In the worst case expense will be 1.25
      assert(first <= end);
      using namespace std;
      
      block compressed_data;
      compressed_data.reserve((end - first) * 0.7);

      while (first < end) {
	// TODO: Is there a way to replace char with template parameter?
	InputIterator last = find_if(first, end, bind2nd(not_equal_to<char>(), *first));
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
      return compressed_data;
    }

    template<typename InputIterator>
      block reverse_rle(InputIterator first, InputIterator end) {
      // TODO: It is possible to process block in several parallel processes
      assert(first <= end);
      using namespace std;

      block decompressed_data;
      decompressed_data.reserve((end - first) * 1.5);

      while (first < end) {
	InputIterator last = find_if(first, end, bind2nd(not_equal_to<char>(), *first));
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
      return decompressed_data;
    }

    template<typename InputIterator>
      block mtf(InputIterator first, InputIterator end) {
      assert(first <= end);
      using namespace std;

       // alphabet contains all possible symbols for 1byte
      list<char> alphabet(256);
      list<char>::iterator pos = alphabet.begin(), end2 = alphabet.end();
      for (char i=0; pos != end2; ++i, ++pos) {
	*pos = i;
      }
      
      block compressed_data;
      compressed_data.reserve(distance(first, end));
      // Map symbols to their indexes in alphabet
      while (first < end) {
	// Locate position of i-th symbol and replace it with index of this symbol in the alphabet
	list<char>::iterator alphabet_pos = find(alphabet.begin(), alphabet.end(), *first);
	size_t index = distance(alphabet.begin(), alphabet_pos);
	compressed_data.push_back(index);
	// Move this symbol to the front of the alphabet
	alphabet.push_front(*alphabet_pos);
	alphabet.erase(alphabet_pos);
	++first;
      }
  
      return compressed_data;
    }

    template<typename InputIterator>
      block reverse_mtf(InputIterator first, InputIterator end) {
      assert(first <= end);
      using namespace std;

       // alphabet contains all possible symbols for 1byte
      list<char> alphabet(256);
      list<char>::iterator pos = alphabet.begin(), end2 = alphabet.end();
      for (char i=0; pos != end2; ++i, ++pos) {
	*pos = i;
      }
      
      block decompressed_data;
      decompressed_data.reserve(distance(first, end));
      // Map symbols to their indexes in alphabet
      while (first < end) {
	// Every blck[i] symbol has a position of real symbol in the alphabet
	size_t index = static_cast<unsigned char>(*first); // Convert char to unsigned char in order to correctly convert it to size_t
	list<char>::iterator alphabet_pos = alphabet.begin();
	advance(alphabet_pos, index);
	decompressed_data.push_back(*alphabet_pos);
	// Move this symbol to the front of the alphabet
	alphabet.push_front(*alphabet_pos);
	alphabet.erase(alphabet_pos);
	++first;
      }
  
      return decompressed_data;
    }
  }
}
