
#include <cmath>
#include "waveform.hpp"

namespace audioelectric {
  
  /*********************** Public Waveform *******************************/

  template<typename T>
  Waveform<T>::~Waveform(void) {}
  
  template<typename T>
  typename Waveform<T>::phasor Waveform<T>::pbegin(double rate, double start) const
  {
    return phasor(new phasor_impl(*this,rate,start));
  }

  template<typename T>
  typename Waveform<T>::phasor Waveform<T>::pbegin(double rate) const
  {
    return pbegin(rate,0);
  }

  template<typename T>
  typename Waveform<T>::phasor Waveform<T>::rpbegin(double rate, double start) const
  {
    start = (long)(start/rate); //We need to adjust the interpolated start position
    return phasor(new phasor_impl(*this,-rate,start));
  }

  template<typename T>
  typename Waveform<T>::phasor Waveform<T>::rpbegin(double rate) const
  {
    return rpbegin(rate,end());
  }
  
  /*********************** phasor_impl *******************************/

  template<typename T>
  Waveform<T>::phasor_impl::phasor_impl(const Waveform<T>& wf, double rate, double start) :
    _wf(wf), _phase(0)
  {
    /*
      phase (iteration) = position(samples)/rate(samples/iteration)
     */
    setRate(rate);
    setPosition(start);
  }

  template<typename T>
  Waveform<T>::phasor_impl::phasor_impl(const Waveform<T>::phasor_impl& other) :
    _wf(other._wf), _phase(other._phase), _rate(other._rate), _dir(other._dir)
  {

  }

  template<typename T>
  T Waveform<T>::phasor_impl::value(void) const
  {
    return _wf.waveform(_phase*_rate);
  }

  template<typename T>
  Waveform<T>::phasor_impl::operator bool(void) const
  {
    return true;  //Default will be to return true
  }
  
  template<typename T>
  bool Waveform<T>::phasor_impl::operator==(const Waveform<T>::phasor_impl& other) const
  {
    return (_dir*_phase*_rate)==(other._dir*other._phase*other._rate);
  }

  template<typename T>
  bool Waveform<T>::phasor_impl::operator!=(const Waveform<T>::phasor_impl& other) const
  {
    return !(*this == other);
  }

  template<typename T>
  bool Waveform<T>::phasor_impl::operator<(const Waveform<T>::phasor_impl& other) const
  {
    return (_dir*_phase*_rate) < (other._dir*other._phase*other._rate);
  }
  
  template<typename T>
  bool Waveform<T>::phasor_impl::operator>(const Waveform<T>::phasor_impl& other) const
  {
    return (_dir*_phase*_rate) > (other._dir*other._phase*other._rate);
  }

    template<typename T>
  bool Waveform<T>::phasor_impl::operator<=(const Waveform<T>::phasor_impl& other) const
  {
    return (_dir*_phase*_rate) <= (other._dir*other._phase*other._rate);
  }

  template<typename T>
  bool Waveform<T>::phasor_impl::operator>=(const Waveform<T>::phasor_impl& other) const
  {
    return (_dir*_phase*_rate) >= (other._dir*other._phase*other._rate);
  }

  template<typename T>
  void Waveform<T>::phasor_impl::setRate(double rate)
  {
    double pos = getPosition();
    _rate = fabs(rate);
    _dir =rate < 0 ? -1 : 1;
    setPosition(pos);
  }
  
  template<typename T>
  typename Waveform<T>::phasor_impl* Waveform<T>::phasor_impl::copy(void) {
    return new phasor_impl(*this);
  }
  
  template<typename T>
  void Waveform<T>::phasor_impl::increment(void)
  {
    _phase+=_dir;
  }

  /*********************** varispeed_phasor *******************************/

  template<typename T>
  Waveform<T>::varispeed_phasor::varispeed_phasor(const Waveform<T>& wf, const phasor& rates, double start) :
    Waveform<T>::phasor_impl(wf, *rates, start), _rate_phasor(rates)
  {
    
  }

  template<typename T>
  Waveform<T>::varispeed_phasor::varispeed_phasor(const Waveform<T>::varispeed_phasor& other) :
    Waveform<T>::phasor_impl(other._wf, other._rate, other._phase), _rate_phasor(other._rate_phasor)
  {
    
  }

  template<typename T>
  void Waveform<T>::varispeed_phasor::setRatePhasor(const Waveform<T>::phasor &rates)
  {
    _rate_phasor = rates;
  }

  template<typename T>
  typename Waveform<T>::varispeed_phasor* Waveform<T>::varispeed_phasor::copy(void)
  {
    return new varispeed_phasor(*this);
  }

  template<typename T>
  void Waveform<T>::varispeed_phasor::increment(void)
  {
    Waveform<T>::phasor_impl::increment();
    Waveform<T>::phasor_impl::setRate(*(++_rate_phasor));
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

  /******************** Constant ****************************/

  template<typename T>
  typename Waveform<T>::phasor Constant<T>::pbegin(double rate, double start) const
  {
    return Waveform<T>::make_phasor(new typename Waveform<T>::phasor_impl(*this, rate, start));
  }

  template<typename T>
  typename Waveform<T>::phasor Constant<T>::rpbegin(double rate, double start) const
  {
    start = (long)(start/rate); //We need to adjust the interpolated start position
    return Waveform<T>::make_phasor(new typename Waveform<T>::phasor_impl(*this,-rate,start));
  }
  
  template class Waveform<double>;
  template class Waveform<float>;
  template class Constant<double>;
  template class Constant<float>;

}
