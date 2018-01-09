
#include <cmath>
#include "waveform.hpp"

namespace audioelectric {

  template<typename T>
  T doInterpolate(const T* a, double offset, InterpType it);
  
  /*********************** Public Waveform *******************************/

  template<typename T>
  Waveform<T>::Waveform(void) : _interptype(InterpType::LINEAR), _data(nullptr), _size(0)
  {
    
  }
  
  template<typename T>
  Waveform<T>::Waveform(std::initializer_list<T> init, InterpType it) : _interptype(it), _data(nullptr), _size(0)
  {
    alloc(init.size());
    T* p = _data;
    for (auto& val : init)
      *(p++) = val;
  }

  template<typename T>
  Waveform<T>::Waveform(const Waveform<T>& other, std::size_t len, InterpType it) : _interptype(it), _data(nullptr), _size(0)
  {
    if (len == 0)
      return;
    alloc(len);
    double speed = (double)other.size()/(double)len;
    auto itr = other.ibegin(speed);
    T* p = _data;
    for (;itr!=other.iend(speed); ++itr, p++)
      *p = *itr;
  }

  template<typename T>
  Waveform<T>::~Waveform(void)
  {
    dealloc();
  }

  template<typename T>
  T Waveform<T>::interpolate(double pos) const
  {
    if (pos < 0 || pos > _size-1)
      return 0;
    return interpLinear(pos);
  }

  template<typename T>
  typename Waveform<T>::interpolator Waveform<T>::ibegin(double speed) const
  {
    if (speed<=0.0)
      return iend(1);
    return interpolator(*this,(long)0,speed);
  }

  template<typename T>
  typename Waveform<T>::interpolator Waveform<T>::iend(double speed) const
  {
    if (speed<=0.0)
      speed = 1;
    long end = 1 + (long)((double)(_size-1)/speed);
    return interpolator(*this,end,speed);
  }

  template<typename T>
  typename Waveform<T>::interpolator Waveform<T>::ribegin(double speed) const
  {
    if (speed<=0.0)
      speed = 1;
    long start = (long)((double)(_size-1)/speed);
    return interpolator(*this,start,-speed);
  }

  template<typename T>
  typename Waveform<T>::interpolator Waveform<T>::riend(double speed) const
  {
    return interpolator(*this,(long)-1,-speed); //Reverse equivalent of size
  }
  
  template<typename T>
  Waveform<T>& Waveform<T>::operator=(const Waveform<T>& other)
  {
    alloc(other._size);
    _interptype = other._interptype;
    memcpy(_data,other._data,sizeof(T)*_size);
    return *this;
  }

  /*********************** Private Waveform *******************************/

  template<typename T>
  void Waveform<T>::alloc(std::size_t len)
  {
    _data = new T[len];
    _size = len;
  }

  template<typename T>
  void Waveform<T>::dealloc(void)
  {
    if (_data)
      delete[] _data;
    _size = 0;
  }

  template<typename T>
  T Waveform<T>::interpLinear(double pos) const
  {
    long p = pos;
    T a = _data[p];
    T b = _data[p+1];
    double diff = fmod(pos,1.0);
    return (b-a)*diff + a;
  }
  
  
  /*********************** interpolator *******************************/

  template<typename T>
  Waveform<T>::interpolator::interpolator(const Waveform<T>& wf, double start, double speed) : _wf(wf), _speed(speed)
  {
    _pos = start/speed;
  }

  template<typename T>
  Waveform<T>::interpolator::interpolator(const Waveform<T>& wf, long start_pos, double speed) :
    _wf(wf), _speed(speed), _pos(start_pos)
  {

  }

  template<typename T>
  Waveform<T>::interpolator::interpolator(const Waveform<T>::interpolator& other) :
    _wf(other._wf), _pos(other._pos), _speed(other._speed)
  {
    
  }

  template<typename T>
  typename Waveform<T>::interpolator& Waveform<T>::interpolator::operator++(void)
  {
    increment();
    return *this;
  }
  
  template<typename T>
  typename Waveform<T>::interpolator Waveform<T>::interpolator::operator++(int)
  {
    auto old = *this;
    increment();
    return old;
  }

  template<typename T>
  void Waveform<T>::interpolator::increment(void)
  {
    _pos++;
  }

  template<typename T>
  typename Waveform<T>::interpolator Waveform<T>::interpolator::operator+(long n) const
  {
    return interpolator(_wf, _pos+n, _speed);
  }

  // template<typename T>
  // typename Waveform<T>::interpolator Waveform<T>::interpolator::operator-(std::size_t n) const
  // {
  //   return interpolator(_wf, _loc-(_speed*n), _speed);
  // }

  template<typename T>
  bool Waveform<T>::interpolator::operator==(const Waveform<T>::interpolator &other) const
  {
    return _wf._data == other._wf._data && _pos==other._pos;
  }

  template<typename T>
  bool Waveform<T>::interpolator::operator!=(const Waveform<T>::interpolator &other) const
  {
    return !(*this == other);
  }
  
  template<typename T>
  T Waveform<T>::interpolator::operator*(void) const
  {
    return _wf.interpolate(_pos*_speed);
  }


  /********************* Interpolation functions ********************/

  template<typename T>
  T interpLinear(const T* ary, double offset)
  {
    T a = ary[(std::size_t)offset];
    T b = ary[(std::size_t)offset+1];
    double diff = fmod(offset,1.0);
    return (b-a)*diff + a;
  }

  template<typename T>
  T doInterpolate(const T* ary, double offset, InterpType it)
  {
    switch (it) {
    case InterpType::LINEAR:
      return interpLinear(ary, offset);
    }
  }

  template class Waveform<double>;
  template class Waveform<float>;
  template double doInterpolate<double>(const double*,double,InterpType);
  template double interpLinear<double>(const double*,double);  
  template float doInterpolate<float>(const float*,double,InterpType);
  template float interpLinear<float>(const float*,double);  
}
