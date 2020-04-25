#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float memUtilized;
  float value, totMem, freeMem = 0.0;
  string key;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;

      if(key=="MemTotal:"){
        totMem= value;
      }

      if(key=="MemFree:"){
        freeMem = value;
        break;
      }

    }
  }

  memUtilized = (totMem - freeMem)/(float)totMem;

  return memUtilized;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  float upTime, idleTime = 0.0;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime >> idleTime;
  }
  return (long) upTime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  vector<string> cpuData = LinuxParser::CpuUtilization();
  long totalJiffies = 0;
  for(auto data : cpuData){
    totalJiffies += std::stol(data);
  }
  totalJiffies = totalJiffies - std::stol(cpuData[8]) - std::stol(cpuData[9]);
  
  return totalJiffies;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long result;
  vector<string> values(25);
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    for(int i=0; i<25; i++){
      linestream >> values[i];
    }
    result = std::stol(values[13]) + std::stol(values[14]) + std::stol(values[15]) + std::stol(values[16]);
    return result;
  }
  return 0;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpuData = LinuxParser::CpuUtilization();
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpuData = LinuxParser::CpuUtilization();
  long idleJiffies = std::stol(cpuData[3]) + std::stol(cpuData[4]);
  return idleJiffies;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpuData(10);
  string key, value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if(stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    linestream >> key;
    int i =0;
    while(i<10){
        linestream >> cpuData[i];            
        i++;
    }
  } 

  return cpuData;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int processes;
  string key;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key;
      if(key=="processes"){
        linestream >> processes;
        return processes;
      }
    }
  }
  return processes;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int RunProcesses;
  string key;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key ;
      if(key=="procs_running"){
        linestream >> RunProcesses;
        return RunProcesses;
      }
    }
  }
  return RunProcesses;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line, cmd;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    linestream >> cmd;
    return cmd;
  }
  return cmd;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, key, value, unit;
  int memUse;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key >> value >> unit;
      if(key=="VmSize:"){
        memUse = std::stoi(value);
        memUse = memUse/1000;
      }
    }
  }
  return std::to_string(memUse);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key=="Uid:"){
        return value;
      }
    }
  }
  return value;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  string line;
  string key, random, value;
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> value >> random >> key;
      if(key==uid){
        return value;
      }
    }
  }
  return value;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string line, value, garb;
  long upTime;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      for(int i=0; i<22; i++){
        linestream >> value;
      }
      upTime = std::stol(value)/sysconf(_SC_CLK_TCK);
      return upTime;
    }
  }
  
  return 0;
 }