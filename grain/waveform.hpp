
#pragma once

#include <cstdlib>
#include <vector>
#include <utility>

namespace audioelectric {

  template<typename T>
  class Constant;

  /*!\brief Provides an iterface for a variety of waveforms. These could be waveforms that get generated on the fly
   * or wavetables, samples, envelopes, etc...
   *
   * \note Waveform has been purposely left as a single-channel base class in order to keep the overhead of multiple channels
   * out of sublasses that only need one channel (like Grain, envelope or probably a handful of other potential waveforms
   * that are used for their waveform properties rather than their pure audio properties). It should be pretty easy for
   * subclasses to add channels though. You'll just need to make separate phasors for each channel. The more interesting
   * challenge will be fixing operator[] so that you can use syntax like sample[chan][pos].  That's not strictly necessary,
   * but it would be a nice touch and make indexing multi-channel waveforms much easier.
   */
  template<typename T>
  class Waveform {
  public:

    class phasor;
    
    /*!\brief An iterator-like class that increments the phase of the waveform at a certain rate
     * 
     * The phase is tracked as an integer value and is simply incremented or decremented, depending on the direction,
     * on each call to increment. The normalized time (with a sample rate of 1Hz) at any position in the waveform is 
     * rate*phase. 
     * 
     * In the following discussion, we need to differentiate between the phase of the phasor, which we'll just call "phase",
     * and the phase of the Waveform which we'll call the "position". 
     * 
     * Positions in functions of this class are given in normalized time (a unit of samples, or in seconds if the sampling
     * frequency is 1Hz). "Rates" are given in positions/iteration, or, to put it slightly differently, in
     * normalized-seconds/iteration. This means that when you give a starting position to the constructor, or query the
     * current position, the unit you use is **seconds*samples/second**, or just **samples**. The unit of a rate is
     * **(seconds*samples/second)/iteration**, or just **samples/iteration**. The unit of phase is also in samples, but 
     * these are samples of the phasor, which are not the same units as the samples of the waveform. Therefore, we'll use
     * call these **iterations** to differentiate them.
     * 
     * In most cases when you give a starting position, or you query the current position, you're mostly concerned with
     * where the phasor is in the Waveform, and not where it is in its own time frame.
     *
     * Some examples:
     * 
     * 1) In general, the relationship between postion, phase and rate is: position = rate*phase.
     * 
     * 2) If the rate is 3.2, and the phase is 15, then the position in the Waveform will be 3.2*15=48 (samples).  If the
     * sampling rate is then 48k samples/second, the true time will be 48/48000 = 1ms, also in the Waveform. The time in 
     * the phasor will simply be 15 iterations, or 15/48000 = 0.3125ms. Note that 1/3.2 = 0.3125.
     * 
     * 3) If you give a start position of 4 and the rate is 0.3, then phase = floor(4/0.3) = 13. 
     * 
     */
    class phasor_impl {
    public:
      virtual ~phasor_impl(void) {}

    protected:
      friend class phasor;
      friend class Waveform;
      
      double _rate;             //!< The rate that the phase changes per iteration
      long _dir;                //!< The direction that the phase moves
      double _phase;              //!< The current phase
      const Waveform<T>& _wf;   //!< The waveform that we're phasing

      /*!\brief Constructs a phasor that starts at a particular position in the waveform. 
       * 
       * See the discussion in the phasor_impl class documentation on how the start position relates to the phase
       * and rate of the phasor.
       * 
       * \param wf The Waveform to iterate over.
       * \param start The starting position in the Waveform
       * \param rate The rate at which to iterate over the Waveform. Positive rate -> forward iteration, negative rate -> 
       *             reverse iteration.
       */
      phasor_impl(const Waveform<T>& wf, double start, double rate);

      phasor_impl(const phasor_impl& other);

      phasor_impl(double constant);

      virtual T value(void) const;                      //!<\brief Returns the value of the Waveform at the current phase
      virtual operator bool(void) const;                //!<\brief Always returns true

      /*!\brief Compare operators compare the location in the uninterpolated waveform (essentially position*rate)
       */
      bool operator==(const phasor_impl& other) const;
      bool operator!=(const phasor_impl& other) const;
      bool operator<(const phasor_impl& other) const;
      bool operator>(const phasor_impl& other) const;
      bool operator<=(const phasor_impl& other) const;
      bool operator>=(const phasor_impl& other) const;

      /*!\brief Sets the rate (useful for vari-rate iterations)
       */
      virtual void setRate(double rate);

      /*!\brief Returns the current position in the waveform
       */
      double getPosition(void) const {return phaseToPosition(_phase);}

      /*!\brief Sets the current position in the waveform
       */
      void setPosition(double pos) {_phase = positionToPhase(pos);}

      /*!\brief Returns the current phase of the phasor
       */
      double getPhase(void) const {return _phase;}

      /*!\brief Copies this phasor implementation
       *
       * This must be overridden by subclasses in order for phasor::operator++ and the phasor copy constructor
       * to work correctly.
       */
      virtual phasor_impl* copy(void);
      
      /*!\brief Increments the phase
       */
      virtual void increment(void);

      double phaseToPosition(double phase) const {return _rate*phase;}

      double positionToPhase(double pos) const {return pos/_rate;}
    };

    /*!\brief The public interfaces for the phasor, which is an iterator-like class used for generalized iteration over
     *        Waveforms.
     * 
     * This wraps a subclass of the phasor_impl and for the most part it acts just like a common iterator, except that
     * the phasor_impls that it wraps usually have a more complicated behavior than common iterators. 
     * 
     * The phasor provides access to interpolated, modulated, or otherwise mathematically complex Waveforms with a simple,
     * familiar interface. It is not meant to be inherited because all subclasses of Waveform deal directly in straight
     * phasors. Instead, the phasor_impl class should be inherited and passed in in the constructor.
     * 
     * phasors are initially created only through the Waveform::make_phasor() function. 
     * 
     */
    class phasor final {
    public:
      phasor(void);
      // phasor(const Waveform<T>* wf, double start, double rate);
      
      // /*!\brief Creates a vari-rate phasor for which the rate is seet by another phasor
      //  */ 
      // phasor(const Waveform<T>* wf, double start, const Waveform<T>::phasor vel_interp);

      phasor(const phasor& other);
      phasor& operator=(const phasor& other);
      phasor& operator++(void);                 //!<\brief Prefix increment
      phasor operator++(int);                   //!<\brief Postfix increment

      /*!\brief Retrieves the current value. Note that this is not a reference, as it would be with a common iterator
       * 
       */
      T operator*(void) const;
      operator bool(void) const;
      
      /*!\brief Compare operators compare the location in the uninterpolated waveform (essentially position*rate)
       */
      bool operator==(const phasor& other) const;
      bool operator!=(const phasor& other) const;
      bool operator<(const phasor& other) const;
      bool operator>(const phasor& other) const;
      bool operator<=(const phasor& other) const;
      bool operator>=(const phasor& other) const;

      /*!\brief Sets the rate (useful for vari-rate iterations)
       */
      void setRate(double rate) {_impl->setRate(rate);}

      /*!\brief Returns the current position in the waveform
       */
      double getPosition(void) const {return _impl->getPosition();}

      /*!\brief Sets the current position in the waveform
       */
      void setPosition(double pos) {_impl->setPosition(pos);}

      /*!\brief Returns the current phase of the phasor
       */
      long getPhase(void) const {return _impl->getPhase();}

    protected:
      friend Waveform<T>;
      
      /*!\brief Constructs a phasor with a phasor_impl pointer
       * 
       * \note This will take posession of the phasor_impl pointer so this should not be deleted anywhere else. 
       * 
       * \param impl The phasor_impl to wrap
       */
      phasor(phasor_impl* impl);
      
    private:
      std::unique_ptr<phasor_impl> _impl;
    };
    
    virtual ~Waveform(void) = 0;

    // virtual bool operator==(const Waveform<T>& other) const = 0;
    // virtual bool operator!=(const Waveform<T>& other) const = 0;
    
    /*!\brief Returns the waveform at a certain phase
     * 
     * \param pos The position of the waveform
     * \return The waveform at the given phase
     */
    virtual T waveform(double pos) const = 0;

    /*!\brief Returns the size of the waveform
     */
    virtual std::size_t size(void) const = 0;

    /*!\brief Returns the end of the waveform
     */
    virtual double end(void) const = 0;

    /*!\brief Provides access to phasor's protected constructor
     */
    phasor make_phasor(phasor_impl* impl) const {return phasor(impl);}

    virtual phasor pbegin(double rate, double start) const;
    
    /*!\brief Returns a phasor that starts at the beginning of the waveform
     */
    virtual phasor pbegin(double rate) const;
    
    virtual phasor rpbegin(double rate, double start) const;

    /*!\brief Returns a reverse phasor that starts at the end of the waveform
     */
    virtual phasor rpbegin(double rate) const;

  };

  template<typename T>
  class Constant : public Waveform<T> {

  public:

    Constant(T value) : _value(value) {}

    virtual ~Constant(void) {}

    virtual T waveform(double pos) const { return _value;}

    virtual std::size_t size(void) const { return 1;}

    virtual double end(void) const { return 1;}

    virtual typename Waveform<T>::phasor pbegin(double rate, double start) const;

    virtual typename Waveform<T>::phasor pbegin(double rate) const;
    
    virtual typename Waveform<T>::phasor rpbegin(double rate, double start) const;

    virtual typename Waveform<T>::phasor rpbegin(double rate) const;
    

  private:
    T _value;
  };

}
