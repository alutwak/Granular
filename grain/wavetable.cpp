
#include <cmath>
#include <cstring>
#include "wavetable.hpp"
#include "phasor.hpp"

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
    auto phs = Phasor<T>(other, rate);
    T* p = _data;
    phs.generate(&_data, len, 1);
  }

  template<typename T>
  Wavetable<T>::~Wavetable(void)
  {
    dealloc();
  }

  template<typename T>
  T Wavetable<T>::waveform(double pos, int channel)
  {
    if (pos < 0 || pos > _size-1)
      return 0;
    return interpLinear(pos);
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
