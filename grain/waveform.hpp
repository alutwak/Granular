
#pragma once

#include <cstdlib>
#include <vector>

namespace audioelectric {

  enum class InterpType {
    LINEAR,
  };

  /*!\brief Contains and manages a set of audio data. Useful for samples, grains or any other chunk of audio data that needs
   * needs to be stored and manipulated.
   * 
   * \note Waveform has been purposely left as a single-channel base class in order to keep the overhead of multiple channels
   * out of sublasses that only need one channel (like Grain or probably a handful of other potential waveforms that are used
   * for their waveform properties rather than their pure audio properties). It should be pretty easy for subclasses to add 
   * channels though. You'll just need to fix the indexing of the interpolations and then make separate interpolators for each
   * channel. The more interesting challenge will be fixing operator[] so that you can use syntax like sample[chan][pos]. 
   * That's not strictly necessary, but it would be a nice touch and make indexing multi-channel waveforms much easier.
   */
  template<typename T>
  class Waveform {
  public:

    /*!\brief An iterator that generalizes element access to allow for interpolation between elements
     */
    class interpolator {
    public:
      interpolator(void);
      interpolator(const Waveform<T>& wf, double start, double velocity);
      interpolator(const Waveform<T>& wf, long start_pos, double velocity);
      interpolator(const interpolator& other);
      interpolator& operator=(const interpolator& other);
      interpolator& operator++(void);               //!<\brief Prefix increment
      interpolator operator++(int);                 //!<\brief Postfix increment
      interpolator operator+(long n) const;         //!<\brief Random access +
      //interpolator operator-(std::size_t n) const;  //!<\brief Random access -
      T operator*(void) const;                  //!<\brief Data retrieval (not a reference since the interpolation doesn't exist in mem)
      operator bool(void) const;
      bool operator==(const interpolator& other) const;
      bool operator!=(const interpolator& other) const;

      /*\brief Compare operators compare the location in the uninterpolated waveform (essentially position*speed)
       */
      bool operator<(const interpolator& other) const;
      bool operator>(const interpolator& other) const;
      bool operator<=(const interpolator& other) const;
      bool operator>=(const interpolator& other) const;
      
    private:
      double _speed;
      long _velocity;
      long _pos;
      long _end;
      const Waveform<T>* _wf;

      void increment(void);
    };

    class iterator {
    public:
      iterator(T* data) : _data(data) {};
      iterator(const iterator& other) : _data(other._data) {};
      iterator& operator++(void);
      iterator operator++(int);
      T& operator*(void);
      bool operator==(const iterator& other) const;
      bool operator!=(const iterator& other) const;      
    private:
      T* _data;
    };

    /*!\brief Creates a waveform of size 0
     */
    Waveform(void);

    /*!\brief Creates and allocates a waveform of size len with all values set to 0
     */
    Waveform(std::size_t len, InterpType it=InterpType::LINEAR);

    /*!\brief Wraps an array of size len in a Waveform to allow it to be interpolated
     */
    Waveform(T* data, std::size_t len, InterpType it=InterpType::LINEAR);

    Waveform(std::initializer_list<T> init, InterpType it=InterpType::LINEAR);
    
    /*!\brief Copies a sample to a new length using interpolation
     */
    Waveform(const Waveform<T>& other, std::size_t len, InterpType it=InterpType::LINEAR);

    ~Waveform(void);

    /*!\brief Sets the interpolation type
     */
    void setInterpType(InterpType it) {_interptype = it;}

    /*!\brief Returns the current interpolation type
     */
    InterpType getInterpType(void) const {return _interptype;}

    /*!\brief Returns the interpolated value at a position in the waveform.
     * 
     * This is a generalized form of the integer-related operator[], in which points between the integer positions are 
     * interpolated.
     * 
     * \param pos The position on the waveform
     * \return The interpolated value at pos
     */
    virtual T interpolate(double pos) const;

    Waveform<T>& operator=(const Waveform<T>& other);
    T& operator[](std::size_t pos) {return _data[pos];}
    const T& operator[](std::size_t pos) const {return _data[pos];}

    /*!\brief Returns a forward interpolator 
     * 
     * There is no checking for start positions that are outside of the waveform or speeds that are <= 0. The caller must be
     * careful not to use such values or it will be possible to get an iterator that never hits the end. Even very small
     * speeds should probably be avoided.
     * 
     * \param start The starting position (in the original waveform, not the interpolated one)
     * \param speed The speed at which to advance through the waveform (>0, <1 is slower, >1 is faster)
     */
    interpolator ibegin(long start, double speed) const;

    /*!\brief Returns a forward interpolator that points to the beginning of the waveform \see ibegin(long,double)
     *
     * \param speed The speed at which to advance through the waveform (>0, <1 is slower, >1 is faster)
     */
    interpolator ibegin(double speed) const;
    
    /*!\brief Returns a forward interpolator that points to the end of the waveform
     */
    interpolator iend(double speed) const;

    /*!\brief Returns a reverse interpolator that points to the end of the waveform
     *
     * Like the forward interpolator, there is no bounds or speed checking. The caller must take care.
     * 
     * \param start The starting position (in the original waveform, not the interpolated one)
     * \param speed The speed at which to advance through the waveform (>0, <1 is slower, >1 is faster)
     */
    interpolator ribegin(long start, double speed) const;

    /*!\brief Returns a reverse interpolator that points to the end of the waveform
     *
     * \param speed The speed at which to advance through the waveform (>0, <1 is slower, >1 is faster)
     */    
    interpolator ribegin(double speed) const;

    iterator begin(void);
    iterator end(void);
    
    /*!\brief Returns a reverse interpolator that points to the beginning of the waveform
     */
    interpolator riend(double speed) const;

    /*!\brief Returns the number of samples in the waveform
     */
    std::size_t size(void) const {return _size;}

    /*!\brief Returns a pointer to the raw data
     */
    T* data(void) {return _data;}
    
  private:

    InterpType _interptype;     //!< The interpolation type
    long _size;                 //!< The size of _data
    T* _data;                   //!< The raw data

    /*!\brief Allocates a data array of length len
     */
    void alloc(std::size_t len);

    /*!\brief Deallocates _data
     */
    void dealloc(void);

    /*!\brief Performs a linear interpolation of the point at posi
     *
     * If pos is <0 or >_size-1, this will always return 0
     */
    virtual T interpLinear(double pos) const;
  };

}
