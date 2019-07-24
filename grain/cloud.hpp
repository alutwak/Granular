/* \file cloud.hpp
 * \brief Defines the Cloud class
 * 
 * (c) AudioElectric. All rights reserved.
 * 
 * Author:             Ayal Lutwak <alutwak@audioelectric.com>
 * Date:               July 23, 2019
 * Last Modified By:   Ayal Lutwak <alutwak@audioelectric.com>
 * Last Modified Date: July 23, 2019
 */
#pragma once

#include "voice.hpp"

namespace audioelectric {

  /*!\brief Describes the set of carriers
   */
  enum class Carrier {
    Sin,                //!< Sine wave
    Triangle,           //!< Triangle wave
    Saw,                //!< Saw wave
    Square,             //!< Square wave
  };

  /*!\brief Describes the set of shapes
   */
  enum class Shape {
    Gaussian,           //!< Gaussian
  };

  template <typename T>
  class Cloud final {

  public:

    /*!\brief Constructs a cloud with zero voices
     */
    Cloud(void);
    
    /*!\brief Constructs a Cloud with a specified number of voices
     * 
     * \param voices  The number of voices
     * \param shape   The shape to use
     * \param carrier The carrier to use
     */
    Cloud(int voices, Shape shape, Carrier carrier);

    /*!\brief Starts a new note by adding a voice to the list of active voices
     * 
     * If a note with the same frequency is already being played then this command will be ignored. 
     * 
     * If the maximum number of voices are already active then the oldest voice will be released to allow the new voice to
     * start.
     * 
     * \param freq The frequency of the note
     * \param ampl The amplitude of the note
     */
    void startNote(T freq, T ampl);

    /*!\brief Releases a note that's already playing
     * 
     * If there is no active voice with the given frequency then this command is ignored.
     * 
     * \param freq The frequency of the note to release
     */
    void releaseNote(T freq);

    T value(void) const;

    void increment(void);
    
    /*!\brief Sets the number of voices
     */
    void setVoiceNumber(int voices);

    void setShape(Shape shape);

    void setCarrier(Carrier carrier);

    GrainParams<T>& params(void) {return _params;}
    GrainParams<T>& rand(void) {return _rand;}
    Envelope<T>& env1(void) {return _env1;}
    Envelope<T>& env2(void) {return _env2;}
    GrainParams<T>& env1Mult(void) {return _env1_mult;}
    GrainParams<T>& env2Mult(void) {return _env2_mult;}
    
  private:

    // Voices
    std::list<Voice<T>> _active;
    std::list<Voice<T>> _inactive;

    // User Parameters
    GrainParams<T> _params;
    GrainParams<T> _rand;

    // Envelopes
    Envelope<T> _env1;
    Envelope<T> _env2;
    GrainParams<T> _env1_mult;          //!< Multipliers for envelope 1
    GrainParams<T> _env2_mult;          //!< Multipliers for envelope 2
    
  };
  
}  // audioelectric
