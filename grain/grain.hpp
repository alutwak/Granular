
#pragma once

#include <utility>
#include <list>

#include "wavetable.hpp"

namespace audioelectric {

  template<typename T>
  class Grain : public Wavetable<T> {
  public:

    class granulator : public Wavetable<T>::interpolator {
    public:
      granulator(const Grain<T>& grn, double start, double rate,
                 const typename Waveform<T>::phasor& phasor_other);
      granulator(const granulator& other);
      //granulator& operator=(const granulator& other);
      T value(void) const;
      operator bool(void) const;
      typename Waveform<T>::phasor_impl* copy(void);
      void increment(void);
      
    private:
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
    /*!\brief Generates a forward grain
     * 
     * \param start Starting point of the grain (in the original waveform, not the interpolated one)
     * \param rate The rate at which to advance through the grain (>0, <1 is slower, >1 is faster)
     * \param phasor_other A phasor to modulate with the grainulator
     */
    typename Waveform<T>::phasor gmake(double start, double rate, const typename Waveform<T>::phasor& phasor_other) const;

    typename Waveform<T>::phasor rgmake(double start, double rate, const typename Waveform<T>::phasor& phasor_other) const;
    
  };

}
