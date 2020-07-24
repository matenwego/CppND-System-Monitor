#include "processor.h"


float Processor::Utilization() { return utilization_; }

void Processor::Utilization(long total, long idle) {
  auto deltatotal = total - total_;
  auto deltaidle = idle - idle_;
  Total(total);
  Idle(idle);
  utilization_ = (deltatotal - (float)deltaidle) / deltatotal;
}

long Processor::Total() { return total_; }
void Processor::Total(long total) {
  total_ = total;
}

long Processor::Idle() { return idle_; }
void Processor::Idle(long idle) {
  idle_ = idle;
}