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
      auto& playing = _playing[i].notes;
      while (notes.front().tstart == _time) {
        auto note = notes.front();
        notes.pop_front();
        _instruments[i].startNote(note.freq, note.velocity);
        playing.push_back(note);
        if (_score[i].loop)
          notes.push_back(note);  // Need to adjust time here somehow (or probably we should use diff times)
      }
      // Now we need to release notes that are done and remove them from _playing
      auto is_done = [](Note n, size_t time) {return time >= n.tstop;};
      playing.remove_if(std::bind(is_done, std::placeholders::_1, _time));
    }
  }
  

}  // audioelectric
