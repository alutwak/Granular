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

  Envelope::Envelope(double delay, double attack, double hold, double decay, double sustain, double release) :
    _delay(delay), _attack(attack), _hold(hold), _decay(decay), _sustain(sustain), _release(release),
    _phase(EnvPhase::inactive), _out(0)
  {
    
  }

  Envelope::Envelope(double attack, double decay, double sustain, double release) :
    _delay(0), _attack(attack), _hold(0), _decay(decay), _sustain(sustain), _release(release),
    _phase(EnvPhase::inactive), _out(0)
  {
    
  }

  void Envelope::increment(void)
  {
    if (!*this || _phase == EnvPhase::sus) return;
    _out += _slope;
    _phs_rem--;
    _updatePhase();
  }

  void Envelope::gate(bool g)
  {
    if (g && _phase == EnvPhase::inactive) {
      _updatePhase();
    }
    else if (!g && _phase != EnvPhase::inactive) {
      _phase = EnvPhase::rel;
      _phs_rem = _release;
      _slope = -_out/_release;
    }
  }

  void Envelope::_updatePhase(void)
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
  
}
