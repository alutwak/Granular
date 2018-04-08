
#include <cmath>
#include <utility>

#include "function.hpp"

#define PI 3.14159265359

namespace audioelectric {

  /******************** Constant ****************************/
  
  class Constant : public Function {

  public:

    virtual ~Constant(void) {}

    virtual double waveform(double pos) const {
      return 1;
    }

  };

  static std::unique_ptr<Constant> _const;

  dphasor make_constant(double value)
  {
    if (!_const) {
      _const = std::make_unique<Constant>();
    }
    return _const->pbegin(1,value);
  };

  /********************** Line **********************/

  class Line : public Function {

  public:

    virtual ~Line(void) {}

    virtual double waveform(double pos) const {
      return pos;
    }
    
  };

  static std::unique_ptr<Line> _line;

  dphasor make_line(double slope, double start)
  {
    if(!_line) {
      _line = std::make_unique<Line>();
    }
    return _line->pbegin(slope, 1, start); //rate=slope, ampl=1, offset=start, start=0
  }
  
  /******************** Sinusoid ****************************/

  class Sinusoid : public Function {

  public:

    virtual ~Sinusoid(void) {}

    virtual double waveform(double pos) const {
      return sin(2*PI*pos);
    }
  };

  static std::unique_ptr<Sinusoid> _sin;

  dphasor make_sinusoid(double freq, double ampl, double offset, double start)
  {
    if (!_sin) {
      _sin = std::make_unique<Sinusoid>();
    }
    return _sin->pbegin(freq,ampl,offset,start);
  }

  dphasor make_sinusoid(dphasor fmod, dphasor amod, double offset, double start)
  {
    if (!_sin) {
      _sin = std::make_unique<Sinusoid>();
    }
    return _sin->pbegin(fmod, amod, offset, start);
  }
  
}
