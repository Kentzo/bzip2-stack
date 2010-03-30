#include "block.h"
#include "algorithm_blockbased.h"
#include "algorithm_iteratorbased.h"
#include <fstream>
#include <iostream>
#include <algorithm>

int main() {
  using namespace std;
  using namespace bzip2;
  using namespace bzip2::algorithm;

  ifstream random("/dev/urandom");
  char data[10000] = {0};
  random.read(data, sizeof(data)/sizeof(char) - 1);
  random.close();

  ofstream results("test_results");

  {
    block test_mtf(data, data + sizeof(data)/sizeof(char));
    mtf(test_mtf);
    reverse_mtf(test_mtf);
    bool is_equal = equal(test_mtf.begin(), test_mtf.end(), data);
    results << "MTF blockbased test is passed: " << is_equal << endl;
    
    test_mtf.replace(test_mtf.begin(), test_mtf.end(), data, data + sizeof(data)/sizeof(char));
    block b1 = mtf(test_mtf.begin(), test_mtf.end());
    block b2 = reverse_mtf(b1.begin(), b1.end());
    is_equal = equal(b2.begin(), b2.end(), data);
    results << "MTF iteratorbased test is passed: " << is_equal << endl;
  }

  {
    block test_rle(data, data + sizeof(data)/sizeof(char));
    rle(test_rle);
    reverse_rle(test_rle);
    bool is_equal = equal(test_rle.begin(), test_rle.end(), data);
    results << "RLE blockbased test is passed: " << is_equal << endl;

    test_rle.replace(test_rle.begin(), test_rle.end(), data, data + sizeof(data)/sizeof(char));
    block b1 = rle(test_rle.begin(), test_rle.end());
    block b2 = reverse_rle(b1.begin(), b1.end());
    is_equal = equal(b2.begin(), b2.end(), data);
    results << "RLE iteratorbased test is passed: " << is_equal << endl;
  }

  block b("abraca");
  cout << b << endl;
  bwt(b);
  cout << b << endl;
  reverse_bwt(b);
  cout << b << endl;

  return 0;
}
