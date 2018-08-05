
#include <cmath>
#include "waveform.hpp"

namespace audioelectric {
  
  /*********************** Public Waveform *******************************/

  template<typename T>
  Waveform<T>::~Waveform(void) {}
  
  template class Waveform<double>;
  template class Waveform<float>;

}
