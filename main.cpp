#include "block.h"
#include "mtf.h"
#include "rle.h"
#include "bwt.h"
#include <fstream>
#include <iostream>
#include <algorithm>

class bzip2_imp : public compression::block_imp {
public:
  bzip2_imp() : index_(0) {}
  void setProperty(PROPERTY name, void* data) {
    switch (name) {
    case BWT_INDEX : 
      index_ = *(size_t*)data;
      break;
    default :
      throw std::string("property not found");
    }
  }
  void getProperty(PROPERTY name, void* var) {
    switch (name) {
    case BWT_INDEX :
      *(size_t*)var = index_;
      break;
    default :
      throw std::string("propery not found");
    }
  }
private:
  size_t index_;
};

void print_codes(const compression::block_base& b) {
  for (size_t i=0; i<b.size(); ++i)
    std::cout << (int)b.at(i) << " ";
  std::cout << std::endl;
}

int main() {
  using namespace std;
  using namespace compression;
  using namespace compression::algorithm;

  ifstream random("/dev/urandom");
  char data[15] = {0};
  random.read(data, sizeof(data)/sizeof(char));
  random.close();

  ofstream results("test_results");
  
  {
    block_base test_mtf(data, data + sizeof(data)/sizeof(char));
    MTF(test_mtf);
    Reverse_MTF(test_mtf);
    bool is_equal = equal(test_mtf.begin(), test_mtf.end(), data, equal_to<block::value_type>());
    results << "MTF blockbased test is passed: " << is_equal << endl;
  }
  
  {
    block_base test_rle(data, data + sizeof(data)/sizeof(char));
    RLE(test_rle);
    Reverse_RLE(test_rle);
    bool is_equal = equal(test_rle.begin(), test_rle.end(), data, equal_to<block::value_type>());
    results << "RLE blockbased test is passed: " << is_equal << endl;
  }
  
  {
    block test_bwt(new bzip2_imp);
    test_bwt.assign(data, data + sizeof(data)/sizeof(char));
    print_codes(test_bwt);
    BWT(test_bwt);
    print_codes(test_bwt);
    Reverse_BWT(test_bwt);
    print_codes(test_bwt);
    bool is_equal = equal(test_bwt.begin(), test_bwt.end(), data, equal_to<block::value_type>());
    cout << "BWT blockbased test is passed: " << is_equal << endl;
    results << "BWT blockbased test is passed: " << is_equal << endl;
  }

  return 0;
}
