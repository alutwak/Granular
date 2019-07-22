/* \file voice.hpp
 * \brief Defines the Voice class
 *
 * (c) AudioElectric. All rights reserved.
 * 
 * Author:             Ayal Lutwak <alutwak@audioelectric.com>
 * Date:               July 21, 2019
 * Last Modified By:   Ayal Lutwak <alutwak@audioelectric.com>
 * Last Modified Date: July 21, 2019
 */
#pragma once

#include "graingenerator.hpp"
#include "envelope.hpp"

namespace audioelectric {

  template <typename T> class Cloud;

  /*!\brief Controls a voice in the grain cloud
   * 
   * A voice controls the output of a note from beginning to end. Once the voice is triggered, it will evaluate to true
   * until the note has been released and all envelopes and grains have completed. At that point it may be safely removed
   * from the set of active voices.
   */
  template <typename T>
  class Voice final {

    friend class Cloud<T>;
    
  public:

    Voice(Waveform<T>& shape, Waveform<T>& carrier, Envelope<T>& env1, Envelope<T>& env2);

    /*!\brief Evaluates to true if the voice is active and false otherwise
     */
    operator bool(void) const {return _env1 || _env2 || _graingen;}

    T value(void) const {return _graingen.value();}

    void increment(void);

    /*!\brief Starts the voice
     *
     * \param params The "base" parameters of the note (which may get modulated by the envelopes)
     */
    void trigger(GrainParams<T> params);

    /*!\brief Causes both envelopes to be released
     */
    void release(void);

  private:

    Envelope<T> _env1;                  //!< Envelope 1
    GrainParams<T> _env1_mult;          //!< Multipliers for envelope 1
    Envelope<T> _env2;                  //!< Envelope 2
    GrainParams<T> _env2_mult;          //!< Multipliers for envelope 2
    GrainGenerator<T> _graingen;        //!< The grain generator
    GrainParams<T> _base_params;        //!< The base parameters
  };
  
}
