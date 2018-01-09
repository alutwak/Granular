
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
    class interpolator {
    public:
      interpolator(const Waveform<T>& wf, double start, double speed);
      interpolator(const Waveform<T>& wf, long start_pos, double speed);
      interpolator(const interpolator& other);
      interpolator& operator++(void);               //!<\brief Prefix increment
      interpolator operator++(int);                 //!<\brief Postfix increment
      interpolator operator+(long n) const;         //!<\brief Random access +
      //interpolator operator-(std::size_t n) const;  //!<\brief Random access -
      T operator*(void) const;                  //!<\brief Data retrieval (not a reference since the interpolation doesn't exist in mem)
      bool operator==(const interpolator& other) const;
      bool operator!=(const interpolator& other) const;

    private:
      const double _speed;
      long _pos;
      const Waveform<T>& _wf;

      void increment(void);
    };

    Waveform(void);

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

    Waveform<T>& operator=(const Waveform<T>& other);
    T& operator[](std::size_t pos) {return _data[pos];}
    const T& operator[](std::size_t pos) const {return _data[pos];}

    /*!\brief Returns a forward interpolator that points to the beginning of the waveform
     */
    interpolator ibegin(double speed=1) const;
    
    /*!\brief Returns a forward interpolator that points to the end of the waveform
     */
    interpolator iend(double speed=1) const;

    /*!\brief Returns a reverse interpolator that points to the end of the waveform
     */
    interpolator ribegin(double speed=1) const;

    /*!\brief Returns a reverse interpolator that points to the beginning of the waveform
     */
    interpolator riend(double speed=1) const;
    
    std::size_t size(void) const {return _size;}
    T* data(void) {return _data;}
    
  private:

    InterpType _interptype;
    long _size;
    T* _data;
    
    void alloc(std::size_t len);
    void dealloc(void);
    T interpLinear(double pos) const;
  };

}
