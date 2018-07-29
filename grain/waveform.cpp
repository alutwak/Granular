
#include <cmath>
#include "waveform.hpp"

namespace audioelectric {
  
  /*********************** Public Waveform *******************************/

  template<typename T>
  Waveform<T>::~Waveform(void) {}
  
  // template<typename T>
  // typename Waveform<T>::phasor Waveform<T>::pbegin(double rate, double start)
  // {
  //   return phasor(new phasor_impl(*this,rate,start));
  // }

  // template<typename T>
  // typename Waveform<T>::phasor Waveform<T>::pbegin(double rate)
  // {
  //   return pbegin(rate,0);
  // }

  /*********************** phasor_impl *******************************/  

  template class Waveform<double>;
  template class Waveform<float>;

}
