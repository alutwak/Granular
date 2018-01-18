
#include "grain.hpp"

namespace audioelectric {

  template<typename T>
  Grain<T>::Grain(void) : Waveform<T>()
  {
    
  }

  template<typename T>
  Grain<T>::Grain(std::size_t len, InterpType it) : Waveform<T>(len, it)
  {
    
  }

  template<typename T>
  Grain<T>::Grain(T* data, std::size_t len, InterpType it) : Waveform<T>(data, len, it)
  {
    
  }

  template<typename T>
  Grain<T>::Grain(std::initializer_list<T> init, InterpType it) : Waveform<T>(init, it)
  {
    
  }

  template<typename T>
  Grain<T>::Grain(const Waveform<T>& other, std::size_t len, InterpType it) : Waveform<T>(other, len, it)
  {
    
  }

  // template<typename T>
  // Grain<T>::~Grain(void)
  // {
  //   ;
  // }

  template<typename T>
  typename Grain<T>::granulator Grain<T>::gmake(long start, double speed,
                                                const typename Waveform<T>::interpolator& interp_other) const

  {
    return granulator(
      Waveform<T>::ibegin(start, speed),
      interp_other
      );
  }

  template<typename T>
  typename Grain<T>::granulator Grain<T>::rgmake(long start, double speed,
                                                 const typename Waveform<T>::interpolator& interp_other) const
  {
    return granulator(
      Waveform<T>::ribegin(start, speed),
      interp_other
      );
  }

  /******************** granulator ********************/

  template<typename T>
  Grain<T>::granulator::granulator(const typename Waveform<T>::interpolator& interp_this,
                                   const typename Waveform<T>::interpolator& interp_other) :
    _interp_this(interp_this), _interp_other(interp_other)
  {
    
  }

  template<typename T>
  Grain<T>::granulator::granulator(const granulator& other) :
    _interp_this(other._interp_this), _interp_other(other._interp_other)
  {
    
  }

  template<typename T>
  typename Grain<T>::granulator& Grain<T>::granulator::operator=(const granulator& other)
  {
    if (&other == this)
      return *this;
    _interp_this = other._interp_this;
    _interp_other = other._interp_other;
    return *this;
  }

  template<typename T>
  typename Grain<T>::granulator& Grain<T>::granulator::operator++(void)
  {
    _interp_this++;
    _interp_other++;
    return *this;
  }

  template<typename T>
  typename Grain<T>::granulator Grain<T>::granulator::operator++(int)
  {
    auto temp = *this;
    _interp_this++;
    _interp_other++;
    return temp;
  }
  
  template<typename T>
  T Grain<T>::granulator::operator*(void) const
  {
    return *_interp_this*(*_interp_other);
  }

  template<typename T>
  Grain<T>::granulator::operator bool(void) const
  {
    return _interp_this && _interp_other;
  }

  template class Grain<double>;
  template class Grain<float>;
  
}
