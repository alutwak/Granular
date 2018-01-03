
#pragma once

#include "waveform.hpp"

namespace audioelectric {

  template<typename DType>
  class Grain : public Waveform<DType> {

  public:

    void multiply(typename Waveform<DType>::const_iterator& other, double speed, DType *output);
    
  private:
    
  };
  
}
