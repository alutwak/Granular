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

#define DEFAULT_SHAPE Shape::Gaussian
#define DEFAULT_CARRIER Carrier::Sin

  template <typename T>
  class Cloud final {

  public:

    /*!\brief Constructs a cloud with zero voices and default shape and carrier
     */
    Cloud(size_t fs);
    
    /*!\brief Constructs a Cloud with a specified number of voices
     * 
     * \param fs      The samplerate
     * \param voices  The number of voices
     * \param shape   The shape to use
     * \param carrier The carrier to use
     */
    Cloud(size_t fs, int voices, Shape shape, Carrier carrier);

    /*!\brief Starts a new note by adding a voice to the list of active voices
     * 
     * The velocity will be used to modulate the user parameters.
     * 
     * If the maximum number of voices are already active then the oldest voice will be retriggered with the new parameters.
     * 
     * If a note with the same frequency is already being played then this command will be ignored. 
     * 
     * \param freq     The frequency of the note
     * \param velocity The velocity of the note.
     */
    void startNote(T freq, T velocity);

    /*!\brief Releases a note that's already playing
     * 
     * If there is no active voice with the given frequency then this command is ignored.
     * 
     * \param freq The frequency of the note to release
     */
    void releaseNote(T freq);

    T value(void) const;

    void increment(void);
    
    /*!\brief Sets the number of voices.
     * 
     * This will remove all active voices without releasing them, so calling it when there are active voices will cause an
     * audible glitch.
     */
    void setVoiceNumber(int voices);

    void setShape(Shape shape);

    void setCarrier(Carrier carrier);

    GrainParams<T>& params(void) {return _params;}
    GrainParams<T>& velocityModulators(void) {return _vel_mod;}
    GrainParams<T>& rand(void) {return _rand;}
    Envelope<T>& env1(void) {return _env1;}
    Envelope<T>& env2(void) {return _env2;}
    GrainParams<T>& env1Mult(void) {return _env1_mult;}
    GrainParams<T>& env2Mult(void) {return _env2_mult;}
    
  private:

    size_t _fs;      //!< The sample rate
    
    // Waveforms
    Waveform<T> _shape;
    Waveform<T> _carrier;
    
    // Voices
    std::list<Voice<T>> _active;        //!< The active voices 
    std::list<Voice<T>> _inactive;      //!< The inactive voices

    // User Parameters
    GrainParams<T> _params;     //!< Base parameters. freq->tuning, ampl->overall volume, density & length -> base grains
    GrainParams<T> _vel_mod;    //!< Amount to modulate the parameters based on velocity
    GrainParams<T> _rand;       //!< The randomization parameters

    // Envelopes
    Envelope<T> _env1;
    Envelope<T> _env2;
    GrainParams<T> _env1_mult;          //!< Multipliers for envelope 1
    GrainParams<T> _env2_mult;          //!< Multipliers for envelope 2

    typename std::list<Voice<T>>::iterator checkForActiveFreq(T freq);
    
  };
  
}  // audioelectric
