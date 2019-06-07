
#include "phasor.hpp"

namespace audioelectric {

  template<typename T>
  Phasor<T>::Phasor(Waveform<T>& wf, double rate, double start, double begin, double end, bool cycle) :
    _wf(wf), _phase(start), _cycle(cycle), _begin(begin)
  {
    setRate(rate);
    setEnd(end);
  }

  template<typename T>
  Phasor<T>::Phasor(const Phasor& other) :
    _wf(other._wf), _phase(other._phase), _rate(other._rate), _begin(other._begin)
  {
    setEnd(other._end);
  }

  template<typename T>
  T Phasor<T>::value(void) const
  {
    return _wf.waveform(_phase);
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
    return checkPhase(_phase);
  }

  template<typename T>
  bool Phasor<T>::checkPhase(double phase) const
  {
    return phase>=_begin && phase<=_end;
  }

  template<typename T>
  void Phasor<T>::setEnd(long end)
  {
    if (end>=0)
      _end = end;
    else
      _end = (double)(_wf.size()-1);
  }

  template<typename T>
  void Phasor<T>::increment(void)
  {
    double nextphase = _phase+_rate;
    if (_cycle && !checkPhase(nextphase)) {
      //We've reached the end of the waveform, cycle around
      //I'd love to use fmod here, but fmod is crap for negative values
      if (_rate > 0)
        _phase = _begin + nextphase-_end;
      else
        _phase = _end - (_begin - nextphase);
    }
    else
      _phase = nextphase;
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
  Phasor<T>* Phasor<T>::copy(void) const {
    return new Phasor(*this);
  }
  
  template<typename T>
  Phasor<T>& Phasor<T>::operator=(const Phasor& other)
  {
    if (this == &other)
      return *this;
    _rate = other._rate;
    _phase = other._phase;
    _wf = other._wf;
    return *this;
  }

  template class Phasor<double>;
  template class Phasor<float>;
  
}
