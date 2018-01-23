
#pragma once

#include <cstdlib>
#include <vector>
#include <utility>

namespace audioelectric {


  /*!\brief Provides an iterface for a variety of waveforms. These could be waveforms that get generated on the fly
   * or wavetables, samples, envelopes, etc...
   */
  template<typename T>
  class Waveform {
  public:

    class phasor;
    
    /*!\brief An iterator-like class that increments the phase of the waveform at a certain rate
     */
    class phasor_impl {
    protected:
      friend class phasor;
      friend class Waveform;
      
      double _rate;             //!< The rate that the phase changes per iteration
      long _dir;                //!< The direction that the phase moves
      long _phase;              //!< The current phase
      const Waveform<T>& _wf;   //!< The waveform that we're phasing

      phasor_impl(const Waveform<T>& wf, double start, double rate);
      phasor_impl(const phasor_impl& other);
      //phasor_impl& operator=(const phasor_impl& other);
      virtual T value(void) const;          //!<\brief Data retrieval (not a reference)
      virtual operator bool(void) const;
      bool operator==(const phasor_impl& other) const;
      bool operator!=(const phasor_impl& other) const;

      /*\brief Compare operators compare the location in the uninterpolated waveform (essentially position*rate)
       */
      bool operator<(const phasor_impl& other) const;
      bool operator>(const phasor_impl& other) const;
      bool operator<=(const phasor_impl& other) const;
      bool operator>=(const phasor_impl& other) const;

      /*!\brief Sets the rate (useful for vari-rate iterations)
       */
      void setRate(double rate);

      /*!\brief Copies this phasor implementation
       *
       * This must be overridden by subclasses in order for phasor::operator++ and the phasor copy constructor
       * to work correctly.
       */
      virtual phasor_impl* copy(void);
      
      /*!\brief Increments the phase
       */
      virtual void increment(void);
    };
 
    class phasor {
    public:
      phasor(void);
      phasor(phasor_impl* impl);
      // phasor(const Waveform<T>* wf, double start, double rate);
      
      // /*!\brief Creates a vari-rate phasor for which the rate is seet by another phasor
      //  */ 
      // phasor(const Waveform<T>* wf, double start, const Waveform<T>::phasor vel_interp);
      phasor(const phasor& other);
      phasor& operator=(const phasor& other);
      phasor& operator++(void);                 //!<\brief Prefix increment
      phasor operator++(int);                   //!<\brief Postfix increment
      //phasor operator+(long n) const;           //!<\brief Random access +
      T operator*(void) const;          //!<\brief Data retrieval (not a reference)
      operator bool(void) const;
      bool operator==(const phasor& other) const;
      bool operator!=(const phasor& other) const;

      /*\brief Compare operators compare the location in the uninterpolated waveform (essentially position*rate)
       */
      bool operator<(const phasor& other) const;
      bool operator>(const phasor& other) const;
      bool operator<=(const phasor& other) const;
      bool operator>=(const phasor& other) const;
    private:
      std::unique_ptr<phasor_impl> _impl;
    };
    
    virtual ~Waveform(void) = 0;

    // virtual bool operator==(const Waveform<T>& other) const = 0;
    // virtual bool operator!=(const Waveform<T>& other) const = 0;
    
    /*!\brief Returns the waveform at a certain phase
     * 
     * \param phase The phase of the waveform
     * \return The waveform at the given phase
     */
    virtual T waveform(double phase) const = 0;

    virtual phasor pbegin(double start, double rate) const;
    virtual phasor pbegin(double rate) const;
    virtual phasor rpbegin(double start, double rate) const;
    virtual phasor rpbegin(double rate) const;

  };

}
