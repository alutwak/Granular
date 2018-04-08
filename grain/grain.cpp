
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

  template<typename T>
  typename Waveform<T>::phasor Grain<T>::gmake(double start, double rate,
                                               const typename Waveform<T>::phasor& phasor_other) const

  {
    auto granu = new granulator(*this,start,rate,phasor_other);
    return Waveform<T>::make_phasor(granu);
  }

  template<typename T>
  typename Waveform<T>::phasor Grain<T>::rgmake(double start, double rate,
                                                const typename Waveform<T>::phasor& phasor_other) const
  {
    auto granu = new granulator(*this,start,-rate,phasor_other);
    return Waveform<T>::make_phasor(granu);
  }

  /******************** granulator ********************/

  template<typename T>
  Grain<T>::granulator::granulator(const Grain<T>& grn, double start, double rate,
                                   const typename Waveform<T>::phasor& interp_other) :
    Wavetable<T>::interpolator(grn,rate,start,start), _phasor_other(interp_other)
  {
    
  }

  template<typename T>
  Grain<T>::granulator::granulator(const granulator& other) :
    Wavetable<T>::interpolator(other), _phasor_other(other._phasor_other)
  {
    
  }
  
  template<typename T>
  T Grain<T>::granulator::value(void) const
  {
    return Wavetable<T>::interpolator::value()*(*_phasor_other);
  }

  template<typename T>
  Grain<T>::granulator::operator bool(void) const
  {
    return Wavetable<T>::interpolator::operator bool() && _phasor_other;
  }

  template<typename T>
  typename Waveform<T>::phasor_impl* Grain<T>::granulator::copy(void) const
  {
    return new granulator(*this);
  }
  
  template<typename T>
  void Grain<T>::granulator::increment(void)
  {
    Waveform<T>::phasor_impl::increment();
    _phasor_other++;
  }

  template class Grain<double>;
  template class Grain<float>;
  
}
