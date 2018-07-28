
#pragma once

#include <list>
#include <utility>
#include <memory>

#include "waveform.hpp"

namespace audioelectric {

  
  /*!\brief A generic envelope
   * 
   * This needs to be some type of function in order to handle the intricacies of an envelope. However, it's going to need to
   * wrap several interconnected functions together, order them, and then increment through them. I think it will be easiest
   * and most efficient if the phasor implementation that accesses the Envelope also has access to all of the functions and 
   * decides how to iterate through them. 
   * 
   * One thing I could do is still represent the phases as a list of Waveforms that the Envelope stores and feeds up to the
   * phasor. Then I could still use the design that I planned without having to sacrifice the design pattern of the phasor
   * immpl that only stores a single Waveform and accesses it through a phase argument. In this case the "phase" would combine
   * the Waveform phase (position in the function) with the Envelope phase (function in the envelope).
   */
  class Envelope : public Waveform<double> {
  public:

    enum class EnvPhase {
      untrig,
      delay,
      attack,
      hold,
      decay,
      release
    };

    friend class enveloper;
    class enveloper : public phasor_impl {
    public:
      virtual ~enveloper(void) {}

      enveloper(Envelope& env, double rate);

      enveloper(const enveloper& other);

      //Putting this here isn't going to work because it's the phasor that the client is going to have ahold of.
      void trigger(bool trig);

    protected:

      virtual enveloper* copy(void) const {return new enveloper(*this);}

      virtual double value(void) const;

      virtual void increment(void);

    private:

      Envelope& _env;

      EnvPhase _envphase;
      
    };

    Envelope(const Envelope& other);
    
    virtual ~Envelope(void) {};

    virtual void setDelay(double delay);
    
    virtual void setAttack(double attack);

    virtual void setHold(double hold);

    virtual void setDecay(double decay);

    virtual void setSustain(double sustain);
    
    virtual void setRelease(double release);

  protected:

    double _out;        //!< The current output
    double _rel_amp;    //!< The amplitude at the time of release
    bool _triggered;    //!< Whether the envelope is in the triggered state
    
    double _delay;      //!< The decay time (in samples)
    double _attack;     //!< The attack time (in samples)
    double _hold;       //!< The hold time (in samples)
    double _decay;      //!< The decay time (in samples)
    double _sustain;    //!< The sustain amplitude [0-1]
    double _release;    //!< The release time (in samples)

    /*!\brief Essentially pos < _delay. No change to _out
     */
    virtual double delay(double pos);
    
    /*!\brief Sets _out according to the attack parameters. Returns false when the attack phase is over
     */
    virtual double attack(double pos);

    /*!\brief Essentially pos < _hold. No change to _out.
     */
    virtual double hold(double pos);

    /*!\brief Sets _out according to the decay and sustain parameters. Always returns true.
     */
    virtual double decay(double pos);

    /*!\brief Sets _out according to the release parameters. Always returns true
     */
    virtual double release(double pos);

  };
  
}


