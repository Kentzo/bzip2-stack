#include "block.h"
#include "algorithm.h"
#include <fstream>
#include <iostream>

int main() {
  using namespace std;
  using namespace bzip2;

  block b("aaaaaaaaaaaaaaaaacbdbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
  block cb = algorithm::rle(b.begin(), b.end());
  block rb = algorithm::reverse_rle(cb.begin(), cb.end());
  cout << (b == rb) << endl;

  return 0;
}
