#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;


Processor& System::Cpu() {
  cpu_.Utilization(LinuxParser::Jiffies(), LinuxParser::IdleJiffies());
  return cpu_; 
}


vector<Process>& System::Processes() { 
  auto hertz = sysconf(_SC_CLK_TCK);
  long upTime = UpTime();
  processes_.clear();
  for(const auto pid : LinuxParser::Pids()) {
    long startTime = LinuxParser::UpTime(pid);
    double seconds = upTime - (startTime / hertz);
    double totalTime = LinuxParser::ActiveJiffies(pid) / hertz;
    double utilization = totalTime / seconds;
    Process p = {pid, LinuxParser::User(pid), LinuxParser::Command(pid), utilization, LinuxParser::Ram(pid), (startTime/hertz)};
    processes_.push_back(p);
  }
  sort(processes_.begin(), processes_.end());
  return processes_; 
}


std::string System::Kernel() { return LinuxParser::Kernel(); }


float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }


std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }


int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }


int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }


long int System::UpTime() { return LinuxParser::UpTime(); }