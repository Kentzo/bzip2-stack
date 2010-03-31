#include "block.h"

void compression::block::setProperty(PROPERTY name, void* data) {
  imp_->setProperty(name, data);
}

void compression::block::getProperty(PROPERTY name, void* var) {
  imp_->getProperty(name, var);
}

compression::block::block(block_imp* imp) : imp_(imp) {}
