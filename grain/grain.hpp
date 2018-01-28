
#pragma once

#include <utility>

#include "wavetable.hpp"

namespace audioelectric {

  template<typename T>
  class Grain : public Wavetable<T> {
  public:

    class granulator {
    public:
      granulator(const typename Waveform<T>::phasor& interp_this,
                 const typename Waveform<T>::phasor& phasor_other);
      granulator(const granulator& other);
      granulator& operator=(const granulator& other);
      granulator& operator++(void);
      granulator operator++(int);
      T operator*(void) const;
      operator bool(void) const;
    private:
      typename Wavetable<T>::phasor _phasor_this;
      typename Waveform<T>::phasor _phasor_other;
    };

    /*!\brief Creates a waveform of size 0
     */
    Grain(void);

    /*!\brief Creates and allocates a waveform of size len with all values set to 0
     */
    Grain(std::size_t len, InterpType it=InterpType::LINEAR);

    /*!\brief Wraps an array of size len in a Grain to allow it to be interpolated
     */
    Grain(T* data, std::size_t len, InterpType it=InterpType::LINEAR);

    Grain(std::initializer_list<T> init, InterpType it=InterpType::LINEAR);
    
    /*!\brief Copies a sample to a new length using interpolation
     */
    Grain(const Waveform<T>& other, double rate, std::size_t len, InterpType it=InterpType::LINEAR);

    //~Grain(void);

    granulator gmake(long start, double speed, const typename Waveform<T>::phasor& phasor_other) const;

    granulator rgmake(long start, double speed, const typename Waveform<T>::phasor& phasor_other) const;
    
  };
  
}
