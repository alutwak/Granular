/* (c) AudioElectric. All rights reserved.
 * 
 * Author:             Ayal Lutwak <alutwak@audioelectric.com>
 * Date:               September 22, 2019
 * Last Modified By:   Ayal Lutwak <alutwak@audioelectric.com>
 * Last Modified Date: September 22, 2019
 */

#include <sndfile.h>
#include <portaudio.h>

#include "composition.hpp"

namespace audioelectric {

  void Composition::generate(float *frames, int nframes)
  {
    float *f = frames;
    float *end = frames + nframes;
    for (; f<end; f++) {
      updateNotes();
      *f = 0;
      for (auto& inst : _instruments) {
        *f += inst.value();
        inst.increment();
      }
      _time++;
    }
  }

  void Composition::updateNotes(void)
  {
    for (int i=0; i<_instruments.size(); i++) {
      auto& notes = _score[i].notes;
      auto& playing = _playing[i];
      auto& lastnote = _score[i].lastnote;
      while (notes.front().tstart >= lastnote) {
        lastnote = 0;
        auto note = notes.front();
        note.length_or_tstop += _time;  // Adding the current time to the length give the stop time
        notes.pop_front();
        _instruments[i].startNote(note.freq, note.velocity);
        playing.push_back(note);
        if (_score[i].loop)
          notes.push_back(note);
      }
      // Now we need to release notes that are done and remove them from _playing
      auto is_done = [](Note n, size_t time) {return time >= n.length_or_tstop;};
      playing.remove_if(std::bind(is_done, std::placeholders::_1, _time));
      lastnote++;
    }
  }
  

}  // audioelectric
