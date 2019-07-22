/* (c) AudioElectric. All rights reserved.
 * 
 * Author:             Ayal Lutwak <ayal@audioelectric.com>
 * Date:               June 26, 2019
 * Last Modified By:   Ayal Lutwak <ayal@audioelectric.com>
 * Last Modified Date: June 26, 2019
 */

#include "grain.hpp"

namespace audioelectric {

  template<typename T>
  Grain<T>::Grain(Waveform<T>& carrier, double crate, Waveform<T>& shape, double srate, T ampl) :
    _carrier(carrier, crate, true), _shape(shape, srate), _ampl(ampl)
  {
    
  }

  template <typename T>
  Grain<T>::Grain(Phasor<T>& carrier, Phasor<T>& shape, T ampl)
    : _carrier(carrier), _shape(shape), _ampl(ampl)
  {
    
  }

  template <typename T>
  Grain<T>::Grain(Phasor<T>&& carrier, Phasor<T>&& shape, T ampl)
    : _carrier(carrier), _shape(shape), _ampl(ampl)
  {
    
  }

  template <typename T>
  T Grain<T>::value(void) const
  {
    return _carrier.value() * _shape.value() * _ampl;
  }

  template <typename T>
  void Grain<T>::increment(void)
  {
    _carrier.increment();
    _shape.increment();
  }

  template <typename T>
  Grain<T>::operator bool(void) const
  {
    return _shape;
  }

  template <typename T>
  void Grain<T>::setCarrierRate(double rate)
  {
    _carrier.setRate(rate);
  }

  template <typename T>
  void Grain<T>::setShapeRate(double rate)
  {
    _shape.setRate(rate);
  }

  template <typename T>
  void Grain<T>::setAmplitude(T ampl)
  {
    _ampl = ampl;
  }

  template <typename T>
  void Grain<T>::setParams(double crate, double srate, T ampl)
  {
    setCarrierRate(crate);
    setShapeRate(srate);
    setAmplitude(ampl);
  }

  template <typename T>
  void Grain<T>::reset(void)
  {
    _carrier.reset();
    _shape.reset();
  }

  template class Grain<double>;
  template class Grain<float>;
  
}
