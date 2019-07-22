/* (c) AudioElectric. All rights reserved.
 *
 * Author:             Ayal Lutwak <alutwak@audioelectric.com>
 * Date:               July 14, 2019
 * Last Modified By:   Ayal Lutwak <alutwak@audioelectric.com>
 * Last Modified Date: July 14, 2019
 */
#include <cmath>

#include "envelope.hpp"

namespace audioelectric {

  template <typename T>
  Envelope<T>::Envelope(double delay, double attack, double hold, double decay, double sustain, double release) :
    _delay(delay), _attack(attack), _hold(hold), _decay(decay), _sustain(sustain), _release(release),
    _phase(EnvPhase::inactive), _phs_rem(0), _out(0)
  {

  }

  template <typename T>
  Envelope<T>::Envelope(double attack, double decay, double sustain, double release) :
    _delay(0), _attack(attack), _hold(0), _decay(decay), _sustain(sustain), _release(release),
    _phase(EnvPhase::inactive), _phs_rem(0), _out(0)
  {

  }

  template <typename T>
  Envelope<T>::Envelope(double delay, double attack, double release) :
    _delay(delay), _attack(attack), _hold(0), _decay(0), _sustain(1), _release(release),
    _phase(EnvPhase::inactive), _phs_rem(0), _out(0)
  {

  }

  template <typename T>
  void Envelope<T>::increment(void)
  {
    if (!*this || _phase == EnvPhase::sus) return;
    _out += _slope;
    _phs_rem--;
    _updatePhase();
  }

  template <typename T>
  void Envelope<T>::gate(bool g)
  {
    if (g && _phase == EnvPhase::inactive) {
      _updatePhase();
    }
    else if (!g && _phase != EnvPhase::inactive) {
      _phase = EnvPhase::rel;
      _phs_rem = _release;
      _slope = -_out/_release;
      _updatePhase();
    }
  }

  template <typename T>
  void Envelope<T>::_updatePhase(void)
  {
    while (_phs_rem == 0) {
      switch (_phase) {
      case EnvPhase::inactive:
        _phase = EnvPhase::del;
        _phs_rem = _delay;
        _slope = 0;
        break;
      case EnvPhase::del:
        _phase = EnvPhase::att;
        _phs_rem = _attack;
        _slope = 1./_attack;
        break;
      case EnvPhase::att:
        _phase = EnvPhase::hol;
        _phs_rem = _hold;
        _slope = 0;
        break;
      case EnvPhase::hol:
        _phase = EnvPhase::dec;
        _phs_rem = _decay;
        _slope = -(_out - _sustain)/_decay;
        break;
      case EnvPhase::dec:
        _phase = EnvPhase::sus;
        // We don't care about slope or _phs_rem because increment() returns immediately in the sus phase
        return;
      case EnvPhase::sus:
        // Shouldn't get here
        return;
      case EnvPhase::rel:
        _phase = EnvPhase::inactive;
        _phs_rem = 0;
        _out = 0;
        return;
      }
    }
  }

  template <typename T>
  void Envelope<T>::setDelay(size_t delay)
  {
    if (_phase == EnvPhase::del) {
      size_t eaten = _delay - _phs_rem;
      if (delay <= eaten) {
        _phs_rem = 0;
        _updatePhase();
      }
      else {
        _phs_rem = delay-eaten;
      }
    }
    _delay = delay;
  }

  template <typename T>
  void Envelope<T>::setAttack(size_t attack)
  {
    if (_phase == EnvPhase::att) {
      size_t eaten = _attack - _phs_rem;
      if (attack <= eaten) {
        _phs_rem = 0;
        _out = 1;
        _updatePhase();
      }
      else {
        _phs_rem = attack-eaten;
        _slope = (1.-_out)/_phs_rem;
      }
    }
    _attack = attack;
  }

  template <typename T>
  void Envelope<T>::setHold(size_t hold)
  {
    if (_phase == EnvPhase::hol) {
      size_t eaten = _hold - _phs_rem;
      if (hold <= eaten) {
        _phs_rem = 0;
        _updatePhase();
      }
      else {
        _phs_rem = hold-eaten;
      }
    }
    _hold = hold;
  }

  template <typename T>
  void Envelope<T>::setDecay(size_t decay)
  {
    if (_phase == EnvPhase::dec) {
      size_t eaten = _decay - _phs_rem;
      if (decay <= eaten) {
        _phs_rem = 0;
        _out = _sustain;
        _updatePhase();
      }
      else {
        _phs_rem = decay-eaten;
        _slope = -(_out - _sustain)/_phs_rem;
      }
    }
    _decay = decay;
  }

  template <typename T>
  void Envelope<T>::setSustain(T sustain)
  {
    if (_phase == EnvPhase::dec)
      _slope = -(_out - sustain)/_phs_rem;
    else if (_phase == EnvPhase::sus)
      _out = sustain;
    _sustain = sustain;
  }

  template <typename T>
  void Envelope<T>::setRelease(size_t release)
  {
    if (_phase == EnvPhase::rel) {
      size_t eaten = _release - _phs_rem;
      if (release <= eaten) {
        _phs_rem = 0;
        _out = 0;
        _updatePhase();
      }
      else {
        _phs_rem = release-eaten;
        _slope = -_out/_phs_rem;
      }
    }
    _release = release;
  }

  template class Envelope<double>;
  template class Envelope<float>;

}
