
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
     * \param cycle Whether or not to cycle over the waveform.
     * \param start The starting phase in the Waveform. If this is not between the front and back phases then it will be
     *              set to front.
     * \param front The front phase in the Wavetable. Values before this in the wavetable will not be played.
     * \param back  The back phase in the Wavetable. A negative value sets the back at the last sample of the waveform.
     */
    Phasor(const Waveform<T>& wf, double rate, bool cycle=false, double start=0, double front=0, double back=-1);

    Phasor(const Phasor& other);

    /*!\brief Returns the value of the Waveform at the current phase
     */
    T value(void) const;

    bool generate(T **outputs, int frames, int channels=1);

    /*!\brief Returns true if the phasor is still running
     * 
     * A phasor will still be running if its phase is between the front phase and the back phase. A cycling phasor
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

    void setWaveform(const Waveform<T>& wf) {_wf = wf;}

    /*!\brief Sets all of the paramters of the waveorm
     *
     * \param rate  The rate at which to iterate over the Waveform. Positive rate -> forward iteration, negative rate -> 
     *              reverse iteration.
     * \param phase The current phase in the Waveform. If this is not between the front and back phases then it will be
     *              set to front.
     * \param front The front phase in the Wavetable. Values before this in the wavetable will not be played.
     * \param back  The back phase in the Wavetable. A negative value sets the back at the last sample of the waveform.
     */
    void setParameters(double rate, double phase, double front, double back);

    /*!\brief Sets the rate (useful for vari-rate iterations)
     */
    void setRate(double rate);

    /*!\brief Sets the current phase
     */
    void setPhase(double phase);

    /*!\brief Sets the front phase of the wavetable
     */
    void setFront(double front);

    /*!\brief Sets the back phase of the wavetable
     * 
     * If the value given is either negative or greater than the length of the waveform, the back phase will be set to the
     * end of the waveform. It should be noted, however, that the back phase is not checked when the waveform is changed
     * with the setWaveform() function. In that case, the value of _back could be greater than the new waveform's end.
     */
    void setBack(double back);

    void setCycle(bool cycle);

    /*!\brief Returns the current phase of the Phasor
     */
    double getPhase(void) const {return _phase;}

    /*!\brief Increments the phase
     */
    void increment(void);

    /*!\brief Resets the phase back to the front 
     */
    void reset(void);

  private:

    double _rate;       //!< The rate that the phase changes per iteration
    double _phase;      //!< The current phase
    double _front;      //!< The start of the wavetable in iterations (the units of the phase)
    double _back;        //!< The back of the wavetable in iterations (the units of the phase)
    bool _cycle;        //!< Whether to cycle the Waveform
    Waveform<T>& _wf;   //!< The waveform that we're phasing

    bool _phase_good;   //!< Whether the phase is between front and back

    /*!\brief Checks whether the given phase is within the start and stop bounds
     */
    inline bool _checkPhase(double phase) const;
    
  };

}
