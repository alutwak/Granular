
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

    /*!\brief An iterator-like class that increments the phase of the waveform at a certain rate
     */
    class phasor {
    public:
      phasor(void);
      phasor(const Waveform<T>& wf, double start, double rate);
      phasor(const Waveform<T>& wf, long start_pos, double rate);
      
      /*!\brief Creates a vari-rate phasor for which the rate is seet by another phasor
       */ 
      phasor(const Waveform<T>& wf, long start_pos, const Waveform<T>::phasor vel_interp);
      phasor(const phasor& other);
      phasor& operator=(const phasor& other);
      phasor& operator++(void);                 //!<\brief Prefix increment
      phasor operator++(int);                   //!<\brief Postfix increment
      phasor operator+(long n) const;           //!<\brief Random access +
      virtual T operator*(void) const;          //!<\brief Data retrieval (not a reference)
      virtual operator bool(void) const;
      virtual bool operator==(const phasor& other) const;
      virtual bool operator!=(const phasor& other) const;

      /*\brief Compare operators compare the location in the uninterpolated waveform (essentially position*rate)
       */
      virtual bool operator<(const phasor& other) const;
      virtual bool operator>(const phasor& other) const;
      virtual bool operator<=(const phasor& other) const;
      virtual bool operator>=(const phasor& other) const;

      /*!\brief Sets the rate (useful for vari-rate iterations)
       */
      void setRate(double rate);
      
    protected:
      double _rate;             //!< The rate that the phase changes per iteration
      long _dir;                //!< The direction that the phase moves
      long _phase;              //!< The current phase
      const Waveform<T>* _wf;   //!< The waveform that we're phasing

      /*!\brief Increments the phase
       */
      virtual void increment(void);
    };

    virtual ~Waveform(void) = 0;

    virtual bool operator==(const Waveform<T>& other) const = 0;
    virtual bool operator!=(const Waveform<T>& other) const = 0;
    
    /*!\brief Returns the waveform at a certain phase
     * 
     * \param phase The phase of the waveform
     * \return The waveform at the given phase
     */
    virtual T waveform(double phase) const = 0;

    virtual std::unique_ptr<phasor> pbegin(double start, double rate) const;
    virtual std::unique_ptr<phasor> pbegin(double rate) const;
    virtual std::unique_ptr<phasor> rpbegin(double start, double rate) const;
    virtual std::unique_ptr<phasor> rpbegin(double rate) const;

  };

}
