
#include <cmath>
#include "waveform.hpp"

namespace audioelectric {
  
  /*********************** Public Waveform *******************************/

  template<typename T>
  Waveform<T>::~Waveform(void) {}
  
  template<typename T>
  typename Waveform<T>::phasor Waveform<T>::pbegin(double rate, double start)
  {
    return phasor(new phasor_impl(*this,rate,start));
  }

  template<typename T>
  typename Waveform<T>::phasor Waveform<T>::pbegin(double rate)
  {
    return pbegin(rate,0);
  }

  /*********************** phasor_impl *******************************/

  template<typename T>
  Waveform<T>::phasor_impl::phasor_impl(Waveform<T>& wf, double rate, double start) :
    _wf(wf), _phase(start)
  {
    setRate(rate);
  }

  template<typename T>
  Waveform<T>::phasor_impl::phasor_impl(const Waveform<T>::phasor_impl& other) :
    _wf(other._wf), _phase(other._phase), _rate(other._rate)
  {

  }

  template<typename T>
  T Waveform<T>::phasor_impl::value(void) const
  {
    return _wf.waveform(_phase);
  }

  template<typename T>
  Waveform<T>::phasor_impl::operator bool(void) const
  {
    return true;  //Default will be to return true
  }
  
  template<typename T>
  bool Waveform<T>::phasor_impl::operator==(const Waveform<T>::phasor_impl& other) const
  {
    return _phase==other._phase;
  }

  template<typename T>
  bool Waveform<T>::phasor_impl::operator!=(const Waveform<T>::phasor_impl& other) const
  {
    return !(*this == other);
  }

  template<typename T>
  bool Waveform<T>::phasor_impl::operator<(const Waveform<T>::phasor_impl& other) const
  {
    return _phase < other._phase;
  }
  
  template<typename T>
  bool Waveform<T>::phasor_impl::operator>(const Waveform<T>::phasor_impl& other) const
  {
    return _phase > other._phase;
  }

    template<typename T>
  bool Waveform<T>::phasor_impl::operator<=(const Waveform<T>::phasor_impl& other) const
  {
    return _phase <= other._phase;
  }

  template<typename T>
  bool Waveform<T>::phasor_impl::operator>=(const Waveform<T>::phasor_impl& other) const
  {
    return _phase >= other._phase;
  }

  template<typename T>
  void Waveform<T>::phasor_impl::setRate(double rate)
  {
    _rate = rate;
  }
  
  template<typename T>
  typename Waveform<T>::phasor_impl* Waveform<T>::phasor_impl::copy(void) const {
    return new phasor_impl(*this);
  }
  
  template<typename T>
  void Waveform<T>::phasor_impl::increment(void)
  {
    _phase+=_rate;
  }

  /*********************** mod_phasor *******************************/

  template<typename T>
  Waveform<T>::mod_phasor::mod_phasor(Waveform<T>& wf, const phasor& rates, double start) :
    Waveform<T>::phasor_impl(wf, *rates, start), _modulator(rates)
  {
    
  }

  template<typename T>
  Waveform<T>::mod_phasor::mod_phasor(const Waveform<T>::mod_phasor& other) :
    Waveform<T>::phasor_impl(other._wf, other._rate, other._phase), _modulator(other._modulator)
  {
    
  }

  template<typename T>
  void Waveform<T>::mod_phasor::setModulator(const Waveform<T>::phasor &rates)
  {
    _modulator = rates;
  }

  template<typename T>
  typename Waveform<T>::mod_phasor* Waveform<T>::mod_phasor::copy(void) const
  {
    return new mod_phasor(*this);
  }

  template<typename T>
  void Waveform<T>::mod_phasor::increment(void)
  {
    Waveform<T>::phasor_impl::increment();
    Waveform<T>::phasor_impl::setRate(*(++_modulator));
  }
  
  /*********************** phasor *******************************/

  template<typename T>
  Waveform<T>::phasor::phasor(void) : _impl(nullptr)
  {

  }

  template<typename T>
  Waveform<T>::phasor::phasor(phasor_impl *impl) : _impl(impl)
  {

  }

  template<typename T>
  Waveform<T>::phasor::phasor(const Waveform<T>::phasor& other) : _impl(nullptr)
  {
    _impl.reset(other._impl->copy());
  }

  template<typename T>
  typename Waveform<T>::phasor& Waveform<T>::phasor::operator=(const Waveform<T>::phasor& other)
  {
    if (this == &other)
      return *this;
    _impl.reset(other._impl->copy());
    return *this;
  }

  template<typename T>
  typename Waveform<T>::phasor& Waveform<T>::phasor::operator++(void)
  {
    _impl->increment();
    return *this;
  }
  
  template<typename T>
  typename Waveform<T>::phasor Waveform<T>::phasor::operator++(int)
  {
    auto old = *this;
    _impl->increment();
    return old;
  }

  template<typename T>
  T Waveform<T>::phasor::operator*(void) const
  {
    return _impl->value();
  }

  template<typename T>
  Waveform<T>::phasor::operator bool(void) const
  {
    return _impl && *_impl;  //First make sure we have an _impl, then get its boo value
  }
  
  template<typename T>
  bool Waveform<T>::phasor::operator==(const Waveform<T>::phasor& other) const
  {
    return *_impl == *other._impl;
  }

  template<typename T>
  bool Waveform<T>::phasor::operator!=(const Waveform<T>::phasor& other) const
  {
    return !(*this == other);
  }

  template<typename T>
  bool Waveform<T>::phasor::operator<(const Waveform<T>::phasor& other) const
  {
    return *_impl < *other._impl;
  }
  
  template<typename T>
  bool Waveform<T>::phasor::operator>(const Waveform<T>::phasor& other) const
  {
    return *_impl > *other._impl;
  }

    template<typename T>
  bool Waveform<T>::phasor::operator<=(const Waveform<T>::phasor& other) const
  {
    return *_impl <= *other._impl;
  }

  template<typename T>
  bool Waveform<T>::phasor::operator>=(const Waveform<T>::phasor& other) const
  {
    return *_impl >= *other._impl;
  }

  template class Waveform<double>;
  template class Waveform<float>;

}
