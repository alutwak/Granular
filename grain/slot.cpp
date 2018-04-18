
#include "slot.hpp"

namespace audioelectric {

  Slot::Slot(const Slot& slot) : _fn(slot._fn), _hash(slot._hash)
  {
    
  }
  
  Slot::Slot(const FnDbl& fn, Hash hash) : _fn(fn), _hash(hash)
  {
    
  }

  Slot::Slot(void(*fn)(double)) : _fn(fn), _hash(0, (void*)fn)
  {

  }
  
  Slot& Slot::operator=(const Slot& rhs)
  {
    if (this == &rhs) return *this;
    _fn = rhs._fn;
    _hash = rhs._hash;
    return *this;
  }

  Slot& Slot::operator=(void(*rhs)(double))
  {
    _fn = rhs;
    _hash = Hash(0,(void*)rhs);
    return *this;
  }
  
  void Slot::operator()(double arg) const
  {
    _fn(arg);
  }

}
