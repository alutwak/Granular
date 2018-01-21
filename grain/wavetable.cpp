
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
      *p = *itr;
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
  typename Waveform<T>::phasor Wavetable<T>::pbegin(double start, double rate) const
  {
    // if (rate<=0.0)
    //   return iend(1);
    auto interp = new interpolator(*this, start, rate);
    return interp;  //Casts to phasor_impl and constructs a phasor
  }

  template<typename T>
  typename Waveform<T>::phasor Wavetable<T>::pbegin(double rate) const
  {
    return pbegin(0,rate);
  }

  template<typename T>
  typename Waveform<T>::phasor Wavetable<T>::rpbegin(double start, double rate) const
  {
    // if (rate<=0.0)
    //   rate = 1;
    start = start/rate; //We need to adjust the interpolated start position
    auto interp = new interpolator(*this,start,-rate);
    return interp; //Casts to phasor_impl and constructs a phasor
  }

  template<typename T>
  typename Waveform<T>::phasor Wavetable<T>::rpbegin(double rate) const
  {
    return rpbegin(_size-1,rate);
  }
  
  template<typename T>
  Wavetable<T>& Wavetable<T>::operator=(const Wavetable<T>& other)
  {
    alloc(other._size);
    _interptype = other._interptype;
    memcpy(_data,other._data,sizeof(T)*_size);
    return *this;
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
    Waveform<T>::phasor_impl(wt, start, rate), _wt(wt), _rate(fabs(rate)), _dir(rate < 0 ? -1 : 1)
  {
    _pos = start/_rate;
    setEnd();
  }

  template<typename T>
  Wavetable<T>::interpolator::interpolator(const Wavetable<T>& wt, long start_pos, double rate) :
    Waveform<T>::phasor_impl(wt, start_pos, rate), _wt(wt), _rate(fabs(rate)), _dir(rate < 0 ? -1 : 1), _pos(start_pos)
  {
    setEnd();
  }

  template<typename T>
  Wavetable<T>::interpolator::interpolator(const Wavetable<T>::interpolator& other) :
    Waveform<T>::phasor_impl(other), _wt(other._wt), _pos(other._pos), _rate(other._rate), _dir(other._dir)
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
  T Wavetable<T>::interpolator::operator*(void) const
  {
    return _wt.waveform(_pos*_rate);
  }

  template<typename T>
  Wavetable<T>::interpolator::operator bool(void) const
  {
    return _dir > 0 ? _pos<_end : _pos>_end;
  }

  template<typename T>
  void Wavetable<T>::interpolator::setEnd(void)
  {
    if (_dir > 0)
      _end = 1 + (long)((double)(_wt._size-1)/_rate);
    else
      _end = -1;
  }

  template<typename T>
  void Wavetable<T>::interpolator::increment(void)
  {
    _pos+=_dir;
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
}
