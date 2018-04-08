
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

  dbl_phs make_constant(double value)
  {
    if (!_const) {
      _const = std::make_unique<Constant>();
    }
    return _const->pbegin(1,value);
  };
  
  /******************** Sinusoid ****************************/

  class Sinusoid : public Function {

  public:

    virtual ~Sinusoid(void) {}

    virtual double waveform(double pos) const {
      return sin(2*PI*pos);
    }
  };

  static std::unique_ptr<Sinusoid> _sin;

  dbl_phs make_sinusoid(double freq, double ampl, double offset, double start)
  {
    if (!_sin) {
      _sin = std::make_unique<Sinusoid>();
    }
    return _sin->pbegin(freq,ampl,offset,start);
  }
  
}
