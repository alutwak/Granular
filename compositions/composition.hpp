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

  class Composition;
  
  struct Note {
    Note (float fr, float vel, size_t start, size_t len) : freq(fr), velocity(vel), tstart(start), length_or_tstop(len) {}
    float freq;                 //!< The frequency of the note
    float velocity;             //!< The amplitude of the note
    size_t tstart;              //!< The starting time of the note (relative to the previous note)
    size_t length_or_tstop;     //!< The length of the note before it is played, the time that it will stop if it is playing
  };

  class Part {
  public:
    friend class Composition;

    Part(bool loop=false) : lastnote(0), loop(loop) {}

    Part& append(float freq, float vel, size_t start, size_t len) {
      notes.emplace_back(freq, vel, start, len);
      return *this;
    }
    
  private:
    std::list<Note> notes;
    size_t lastnote;            //!< How long it's been since the most recent note in this part was played
    bool loop;
  };


  class Composition {

  public:

    /*
     * \param sampwidth The number of bytes per sample of the file
     * \param framerate The frame rate of the file
     * \param chans     The number of channels to generate (only 1 is supported right now)
     */
    Composition(float fs, int sampwidth=3, int chans=1);

    /*!\brief Writes a composition to a .wav file
     * 
     * \param filename  The path of the file to write to 
     * 
     * \param time      The maximum length of time for the composition to last (in seconds). The generated file may be shorter
     *                  than this if the composition is shorter than this. A time of 0 generates the file until it's complete.
     */
    void write(std::string filename, double time, int bufsize=32768);

    /*!\brief Plays for the given amount of time (or continuously if time=0)
     * 
     * \param time The amount of time to play (in seconds)
     */
    void play(double time=0);

    /*!\brief Adds a part (with an instrument) to the score
     * 
     * \return The index of the score
     */
    int addPart(Part part, Cloud<float> inst);
    
  private:

    const float _fs;
    const int _sampwidth;
    const int _chans;
    size_t _time;
    std::vector<Cloud<float>> _instruments;
    std::vector<Part> _score;
    std::vector<std::list<Note>> _playing;

    /*!\brief Generates the next set of frames from the instruments
     */
    void generate(float *frames, int nframes);

    /*!\brief Starts or releases any notes that are queued up to be started or released
     */
    void updateNotes(void);
    
  };

}  // audioelectric
