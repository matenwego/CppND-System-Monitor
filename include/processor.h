#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <vector>
#include <string>

using std::vector;
using std::string;

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp
  void Utilization(long total, long idle);
  long Total();
  void Total(long total);
  long Idle();
  void Idle(long idle);
  Processor(long total, long idle) : total_{total}, idle_{idle}{}

  // TODO: Declare any necessary private members
 private:
  float utilization_;
  long total_;
  long idle_;
};

#endif