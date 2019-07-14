
#include <cmath>

#include "envelope.hpp"

namespace audioelectric {

  Envelope::Envelope(double attack, double decay, double sustain, double release) :
    _attack(attack), _decay(decay), _sustain(sustain), _release(release), _phase(EnvPhase::inactive)
  {
    
  }

  Envelope::operator bool(void)
  {
    return _phase != EnvPhase::inactive;
  }

  void Envelope::increment(void)
  {
    if (!*this || _phase == EnvPhase::sus) return;
    _out += _slope;
    _phs_rem--;
    if (_phs_rem <= 0) {
      if (_phase == EnvPhase::att) {
        _phase = EnvPhase::dec;
        _phs_rem = _decay;
        _slope = (_out - _sustain)/_decay;
      }
      else if (_phase == EnvPhase::dec) {
        _phase = EnvPhase::sus;
        // We don't care about _phs_rem or slope here
      }
      else if (_phase == EnvPhase::rel) {
        _phase = EnvPhase::inactive;
      }
    }
  }

  void Envelope::gate(bool g)
  {
    if (g && _phase == EnvPhase::inactive) {
      _phase = EnvPhase::att;
      _phs_rem = _attack;
      _slope = 1./_attack;
    }
    else if (!g && _phase != EnvPhase::inactive) {
      _phase = EnvPhase::rel;
      _phs_rem = _release;
      _slope = _out/_release;
    }
  }

  void Envelope::setAttack(double attack) {
    _attack = attack;
  }

  void Envelope::setDecay(double decay) {
    _decay = decay;
  }

  void Envelope::setSustain(double sustain) {
    _sustain = sustain;
  }

  void Envelope::setRelease(double release) {
    _release = release;
  }

}
