
#pragma once

#include "waveform.hpp"

namespace audioelectric {

  dphasor make_constant(double value);

  dphasor make_line(double slope, double start);
  
  dphasor make_sinusoid(double freq, double ampl=1, double offset=0, double start=0);

  dphasor make_sinusoid(dphasor fmod, dphasor amod, double offset=0, double start=0);
  
  class Function : public Waveform<double> {

  public:

    using ph_im = typename Waveform<double>::phasor_impl;

    class function_phs : public ph_im {
    public:

      virtual ~function_phs(void) {}

      function_phs(const Function& fun, double freq, double ampl, double offset, double start) :
        ph_im(fun, freq, start), _ampl(ampl), _offset(offset)
        {
          
        }

      function_phs(const function_phs& other) : ph_im(other), _ampl(other._ampl), _offset(other._offset) {}

    protected:

      double _ampl;
      double _offset;

      virtual function_phs* copy(void) const {return new function_phs(*this);}

      virtual double value(void) const {
        return _ampl*ph_im::value() + _offset;
      }

    };

    class function_mod : public function_phs {
    public:

      virtual ~function_mod(void) {}

      function_mod(const Function& fun, phasor fmod, phasor amod, double offset, double start) :
        function_phs(fun, *fmod, *amod, offset, start), _fmod(fmod), _amod(amod) {}
      
      function_mod(const function_mod& other) : function_phs(other), _fmod(other._fmod), _amod(other._amod) {}

    protected:

      virtual function_mod* copy(void) const {return new function_mod(*this);}

      virtual void increment(void) {
        ph_im::increment();
        ph_im::setRate(*(++_fmod));
        function_phs::_ampl = *(++_amod);
      }

    private:
      phasor _fmod;
      phasor _amod;
  };

    Function(void) {}

    virtual ~Function(void) {}

    virtual double waveform(double pos) const = 0;

    virtual std::size_t size(void) const {return 0;}

    virtual double end(void) const {return 0;}

    virtual dphasor pbegin(double freq, double ampl=1, double offset=0, double start=0) const {
      return Waveform<double>::make_phasor(new function_phs(*this, freq, ampl, offset, start));
    }

    virtual dphasor rpbegin(double freq, double ampl=1, double offset=0, double start=0) const {
      return Waveform<double>::make_phasor(new function_phs(*this, -freq, ampl, offset, start));
    }

    virtual dphasor pbegin(phasor fmod, phasor amod, double offset=0, double start=0) const {
      return Waveform<double>::make_phasor(new function_mod(*this, fmod, amod, offset, start));
    }

  };

}
