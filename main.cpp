#include "block.h"
#include <fstream>

int main() {
  using namespace std;

  ifstream f("testfile");
  ofstream o("tmp");
  if (!f.is_open())
    return 1;
  while (!f.eof()) {
    block b;
    f >> b;
    o << b;
  }

  return 0;
}
