
#include "phasor.hpp"

namespace audioelectric {

  template<typename T>
  Phasor<T>::Phasor(Waveform<T>& wf, double rate, bool cycle, double start, double begin, double end) :
    _wf(wf), _phase(start), _cycle(cycle), _begin(begin)
  {
    setRate(rate);
    _setEnd(end);
    if (!_checkPhase(_phase))
      _phase = _begin;
    _phase_good = true;
  }

  template<typename T>
  Phasor<T>::Phasor(const Phasor& other) :
    _wf(other._wf), _phase(other._phase), _rate(other._rate), _begin(other._begin), _cycle(other._cycle)
  {
    _setEnd(other._end);
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
  bool Phasor<T>::_checkPhase(double phase) const
  {
    return phase>=_begin && phase<=_end;
  }

  template<typename T>
  void Phasor<T>::_setEnd(long end)
  {
    if (end>=0)
      _end = end;
    else
      _end = (double)(_wf.size()-1);
    _phase_good = _checkPhase(_phase);
  }

  template<typename T>
  void Phasor<T>::increment(void)
  {
    double nextphase = _phase+_rate;
    if (_cycle && !_checkPhase(nextphase)) {
      // We've reached the end of the waveform, cycle around
      // No need to set _phase_good because it's always good when we cycle
      if (_rate > 0)
        _phase = _begin + nextphase-_end;
      else
        _phase = _end - (_begin - nextphase);
    }
    else {
      _phase = nextphase;
      _phase_good = _checkPhase(_phase);
    }
      
  }

  template <typename T>
  void Phasor<T>::reset(void)
  {
    _phase = _begin;
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

  template<typename T>
  void Phasor<T>::setRate(double rate)
  {
    _rate = rate;
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

  template class Phasor<double>;
  template class Phasor<float>;
  
}
