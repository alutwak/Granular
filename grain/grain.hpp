/* \file grain.hpp
 * \brief Defines the interface for a grain
 *
 * (c) AudioElectric. All rights reserved.
 * 
 * Author:             Ayal Lutwak <alutwak@audioelectric.com>
 * Date:               June 26, 2019
 * Last Modified By:   Ayal Lutwak <alutwak@audioelectric.com>
 * Last Modified Date: June 26, 2019
 */

#include "phasor.hpp"

namespace audioelectric {

  template<typename T>
  class Grain final {
  public:
    /*!
     * \param carrier The carrier waveform
     * \param crate   The carrier rate
     * \param shape   The modulation shape
     * \param srate   The shape rate
     * \param ampl    The amplitude of the grain
     */
    Grain(Waveform<T>& carrier, double crate, Waveform<T>& shape, double srate, T ampl);

    Grain(Phasor<T>& carrier, Phasor<T>& shape, T ampl);

    Grain(Phasor<T>&& carrier, Phasor<T>&& shape, T ampl);    

    /*!\brief Returns the current value of the grain
     */
    T value(void) const;

    /*!\brief Increments the phases of the carrier and the shape
     */
    void increment(void);

    /*!\brief Returns true if the grain is still running
     */
    operator bool(void) const;

    void setCarrier(Waveform<T>& carrier) {_carrier.setWaveform(carrier);}

    void setShape(Waveform<T>& shape) {_shape.setWaveform(shape);}
    
    /*!\brief Sets the carrier rate
     */
    void setCarrierRate(double rate);

    /*!\brief Sets the shape rate
     */
    void setShapeRate(double rate);

    /*!\brief Sets the amplitude
     */
    void setAmplitude(T ampl);

    void setParams(double crate, double srate, T ampl);
    
    /*!\brief Resets the carrier and the shape back to their beginning phases
     */
    void reset(void);

  private:
    Phasor<T> _carrier;
    Phasor<T> _shape;
    T _ampl;
  };
  
}
