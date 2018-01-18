
#pragma once

#include <utility>

#include "waveform.hpp"

namespace audioelectric {

  template<typename T>
  class Grain : public Waveform<T> {
  public:

    class granulator {
    public:
      granulator(const typename Waveform<T>::interpolator& interp_this,
                 const typename Waveform<T>::interpolator& interp_other);
      granulator(const granulator& other);
      granulator& operator=(const granulator& other);
      granulator& operator++(void);
      granulator operator++(int);
      T operator*(void) const;
      operator bool(void) const;
    private:
      typename Waveform<T>::interpolator _interp_this;
      typename Waveform<T>::interpolator _interp_other;
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
    Grain(const Waveform<T>& other, std::size_t len, InterpType it=InterpType::LINEAR);

    //~Grain(void);

    granulator gmake(long start, double speed, const typename Waveform<T>::interpolator& interp_other) const;

    granulator rgmake(long start, double speed, const typename Waveform<T>::interpolator& interp_other) const;
    
  };
  
}
