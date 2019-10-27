
#include <cmath>

#include "phasor.hpp"

namespace audioelectric {

  template<typename T>
  Phasor<T>::Phasor(Waveform<T>& wf, double rate, bool cycle, double start, double front, double back) :
    _wf(wf), _cycle(cycle)
  {
    setParameters(rate, start, front, back);
  }

  template<typename T>
  Phasor<T>::Phasor(const Phasor& other) :
    _wf(other._wf), _phase(other._phase), _rate(other._rate), _front(other._front), _cycle(other._cycle), _back(other._back)
  {
    _phase_good = _checkPhase(_phase);
  }

  template<typename T>
  T Phasor<T>::value(void) const
  {
    if (_phase_good)
      return _wf.waveform(_phase);
    return 0;
  }

  template<typename T>
  bool Phasor<T>::generate(T **outputs, int frames, int chans)
  {
    for (int frame=0; frame<frames; frame++) {
      for (int chan=0; chan<chans; chan++)
        outputs[chan][frame] = _wf.waveform(_phase,chan);
      this->increment();
    }
    return *this;
  }

  template<typename T>
  Phasor<T>::operator bool(void) const
  {
    return _phase_good;
  }

  template<typename T>
  void Phasor<T>::increment(void)
  {
    double nextphase = _phase+_rate;
    bool good = _checkPhase(nextphase);
    if (_cycle && !good) {
      // We've reached the back of the waveform, cycle around
      if (_rate > 0)
        _phase = fmod(nextphase - _front, _back - _front) + _front;
      else
        _phase = _back - fmod(_back - nextphase, _back - _front);
      _phase_good = true;
    }
    else {
      _phase = nextphase;
      _phase_good = good;
    }

  }

  template <typename T>
  void Phasor<T>::reset(void)
  {
    _phase = _front;
    _phase_good = true;
  }


  template<typename T>
  bool Phasor<T>::operator==(const Phasor& other) const
  {
    return _phase==other._phase;
  }

  template<typename T>
  bool Phasor<T>::operator!=(const Phasor& other) const
  {
    return !(*this == other);
  }

  template<typename T>
  bool Phasor<T>::operator<(const Phasor& other) const
  {
    return _phase < other._phase;
  }

  template<typename T>
  bool Phasor<T>::operator>(const Phasor& other) const
  {
    return _phase > other._phase;
  }

  template<typename T>
  bool Phasor<T>::operator<=(const Phasor& other) const
  {
    return _phase <= other._phase;
  }

  template<typename T>
  bool Phasor<T>::operator>=(const Phasor& other) const
  {
    return _phase >= other._phase;
  }

  template <typename T>
  void Phasor<T>::setParameters(double rate, double phase, double front, double back)
  {
    _rate = rate;
    _phase = phase;
    setFront(front);
    setBack(back);
    // if (!_phase_good)   // _phase_good was checked by setBack()
    //   _phase = _front;
    _phase_good = true;
  }
  
  template<typename T>
  void Phasor<T>::setRate(double rate)
  {
    _rate = rate;
  }

  template<typename T>
  void Phasor<T>::setFront(double front)
  {
    _front = front > 0 ? front : 0;
    _phase_good = _checkPhase(_phase);
  }

  template<typename T>
  void Phasor<T>::setPhase(double phase)
  {
    _phase = phase;
    _phase_good = _checkPhase(_phase);
  }

  template<typename T>
  void Phasor<T>::setBack(double back)
  {
    if (back>=0)
      _back = back < _wf.end() ? back : _wf.end();
    else
      _back = (double)(_wf.end());
    _phase_good = _checkPhase(_phase);
  }

  template <typename T>
  void Phasor<T>::setCycle(bool cycle)
  {
    _cycle = cycle;
  }

  template<typename T>
  Phasor<T>& Phasor<T>::operator=(const Phasor& other)
  {
    if (this == &other)
      return *this;
    _rate = other._rate;
    _phase = other._phase;
    _wf = other._wf;
    _phase_good = other._phase_good;
    return *this;
  }

  template<typename T>
  bool Phasor<T>::_checkPhase(double phase) const
  {
    return phase <= _back && phase>=_front;
  }

  template class Phasor<double>;
  template class Phasor<float>;

}
