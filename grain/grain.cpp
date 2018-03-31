
#include "grain.hpp"

namespace audioelectric {

  template<typename T>
  Grain<T>::Grain(void) : Wavetable<T>()
  {
    
  }

  template<typename T>
  Grain<T>::Grain(std::size_t len, InterpType it) : Wavetable<T>(len, it)
  {
    
  }

  template<typename T>
  Grain<T>::Grain(T* data, std::size_t len, InterpType it) : Wavetable<T>(data, len, it)
  {
    
  }

  template<typename T>
  Grain<T>::Grain(std::initializer_list<T> init, InterpType it) : Wavetable<T>(init, it)
  {
    
  }

  template<typename T>
  Grain<T>::Grain(const Waveform<T>& other, double rate, std::size_t len, InterpType it) : Wavetable<T>(other, rate, len, it)
  {
    
  }

  // template<typename T>
  // Grain<T>::~Grain(void)
  // {
  //   ;
  // }

  template<typename T>
  typename Grain<T>::granulator Grain<T>::gmake(long start, double rate,
                                                const typename Waveform<T>::phasor& phasor_other) const

  {
    return granulator(
      Wavetable<T>::pbegin(start, rate),
      phasor_other
      );
  }

  template<typename T>
  typename Grain<T>::granulator Grain<T>::rgmake(long start, double rate,
                                                 const typename Waveform<T>::phasor& phasor_other) const
  {
    return granulator(
      Wavetable<T>::rpbegin(start, rate),
      phasor_other
      );
  }

  /******************** granulator ********************/

  template<typename T>
  Grain<T>::granulator::granulator(const typename Waveform<T>::phasor& interp_this,
                                   const typename Waveform<T>::phasor& interp_other) :
    _phasor_this(interp_this), _phasor_other(interp_other)
  {
    
  }

  template<typename T>
  Grain<T>::granulator::granulator(const granulator& other) :
    _phasor_this(other._phasor_this), _phasor_other(other._phasor_other)
  {
    
  }

  template<typename T>
  typename Grain<T>::granulator& Grain<T>::granulator::operator=(const granulator& other)
  {
    if (&other == this)
      return *this;
    _phasor_this = other._phasor_this;
    _phasor_other = other._phasor_other;
    return *this;
  }

  template<typename T>
  typename Grain<T>::granulator& Grain<T>::granulator::operator++(void)
  {
    _phasor_this++;
    _phasor_other++;
    return *this;
  }

  template<typename T>
  typename Grain<T>::granulator Grain<T>::granulator::operator++(int)
  {
    auto temp = *this;
    _phasor_this++;
    _phasor_other++;
    return temp;
  }
  
  template<typename T>
  T Grain<T>::granulator::operator*(void) const
  {
    return *_phasor_this*(*_phasor_other);
  }

  template<typename T>
  Grain<T>::granulator::operator bool(void) const
  {
    return _phasor_this && _phasor_other;
  }

  template class Grain<double>;
  template class Grain<float>;
  
}
