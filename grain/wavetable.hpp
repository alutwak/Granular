
#pragma once

#include "waveform.hpp"

namespace audioelectric {

  /*!\brief Different types of interpolation
   */
  enum class InterpType {
    LINEAR,     //!< Linear interpolation
  };

  
  /*!\brief Contains and manages a set of audio data. Useful for samples, grains or any other chunk of audio data that needs
   * needs to be stored and manipulated.
   */
  template<typename T>
  class Wavetable : public Waveform<T> {
  public:

    using ph_im = typename Waveform<T>::phasor_impl;
    
    /*!\brief A phasor that runs over a Wavetable and can interpolate sub-sample intervals.
     * 
     * The interpolator can run forward and backward and can either be a one-shot or it can cycle. 
     * 
     */
    class interpolator : public Waveform<T>::phasor_impl {
    public:
      
      virtual ~interpolator(void) {}


      /*!\brief Constructs an interpolator that starts at an interpolated position in the wavetable. 
       * 
       * See the discussion of how position relates to phase and rate in the Waveform::phasor_impl class documentation.
       * 
       * \param wt    The Wavetable to iterate over.
       * \param rate   The rate at which to iterate over the Wavetable. Positive rate -> forward iteration, negative rate -> 
       *              reverse iteration.
       * \param start The starting position in the Wavetable
       * \param end   The ending position in the Wavetable. A negative value sets the ending at the last sample for forward 
       *              interpolators and at the first sample for reverse interpolators.
       * \param cycle Whether or not to cycle over the waveform.
       */
      interpolator(const Wavetable<T>& wt, double rate, double start, double end=-1, bool cycle=false);

      // /*!\brief Creates a vari-rate interpolator for which the rate is seet by another interpolator
      //  */ 
      // interpolator(const Wavetable<T>* wt, typename Waveform<T>::phasor& rate, double start, double end=-1, bool cycle=false);
      
      interpolator(const interpolator& other);
      
    protected:

      friend class Wavetable;

      double _start;              //!< The start of the wavetable in iterations (the units of the phase)
      double _end;                //!< The end of the wavetable in iterations (the units of the phase)
      bool _cycle;              //!< Whether to cycle the Waveform
      //const Wavetable<T>& _wt;  //!< The wavetable to interpolate over

      interpolator operator+(long n) const;     //!<\brief Random access +

      virtual T value(void) const;              //!<\brief Returns the value of the Wavetable at the current phase

      /*!\brief Returns false if the current phase is outside the bounds of the wavetable. Otherwise returns true.
       */
      virtual operator bool(void) const;        

      /*!\brief Sets the rate (useful for vari-rate iterations)
       */
      virtual void setRate(double rate);
      
      /*!\brief Copies this interpolator and returns a pointer to the newly constructed object
       */
      virtual typename Waveform<T>::phasor_impl* copy(void);

      /*!\brief Increments the phase
       */
      virtual void increment(void);

    private:
      
      /*!\brief Sets the value of the _end member.
       */
      void setEnd(long end);

      /*!\brief Checks whether the given phase is within the start and stop bounds
       */
      bool checkPhase(double phase) const;
    };

    class varispeed_interpolator : public interpolator {

    public:

      virtual ~varispeed_interpolator(void) {}

    protected:

      friend class Wavetable;

      /*!\todo Step thorugh this in lldb to see whether all the phasor_impl bits get initialized twice
       */
      varispeed_interpolator(const Wavetable<T>& wt, const typename Waveform<T>::phasor& rates,
                             double start, double end=-1, bool cycle=false);

      varispeed_interpolator(const varispeed_interpolator& other);

      virtual void setRatePhasor(const typename Waveform<T>::phasor& rates);

      virtual varispeed_interpolator* copy(void);

      void increment(void);

    private:

      typename Waveform<T>::phasor _rate_phasor;
      
    };
    
    /*!\brief A common iterator for the Wavetable. This works just like that of a vector's iterator
     */
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
     * \param rate The rate at which to advance through the waveform (>0, <1 is slower, >1 is faster)
     * \param start The starting position (in the original waveform, not the interpolated one)
     * \param end The ending position
     * \param cycle Whether or not to cycle over the wavetable
     */
    typename Waveform<T>::phasor pbegin(double rate, double start, double end=-1, bool cycle=false) const;

    /*!\brief Returns a forward varispeed interpolator
     */
    typename Waveform<T>::phasor pbegin(const typename Waveform<T>::phasor& rates,
                                        double start, double end=-1, bool cycle=false) const;    

    /*!\brief Returns a forward interpolator that points to the beginning of the waveform \see ibegin(long,double)
     *
     * \param rate The rate at which to advance through the waveform (>0, <1 is slower, >1 is faster)
     */
    typename Waveform<T>::phasor pbegin(double rate) const;

    typename Waveform<T>::phasor pbegin(const typename Waveform<T>::phasor& rate) const;    
    
    // /*!\brief Returns a forward interpolator that points to the end of the waveform
    //  */
    // interpolator iend(double rate) const;

    /*!\brief Returns a reverse interpolator that points to the end of the waveform
     *
     * Like the forward interpolator, there is no bounds or rate checking. The caller must take care.
     * 
     * \param rate  The rate at which to advance through the waveform (>0, <1 is slower, >1 is faster).
     * \param start The starting position (in the original waveform, not the interpolated one)
     * \param end   The ending position of the interpolator \see interpolator.
     * \param cycle Whether or not to cycle.
     */
    typename Waveform<T>::phasor rpbegin(double rate, double start, double end=-1, bool cycle=false) const;

    // typename Waveform<T>::phasor rpbegin(const typename Waveform<T>::phasor& rates,
    //                                     double start, double end=-1, bool cycle=false) const;    

    /*!\brief Returns a reverse interpolator that points to the end of the waveform
     *
     * \param rate The rate at which to advance through the waveform (>0, <1 is slower, >1 is faster)
     * \param start The starting position (in the original waveform, not the interpolated one)
     * \param end   The ending position of the interpolator \see interpolator.
     * \param cycle Whether or not to cycle.
     */    
    typename Waveform<T>::phasor rpbegin(double rate) const;

//    typename Waveform<T>::phasor rpbegin(const typename Waveform<T>::phasor& rate) const;    

    // /*!\brief Returns a reverse interpolator that points to the beginning of the waveform
    //  */
    // interpolator riend(double rate) const;

    iterator ibegin(void);
    iterator iend(void);
    
    /*!\brief Returns the number of samples in the Wavetable
     */
    virtual std::size_t size(void) const {return _size;}

    /*!\brief Returns the end sample of the Wavetable
     *
     */
    virtual double end(void) const {return _size-1;}

    /*!\brief Resizes the Wavetable. All data is cleared
     */
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

