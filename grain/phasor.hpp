
#pragma once

#include "waveform.hpp"

namespace audioelectric {

  /*!\brief An iterator-like class that increments the phase of the waveform at a certain rate
   * 
   * The Phasor provides access to interpolated, modulated, or otherwise mathematically complex Waveforms with a simple,
   * familiar interface. It is not meant to be inherited because all subclasses of Waveform deal directly in straight
   * phasors. Instead, the phasor_impl class should be inherited and passed in in the constructor.
   * 
   * Phasors are initially created only through the Waveform::make_phasor() function. 
   * 
   * \note Phases are in units of **samples** and rates are in units of **samples/iteration**. It may seem strange when I
   * talk about units of samples that are represented in floating point values. But because the Phasor is meant to be used
   * for synthesis, rather than simple audio playback and processing, samples become somewhat fuzzy untis. Even when we're
   * using wavetables and sound samples, we're going to want to interpolate between the original samples for variable pitch
   * sampling. In the end, it's just easier to express our waveforms as continuous functions and let the back end deal with
   * the digital realities.
   */
  template<typename T>
  class Phasor final {
  public:

    ~Phasor(void) {}

    /*!\brief Constructs a Phasor that starts at a particular phase in the waveform. 
     * 
     * See the discussion in the Phasor class documentation on how the start phase relates to the phase
     * and rate of the Phasor.
     * 
     * \param wf    The Waveform to iterate over.
     * \param rate  The rate at which to iterate over the Waveform. Positive rate -> forward iteration, negative rate -> 
     *              reverse iteration.
     * \param start The starting phase in the Waveform. If this is not between the begin and end phases then it will be
     *              set to begin.
     * \param begin The beginning phase in the Wavetable. Values before this in the wavetable will not be played
     * \param end   The ending phase in the Wavetable. A negative value sets the ending at the last sample for forward 
     *              interpolators and at the first sample for reverse interpolators.
     * \param cycle Whether or not to cycle over the waveform.
     */
    Phasor(Waveform<T>& wf, double rate, double start=0, double begin=0, double end=-1, bool cycle=false);

    Phasor(const Phasor& other);

    T value(void) const;                      //!<\brief Returns the value of the Waveform at the current phase

    bool generate(T **outputs, int frames, int channels=1);

    /*!\brief Returns true if the phasor is still running
     * 
     * A phasor will still be running if its phase is between the begin phase and the end phase. A cycling phasor
     * will always return true.
     */
    operator bool(void) const;                

    Phasor& operator=(const Phasor& other);

    /*!\brief Compare operators compare the location in the uninterpolated waveform (essentially phase*rate)
     */
    bool operator==(const Phasor& other) const;
    bool operator!=(const Phasor& other) const;
    bool operator<(const Phasor& other) const;
    bool operator>(const Phasor& other) const;
    bool operator<=(const Phasor& other) const;
    bool operator>=(const Phasor& other) const;

    /*!\brief Sets the rate (useful for vari-rate iterations)
     */
    void setRate(double rate);

    /*!\brief Returns the current phase of the Phasor
     */
    double getPhase(void) const {return _phase;}

    /*!\brief Increments the phase
     */
    void increment(void);

    void reset(void) {_phase = _begin;}    

  private:

    double _rate;       //!< The rate that the phase changes per iteration
    double _phase;      //!< The current phase
    double _begin;      //!< The start of the wavetable in iterations (the units of the phase)
    double _end;        //!< The end of the wavetable in iterations (the units of the phase)
    bool _cycle;        //!< Whether to cycle the Waveform
    Waveform<T>& _wf;   //!< The waveform that we're phasing

    /*!\brief Sets the value of the _end member.
     */
    void _setEnd(long end);
    
    /*!\brief Checks whether the given phase is within the start and stop bounds
     */
    bool _checkPhase(double phase) const;
    
  };

}
