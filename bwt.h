#include "block.h"

namespace compression {
  namespace algorithm {
    void BWT(block& blck);
    void Reverse_BWT(block& blck);

    class suffix_compare {
      suffix_compare();
    public:
      explicit suffix_compare(const block&);
      bool operator()(size_t, size_t);
    private:
      const block& _blck;
    };
  }
}
