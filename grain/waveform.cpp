
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
  typename Waveform<T>::iterator Waveform<T>::ibegin(double speed) const
  {
    return iterator(*this,(long)0,speed);
  }

  template<typename T>
  typename Waveform<T>::iterator Waveform<T>::iend(double speed) const
  {
    long end = 1 + (long)((double)(_size-1)/speed);
    return iterator(*this,end,speed);
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
  
  
  /*********************** iterator *******************************/

  template<typename T>
  Waveform<T>::iterator::iterator(const Waveform<T>& wf, double start, double speed) : _wf(wf), _speed(speed)
  {
    _pos = start/speed;
  }

  template<typename T>
  Waveform<T>::iterator::iterator(const Waveform<T>& wf, long start_pos, double speed) :
    _wf(wf), _speed(speed), _pos(start_pos)
  {

  }

  template<typename T>
  Waveform<T>::iterator::iterator(const Waveform<T>::iterator& other) :
    _wf(other._wf), _pos(other._pos), _speed(other._speed)
  {
    
  }

  template<typename T>
  typename Waveform<T>::iterator& Waveform<T>::iterator::operator++(void)
  {
    increment();
    return *this;
  }
  
  template<typename T>
  typename Waveform<T>::iterator Waveform<T>::iterator::operator++(int)
  {
    auto old = *this;
    increment();
    return old;
  }

  template<typename T>
  void Waveform<T>::iterator::increment(void)
  {
    _pos++;
    // if (_offset >= 1.0) {
    //   _p += (std::size_t)_offset;
    //   _offset = fmod(_offset,1.0);
    // }
  }

  template<typename T>
  typename Waveform<T>::iterator Waveform<T>::iterator::operator+(long n) const
  {
    return iterator(_wf, _pos+n, _speed);
  }

  // template<typename T>
  // typename Waveform<T>::iterator Waveform<T>::iterator::operator-(std::size_t n) const
  // {
  //   return iterator(_wf, _loc-(_speed*n), _speed);
  // }

  template<typename T>
  bool Waveform<T>::iterator::operator==(const Waveform<T>::iterator &other) const
  {
    return _wf._data == other._wf._data && _pos==other._pos;
  }

  template<typename T>
  bool Waveform<T>::iterator::operator!=(const Waveform<T>::iterator &other) const
  {
    return !(*this == other);
  }
  
  template<typename T>
  T Waveform<T>::iterator::operator*(void) const
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
