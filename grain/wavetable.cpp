
#include <cmath>
#include "wavetable.hpp"

namespace audioelectric {

  template<typename T>
  T doInterpolate(const T* a, double offset, InterpType it);
  
  /*********************** Public Wavetable *******************************/

  template<typename T>
  Wavetable<T>::Wavetable(void) : _interptype(InterpType::LINEAR), _data(nullptr), _size(0)
  {
    
  }

  template<typename T>
  Wavetable<T>::Wavetable(std::size_t len, InterpType it) : _interptype(it), _data(nullptr), _size(0)
  {
    alloc(len);
    memset(_data, 0, sizeof(T)*len);
  }
  
  template<typename T>
  Wavetable<T>::Wavetable(T* data, std::size_t len, InterpType it) : _interptype(it), _data(nullptr), _size(len)
  {
    alloc(len);
    memcpy(_data, data, sizeof(T)*len);
  }

  template<typename T>
  Wavetable<T>::Wavetable(std::initializer_list<T> init, InterpType it) : _interptype(it), _data(nullptr), _size(0)
  {
    alloc(init.size());
    T* p = _data;
    for (auto& val : init)
      *(p++) = val;
  }

  template<typename T>
  Wavetable<T>::Wavetable(Waveform<T>& other, double rate, std::size_t len, InterpType it) :
    _interptype(it), _data(nullptr), _size(0)
  {
    alloc(len);
    auto itr = other.pbegin(rate);
    T* p = _data;
    while(itr && (len-- > 0))
      *p++ = *itr++;
  }

  template<typename T>
  Wavetable<T>::~Wavetable(void)
  {
    dealloc();
  }

  template<typename T>
  T Wavetable<T>::waveform(double pos)
  {
    if (pos < 0 || pos > _size-1)
      return 0;
    return interpLinear(pos);
  }
  
  template<typename T>
  typename Waveform<T>::phasor Wavetable<T>::pbegin(double rate, double start, double begin, double end, bool cycle)
  {
    auto interp = new interpolator(*this, rate, start, begin, end, cycle);
    return Waveform<T>::make_phasor(interp);  //Casts to phasor_impl and constructs a phasor
  }

  template<typename T>
  typename Waveform<T>::phasor Wavetable<T>::pbegin(const dphasor& modulator, double start,
                                                    double begin, double end, bool cycle)
  {
    auto interp = new mod_interpolator(*this, modulator, start, begin, end, cycle);
    return Waveform<T>::make_phasor(interp);  //Casts to phasor_impl and constructs a phasor
  }

  
  template<typename T>
  typename Waveform<T>::phasor Wavetable<T>::pbegin(double rate)
  {
    return pbegin(rate,0);
  }

  template<typename T>
  typename Waveform<T>::phasor Wavetable<T>::pbegin(const dphasor& modulator)
  {
    return pbegin(modulator,0);
  }
  
  // template<typename T>
  // typename Waveform<T>::phasor Wavetable<T>::rpbegin(double rate, double start, double end, bool cycle) const
  // {
  //   auto interp = new interpolator(*this, -rate, start, end, cycle);
  //   return Waveform<T>::make_phasor(interp); //Casts to phasor_impl and constructs a phasor
  // }

  // template<typename T>
  // typename Waveform<T>::phasor Wavetable<T>::rpbegin(double rate) const
  // {
  //   return rpbegin(rate,end());
  // }
  
  template<typename T>
  Wavetable<T>& Wavetable<T>::operator=(const Wavetable<T>& other)
  {
    alloc(other._size);
    _interptype = other._interptype;
    memcpy(_data,other._data,sizeof(T)*_size);
    return *this;
  }


  template<typename T>
  typename Wavetable<T>::iterator Wavetable<T>::ibegin(void)
  {
    return iterator(_data);
  }

  template<typename T>
  typename Wavetable<T>::iterator Wavetable<T>::iend(void)
  {
    return iterator(_data+_size);
  }

  template<typename T>
  void Wavetable<T>::resize(std::size_t len) {
    if (len == _size)
      return;
    alloc(len);
  }
  
  /*********************** Private Wavetable *******************************/

  template<typename T>
  void Wavetable<T>::alloc(std::size_t len)
  {
    dealloc();
    _data = new T[len];
    _size = len;
  }

  template<typename T>
  void Wavetable<T>::dealloc(void)
  {
    if (_data)
      delete[] _data;
    _size = 0;
  }

  template<typename T>
  T Wavetable<T>::interpLinear(double pos) const
  {
    long p = pos;
    T a = _data[p];
    T b = _data[p+1];
    double diff = fmod(pos,1.0);
    return (b-a)*diff + a;
  }
  
  /*********************** interpolator *******************************/

  template<typename T>
  Wavetable<T>::interpolator::interpolator(Wavetable<T>& wt, double rate, double start,
                                           double begin, double end, bool cycle) :
    ph_im(wt, rate, start), _cycle(cycle), _begin(begin)
  {
    //_begin = ph_im::_phase;
    setEnd(end);
  }

  template<typename T>
  Wavetable<T>::interpolator::interpolator(const Wavetable<T>::interpolator& other) :
    ph_im(other), _cycle(other._cycle), _begin(other._begin)
  {
    setEnd(other._end);
  }

  template<typename T>
  Wavetable<T>::interpolator::operator bool(void) const
  {
    return checkPhase(ph_im::_phase);
  }

  template<typename T>
  void Wavetable<T>::interpolator::setRate(double rate)
  {
    ph_im::setRate(rate);
  }

  template<typename T>
  bool Wavetable<T>::interpolator::checkPhase(double phase) const
  {
    return phase>=_begin && phase<=_end;
  }

  template<typename T>
  void Wavetable<T>::interpolator::setEnd(long end)
  {
    if (end>=0)
      _end = end;
    else
      _end = (double)(ph_im::_wf.size()-1);
  }

  template<typename T>
  void Wavetable<T>::interpolator::increment(void)
  {
    double nextphase = ph_im::_phase+ph_im::_rate;
    if (_cycle && !checkPhase(nextphase)) {
      //We've reached the end of the waveform, cycle around
      //I'd love to use fmod here, but fmod is crap for negative values
      if (ph_im::_rate > 0)
        ph_im::_phase = _begin + nextphase-_end;
      else
        ph_im::_phase = _end - (_begin - nextphase);
    }
    else
      ph_im::_phase = nextphase;
  }

  template<typename T>
  typename Waveform<T>::phasor_impl* Wavetable<T>::interpolator::copy(void) const {
    return new interpolator(*this);
  }

  /********************* mod_interpolator ********************/

  template<typename T>
  Wavetable<T>::mod_interpolator::mod_interpolator(Wavetable<T>& wt, const dphasor& modulator,
                                                   double start, double begin, double end, bool cycle) :
    Wavetable<T>::interpolator(wt, *modulator, start, begin, end, cycle), _modulator(modulator)
  {
  }

  template<typename T>
  Wavetable<T>::mod_interpolator::mod_interpolator(const Wavetable<T>::mod_interpolator& other) :
    Wavetable<T>::interpolator(other), _modulator(other._modulator)
  {
  }

  template<typename T>
  void Wavetable<T>::mod_interpolator::setModulator(const dphasor& modulator)
  {
    _modulator = modulator;
  }

  template<typename T>
  typename Wavetable<T>::mod_interpolator* Wavetable<T>::mod_interpolator::copy(void) const
  {
    return new mod_interpolator(*this);
  }
  
  template<typename T>
  void Wavetable<T>::mod_interpolator::increment(void)
  {
    Wavetable<T>::interpolator::increment();
    ph_im::setRate(*(++_modulator));
  }
  
  /********************* iterator ********************/
  
  template<typename T>
  typename Wavetable<T>::iterator& Wavetable<T>::iterator::operator++(void)
  {
    _data++;
    return *this;
  }

  template<typename T>
  typename Wavetable<T>::iterator Wavetable<T>::iterator::operator++(int)
  {
    iterator temp = *this;
    _data++;
    return temp;
  }

  template<typename T>
  T& Wavetable<T>::iterator::operator*(void)
  {
    return *_data;
  }

  template<typename T>
  bool Wavetable<T>::iterator::operator==(const Wavetable<T>::iterator& other) const
  {
    return _data == other._data;
  }
  
  template<typename T>
  bool Wavetable<T>::iterator::operator!=(const Wavetable<T>::iterator& other) const
  {
    return _data != other._data;
  }

  template class Wavetable<double>;
  template class Wavetable<float>;

  template<typename T>
  void GenerateGaussian(Wavetable<T> *wt, std::size_t len, T sigma) {
    wt->resize(len);
    T flen = len;
    T mid = -flen/2;
    T sigma_norm = -2*(sigma*flen)*(sigma*flen);
    T offset = exp(mid*mid/sigma_norm);
    T norm = 1.0/(1.0-offset);
    T* data = wt->data();
    for (std::size_t i=0; i<len; i++) {
      mid += 1.0;
      data[i] = norm*(exp(mid*mid/sigma_norm)-offset);
    }
  }

  template void GenerateGaussian<float>(Wavetable<float>*, std::size_t, float);
  template void GenerateGaussian<double>(Wavetable<double>*, std::size_t, double);  
}
