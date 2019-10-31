
#include <cmath>
#include <cstring>
#include <type_traits>

#include "waveform.hpp"
#include "phasor.hpp"

namespace audioelectric {

  #define PI M_PI

  /*********************** Public Waveform *******************************/

  template<typename T>
  T doInterpolate(const T* a, double offset, InterpType it);

  /*********************** Public Waveform *******************************/

  template<typename T>
  Waveform<T>::Waveform(void) : _interptype(InterpType::LINEAR), _data(nullptr), _size(0), _end(0), _samplerate(0)
  {
    
  }

  template<typename T>
  Waveform<T>::Waveform(std::size_t len, T sr, InterpType it) : _interptype(it), _data(nullptr), _size(0), _end(0)
  {
    alloc(len);
    memset(_data, 0, sizeof(T)*len);
    if (sr == 0)
      _samplerate = len;
    else
      _samplerate = sr;    
  }

  template<typename T>
  Waveform<T>::Waveform(T* data, std::size_t len, T sr, InterpType it) : _interptype(it), _data(nullptr), _size(len), _end(len-1)
  {
    alloc(len);
    memcpy(_data, data, sizeof(T)*len);
    if (sr == 0)
      _samplerate = len;
    else
      _samplerate = sr;    
  }

  template<typename T>
  Waveform<T>::Waveform(std::initializer_list<T> init, T sr, InterpType it) : _interptype(it), _data(nullptr), _size(0), _end(0)
  {
    alloc(init.size());
    T* p = _data;
    for (auto& val : init)
      *(p++) = val;
    if (sr == 0)
      _samplerate = init.size();
    else
      _samplerate = sr;
  }

  template<typename T>
  Waveform<T>::Waveform(std::string afile, size_t begin, size_t end, InterpType it) :
    _interptype(it), _data(nullptr), _size(0), _end(0)
  {
  
    SF_INFO info;
    SNDFILE* f = sf_open(afile.c_str(), SFM_READ, &info);
    if (!f)
      throw WaveformError("Error when reading file: " + afile + ": " + sf_strerror(f));
    
    if (end == 0 || end >= info.frames)
      end = info.frames;
    if (end <= begin)
      throw WaveformError("Ending frame of an audio file waveform must be greater than the beginning frame");
    else if (begin >= info.frames)
      throw WaveformError("Beginning frame was greater than the number of frames in the audio file");
    alloc(end-begin);

    if (info.channels == 1)
      readOneChannelFile(f, &info, begin, end);
    else
      readMultiChannelFile(f, &info, begin, end);
    _samplerate = info.samplerate;
    sf_close(f);
  }

  #define RDSIZE 4096

  template <typename T>
  void Waveform<T>::readOneChannelFile(SNDFILE* f, SF_INFO* info, size_t begin, size_t end)
  {
    sf_seek(f, begin, SEEK_SET);
    T* buf = _data;
    while (begin < end) {
      size_t toread;
      if (end - begin >= RDSIZE)
        toread = RDSIZE;
      else
        toread = end - begin;
      size_t nread;
      // Requires c++17 to compile: 
      if constexpr (std::is_same<T, double>::value)
        nread = sf_read_double(f, buf, toread);
      else if constexpr (std::is_same<T, float>::value)
        nread = sf_read_float(f, buf, toread);
      if (nread < toread) break; // This will happen if info.frames < end
      begin += nread;
      buf += nread;
    }
  }

  template <typename T>
  void Waveform<T>::readMultiChannelFile(SNDFILE* f, SF_INFO* info, size_t begin, size_t end)
  {
    sf_seek(f, begin, SEEK_SET);
    T* rdbuf = new T[info->channels*RDSIZE];
    T* buf = _data;
    while (begin < end) {
      size_t toread;
      if (end - begin >= RDSIZE)
        toread = RDSIZE;
      else
        toread = end - begin;
      size_t nread;
      // Requires c++17 to compile:
      if constexpr (std::is_same<T, double>::value)
        nread = sf_readf_double(f, rdbuf, toread);
      else if constexpr (std::is_same<T, float>::value)
        nread = sf_readf_float(f, rdbuf, toread);

      // Now, we sum (well, really we average) all of the channels
      // TODO: write a better summing algorithm
      T* p = rdbuf;
      for (size_t i=0; i<nread; i++) {
        *buf = 0;
        for (int c=0; c<info->channels; c++) {
          *buf += *p++;
        }
        *buf /= info->channels;
        buf++;
      }
      if (nread < toread) break; // This will happen if info.frames < end
      begin += nread;
    }

    delete[] rdbuf;
  }

  template<typename T>
  Waveform<T>::Waveform(T (*generator)(size_t), size_t len, T sr, InterpType it) :
    _interptype(it), _data(nullptr), _size(0), _end(0)
  {
    generate(generator, len);
    if (sr == 0)
      _samplerate = len;
    else
      _samplerate = sr;
  }

  template<typename T>
  Waveform<T>::Waveform(Waveform<T>& other, double rate, std::size_t len, InterpType it) :
    _interptype(it), _data(nullptr), _size(0), _end(0), _samplerate(other._samplerate)
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
    if (pos < 0 || pos > _end)
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
    _end = len-1;
  }

  template<typename T>
  void Waveform<T>::dealloc(void)
  {
    if (_data)
      delete[] _data;
    _size = 0;
    _end = 0;
  }

  template<typename T>
  T Waveform<T>::interpLinear(double pos) const
  {
    long p = pos;
    T a = _data[p];
    T b = _data[p+1];
    double diff = pos - (double)p;
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
  void GenerateGaussian(Waveform<T>& wf, std::size_t len, T sigma)
  {
    wf.resize(len);
    T flen = len;
    T mid = -flen/2;
    T sigma_norm = -2*(sigma*flen)*(sigma*flen);
    T offset = exp(mid*mid/sigma_norm);
    T norm = 1.0/(1.0-offset);
    T* data = wf.data();
    for (std::size_t i=0; i<len; i++) {
      mid += 1.0;
      data[i] = norm*(exp(mid*mid/sigma_norm)-offset);
    }
  }

  template void GenerateGaussian<double>(Waveform<double>&, std::size_t, double);
  template void GenerateGaussian<float>(Waveform<float>&, std::size_t, float);

  template <typename T>
  void GenerateSin(Waveform<T>& wf, std::size_t len)
  {
    wf.resize(len);
    T w = 2.*PI/len;
    T* data = wf.data();
    for (size_t i=0; i<len; i++) {
      data[i] = sin(i*w);
    }
  }

  template void GenerateSin<double>(Waveform<double>&, std::size_t);
  template void GenerateSin<float>(Waveform<float>&, std::size_t);

  template <typename T>
  void GenerateTriangle(Waveform<T>& wf, std::size_t len, T slant)
  {
    wf.resize(len);
    size_t uplen = (len/2)*(1 + slant);
    T upslope = 1./uplen;
    size_t downlen = len - uplen;
    T dwnslope = 1./downlen;
    T* data = wf.data();
    size_t i = 0;
    for (; i<uplen; i++)
      data[i] = i*upslope;
    for (size_t j=0; i<len; i++, j++)
      data[i] = 1.0 - j*dwnslope;
  }

  template void GenerateTriangle<double>(Waveform<double>&, std::size_t, double);
  template void GenerateTriangle<float>(Waveform<float>&, std::size_t, float);


  template <typename T>
  void GenerateSquare(Waveform<T>& wf, std::size_t len, T width)
  {
    wf.resize(len);
    size_t rise_time = len*width;
    T* data = wf.data();
    for (size_t i=0; i<rise_time; i++)
      data[i] = 0;
    for (size_t i=rise_time; i<len; i++)
      data[i] = 1.0;
  }

  template void GenerateSquare<double>(Waveform<double>&, std::size_t, double);
  template void GenerateSquare<float>(Waveform<float>&, std::size_t, float);

}
