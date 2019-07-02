/* (c) AudioElectric. All rights reserved.
 * 
 * Author:             Ayal Lutwak <alutwak@audioelectric.com>
 * Date:               June 28, 2019
 * Last Modified By:   Ayal Lutwak <alutwak@audioelectric.com>
 * Last Modified Date: June 28, 2019
 */

#include "graingenerator.hpp"

namespace audioelectric {

#define DEFAULT_CARRIER Carriers::Sin
#define DEFAULT_SHAPE Shapes::Gaussian
#define GRAIN_ALLOC_NUM 5

  template <typename T>
  GrainGenerator<T>::GrainGenerator(double fs) :
    _last_grain_t(0), _density_rnd(0), _length_rnd(0), _freq_rnd(0), _ampl_rnd(0), _fs(fs), _rand(-1,1)
  {
    std::random_device rd;
    _gen(rd());
    setCarrier(DEFAULT_CARRIER);
    setShape(DEFAULT_SHAPE);
    _allocateGrains();
  }

  template <typename T>
  T GrainGenerator<T>::value(void)
  {
    T val = 0;
    for (auto& grn : _active)
      val += grn.value();
    return val;
  }

  template <typename T>
  void GrainGenerator<T>::increment(void)
  {
    for (auto& grn : _active)
      grn.increment();
    _last_grain_t++;
  }

  template <typename T>
  void GrainGenerator<T>::updateGrains(double density, double length, double freq, T ampl)
  {
    // Generate a grain if it is time
    if (_last_grain_t >= density*(1. + _random()*_density_rnd)) {
      if (_inactive.empty())
        _allocateGrains();
      _moveAndSetGrain(density*(1. + _density_rnd*_random()),
                       length*(1. + _length_rnd*_random()),
                       freq*(1. + _freq_rnd*_random()),
                       ampl*(1. + _ampl_rnd*random()));
    }

    // Remove all completed grains to the _inactive list
    auto itr = _active.begin();
    while (itr != _active.end()) {
      auto old_itr = itr;
      itr++;
      if (!*old_itr) {
        _inactive.splice(_inactive.end(), _active, old_itr);
      }
    }
  }

  template <typename T>
  void GrainGenerator<T>::setCarrier(Carriers wvfm)
  {
    switch(wvfm) {
    case Carriers::Sin:
      GenerateSin(_carrier, _fs);
      return;
    case Carriers::Triangle:
      GenerateTriangle(_carrier, _fs, 0);
      return;
    case Carriers::Saw:
      GenerateTriangle(_carrier, _fs, 0.8);
      return;
    case Carriers::Square:
      GenerateSquare(_carrier, _fs, 0.5);
      return;
    }
  }

  template <typename T>
  void GrainGenerator<T>::setShape(Shapes shape)
  {
    switch(shape) {
    case Shapes::Gaussian:
      GenerateGaussian(_shape, _fs, 0.15);
      return;
    }
  }

  template <typename T>
  void GrainGenerator<T>::_allocateGrains(void)
  {
    size_t old_size = _inactive.size();
    _inactive.resize(old_size + GRAIN_ALLOC_NUM);
    Grain<T> *new_grains = _inactive.data() + old_size;
    new_grains = new Grain<T>[GRAIN_ALLOC_NUM];
    for (size_t i=old_size; i<_inactive.size(); i++) {
      new_grains[i].setCarrier(_carrier);
      new_grains[i].setShape(_shape);
    }
  }

  template <typename T>
  void GrainGenerator<T>::_moveAndSetGrain(double density, double length, double freq, T ampl)
  {
    _inactive.front().setParameters(density, freq, 1./length, ampl);
    _active.splice(_active.end(), _inactive, _inactive.begin());
  }
  
  
  template <typename T>
  double GrainGenerator<T>::_random(void)
  {
    return _rand(_gen);
  }
  

}  // audioelectric
