
#pragma once

#include "waveform.hpp"

namespace audioelectric {

  enum class InterpType {
    LINEAR,
  };

  
  /*!\brief Contains and manages a set of audio data. Useful for samples, grains or any other chunk of audio data that needs
   * needs to be stored and manipulated.
   */
  template<typename T>
  class Wavetable : public Waveform<T> {
  public:

    /*!\brief An iterator that generalizes element access to allow for interpolation between elements
     * 
     */
    class interpolator : public Waveform<T>::phasor_impl {
    protected:

      friend class Wavetable;
      using ph_im = typename Waveform<T>::phasor_impl;

      // /*!\brief Constructs an interpolator that starts at a particular sample in the wavetable.
      //  */
      // interpolator(const Wavetable<T>& wt, long start_pos, double rate);
      
      /*!\brief Constructs an interpolator that starts at an interpolated position in the wavetable. 
       * 
       * See the discussion of how position relates to phase and rate in the Waveform::phasor_impl class documentation.
       * 
       * \param wt The Wavetable to iterate over.
       * \param start The starting position in the Wavetable
       * \param rate The rate at which to iterate over the Wavetable. Positive rate -> forward iteration, negative rate -> 
       *             reverse iteration.
       */
      interpolator(const Wavetable<T>& wt, double start, double rate);

      /*!\brief Creates a vari-rate interpolator for which the rate is seet by another interpolator
       */ 
      //interpolator(const Wavetable<T>* wf, long start_pos, std::unique_ptr<typename Waveform<T>::phasor> vel_interp);
      interpolator(const interpolator& other);
      //interpolator& operator=(const interpolator& other);
      interpolator operator+(long n) const;                 //!<\brief Random access +
      T value(void) const;                                  //!<\brief Data retrieval (not a reference)
      operator bool(void) const;

      virtual typename Waveform<T>::phasor_impl* copy(void);
      
    private:
      long _end;
      const Wavetable<T>& _wt;

      void setEnd(void);
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
    Wavetable(void);

    /*!\brief Creates and allocates a waveform of size len with all values set to 0
     */
    Wavetable(std::size_t len, InterpType it=InterpType::LINEAR);

    /*!\brief Wraps an array of size len in a Waveform to allow it to be interpolated
     */
    Wavetable(T* data, std::size_t len, InterpType it=InterpType::LINEAR);

    Wavetable(std::initializer_list<T> init, InterpType it=InterpType::LINEAR);
    
    /*!\brief Copies a sample to a new length using interpolation
     */
    Wavetable(const Waveform<T>& other, double rate, std::size_t len, InterpType it=InterpType::LINEAR);

    ~Wavetable(void);

    /*!\brief Sets the interpolation type
     */
    void setInterpType(InterpType it) {_interptype = it;}

    /*!\brief Returns the current interpolation type
     */
    InterpType getInterpType(void) const {return _interptype;}

    /*!\brief Returns the interpolated value at a position in the waveform.
     * 
     * The position is a generalized index of the samples in the waveform, such that an integer position will return 
     * the same value as that returned by Wavetable[], and a non-integer position will return an interpolated value as though
     * the waveform were continuous. 
     * 
     * Positions outside of the waveform -- those less than 0 and greater than the size of the waveform (in samples) -- will 
     * return a value of 0. This is done so that a Wavetable can be easily mixed with other Waveforms without having to worry
     * about bounds. 
     * 
     * \param pos The position on the waveform
     * \return The interpolated value at pos
     */
    virtual T waveform(double pos) const;

    Wavetable<T>& operator=(const Wavetable<T>& other);

    T& operator[](std::size_t pos) {return _data[pos];}
    const T& operator[](std::size_t pos) const {return _data[pos];}

    /*!\brief Returns a forward interpolator 
     * 
     * There is no checking for start positions that are outside of the waveform or rates that are <= 0. The caller must be
     * careful not to use such values or it will be possible to get an iterator that never hits the end. Even very small
     * rates should probably be avoided.
     * 
     * \param start The starting position (in the original waveform, not the interpolated one)
     * \param rate The rate at which to advance through the waveform (>0, <1 is slower, >1 is faster)
     */
    typename Waveform<T>::phasor pbegin(double start, double rate) const;

    /*!\brief Returns a forward interpolator that points to the beginning of the waveform \see ibegin(long,double)
     *
     * \param rate The rate at which to advance through the waveform (>0, <1 is slower, >1 is faster)
     */
    typename Waveform<T>::phasor pbegin(double rate) const;
    
    // /*!\brief Returns a forward interpolator that points to the end of the waveform
    //  */
    // interpolator iend(double rate) const;

    /*!\brief Returns a reverse interpolator that points to the end of the waveform
     *
     * Like the forward interpolator, there is no bounds or rate checking. The caller must take care.
     * 
     * \param start The starting position (in the original waveform, not the interpolated one)
     * \param rate The rate at which to advance through the waveform (>0, <1 is slower, >1 is faster)
     */
    typename Waveform<T>::phasor rpbegin(double start, double rate) const;

    /*!\brief Returns a reverse interpolator that points to the end of the waveform
     *
     * \param rate The rate at which to advance through the waveform (>0, <1 is slower, >1 is faster)
     */    
    typename Waveform<T>::phasor rpbegin(double rate) const;

    // /*!\brief Returns a reverse interpolator that points to the beginning of the waveform
    //  */
    // interpolator riend(double rate) const;

    iterator begin(void);
    iterator end(void);
    
    /*!\brief Returns the number of samples in the waveform
     */
    std::size_t size(void) const {return _size;}

    void resize(std::size_t len);

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

  /*!\brief Generates a gaussian function and puts it in a wavetable
   * 
   * The generated function is: r*(e^(-(x-len/2)^2/sigma)-y), where y is the value of the normal Gaussian at the 
   * beginning and end of the signal and r renormalizes the signal after that subtraction. This ensures that the signal
   * begins and ends with a value of zero.
   *
   * \param[in,out] wt The Wavetable to fill (this will overwrite any function already in it)
   * \param len The length of the gaussian function
   * \param sigma The variance of the gaussian
   */
  template<typename T>
  void GenerateGaussian(Wavetable<T> *wt, std::size_t len, T sigma);

}

