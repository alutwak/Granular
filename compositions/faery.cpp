
#include "composition.hpp"
#include <cloud.hpp>

using namespace audioelectric;

int main(int argc, char **argv)
{

  double fs = 48000;
  Composition comp(fs);

  Part part1(true);
  part1.append(300, 0.5, 1.2*fs, 0.1*fs)
    .append(300*1.5, 0.25, 0, 0.09*fs)
    .append(300*4./3, 0.1, 0.05*fs, 1*fs);

  Cloud<float> inst1 (fs, 8, Shape::Gaussian, Carrier::Saw);
  inst1.params().density = 100;
  inst1.params().length = 0.1;

  comp.addPart(part1, inst1);

  comp.write("faery.wav", 30);
}
