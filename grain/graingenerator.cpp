/* (c) AudioElectric. All rights reserved.
 * 
 * Author:             Ayal Lutwak <alutwak@audioelectric.com>
 * Date:               June 28, 2019
 * Last Modified By:   Ayal Lutwak <alutwak@audioelectric.com>
 * Last Modified Date: June 28, 2019
 */

#include "graingenerator.hpp"

namespace audioelectric {

#define GRAIN_ALLOC_NUM 5

  template <typename T>
  GrainGenerator<T>::GrainGenerator(Shapes shape, Carriers carrier, double fs) :
    _last_grain_t(0), _rand_grain_t(0), _params({1e-9, 0, 0, 0}), _rand({0,0,0,0}),
    _fs(fs), _dist(-1,1)
  {
    std::random_device rd;
    _gen = std::mt19937(rd());
    setShape(shape);
    setCarrier(carrier);
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
    // Increment grains and move completed grains to the _inactive list
    auto itr = _active.begin();
    while (itr != _active.end()) {
      itr->increment();
      auto old_itr = itr;
      itr++;
      if (!*old_itr) {
        _inactive.splice(_inactive.end(), _active, old_itr);
      }
    }

    // Generate a grain if it is time
    double grain_period = _fs/_params.density;
    if (_last_grain_t >= grain_period*(1. + _rand_grain_t*_rand.density)) {
      _rand_grain_t = _random(); 
      _last_grain_t = 0;
      if (_inactive.empty())
        _allocateGrains();
      _moveAndSetGrain(_params.freq*(1. + _rand.freq*_random()),
                       (1. + _rand.length*_random())/_params.length,
                       _params.ampl*(1. + _rand.ampl*_random()));
    }

    _last_grain_t++;
  }

  template <typename T>
  void GrainGenerator<T>::applyInputs(GrainParams<T> params)
  {
    _params = params;
    if (_params.density == 0)
      _params.density = 1e-9;
  }

  template <typename T>
  void GrainGenerator<T>::setCarrier(Carriers wvfm)
  {
    switch(wvfm) {
    case Carriers::Sin:
      GenerateSin(_carrier, _fs);
      return;
    case Carriers::Triangle:
      GenerateTriangle(_carrier, _fs, T(0));
      return;
    case Carriers::Saw:
      GenerateTriangle(_carrier, _fs, T(0.8));
      return;
    case Carriers::Square:
      GenerateSquare(_carrier, _fs, T(0.5));
      return;
    }
  }

  template <typename T>
  void GrainGenerator<T>::setShape(Shapes shape)
  {
    switch(shape) {
    case Shapes::Gaussian:
      GenerateGaussian(_shape, _fs, T(0.15));
      return;
    }
  }

  template <typename T>
  void GrainGenerator<T>::_allocateGrains(void)
  {
    _inactive.resize(GRAIN_ALLOC_NUM, Grain<T>(_carrier, 0, _shape, 0, 1));
  }

  template <typename T>
  void GrainGenerator<T>::_moveAndSetGrain(double crate, double srate, T ampl)
  {
    _inactive.front().setParams(crate, srate, ampl);
    _inactive.front().reset();
    _active.splice(_active.end(), _inactive, _inactive.begin());
  }
  
  
  template <typename T>
  double GrainGenerator<T>::_random(void)
  {
    return _dist(_gen);
  }

  template class GrainGenerator<double>;
  template class GrainGenerator<float>;

}  // audioelectric
