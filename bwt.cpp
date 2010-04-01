#include "bwt.h"

#include <vector>
#include <algorithm>

/*
  In order to construct BWT output we make suffix array using IT99.
  But ours algorithm a little differ from IT99. We cannot use special character '$' 
  that is less than all characters in alphabet. Instead of, we use character '\0'.
  This approach require some actions which must be done manually. See comments.
*/
#define SPECIAL_CHARACTER (block::value_type)0 // Ours Special Character
void compression::algorithm::BWT(block& blck) {
  using namespace std;

  blck.push_back(SPECIAL_CHARACTER); // Add Special Character '\0' to the end of blck 

  size_t bucketptr_a[256] = {0}, bucketptr_b[256] = {0};
  size_t bucketsize_b[256] = {0};
  
  // Initialize bucketptrs
  {
    // Count sizes of buckets
    size_t bucketsize_a[256] = {0};
    for (size_t i=0, size=blck.size()-1; i<size; ++i) {
      block::value_type cur_char = blck.at(i), next_char = blck.at(i + 1);
      if (cur_char > next_char)
	bucketsize_a[cur_char] += 1;
      else
	bucketsize_b[cur_char] += 1;
    }
    bucketsize_a[SPECIAL_CHARACTER] += 1; // Special Character is always of Type A

    // Set bucketptrs
    for (size_t ch=0, size=256, ptr=0; ch<size; ++ch) {
      bucketptr_a[ch] = ptr;
      ptr += bucketsize_a[ch];
      ptr += bucketsize_b[ch];
      bucketptr_b[ch] = ptr; // bucketptr_b pointer to the end of bucket on the suffix_array, we will fix it at the next step
    }
  }

  // Form suffix_array
  vector<size_t> suffix_array(blck.size(), -1);

  // Set indexes of Type B on suffix_array
  for (size_t i=0, size=blck.size()-1; i<size; ++i) {
    block::value_type cur_char = blck.at(i), next_char = blck.at(i + 1);
    if (cur_char <= next_char) {
      bucketptr_b[cur_char] -= 1;
      suffix_array.at(bucketptr_b[cur_char]) = i;
    }
  }
  suffix_array.at(bucketptr_a[SPECIAL_CHARACTER]) = blck.size() - 1; // Special Character is always the first

  // Sort buckets of Type B on suffix_array
  for (size_t ch=0, size=256; ch<size; ++ch) {
    if (bucketsize_b[ch] > 1) {
      vector<size_t>::iterator first = suffix_array.begin(); advance(first, bucketptr_b[ch]);
      vector<size_t>::iterator end = first; advance(end, bucketsize_b[ch]);
      sort(first, end, suffix_compare(blck));
    }
  }

  // Sort buckets of Type A on suffix_array
  for (size_t i=0, size=suffix_array.size(); i<size; ++i) {
    size_t cur_pos = suffix_array.at(i);
    if (cur_pos) { // If cur_pos == 0, there is nothing to do
      size_t prev_pos = cur_pos - 1;
      if (blck.at(prev_pos) > blck.at(cur_pos)) { // If prev_pos is a suffix of Type A
	size_t char_index = blck.at(prev_pos);
	suffix_array.at(bucketptr_a[char_index]) = prev_pos;
	bucketptr_a[char_index] += 1;
      }
    }
  }

  // Get index of original string on suffix_array
  vector<size_t>::iterator pos = find(suffix_array.begin(), suffix_array.end(), 0);
  size_t I = distance(suffix_array.begin(), pos);
  blck.setProperty(BWT_INDEX, &I);

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
}

void compression::algorithm::Reverse_BWT(block& blck) {
  using namespace std;

  size_t I = -1;
  blck.getProperty(BWT_INDEX, &I);

  size_t C[256] = {0}; // C[ch] is the total number of instances in L of characters preceding character ch in the alphabet
  vector<size_t> P(blck.size(), 0); // P[i] is the number of instances of character blck[i] in the prefix blck[0,...,i-1]
  
  for (size_t i=0, size=blck.size(); i<size; ++i) {
    size_t char_index = blck.at(i);
    if (i != I) { // I'th character is ours Special Character. Skip it.
      P.at(i) = C[char_index];
      C[char_index] += 1;
    }
  } // Now C[ch] is the number of instances in blck of character ch
  
  size_t sum = 1; // Emulate order of characters. Ours Special Character is always at the first place on alphabet
  for (size_t i=0, size=256; i<size; ++i) {
    sum += C[i];
    C[i] = sum - C[i];
  } // Now C[ch] is the total number of instances in blck of characters preceding ch in the alphabet

  // Fill output string back to front
  block_base decompressed_data(blck.size(), 0);
  decompressed_data.back() = blck.at(I); 
  size_t j = 0; // Next character always has index = 0
  for (size_t i=2, size=blck.size(); i<=size; ++i) {
    decompressed_data.at(size - i) = blck.at(j);
    size_t index = blck.at(j);
    j = P.at(j) + C[index];
  }
  decompressed_data.pop_back();
  blck.swap(decompressed_data);
}

compression::algorithm::suffix_compare::suffix_compare(const block_base& blck) : _blck(blck) {}
bool compression::algorithm::suffix_compare::operator()(size_t left, size_t right) {
  block_base::const_iterator lbegin = _blck.begin(), rbegin = _blck.begin(), end = _blck.end();
  std::advance(lbegin, left + 1);
  std::advance(rbegin, right + 1);

  return std::lexicographical_compare(lbegin, end, rbegin, end);
}

#undef SPECIAL_CHARACTER
