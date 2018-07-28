
#include <cmath>

#include "envelope.hpp"

namespace audioelectric {

  #define ZADJ 1.001

  //This seems a little weird. Maybe the envelope's phasor should deal with this math
  void Envelope::enveloper::increment(void)
  {
    
    _phase+=phasor_impl::_rate;
    switch(_envphase) {
      //Nothing to do here
      return;
    case EnvPhase::delay:
      if (_phase>=_env._delay) {
        _envphase=EnvPhase::attack;
        _phase = 0;
      }
      return;
    case EnvPhase::attack:
      if (_phase>=_env._attack) {
        if (_env._hold > 0)
          _envphase=EnvPhase::hold;
        else
          _envphase=EnvPhase::decay;
        _phase = 0;
      }
      return;
    case EnvPhase::hold:
      if (_phase>=_env._hold) {
        _envphase=EnvPhase::decay;
        _phase=0;
      }
      return;
    case EnvPhase::decay:
    case EnvPhase::release:
    case EnvPhase::untrig:
      return;
      //Untrig, decay and release states don't end from incrementing
    }
  }

  void Envelope::enveloper::trigger(bool trig)
  {
    if (trig) {
      if (_env._delay > 0)
        _envphase=EnvPhase::delay;
      else
        _envphase=EnvPhase::attack;
    }
    else {
      _env._rel_amp = value();
      _envphase=EnvPhase::release;
    }
    _phase=0;
  }

  void Envelope::setDelay(double delay) {
    _delay = delay;
  }
  
  void Envelope::setAttack(double attack) {
    _attack = attack;
    //_attack = attack*(-log(1.-1./ZADJ)); //This scales so that waveform(1) -> 1
  }

  void Envelope::setHold(double hold) {
    _hold = hold;
  }

  /*
   * (1-sus/zadj)*exp(x)+sus/zadj
   * exp(x)-sus/zadj*exp(x)+sus/zadj
   * exp(x)+sus/zadj*(1-exp(x))
   *   sus                     = (1-sus/zadj)*exp(-x/t) + sus/zadj
   * = sus*(1-1/zadj)          = (1-sus)*exp(-x/t)
   * = sus*(1-1/zadj)/(1-sus)     = exp(-x/t)
   * = ln(sus*(1-1/zadj)/(1-sus)) = -x/t
   * = -t*ln(sus*(1-1/zadj)/(1-sus)) = x
   */
  void Envelope::setDecay(double decay) {
    _decay = decay;
    //_decay = decay*(-log(_sustain*(1.-1/ZADJ)/(1-_sustain)));
  }

  void Envelope::setSustain(double sustain) {
    _sustain = sustain;
    // //First extract the true decay
    // double dec = _decay/(-log(_sustain*(1.-1/ZADJ)/(1-_sustain)));
    // _sustain = sustain;
    // //Reset the decay with the new sustain
    // setDecay(dec);
  }

  void Envelope::setRelease(double release) {
    _release = release;
  }

  double Envelope::delay(double pos) {
    return 0;
  }
  
  double Envelope::attack(double pos) {
    return 1-exp(-pos/_attack);
  }

  double Envelope::hold(double pos) {
    return 1;
  }

  /* (1-sustain)exp(-pos/decay) + sustain
   */
  double Envelope::decay(double pos) {
    return (1-_sustain)*exp(-pos/_decay) + _sustain;
  }
  
  double Envelope::release(double pos) {
    return _rel_amp*exp(-pos/_release);
  }

}
