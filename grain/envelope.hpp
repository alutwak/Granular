/* \file envelope.hpp
 * \brief Contains the Envelope class
 *
 * (c) AudioElectric. All rights reserved.
 * 
 * Author:             Ayal Lutwak <alutwak@audioelectric.com>
 * Date:               July 14, 2019
 * Last Modified By:   Ayal Lutwak <alutwak@audioelectric.com>
 * Last Modified Date: July 14, 2019
 */
#pragma once

#include <list>
#include <utility>
#include <memory>

#include "waveform.hpp"

namespace audioelectric {

  
  /*!\brief A generic envelope
   */
  template <typename T>
  class Envelope final {
  public:

    Envelope(double delay, double attack, double hold,  double decay, double sustain, double release);

    Envelope(double attack, double decay, double sustain, double release);

    /*!\brief Returns the current value of the envelope. 
     * 
     * This value is undefined if operator bool returns false
     */
    double value(void) const {return _out;}

    void increment(void);

    operator bool(void) const {return _phase != EnvPhase::inactive;}

    /*!\brief Controls the gate value
     *
     * \param g The gate value. true opens the gate and false closes it
     */
    void gate(bool g);

    /*!\brief Sets the delay [0..inf]
     */
    void setDelay(size_t delay);
    
    /*!\brief Sets the attack in samples [1..inf]
     */
    void setAttack(size_t attack);

    /*!\brief Sets the hold in samples [0..inf]
     */
    void setHold(size_t hold);

    /*!\brief Sets the decay in samples [1..inf]
     */
    void setDecay(size_t decay);

    /*!\brief Sets the sustain amplitude [0..1]
     */
    void setSustain(T sustain);

    /*!\brief Sets the release in samples [1..inf]
     */
    void setRelease(size_t release);

  private:

    enum class EnvPhase {
      inactive,
      del,
      att,
      hol,
      dec,
      sus,
      rel
    };

    T _out;             //!< The current output
    T _slope;           //!< The current slope
    EnvPhase _phase;    //!< The current phase of the envelope
    size_t _phs_rem;    //!< The remaining time in the current phase

    size_t _delay;      //!< The delay time (in samples)
    size_t _attack;     //!< The attack time (in samples)
    size_t _hold;       //!< The hold time (in samples)
    size_t _decay;      //!< The decay time (in samples)
    T _sustain;         //!< The sustain amplitude [0-1]
    size_t _release;    //!< The release time (in samples)

    void _updatePhase(void);
    
  };
  
}


