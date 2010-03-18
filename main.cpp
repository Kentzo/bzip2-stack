#include "block.h"
#include "algorithm_blockbased.h"
#include <fstream>
#include <iostream>

int main() {
  using namespace std;
  using namespace bzip2;

  block b("aaaaaaaaaaaaaaaaacbdbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
  cout << b << endl;
  block &cb = algorithm::rle(b);
  block &rb = algorithm::reverse_rle(cb);
  cout << rb << endl;

  return 0;
}
