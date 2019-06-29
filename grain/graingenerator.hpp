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

#include <vector>
#include "grain.hpp"

namespace audioelectric {

  /*!\brief Describes the set of carriers
   */
  enum Carriers {
    Sin,                //!< Sine wave
    Saw,                //!< Saw wave
    Triangle,           //!< Triangle wave
    Square,             //!< Square wave
    CarriersSize
  };

  /*!\brief Describes the set of shapes
   */
  enum Shapes {
    Gaussian,           //!< Gaussian
    ShapesSize
  };

  /*!\brief Generates grains according to various parameters
   */
  template <typename T>
  class GrainGenerator final {
  public:
    
    GrainGenerator(void);

    /*!\brief Returns the sum of all the active grains
     */
    T value(void);

    /*!\brief Increments all of the active grains
     */
    void increment(void);

    /*!\brief Updates the values of the inputs
     */
    void adjustInputs(double density, double length, double freq, double ampl);

    /*!\brief Sets the carrier waveform to use
     */
    void setCarrier(Carriers wvfm);

    /*!\brief Sets the grain shape
     */
    void setShape(Shapes shape);

    /*!\brief Sets the amount of desnity randomization [0,1]
     */
    void setDensityRand(double rand);

    /*!\brief Sets the amount of length randomization [0,1]
     */
    void setLengthRand(double rand);

    /*!\brief Sets the amount of amplitude randomization [0,1]
     */
    void setAmplRand(double rand);

    /*!\brief Sets the amount of frequency randomization [0,1]
     */
    void setFrequRand(double rand);
    
  private:
    // Grains
    std::vector<Grain<T>*> _active;     //!< The active grains
    std::vector<Grain<T>*> _inactive;   //!< The inactive grains
    size_t _last_grain_t;               //!< The time since the last grain was generated

    // Inputs (signals that come from outside)
    double _density;                    //!< The last density input
    double _length;                     //!< The last length input
    double _freq;                       //!< The last frequency input
    double _ampl;                       //!< The last amplitude input
    
    // Controls (signals that are controlled by the generator)
    Carriers _crnt_car;                         //!< The current carrier waveform
    Waveform<T> _waveforms[CarriersSize];       //!< The set of available carrier waveforms
    Shapes _crnt_shape;                         //!< The current shape waveform
    Waveform<T> _shapes[ShapesSize];            //!< The set of available shape waveforms
    double density_rnd;                         //!< The density randomization amount
    double length_rnd;                          //!< The length randomization amount
    double freq_rnd;                            //!< The frequency randomization amount
    double ampl_rnd;                            //!< The amplitude randomization amount
  };

}  // audioelectric
