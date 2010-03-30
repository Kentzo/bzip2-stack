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

bzip2::block& bzip2::algorithm::bwt(block& blck) {
  using namespace std;

  size_t bucketptr_a[256] = {0}, bucketptr_b[256] = {0};
  size_t bucketsize_b[256] = {0};
  
  // Initialize bucketptrs
  {
    size_t bucketsize_a[256] = {0};
    for (size_t i=0, size=blck.size()-1; i<size; ++i) {
      char cur_char = blck.at(i), next_char = blck.at(i + 1);
      size_t index = static_cast<unsigned char>(cur_char);
      if (cur_char > next_char)
	bucketsize_a[index] += 1;
      else
	bucketsize_b[index] += 1;
    }
    bucketsize_b[0] = 1;

    for (size_t i=0, size=256, ptr=0; i<size; ++i) {
      bucketptr_a[i] = ptr;
      ptr += bucketsize_a[i];
      ptr += bucketsize_b[i];
      bucketptr_b[i] = ptr; // bucketptr_b pointer to the end of bucket on the suffix_array, we will fix it at the next step
    }
  }

  vector<size_t> suffix_array(blck.size(), 0);

  // Set indexes of Type B on the suffix_array
  for (size_t i=0, size=blck.size()-1; i<size; ++i) {
    char cur_char = blck.at(i), next_char = blck.at(i + 1);
    size_t index = static_cast<unsigned char>(cur_char);
    if (cur_char <= next_char) {
      bucketptr_b[index] -= 1;
      suffix_array.at(bucketptr_b[index]) = i;
    }
  }
  suffix_array.at(0) = blck.size() - 1;
  
  // Sort buckets of Type B on suffix_array
  for (size_t i=0, size=256; i<size; ++i) {
    if (bucketsize_b[i] > 1) {
      vector<size_t>::iterator first = suffix_array.begin() + bucketptr_b[i];
      vector<size_t>::iterator end = first + bucketsize_b[i];
      sort(first, end, suffix_compare(blck));
    }
  }

  // Sort buckets of Type A on suffix_array
  for (size_t i=0, size=suffix_array.size()-1; i<size; ++i) {
    size_t cur_pos = suffix_array.at(i);
    if (cur_pos) { // If cur_pos == 0, there is nothing to do
      size_t prev_pos = cur_pos - 1;
      if (blck.at(prev_pos) > blck.at(cur_pos)) {
	size_t index = static_cast<unsigned char>(blck.at(prev_pos));
	suffix_array.at(bucketptr_a[index]) = prev_pos;
	bucketptr_a[index] += 1;
      }
    }
  }
  
  block compressed_data(blck.size(), 0);
  for (size_t i=0, size=blck.size(); i<size; ++i) {
    size_t index = suffix_array.at(i);
    if (index)
      compressed_data.at(i) = blck.at(index - 1);
    else
      compressed_data.at(i) = blck.at(blck.size() - 1);
  }
  blck.swap(compressed_data);

  return blck;
}

bzip2::block& bzip2::algorithm::reverse_bwt(block& blck) {
  using namespace std;

  size_t C[256] = {0};
  vector<size_t> P(blck.size(), 0);
  
  for (size_t i=0, size=blck.size(); i<size; ++i) {
    size_t index = static_cast<unsigned char>(blck.at(i));
    P.at(i) = C[index];
    C[index] += 1;
  }

//   for (int i=0; i<blck.size(); ++i)
//     cout << P.at(i) << endl; 
//   cout << endl;


  size_t sum = 0;
  for (size_t i=0, size=256; i<size; ++i) {
    sum += C[i];
    C[i] = sum - C[i];
  }
 
  vector<size_t> T(blck.size());
  for (size_t i=0, size=blck.size(); i<size; ++i) {
    size_t index = static_cast<unsigned char>(blck.at(i));
    T.at(i) = P.at(i) + C[index];
  }

  block decompressed_data(blck.size(), 0);
  for (size_t i=1, j=0, size=blck.size(); i<=size; ++i) {
    decompressed_data.at(size - i) = blck.at(j);
    j = T[j];
  }
  blck.swap(decompressed_data);

  return blck;
}

bzip2::algorithm::suffix_compare::suffix_compare(const block& blck) : _blck(blck) {}
bool bzip2::algorithm::suffix_compare::operator()(size_t left, size_t right) {
  block::const_iterator lbegin = _blck.begin(), lend = _blck.end(), rbegin = _blck.begin(), rend = _blck.end();
  std::advance(lbegin, left);
  std::advance(rbegin, right);
  return std::lexicographical_compare(lbegin, lend, rbegin, rend);
}
