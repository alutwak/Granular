
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
  Wavetable<T>::Wavetable(const Waveform<T>& other, double rate, std::size_t len, InterpType it) :
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
  T Wavetable<T>::waveform(double pos) const
  {
    if (pos < 0 || pos > _size-1)
      return 0;
    return interpLinear(pos);
  }
  
  template<typename T>
  typename Waveform<T>::phasor Wavetable<T>::pbegin(double rate, double start) const
  {
    // if (rate<=0.0)
    //   return iend(1);
    auto interp = new interpolator(*this, start, rate);
    return interp;  //Casts to phasor_impl and constructs a phasor
  }

  template<typename T>
  typename Waveform<T>::phasor Wavetable<T>::pbegin(double rate) const
  {
    return pbegin(rate,0);
  }

  template<typename T>
  typename Waveform<T>::phasor Wavetable<T>::rpbegin(double rate, double start) const
  {
    // if (rate<=0.0)
    //   rate = 1;
    auto interp = new interpolator(*this,start,-rate);
    return interp; //Casts to phasor_impl and constructs a phasor
  }

  template<typename T>
  typename Waveform<T>::phasor Wavetable<T>::rpbegin(double rate) const
  {
    return rpbegin(rate,end());
  }
  
  template<typename T>
  Wavetable<T>& Wavetable<T>::operator=(const Wavetable<T>& other)
  {
    alloc(other._size);
    _interptype = other._interptype;
    memcpy(_data,other._data,sizeof(T)*_size);
    return *this;
  }


  template<typename T>
  typename Wavetable<T>::iterator Wavetable<T>::begin(void)
  {
    return iterator(_data);
  }

  template<typename T>
  typename Wavetable<T>::iterator Wavetable<T>::end(void)
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
  Wavetable<T>::interpolator::interpolator(const Wavetable<T>& wt, double start, double rate) :
    ph_im(wt, start, rate), _wt(wt)
  {
    //ph_im::_phase = start/ph_im::_rate;
    setEnd();
  }

  // template<typename T>
  // Wavetable<T>::interpolator::interpolator(const Wavetable<T>& wt, long start_phase, double rate) :
  //   ph_im(wt, start_pos, rate), _wt(wt), _rate(fabs(rate)), _dir(rate < 0 ? -1 : 1)
  // {
  //   setEnd();
  // }

  template<typename T>
  Wavetable<T>::interpolator::interpolator(const Wavetable<T>::interpolator& other) :
    ph_im(other), _wt(other._wt)
  {
    setEnd();
  }

  // template<typename T>
  // typename Wavetable<T>::interpolator& Wavetable<T>::interpolator::operator=(const Wavetable<T>::interpolator& other)
  // {
  //   if (&other == this)
  //     return *this;
  //   _wf = other._wf;
  //   _pos = other._pos;
  //   _rate = other._rate;
  //   _dir = other._dir;
  //   return *this;
  // }

  template<typename T>
  T Wavetable<T>::interpolator::value(void) const
  {
    return _wt.waveform(ph_im::_phase*ph_im::_rate);
  }

  template<typename T>
  Wavetable<T>::interpolator::operator bool(void) const
  {
    return ph_im::_dir > 0 ? ph_im::_phase<_end : ph_im::_phase>_end;
  }

  template<typename T>
  void Wavetable<T>::interpolator::setEnd(void)
  {
    if (ph_im::_dir > 0)
      _end = 1 + (long)((double)(_wt._size-1)/ph_im::_rate);
    else
      _end = -1;
  }

  // template<typename T>
  // void Wavetable<T>::interpolator::increment(void)
  // {
  //   ph_im::_phase+=_dir;
  // }

  template<typename T>
  typename Waveform<T>::phasor_impl* Wavetable<T>::interpolator::copy(void) {
    return new interpolator(*this);
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
}
