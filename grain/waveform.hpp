
#pragma once

#include <cstdlib>
#include <vector>
#include <utility>

namespace audioelectric {

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
    
    /*!\brief The class that actually implements the behavior of the phasor. 
     * 
     * This is the class that must be inherited in order to implement new types of phasors. 
     */
    class phasor_impl {
    public:
      virtual ~phasor_impl(void) {}

      /*!\brief Constructs a phasor that starts at a particular position in the waveform. 
       * 
       * See the discussion in the phasor_impl class documentation on how the start position relates to the phase
       * and rate of the phasor.
       * 
       * \param wf The Waveform to iterate over.
       * \param rate The rate at which to iterate over the Waveform. Positive rate -> forward iteration, negative rate -> 
       *             reverse iteration.
       * \param start The starting position in the Waveform
       */
      phasor_impl(const Waveform<T>& wf, double rate, double start);

      phasor_impl(const phasor_impl& other);

    protected:
      friend class phasor;
      friend class Waveform;

      double _rate;             //!< The rate that the phase changes per iteration
      double _phase;            //!< The current phase
      const Waveform<T>& _wf;   //!< The waveform that we're phasing

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

      /*!\brief Returns the current phase of the phasor
       */
      double getPhase(void) const {return _phase;}

      /*!\brief Copies this phasor implementation
       *
       * This must be overridden by subclasses in order for phasor::operator++ and the phasor copy constructor
       * to work correctly.
       */
      virtual phasor_impl* copy(void) const;
      
      /*!\brief Increments the phase
       */
      virtual void increment(void);
    };

    class mod_phasor : public phasor_impl {
    public:
      virtual ~mod_phasor(void) {}

      mod_phasor(const Waveform<T>& wf, const phasor& rates, double start);

      mod_phasor(const mod_phasor& other);

    protected:
      friend class Waveform;

      virtual void setModulator(const phasor& rates);

      virtual mod_phasor* copy(void) const;

      virtual void increment(void);

    private:

      phasor _modulator;
      
    };

    /*!\brief An iterator-like class that increments the phase of the waveform at a certain rate
     * 
     * The phasor provides access to interpolated, modulated, or otherwise mathematically complex Waveforms with a simple,
     * familiar interface. It is not meant to be inherited because all subclasses of Waveform deal directly in straight
     * phasors. Instead, the phasor_impl class should be inherited and passed in in the constructor.
     * 
     * phasors are initially created only through the Waveform::make_phasor() function. 
     * 
     * \note Phases are in units of **samples** and rates are in units of **samples/iteration**. It may seem strange when I
     * talk about units of samples that are represented in floating point values. But because the phasor is meant to be used
     * for synthesis, rather than simple audio playback and processing, samples become somewhat fuzzy untis. Even when we're
     * using wavetables and sound samples, we're going to want to interpolate between the original samples for variable pitch
     * sampling. In the end, it's just easier to express our waveforms as continuous functions and let the back end deal with
     * the digital realities.
     */
    class phasor final {
    public:
      phasor(void);
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

      /*!\brief Returns the current phase of the phasor
       */
      double getPhase(void) const {return _impl->getPhase();}

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
  
}
