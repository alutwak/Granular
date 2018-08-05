
#pragma once

#include <cstdlib>

namespace audioelectric {

  /*!\brief Provides an iterface for a variety of waveforms. These could be waveforms that get generated on the fly
   * or wavetables, samples, envelopes, etc...
   *
   * \note Waveform has been purposely left as a single-channel base class in order to keep the overhead of multiple channels
   * out of sublasses that only need one channel (like Grain, envelope or probably a handful of other potential waveforms
   * that are used for their waveform properties rather than their pure audio properties). It should be pretty easy for
   * subclasses to add channels though. You'll just need to make separate phasors for each channel. The more interesting
   * challenge will be fixing operator[] so that you can use syntax like sample[chan][pos].  That's not strictly necessary,
   * but it would be a nice touch and make indexing multi-channel waveforms much easier.
   */
  template<typename T>
  class Waveform {
  public:

    virtual ~Waveform(void) = 0;
    
    /*!\brief Returns the waveform at a certain phase
     * 
     * \param pos The position of the waveform
     * \return The waveform at the given phase
     */
    virtual T waveform(double pos, int channel=0) = 0;

    /*!\brief Returns the number of samples of the waveform
     */
    virtual std::size_t size(void) const = 0;

    /*!\brief Returns the end of the waveform
     */
    virtual double end(void) const = 0;

  };

}
