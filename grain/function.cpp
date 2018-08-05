
#include <type_traits>

#include "function.hpp"

namespace audioelectric {

  /******************** Constant ****************************/

  static Constant<float> _fconst;
  static Constant<double> _dconst;

  template <>
  FunPhasor<float> makeConstant(float value)
  {
    return FunPhasor<float>(_fconst,1,value);
  };

  template <>
  FunPhasor<double> makeConstant(double value)
  {
    return FunPhasor<double>(_dconst,1,value);
  };

  // template FunPhasor<double> makeConstant<double>(double value);
  // template FunPhasor<float> makeConstant<float>(float value);

  /********************** Line **********************/

  static Line<float> _fline;
  static Line<double> _dline;
  
  template <>
  FunPhasor<float> makeLine(float slope, float start)
  {
    return FunPhasor<float>(_fline,slope,start);
  }
  
  template <>
  FunPhasor<double> makeLine(double slope, double start)
  {
    return FunPhasor<double>(_dline,slope,start);
  }
  // /******************** Sinusoid ****************************/

  static Sinusoid<float> _fsin;
  static Sinusoid<double> _dsin;

  template <>
  FunPhasor<float> makeSinusoid(double freq, float ampl, float offset, double start)
  {
    return FunPhasor<float>(_fsin,freq,ampl,offset,start);
  }

  template <>
  FunPhasor<double> makeSinusoid(double freq, double ampl, double offset, double start)
  {
    return FunPhasor<double>(_dsin,freq,ampl,offset,start);
  }

  template <>
  FunModPhasor<float> makeSinusoid(Phasor<float> fmod, Phasor<float> amod, float offset, double start)
  {
    return FunModPhasor<float>(_fsin,fmod,amod,offset,start);
  }

  template <>
  FunModPhasor<double> makeSinusoid(Phasor<double> fmod, Phasor<double> amod, double offset, double start)
  {
    return FunModPhasor<double>(_dsin,fmod,amod,offset,start);
  }
  
  
}
