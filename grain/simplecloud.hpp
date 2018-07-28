
#include <list>

#include "voice.hpp"
#include "cloud.hpp"
#include "envelope.hpp"

namespace audioelectric {
  
  class SimpleCloud {

  public:

    void playNote(double freq, double ampl);
    
  private:

    Cloud<double> _cloud;
    Envelope _envlp;
    std::list<Voice> _voices;        //!< Note->voice map
    
  };

}
