
#pragma once

#include <vector>

#include "grain.hpp"

namespace audioelectric {

  /*!\brief Generates a cloud of grains and distributes them across its channels
   */
  template<typename T>
  class Cloud : public Waveform<T> {

    using MultiT = std::vector<T>;
    
    class vaporizer {
    public:
      virtual ~vaporizer(void) {}

    protected:
      friend Cloud;

      /*!\brief Constructs a vaporizer
       * 
       * \param cloud        The cloud that owns the vaporizer object
       * \param phasor_other A phasor for a waveform to vaporize
       */
      vaporizer(const Cloud& cloud, const typename Waveform<T>::phasor& phasor_other);
      vaporizer(const vaporizer& other);

      virtual MultiT value(void) const;

    private:
      typename Waveform<T>::phasor _phasor_other;
      
    };

    virtual MultiT waveform(double pos) const;

    typename Waveform<MultiT>::phasor vaporize(const typename Waveform<T>::phasor& phasor_other);
    
  };

  
}
