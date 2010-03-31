#pragma once

#include <vector>
#include <memory>

enum PROPERTY {
  BWT_INDEX
};

/*
  If you do not use get/setProperty, you will use block_base instead of block and subclasses
 */

namespace compression {
  typedef std::vector<unsigned char> block_base;

  class block_imp {
  public:
    virtual void setProperty(PROPERTY name, void* data) = 0;
    virtual void getProperty(PROPERTY name, void* var) = 0;
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
