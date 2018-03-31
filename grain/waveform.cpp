
#include <cmath>
#include "waveform.hpp"

namespace audioelectric {
  
  /*********************** Public Waveform *******************************/

  template<typename T>
  Waveform<T>::~Waveform(void) {}
  
  template<typename T>
  typename Waveform<T>::phasor Waveform<T>::pbegin(double start, double rate) const
  {
    // if (rate<=0.0)
    //   return iend(1);
    return phasor(new phasor_impl(*this,start,rate));
  }

  template<typename T>
  typename Waveform<T>::phasor Waveform<T>::pbegin(double rate) const
  {
    return pbegin(0,rate);
  }

  template<typename T>
  typename Waveform<T>::phasor Waveform<T>::rpbegin(double start, double rate) const
  {
    // if (rate<=0.0)
    //   rate = 1;
    start = (long)(start/rate); //We need to adjust the interpolated start position
    return phasor(new phasor_impl(*this,start,-rate));
  }

  template<typename T>
  typename Waveform<T>::phasor Waveform<T>::rpbegin(double rate) const
  {
    return rpbegin(0,rate);
  }
  
  /*********************** phasor_impl *******************************/

  template<typename T>
  Waveform<T>::phasor_impl::phasor_impl(const Waveform<T>& wf, double start, double rate) :
    _wf(wf), _rate(fabs(rate)), _dir(rate < 0 ? -1 : 1)
  {
    /*
      phase (iteration) = position(samples)/rate(samples/iteration)
     */
    _phase = start/_rate;
  }

  template<typename T>
  Waveform<T>::phasor_impl::phasor_impl(const Waveform<T>::phasor_impl& other) :
    _wf(other._wf), _phase(other._phase), _rate(other._rate), _dir(other._dir)
  {

  }

  // template<typename T>
  // typename Waveform<T>::phasor_impl& Waveform<T>::phasor_impl::operator=(const Waveform<T>::phasor_impl& other)
  // {
  //   if (&other == this)
  //     return *this;
  //   _wf = other._wf;
  //   _phase = other._phase;
  //   _rate = other._rate;
  //   _dir = other._dir;
  //   return *this;
  // }

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
    return _phase==other._phase; //_wf == other._wf (let's not worry about this right now)
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
  typename Waveform<T>::phasor_impl* Waveform<T>::phasor_impl::copy(void) {
    return new phasor_impl(*this);
  }
  
  template<typename T>
  void Waveform<T>::phasor_impl::increment(void)
  {
    _phase+=_dir;
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
    return *_impl;
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
