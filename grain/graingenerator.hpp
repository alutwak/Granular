/* \file graingenerator.hpp
 * \brief Defines the GrainGenerator class
 *
 * (c) AudioElectric. All rights reserved.
 * 
 * Author:             Ayal Lutwak alutwak@audioelectric.com
 * Date:               June 28, 2019
 * Last Modified By:   Ayal Lutwak alutwak@audioelectic.com
 * Last Modified Date: June 28, 2019
 */

#include <list>
#include <random>
#include "grain.hpp"

namespace audioelectric {

  /*!\brief Describes the set of carriers
   */
  enum class Carriers {
    Sin,                //!< Sine wave
    Triangle,           //!< Triangle wave
    Saw,                //!< Saw wave
    Square,             //!< Square wave
  };

  /*!\brief Describes the set of shapes
   */
  enum class Shapes {
    Gaussian,           //!< Gaussian
  };

  /*!\brief Generates grains according to various parameters
   */
  template <typename T>
  class GrainGenerator final {
  public:
    
    GrainGenerator(double fs);

    /*!\brief Returns the sum of all the active grains
     */
    T value(void);

    /*!\brief Increments all of the active grains
     */
    void increment(void);

    /*!\brief Generates a new grain if it needs to be generated and recycles old grains
     */
    void updateGrains(double density, double length, double freq, T ampl);

    /*!\brief Sets the carrier waveform to use
     */
    void setCarrier(Carriers wvfm);

    /*!\brief Sets the grain shape
     */
    void setShape(Shapes shape); 

    /*!\brief Sets the amount of desnity randomization [0,1]
     */
    void setDensityRand(double rand) {_density_rnd = rand;}

    /*!\brief Sets the amount of length randomization [0,1]
     */
    void setLengthRand(double rand) {_length_rnd = rand;}

    /*!\brief Sets the amount of amplitude randomization [0,1]
     */
    void setAmplRand(double rand) {_ampl_rnd = rand;}

    /*!\brief Sets the amount of frequency randomization [0,1]
     */
    void setFrequRand(double rand) {_freq_rnd = rand;}
    
  private:
    double _fs;                                 //!< The sample rate

    // Random
    std::mt19937 _gen;
    std::uniform_real_distribution<> _rand;      //!< Random number generator
    
    // Grains
    std::list<Grain<T>*> _active;       //!< The active grains
    std::list<Grain<T>*> _inactive;     //!< The inactive grains
    size_t _last_grain_t;               //!< The time since the last grain was generated

    // Controls (signals that are controlled by the generator)
    Waveform<T> _carrier;               //!< The carrier waveform
    Waveform<T> _shape;                 //!< The shape waveform
    double _density_rnd;                //!< The density randomization amount
    double _length_rnd;                 //!< The length randomization amount
    double _freq_rnd;                   //!< The frequency randomization amount
    double _ampl_rnd;                   //!< The amplitude randomization amount

    void _allocateGrains(void);

    void _moveAndSetGrain(double density, double length, double freq, T ampl);

    /*!\brief Generates a random number on the interval of [-1,1]
     * 
     * \todo Right now this uses the mt19937 algorithm, but at some point I need to run some tests to find the fastest
     *       algorithm
     */
    inline double _random(void);
    
  };

}  // audioelectric
