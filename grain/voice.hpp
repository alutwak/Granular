
#pragma once

#include "slot.hpp"

namespace audioelectric {

  class Voice {

  public:

    Voice(void) {}

    void connectFreqSlot(Slot slot) {_freq_sig.insert(slot);}
    void connectAmplSlot(Slot slot) {_ampl_sig.insert(slot);}    
    void disconnectFreqSlot(Slot slot) {_freq_sig.erase(slot);}
    void disconnectAmplSlot(Slot slot) {_ampl_sig.erase(slot);}    
    
    void trigger(double freq, double ampl) {
      for (auto& slot : _freq_sig)
        slot(freq);
      for (auto& slot : _ampl_sig)
        slot(ampl);
    }

    void release(void) {
      for (auto& slot : _ampl_sig)
        slot(0);
    }

  private:
    Signal _freq_sig;
    Signal _ampl_sig;
  };
  
}
