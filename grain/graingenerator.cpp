/* (c) AudioElectric. All rights reserved.
 *
 * Author:             Ayal Lutwak <alutwak@audioelectric.com>
 * Date:               June 28, 2019
 * Last Modified By:   Ayal Lutwak <alutwak@audioelectric.com>
 * Last Modified Date: June 28, 2019
 */

#include "graingenerator.hpp"
#include "algorithm.hpp"

namespace audioelectric {

#define GRAIN_ALLOC_NUM 5

  template <typename T>
  void GrainParams<T>::modulate(GrainParams<T>& other)
  {
    density *= 1. + other.density;
    length  *= 1. + other.length;
    freq    *= 1. + other.freq;
    ampl    *= 1. + other.ampl;
    front   *= 1. + other.front;
    back    *= 1. + other.back;
  }

  template <typename T>
  GrainParams<T>& GrainParams<T>::operator*=(const GrainParams<T> &rhs)
  {
    density *= rhs.density;
    length *= rhs.length;
    freq *= rhs.freq;
    ampl *= rhs.ampl;
    front *= rhs.front;
    back *= rhs.back;
    return *this;
  }

  template <typename T>
  GrainParams<T>& GrainParams<T>::operator*=(T rhs)
  {
    density *= rhs;
    length *= rhs;
    freq *= rhs;
    ampl *= rhs;
    front *= rhs;
    back *= rhs;
    return *this;
  }

  template <typename T>
  GrainParams<T>& GrainParams<T>::operator+=(const GrainParams<T> &rhs)
  {
    density += rhs.density;
    length += rhs.length;
    freq += rhs.freq;
    ampl += rhs.ampl;
    front += rhs.front;
    back += rhs.back;
    return *this;
  }

  template <typename T>
  GrainGenerator<T>::GrainGenerator(Waveform<T>& shape, Waveform<T>& carrier) :
    _last_grain_t(0), _rand_grain_t(0), _params(), _rand({0,0,0,0,0,0}), _dist(-1,1),
    _shape(shape), _carrier(carrier)
  {
    std::random_device rd;
    _gen = std::ranlux48_base(rd());
    _allocateGrains();
  }

  template <typename T>
  GrainGenerator<T>::operator bool(void) const
  {
    return !_active.empty();
  }

  template <typename T>
  T GrainGenerator<T>::value(void) const
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
    incrementAndRemove(_active, _inactive);

    // Generate a grain if it is time
    double grain_period = 1./_params.density;
    if (_last_grain_t >= grain_period*(1. + _rand_grain_t*_rand.density)) {
      _rand_grain_t = _random();
      _last_grain_t = 0;
      if (_inactive.empty())
        _allocateGrains();
      _moveAndSetGrain(_params.freq*(1. + _random(_rand.freq)),         // crate
                       (1. + _random(_rand.length))/_params.length,     // srate
                       _params.ampl*(1. + _random(_rand.ampl)),         // ampl
                       _params.front*(1. + _random(_rand.front)),       // front
                       _params.back*(1. + _random(_rand.back)));        // back
    }

    _last_grain_t++;
  }

  template <typename T>
  void GrainGenerator<T>::applyInputs(GrainParams<T> params)
  {
    _params = params;
    if (_params.density <= MIN_DENSITY)
      _params.density = MIN_DENSITY;
    //_params.length = 1/_params.length; //Assumes that the grain length = 1 second
  }

  template <typename T>
  void GrainGenerator<T>::_allocateGrains(void)
  {
    _inactive.resize(GRAIN_ALLOC_NUM, Grain<T>(_carrier, 0, _shape, 0, 1));
  }

  template <typename T>
  void GrainGenerator<T>::_moveAndSetGrain(double crate, double srate, T ampl, double front, double back)
  {
    _inactive.front().setParams(crate, srate, ampl, front, back);
    _inactive.front().reset();
    _active.splice(_active.end(), _inactive, _inactive.begin());
  }


  template <typename T>
  T GrainGenerator<T>::_random(T mult)
  {
    if (mult == 0)
      return 0;
    return mult*_dist(_gen);
  }

  template class GrainParams<double>;
  template class GrainParams<float>;
  
  template class GrainGenerator<double>;
  template class GrainGenerator<float>;

}  // audioelectric

