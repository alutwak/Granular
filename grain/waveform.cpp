
#include <cmath>
#include <cstring>
#include "waveform.hpp"
#include "phasor.hpp"

namespace audioelectric {
  
  /*********************** Public Waveform *******************************/

  template<typename T>
  T doInterpolate(const T* a, double offset, InterpType it);
  
  /*********************** Public Waveform *******************************/

  template<typename T>
  Waveform<T>::Waveform(void) : _interptype(InterpType::LINEAR), _data(nullptr), _size(0)
  {
    
  }

  template<typename T>
  Waveform<T>::Waveform(std::size_t len, InterpType it) : _interptype(it), _data(nullptr), _size(0)
  {
    alloc(len);
    memset(_data, 0, sizeof(T)*len);
  }
  
  template<typename T>
  Waveform<T>::Waveform(T* data, std::size_t len, InterpType it) : _interptype(it), _data(nullptr), _size(len)
  {
    alloc(len);
    memcpy(_data, data, sizeof(T)*len);
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
  Waveform<T>::Waveform(T (*generator)(size_t), size_t len, InterpType it) : _interptype(it), _data(nullptr), _size(0)
  {
    generate(generator, len);
  }

  template<typename T>
  Waveform<T>::Waveform(Waveform<T>& other, double rate, std::size_t len, InterpType it) :
    _interptype(it), _data(nullptr), _size(0)
  {
    alloc(len);
    auto phs = Phasor<T>(other, rate);
    T* p = _data;
    phs.generate(&_data, len, 1);
  }

  template<typename T>
  Waveform<T>::~Waveform(void)
  {
    dealloc();
  }

  template<typename T>
  void Waveform<T>::generate(T (*generator)(size_t), size_t len)
  {
    resize(len);
    for (size_t i=0; i<len; i++) {
      _data[i] = generator(i);
    }
  }

  template<typename T>
  T Waveform<T>::waveform(double pos, int channel)
  {
    if (pos < 0 || pos > _size-1)
      return 0;
    return interpLinear(pos);
  }
  
  template<typename T>
  Waveform<T>& Waveform<T>::operator=(const Waveform<T>& other)
  {
    alloc(other._size);
    _interptype = other._interptype;
    memcpy(_data,other._data,sizeof(T)*_size);
    return *this;
  }


  template<typename T>
  typename Waveform<T>::iterator Waveform<T>::ibegin(void)
  {
    return iterator(_data);
  }

  template<typename T>
  typename Waveform<T>::iterator Waveform<T>::iend(void)
  {
    return iterator(_data+_size);
  }

  template<typename T>
  void Waveform<T>::resize(std::size_t len) {
    if (len == _size)
      return;
    alloc(len);
  }
  
  /*********************** Private Waveform *******************************/

  template<typename T>
  void Waveform<T>::alloc(std::size_t len)
  {
    dealloc();
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
  
  /********************* iterator ********************/
  
  template<typename T>
  typename Waveform<T>::iterator& Waveform<T>::iterator::operator++(void)
  {
    _data++;
    return *this;
  }

  template<typename T>
  typename Waveform<T>::iterator Waveform<T>::iterator::operator++(int)
  {
    iterator temp = *this;
    _data++;
    return temp;
  }

  template<typename T>
  T& Waveform<T>::iterator::operator*(void)
  {
    return *_data;
  }

  template<typename T>
  bool Waveform<T>::iterator::operator==(const Waveform<T>::iterator& other) const
  {
    return _data == other._data;
  }
  
  template<typename T>
  bool Waveform<T>::iterator::operator!=(const Waveform<T>::iterator& other) const
  {
    return _data != other._data;
  }

  template class Waveform<double>;
  template class Waveform<float>;

  template<typename T>
  void GenerateGaussian(Waveform<T> *wt, std::size_t len, T sigma) {
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
