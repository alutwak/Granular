/* (c) AudioElectric. All rights reserved.
 *
 * Author:             Ayal Lutwak <alutwak@audioelectric.com>
 * Date:               July 27, 2019
 * Last Modified By:   Ayal Lutwak <alutwak@audioelectric.com>
 * Last Modified Date: July 27, 2019
 */
#include "cloud.hpp"
#include "algorithm.hpp"
#include "waveform.hpp"

namespace audioelectric {

  template <typename T>
  Cloud<T>::Cloud(size_t fs) : _fs(fs)
  {
    setShape(DEFAULT_SHAPE);
    setCarrier(DEFAULT_CARRIER);
  }

  template <typename T>
  Cloud<T>::Cloud(size_t fs, int voices, Shape shape, Carrier carrier) : _fs(fs)
  {
    setShape(shape);
    setCarrier(carrier);
    setVoiceNumber(voices);
  }

  template <typename T>
  void Cloud<T>::startNote(T freq, T velocity)
  {
    GrainParams<T> p = _params;
    p.freq *= freq;
    p += _vel_mod*velocity;

    // We'll assume we've got some inactive voices to start with
    auto source = &_inactive;

    // First, check if we're already using this frequency
    auto new_voice = checkForActiveFreq(freq);
    if (new_voice != _active.end()) {
      // We have this frequency already
      source = &_active;
    }
    else if (_inactive.empty()) {
      // We're at the voice limit so we take the voice from the active list
      new_voice = _active.begin();
      source = &_active;
    }
    else {
      // We've got a free voice
      new_voice = _inactive.begin();
    }

    // Trigger the voice and put it at the end of the active voices
    new_voice->trigger(p);
    _active.splice(_active.end(), *source, new_voice);
  }

  template <typename T>
  void Cloud<T>::releaseNote(T freq)
  {
    auto voice = checkForActiveFreq(freq);
    if (voice != _active.end())
      voice->release();
  }

  template <typename T>
  T Cloud<T>::value(void) const
  {
    T out = 0;
    for (auto& voice : _active)
        out += voice.value();
    return out;
  }

  template <typename T>
  void Cloud<T>::increment(void)
  {
    incrementAndRemove(_active, _inactive);
  }

  template <typename T>
  void Cloud<T>::setVoiceNumber(int voices)
  {
    Voice<T> tmplt(_shape, _carrier);
    _active.clear();
    _inactive.resize(voices, tmplt);
  }

  template <typename T>
  void Cloud<T>::setShape(Shape shape)
  {

    switch(shape) {
    case Shape::Gaussian:
      GenerateGaussian(_shape, _fs, T(0.15));
    }
    
    for (auto& voice : _inactive)
      voice._graingen.setShape(_shape);
  }

  template <typename T>
  void Cloud<T>::setCarrier(Carrier carrier)
  {
    switch(carrier) {
    case Carrier::Sin:
      GenerateSin(_carrier, _fs);
      break;
    case Carrier::Triangle:
      GenerateTriangle(_carrier, _fs, T(0));
      break;
    case Carrier::Saw:
      GenerateTriangle(_carrier, _fs, T(0.8));
      break;
    case Carrier::Square:
      GenerateSquare(_carrier, _fs, T(0.5));
    }

    for (auto& voice : _inactive)
      voice._graingen.setCarrier(_carrier);
  }

  template <typename T>
  void Cloud<T>::setCarrier(std::string afile, size_t begin, size_t end)
  {
    _carrier = Waveform<T>(afile, begin, end);
  }

  /******************** Private Functions ********************/
  
  template <typename T>
  typename std::list<Voice<T>>::iterator Cloud<T>::checkForActiveFreq(T freq)
  {
    auto voice = _active.begin();
    for (; voice != _active.end(); voice++) {
      if (voice->_base_params.freq == freq)
        break;
    }
    return voice;
  }


  template class Cloud<double>;
  template class Cloud<float>;
  
}  // audioelectric
