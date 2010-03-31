#include "bwt.h"

#include <vector>
#include <algorithm>

void compression::algorithm::BWT(block& blck) {
  using namespace std;

  //blck.push_back(block::traits_type::eof());

  size_t bucketptr_a[256] = {0}, bucketptr_b[256] = {0};
  size_t bucketsize_b[256] = {0};
  
  // Initialize bucketptrs
  {
    // Count sizes of buckets
    size_t bucketsize_a[256] = {0};
    for (size_t i=0, size=blck.size()-1; i<size; ++i) {
      block::value_type cur_char = blck.at(i), next_char = blck.at(i + 1);
      size_t index = cur_char;
      if (cur_char > next_char)
	bucketsize_a[index] += 1;
      else
	bucketsize_b[index] += 1;
    }
    size_t back_char = blck.at(blck.size() - 1);
    bucketsize_a[back_char] += 1;

    // Set bucketptrs
    for (size_t i=0, size=256, ptr=0; i<size; ++i) {
      bucketptr_a[i] = ptr;
      ptr += bucketsize_a[i];
      ptr += bucketsize_b[i];
      bucketptr_b[i] = ptr; // bucketptr_b pointer to the end of bucket on the suffix_array, we will fix it at the next step
    }
  }

  // Form suffix_array
  vector<size_t> suffix_array(blck.size(), 11);
  //print(suffix_array);
  // Set indexes of Type B on suffix_array
  for (size_t i=0, size=blck.size()-1; i<size; ++i) {
    block::value_type cur_char = blck.at(i), next_char = blck.at(i + 1);
    size_t index = cur_char;
    if (cur_char <= next_char) {
      bucketptr_b[index] -= 1;
      suffix_array.at(bucketptr_b[index]) = i;
    }
  }
  //print(suffix_array);
  // Sort buckets of Type B on suffix_array
  for (size_t i=0, size=256; i<size; ++i) {
    if (bucketsize_b[i] > 1) {
      vector<size_t>::iterator first = suffix_array.begin(); advance(first, bucketptr_b[i]);
      vector<size_t>::iterator end = first; advance(end, bucketsize_b[i]);
      sort(first, end, suffix_compare(blck));
    }
  }
  // print(suffix_array);
  // Sort buckets of Type A on suffix_array
  size_t back_char = blck.at(blck.size() - 1);
  suffix_array.at(bucketptr_a[back_char]) = blck.size() - 1;
  bucketptr_a[back_char] += 1;
  for (size_t i=0, size=suffix_array.size(); i<size; ++i) {
    size_t cur_pos = suffix_array.at(i);
    if (cur_pos) { // If cur_pos == 0, there is nothing to do
      size_t prev_pos = cur_pos - 1;
      if (blck.at(prev_pos) > blck.at(cur_pos)) { // If prev_pos is a suffix of Type A
	size_t index = blck.at(prev_pos);
	suffix_array.at(bucketptr_a[index]) = prev_pos;
	bucketptr_a[index] += 1;
      }
    }
  }
  //print(suffix_array);
  // Get index of original string on suffix_array
  vector<size_t>::iterator pos = find(suffix_array.begin(), suffix_array.end(), 0);
  size_t I = distance(suffix_array.begin(), pos);

  // Calculate BWT output
  block_base compressed_data(blck.size(), 0);
  for (size_t i=0, size=blck.size(); i<size; ++i) {
    size_t index = suffix_array.at(i);
    if (index)
      compressed_data.at(i) = blck.at(index - 1);
    else
      compressed_data.at(i) = blck.at(blck.size() - 1);
  }
  blck.swap(compressed_data);
  blck.setProperty(BWT_INDEX, &I);
}

void compression::algorithm::Reverse_BWT(block& blck) {
  using namespace std;

  size_t C[256] = {0}; // C[ch] is the total number of instances in L of characters preceding character ch in the alphabet
  vector<size_t> P(blck.size(), 0); // P[i] is the number of instances of character blck[i] in the prefix blck[0,...,i-1]
  
  for (size_t i=0, size=blck.size(); i<size; ++i) {
    size_t index = blck.at(i);
    P.at(i) = C[index];
    C[index] += 1;
  } // Now C[ch] is the number of instances in blck of character ch
  
  size_t sum = 0;
  for (size_t i=0, size=256; i<size; ++i) {
    sum += C[i];
    C[i] = sum - C[i];
  } // Now C[ch] is the total number of instances in blck of characters preceding ch in the alphabet

  block_base decompressed_data(blck.size(), 0);
  size_t j = 0;
  blck.getProperty(BWT_INDEX, &j);
  // Fill output string back to front
  for (size_t i=1, size=blck.size(); i<=size; ++i) {
    decompressed_data.at(size - i) = blck.at(j);
    size_t index = blck.at(j);
    j = P.at(j) + C[index];
  }
  blck.swap(decompressed_data);
}

compression::algorithm::suffix_compare::suffix_compare(const block& blck) : _blck(blck) {}
bool compression::algorithm::suffix_compare::operator()(size_t left, size_t right) {
  block::const_iterator lbegin = _blck.begin(), lend = _blck.end(), rbegin = _blck.begin(), rend = _blck.end();
  std::advance(lbegin, left);
  std::advance(rbegin, right);
  return std::lexicographical_compare(lbegin, lend, rbegin, rend);
}
