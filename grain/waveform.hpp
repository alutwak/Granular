
#pragma once

#include <cstdlib>
#include <vector>

namespace audioelectric {

  enum class InterpType {
    LINEAR,
  };

  /*!\brief Conatins and manages a set of audio data. Useful for samples, grains or any other chunk of audio data that needs
   * needs to be stored and manipulated.
   */
  template<typename T>
  class Waveform {
  public:

    /*!\brief An iterator that generalizes element access to allow for interpolation between elements
     */
    class iterator {
    public:
      iterator(T* ary, double start, double speed, InterpType it);
      iterator(const iterator& other);
      iterator& operator++(void);               //!<\brief Prefix increment
      iterator operator++(int);                 //!<\brief Postfix increment
      iterator operator+(std::size_t n) const;  //!<\brief Random access +
      //iterator operator-(std::size_t n) const;  //!<\brief Random access -
      T operator*(void) const;                  //!<\brief Data retrieval (not a reference since the interpolation doesn't exist in mem)
      bool operator==(const iterator& other) const;
      bool operator!=(const iterator& other) const;
    private:
      const InterpType _interptype;
      const double _speed;
      double _offset;
      T* _p;

      void increment(void);
    };

    typedef const iterator const_iterator;

    Waveform(std::initializer_list<T> init, InterpType it=InterpType::LINEAR);
    
    /*!\brief Copies a sample to a new length using interpolation
     */
    Waveform(const Waveform<T>& other, std::size_t len, InterpType it=InterpType::LINEAR);

    ~Waveform(void);

    void setInterpType(InterpType it) {_interptype = it;}

    InterpType getInterpType(void) const {return _interptype;}

    /*!\brief Returns the interpolated value at a position in the waveform.
     * 
     * This is a generalized form of the integer-related operator[], in which points between the integer positions are 
     * interpolated.
     * 
     * \param pos The position on the waveform
     * \return The interpolated value at pos
     */
    T interpolate(double pos) const;

    T& operator[](std::size_t pos) {return _data[pos];}
    const T& operator[](std::size_t pos) const {return _data[pos];}

    iterator ibegin(double speed=1) const;
    iterator iend(double speed=1) const;
    std::size_t size(void) const {return _size;}
    T* data(void) {return _data;}
    
  private:

    InterpType _interptype;
    std::size_t _size;
    T* _data;
    
    void alloc(std::size_t len);
    void dealloc(void);
  
  };

}
