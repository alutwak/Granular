
#pragma once

#include <utility>
#include <list>

#include "wavetable.hpp"

namespace audioelectric {

  /*!\brief A grain is basically a wavetable that can generate a grains of sound
   * 
   * The only thing special about this class is the granulator class that it contains and the gmake and rgmake
   * functions. A Grain works by using its waveform as an envelope to modulate the amplitude of another Waveform.
   * Typically, Grains last a short period of time (on the order of about 10 to 100ms), though there's no reason that
   * they couldn't be longer or shorter. 
   * 
   * By itself, a single grain is generatlly not that interesting, but when many grains are played together, in either a 
   * random or or structured arrangement, some very interesting things can happen. The best way to do that is with a Cloud
   * class.
   */
  template<typename T>
  class Grain : public Wavetable<T> {
  public:

    /*!\brief An interpolator that also holds a phasor and 
     */
    class granulator : public Wavetable<T>::interpolator {
      
    public:
      virtual ~granulator(void) {}
      
    protected:
      friend class Grain;
      granulator(const Grain<T>& grn, double start, double rate,
                 const typename Waveform<T>::phasor& phasor_other);
      granulator(const granulator& other);
      //granulator& operator=(const granulator& other);

      /*!\brief The product of the current interpolated values of the Grain and the phasor
       */
      T value(void) const;
      operator bool(void) const;
      typename Waveform<T>::phasor_impl* copy(void) const;
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
