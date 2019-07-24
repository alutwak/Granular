/* (c) AudioElectric. All rights reserved.
 *
 * Author:             Ayal Lutwak <alutwak@audioelectric.com>
 * Date:               July 21, 2019
 * Last Modified By:   Ayal Lutwak <alutwak@audioelectric.com>
 * Last Modified Date: July 21, 2019
 */

#include "voice.hpp"

namespace audioelectric {

  template <typename T>
  Voice<T>::Voice(Waveform<T>& shape, Waveform<T>& carrier) :
    _graingen(shape, carrier), _env1_mult(0, 0, 0, 0), _env2_mult(0, 0, 0, 0)
  {

  }

  template <typename T>
  void Voice<T>::increment(void)
  {
    /* Note: if we need to switch using a control rate for the controls then we can divide the length of all envelope
     * parameters by the control rate, and then only do the parameter calculations and the envelope increments on every
     * nth increment.
     */
    _env1.increment();
    _env2.increment();
    GrainParams<T> params = _base_params;
    GrainParams<T> mod = _env1.value()*_env2_mult + _env2.value()*_env2_mult;
    params.modulate(mod);
    _graingen.applyInputs(params);
    _graingen.increment();
  }

  template <typename T>
  void Voice<T>::trigger(GrainParams<T> params)
  {
    _base_params = params;
    _env1.gate(true);
    _env2.gate(true);
  }

  template <typename T>
  void Voice<T>::release(void)
  {
    _env1.gate(false);
    _env2.gate(false);
  }

  template class Voice<double>;
  template class Voice<float>;

}  // audioelectric
