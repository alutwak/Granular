/* \file composition.hpp
 * \brief Contains the base class for Granular compositions
 *
 * (c) AudioElectric. All rights reserved.
 * 
 * Author:             Ayal Lutwak <alutwak@audioelectric.com>
 * Date:               September 22, 2019
 * Last Modified By:   Ayal Lutwak <alutwak@audioelectric.com>
 * Last Modified Date: September 22, 2019
 */

#include <cloud.hpp>
#include <vector>

namespace audioelectric {

  struct Note {
    Note (float fr, float vel, float start, float stop) : freq(fr), velocity(vel), tstart(start), tstop(stop) {}
    float freq;
    float velocity;
    size_t tstart;
    size_t tstop;
  };

  struct Part {
    std::list<Note> notes;
    bool loop;
  };

  class Composition {

  public:

    Composition(float fs);

    /*!\brief Writes a composition to a .wav file
     * 
     * \param filename  The path of the file to write to 
     * 
     * \param time      The maximum length of time for the composition to last (in seconds). The generated file may be shorter
     *                  than this if the composition is shorter than this. A time of 0 generates the file until it's complete.
     * \param sampwidth The number of bytes per sample of the file
     * \param framerate The frame rate of the file
     * \param chans     The number of channels to generate (only 1 is supported right now)
     */
    void write(std::string filename, double time, int sampwidth=3, int framerate=48000, int chans=1);

    void play(int sampwidth=24, int framerate=48000, int chans=1);
    
  private:

    const float _fs;
    size_t _time;
    std::vector<Cloud<float>> _instruments;
    std::vector<Part> _score;
    std::vector<Part> _playing;

    /*!\brief Generates the next set of frames from the instruments
     */
    void generate(float *frames, int nframes);

    /*!\brief Starts or releases any notes that are queued up to be started or released
     */
    void updateNotes(void);
    
  };

}  // audioelectric