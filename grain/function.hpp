
#pragma once

#include <cmath>
#include "waveform.hpp"
#include "phasor.hpp"

#define PI 3.14159265359

namespace audioelectric {

  template <typename T>
  class Function : public Waveform<T> {

  public:

    Function(void) {}

    virtual ~Function(void) {}

    virtual T waveform(double pos, int channel) = 0;

    virtual std::size_t size(void) const {return 0;}

    virtual double end(void) const {return 0;}

  };

  template <typename T>
  class Constant : public Function<T> {

  public:

    virtual ~Constant(void) {}

    virtual T waveform(double pos,int channel=0) {
      return 1;
    }

  };

  template <typename T>
  class Line : public Function<T> {

  public:

    virtual ~Line(void) {}

    virtual T waveform(double pos, int channel=0) {
      return pos;
    }
    
  };
  
  template <typename T>
  class Sinusoid : public Function<T> {

  public:

    virtual ~Sinusoid(void) {}

    virtual T waveform(double pos, int channel=0) {
      return sin(2*PI*pos);
    }
  };

  template <typename T>
  FunPhasor<T> makeConstant(T value);

  template <typename T>
  FunPhasor<T> makeLine(T slope, T start);

  template <typename T>
  FunPhasor<T> makeSinusoid(double freq, T ampl, T offset=0, double start=0);

  template <typename T>
  FunModPhasor<T> makeSinusoid(Phasor<T> fmod, Phasor<T> amod, T offset, double start);
  
}
