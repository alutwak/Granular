
#include <cmath>
#include "waveform.hpp"

namespace audioelectric {
  
  /*********************** Public Waveform *******************************/

  template<typename T>
  typename std::unique_ptr<typename Waveform<T>::phasor> Waveform<T>::pbegin(double start, double rate) const
  {
    // if (rate<=0.0)
    //   return iend(1);
    return std::make_unique<phasor>(*this,start,rate);
  }

  template<typename T>
  typename std::unique_ptr<typename Waveform<T>::phasor> Waveform<T>::pbegin(double rate) const
  {
    return pbegin(0,rate);
  }

  template<typename T>
  typename std::unique_ptr<typename Waveform<T>::phasor> Waveform<T>::rpbegin(double start, double rate) const
  {
    // if (rate<=0.0)
    //   rate = 1;
    start = (long)(start/rate); //We need to adjust the interpolated start position
    return std::make_unique<phasor>(*this,start,-rate);
  }

  template<typename T>
  typename std::unique_ptr<typename Waveform<T>::phasor> Waveform<T>::rpbegin(double rate) const
  {
    return rpbegin(0,rate);
  }
  
  /*********************** phasor *******************************/

  template<typename T>
  Waveform<T>::phasor::phasor(void) :
    _wf(nullptr), _rate(1), _dir(1), _phase(0)
  {
    
  }
  
  template<typename T>
  Waveform<T>::phasor::phasor(const Waveform<T>& wf, double start, double rate) :
    _wf(&wf), _rate(fabs(rate)), _dir(rate < 0 ? -1 : 1)
  {
    _phase = start/_rate;
  }

  template<typename T>
  Waveform<T>::phasor::phasor(const Waveform<T>& wf, long start_phase, double rate) :
    _wf(&wf), _rate(fabs(rate)), _dir(rate < 0 ? -1 : 1), _phase(start_phase)
  {

  }

  template<typename T>
  Waveform<T>::phasor::phasor(const Waveform<T>::phasor& other) :
    _wf(other._wf), _phase(other._phase), _rate(other._rate), _dir(other._dir)
  {

  }

  template<typename T>
  typename Waveform<T>::phasor& Waveform<T>::phasor::operator=(const Waveform<T>::phasor& other)
  {
    if (&other == this)
      return *this;
    _wf = other._wf;
    _phase = other._phase;
    _rate = other._rate;
    _dir = other._dir;
    return *this;
  }

  template<typename T>
  typename Waveform<T>::phasor& Waveform<T>::phasor::operator++(void)
  {
    increment();
    return *this;
  }
  
  template<typename T>
  typename Waveform<T>::phasor Waveform<T>::phasor::operator++(int)
  {
    auto old = *this;
    increment();
    return old;
  }

  template<typename T>
  typename Waveform<T>::phasor Waveform<T>::phasor::operator+(long n) const
  {
    return phasor(*_wf, _phase+n, _dir*_rate);
  }

  template<typename T>
  T Waveform<T>::phasor::operator*(void) const
  {
    return _wf->waveform(_phase*_rate);
  }

  template<typename T>
  Waveform<T>::phasor::operator bool(void) const
  {
    return true;  //Default will be to return true
  }
  
  // template<typename T>
  // typename Waveform<T>::phasor Waveform<T>::phasor::operator-(std::size_t n) const
  // {
  //   return phasor(_wf, _loc-(_rate*n), _rate);
  // }

  template<typename T>
  bool Waveform<T>::phasor::operator==(const Waveform<T>::phasor& other) const
  {
    return _wf == other._wf && _phase==other._phase;
  }

  template<typename T>
  bool Waveform<T>::phasor::operator!=(const Waveform<T>::phasor& other) const
  {
    return !(*this == other);
  }

  template<typename T>
  bool Waveform<T>::phasor::operator<(const Waveform<T>::phasor& other) const
  {
    return (_dir*_phase*_rate) < (other._dir*other._phase*other._rate);
  }
  
  template<typename T>
  bool Waveform<T>::phasor::operator>(const Waveform<T>::phasor& other) const
  {
    return (_dir*_phase*_rate) > (other._dir*other._phase*other._rate);
  }

    template<typename T>
  bool Waveform<T>::phasor::operator<=(const Waveform<T>::phasor& other) const
  {
    return (_dir*_phase*_rate) <= (other._dir*other._phase*other._rate);
  }

  template<typename T>
  bool Waveform<T>::phasor::operator>=(const Waveform<T>::phasor& other) const
  {
    return (_dir*_phase*_rate) >= (other._dir*other._phase*other._rate);
  }
  
  template<typename T>
  void Waveform<T>::phasor::increment(void)
  {
    _phase+=_dir;
  }

  template class Waveform<double>;
  template class Waveform<float>;

}
