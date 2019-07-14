
#pragma once

#include <list>
#include <utility>
#include <memory>

#include "waveform.hpp"

namespace audioelectric {

  
  /*!\brief A generic envelope
   */
  class Envelope final {
  public:

    Envelope(double attack, double decay, double sustain, double release);
    
    double value(void) {return _out;}

    void increment(void);

    operator bool(void);

    /*!\brief Controls the gate value
     *
     * \param g The gate value. true opens the gate and false closes it
     */
    void gate(bool g);

    /*!\brief Sets the attack in samples
     */
    void setAttack(double attack);

    /*!\brief Sets the decay in samples
     */
    void setDecay(double decay);

    /*!\brief Sets the sustain amplitude [0-1]
     */
    void setSustain(double sustain);

    /*!\brief Sets the release in samples
     */
    void setRelease(double release);

  private:

    enum class EnvPhase {
      inactive,
      att,
      dec,
      sus,
      rel
    };

    double _out;        //!< The current output
    double _slope;      //!< The current slope
    EnvPhase _phase;    //!< The current phase of the envelope
    double _phs_rem;    //!< The remaining time in the current phase
    
    double _attack;     //!< The attack time (in samples)
    double _decay;      //!< The decay time (in samples)
    double _sustain;    //!< The sustain amplitude [0-1]
    double _release;    //!< The release time (in samples)

  };
  
}


