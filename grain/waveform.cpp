
#include <cmath>
#include "waveform.hpp"

namespace audioelectric {


  template<typename T>
  T doInterpolate(const T* a, double offset, InterpType it);
  
  /*********************** Public Waveform *******************************/

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
    alloc(len);
    double speed = (double)size()/(double)len;
    auto itr = other.ibegin(speed);
    T* p = _data;
    for (;itr!=other.iend(); ++itr, p++)
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
    return doInterpolate(_data+(std::size_t)(pos),fmod(pos,1.0),_interptype);
  }

  template<typename T>
  typename Waveform<T>::iterator Waveform<T>::ibegin(double speed) const
  {
    return iterator(_data,0.0,speed,_interptype);
  }

  template<typename T>
  typename Waveform<T>::iterator Waveform<T>::iend(double speed) const
  {
    double end = fmod(speed*(floor(size()/speed)+1),1.0);
    return iterator(_data,end,speed,_interptype);
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
  
  /*********************** iterator *******************************/

  template<typename T>
  Waveform<T>::iterator::iterator(T* ary, double start, double speed, InterpType it) : _speed(speed), _interptype(it)
  {
    _offset = fmod(start,1.0);
    _p = ary + (std::size_t)start;
  }

  template<typename T>
  Waveform<T>::iterator::iterator(const Waveform<T>::iterator& other) :
    _p(other._p), _offset(other._offset), _speed(other._speed), _interptype(other._interptype)
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
    _offset += _speed;
    // if (_offset >= 1.0) {
    //   _p += (std::size_t)_offset;
    //   _offset = fmod(_offset,1.0);
    // }
  }

  template<typename T>
  typename Waveform<T>::iterator Waveform<T>::iterator::operator+(std::size_t n) const
  {
    return iterator(_p, _offset+(_speed*n), _speed, _interptype);
  }

  // template<typename T>
  // typename Waveform<T>::iterator Waveform<T>::iterator::operator-(std::size_t n) const
  // {
  //   return iterator(_wf, _loc-(_speed*n), _speed);
  // }

  template<typename T>
  bool Waveform<T>::iterator::operator==(const Waveform<T>::iterator &other) const
  {
    return _p == other._p && _offset==other._offset && _interptype==other._interptype;
  }

  template<typename T>
  bool Waveform<T>::iterator::operator!=(const Waveform<T>::iterator &other) const
  {
    return !(*this == other);
  }
  
  template<typename T>
  T Waveform<T>::iterator::operator*(void) const
  {
    return doInterpolate(_p, _offset, _interptype);
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
