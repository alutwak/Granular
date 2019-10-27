/* \file graingenerator.hpp
 * \brief Defines the GrainGenerator class
 *
 * (c) AudioElectric. All rights reserved.
 *
 * Author:             Ayal Lutwak <alutwak@audioelectric.com>
 * Date:               June 28, 2019
 * Last Modified By:   Ayal Lutwak <alutwak@audioelectic.com>
 * Last Modified Date: June 28, 2019
 */

#include <list>
#include <random>
#include "grain.hpp"

namespace audioelectric {

  template <typename T>
  struct GrainParams {
    T density;  //!< The number of grains per sample
    T length;   //!< The length of the grains (in samples)
    T freq;     //!< The frequency of the carrier (normalized to the sample rate)
    T ampl;     //!< The amplitude of the grains [0,1]
    T front;    //!< The front position of the waveform
    T back;     //!< The back position of the waveform

    GrainParams<T>(T d, T l, T fq, T a, T ft=0, T b=-1) : density(d), length(l), freq(fq), ampl(a), front(ft), back(b) {}
    GrainParams<T>(void) : density(1e-9), length(0), freq(0), ampl(0), front(0), back(-1) {}
    void modulate(GrainParams<T>& other);
    GrainParams<T>& operator *=(T rhs);
    GrainParams<T>& operator *=(const GrainParams<T>& rhs);
    GrainParams<T>& operator +=(const GrainParams<T>& rhs);
    friend GrainParams<T>& operator*(GrainParams<T> lhs, T rhs);
    friend GrainParams<T>& operator*(T lhs, GrainParams<T> rhs);
    friend GrainParams<T>& operator*(GrainParams<T> lhs, const GrainParams<T>& rhs);
    friend GrainParams<T>& operator+(GrainParams<T> lhs, const GrainParams<T>& rhs);
  };


  /*!\brief Generates grains according to various parameters
   */
  template <typename T>
  class GrainGenerator final {
  public:

    GrainGenerator(Waveform<T>& shape, Waveform<T>& carrier);

    GrainGenerator(void) = delete;

    operator bool(void) const;

    /*!\brief Returns the sum of all the active grains
     */
    T value(void) const;

    /*!\brief Increments all of the active grains
     */
    void increment(void);

    /*!\brief Updates the values of the inputs
     *
     * \param params The input parameters
     */
    void applyInputs(GrainParams<T> params);

    /*!\brief Sets the carrier waveform to use
     */
    void setCarrier(Waveform<T>& carrier) {_carrier = carrier;}

    /*!\brief Sets the grain shape
     */
    void setShape(Waveform<T>& shape) {_shape = shape;}

    /*!\brief Sets the random parameters
     */
    void setRandParams(GrainParams<T> rand) {_rand = rand;}
    
    /*!\brief Sets the amount of desnity randomization [0,1]
     */
    void setDensityRand(double rand) {_rand.density = rand;}

    /*!\brief Sets the amount of length randomization [0,1]
     */
    void setLengthRand(double rand) {_rand.length = rand;}

    /*!\brief Sets the amount of amplitude randomization [0,1]
     */
    void setAmplRand(double rand) {_rand.ampl = rand;}

    /*!\brief Sets the amount of frequency randomization [0,1]
     */
    void setFreqRand(double rand) {_rand.freq = rand;}

  private:

    // Random
    std::mt19937 _gen;                          //!< Random number algorithm
    std::uniform_real_distribution<> _dist;     //!< Random number generator

    // Grains
    std::list<Grain<T>> _active;       //!< The active grains
    std::list<Grain<T>> _inactive;     //!< The inactive grains
    double _last_grain_t;              //!< The time since the last grain was generated
    double _rand_grain_t;              //!< The time of the next grain

    // Inputs (signals that come from signal generators of some sort)
    GrainParams<T> _params;

    // Controls (settings that are controlled by the user)
    Waveform<T>& _carrier;              //!< The carrier waveform
    Waveform<T>& _shape;                //!< The shape waveform
    GrainParams<T> _rand;               //!< Thre randomization amount for the params

    void _allocateGrains(void);

    void _moveAndSetGrain(double crate, double srate, T ampl, double front, double back);

    /*!\brief Generates a random number on the interval of [-1,1]
     *
     * \todo Right now this uses the mt19937 algorithm, but at some point I need to run some tests to find the fastest
     *       algorithm
     */
    inline double _random(void);

  };

}  // audioelectric
