#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;


string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}


string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}


vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}


float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  bool leave = false;
  float value;
  float total;
  float free;
  float used = 0.0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          total = value;
        } else if (key == "MemFree:") {
          free = value;
        } else if (key == "Cached:") {
          leave = true;
          break;
        }
      }
      if (leave) {
        used = (total - free) / total;
        break;
      }
    }
  }
  return used;
}


long LinuxParser::UpTime() {
  string line;
  long uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}


long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }


double LinuxParser::ActiveJiffies(int pid) { 
  string line;
  double jiffies = 0;
  vector<double> tokens;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if(!stream.is_open()) return jiffies;
  while(std::getline(stream, line, ' ')) {
    std::istringstream linestream(line);
    linestream >> jiffies;
    tokens.push_back(jiffies);
  }
  return tokens[13] + tokens[14] + tokens[15] + tokens[16]; 
}


long LinuxParser::ActiveJiffies() { 
  vector<string> utilization = CpuUtilization();
  long total = 0;
  unsigned int i;
  for (i = 0; i < utilization.size(); i++) {
  	switch(i) {
      case CPUStates::kUser_:
      case CPUStates::kNice_:
      case CPUStates::kSystem_:
      case CPUStates::kIRQ_:
      case CPUStates::kSoftIRQ_:
      case CPUStates::kSteal_: 
        {
          std::stringstream in(utilization[i]);
          long jiffie;
          in >> jiffie;
          total += jiffie;
          break;
        }
      default:
        break;
    }
  }
  return total;
}

long LinuxParser::IdleJiffies() {
  vector<string> utilization = CpuUtilization();
  long total = 0;
  unsigned int i;
  for (i = 0; i < utilization.size(); i++) {
  	switch(i) {
      case CPUStates::kIdle_:
      case CPUStates::kIOwait_:
        {
          std::stringstream in(utilization[i]);
          long jiffie;
          in >> jiffie;
          total += jiffie;
          break;
        }
      default:
        break;
    }
  }
  return total;
}

vector<string> LinuxParser::CpuUtilization() { 
  vector<string> tokens = {};
  string line;
  string token;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(!stream.is_open()) return tokens;
  std::getline(stream, line);
  std::istringstream linestream(line);
  while(std::getline(linestream, token, ' ')) {
    if(token == "cpu") continue;
    tokens.push_back(token);
  }
  
  return tokens; 
}

int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (!filestream.is_open()) return value;
  while (std::getline(filestream, line)) {
    std::istringstream linestream(line);
    while (linestream >> key >> value) {
      if (key != "processes") continue;
      return value;
    }
  }
  
  return value;
}

int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (!filestream.is_open()) return value;
  while (std::getline(filestream, line)) {
    std::istringstream linestream(line);
    while (linestream >> key >> value) {
      if (key != "procs_running") continue;
      return value;
    }
  }
  
  return value;
}

string LinuxParser::Command(int pid) { 
  string command = "";
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(!stream.is_open()) return command;
  std::getline(stream, command);
  return command;
}

string LinuxParser::Ram(int pid) {
  string line;
  long ram = 0;
  string key;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(!stream.is_open()) return to_string(ram);
  while(std::getline(stream, line)) {
    std::istringstream linestream(line);
    while(linestream >> key >> ram) {
      if(key != "VmSize:") continue;
      ram = ram / 1000;
      return to_string(ram);
    }
  }
  return to_string(ram); 
}

string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string userId = "";
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(!stream.is_open()) return userId;
   while(std::getline(stream, line)) {
     std::istringstream linestream(line);
     while(linestream >> key >> userId) {
       if(key != "Uid:") continue;
       return userId;
     }
   }
  
  return userId;
}

string LinuxParser::User(int pid) { 
  string line;
  string key;
  string userId = Uid(pid);
  string idCheck;
  string username = "";
  string pass;
  if(userId == "") return username;
  std::ifstream stream(kPasswordPath);
  if(!stream.is_open()) return username;
  while(std::getline(stream, line)) {
    std::replace(line.begin(), line.end(), ' ', '_');
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream linestream(line);
    while(linestream >> username >> pass >> idCheck) {
      if(idCheck != userId) continue;
      return username;
    }
  }
  
  return username; 
}

float LinuxParser::UpTime(int pid) { 
  string line;
  float uptime = 0;
  vector<float> tokens;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if(!stream.is_open()) return uptime;
  while(std::getline(stream, line, ' ')) {
    std::istringstream linestream(line);
    linestream >> uptime;
    tokens.push_back(uptime);
  }
  return tokens[21]; 
}