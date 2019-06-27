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
    _carrier(carrier, crate), _shape(shape, srate), _ampl(ampl)
  {
    
  }

  template <typename T>
  T Grain<T>::value(void)
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
    return _carrier && _shape;
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
  void Grain<T>::reset(void)
  {
    _carrier.reset();
    _shape.reset();
  }
}
