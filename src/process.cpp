#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid){
    pid_ = pid;
    user_ = LinuxParser::User(pid_);
    command_ = LinuxParser::Command(pid_);
    ram_ = LinuxParser::Ram(pid_);
    upTime_ = LinuxParser::UpTime(pid_);
}

// Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
    float cpuUtil;
    
    long totTime = LinuxParser::ActiveJiffies(pid_);
    long sysUpTime = LinuxParser::UpTime();

    vector<string> values(26);
    string line;
    std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid_) + LinuxParser::kStatFilename);
    if(stream.is_open()){
        std::getline(stream,line);
        std::istringstream linestream(line);
        for(int i=0; i<25; i++){
        linestream >> values[i];
        }
    }

    long seconds = sysUpTime - (std::stol(values[21])/sysconf(_SC_CLK_TCK));
    cpuUtil = (totTime/sysconf(_SC_CLK_TCK)) / (float) seconds;
    cpuUtil_ = cpuUtil;
    return cpuUtil_;
}

// Return the command that generated this process
string Process::Command() { return command_; }

// Return this process's memory utilization
string Process::Ram() { return ram_; }

// Return the user (name) that generated this process
string Process::User() { return user_; }

// Return the age of this process (in seconds)
long int Process::UpTime() { return upTime_; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    return ram_ < a.ram_;
}