#pragma once

#include <vector>
#include <memory>

enum PROPERTY {
  BWT_INDEX // size_t
};

/*
We use bridge design pattern in order to separate part that responsible for 
storing block properties (such as index of string in BWT matrix) from 
part that responsible for block data managing.
 */

namespace compression {
  typedef std::vector<unsigned char> block_base;

  class block_imp {
  public:
    virtual void setProperty(PROPERTY name, void* data) = 0;
    virtual void getProperty(PROPERTY name, void* var) = 0;
    virtual ~block_imp() {};
  };

  class block : public block_base {
  public:
    explicit block(block_imp* imp);
    virtual void setProperty(PROPERTY name, void* data);
    virtual void getProperty(PROPERTY name, void* var);
  private:
    std::auto_ptr<block_imp> imp_;
  };
}
