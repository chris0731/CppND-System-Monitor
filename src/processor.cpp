#include "processor.h"
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <iostream>
#include "linux_parser.h"

using std::string;
using std::vector;


// Return the aggregate CPU utilization
float Processor::Utilization() { 
    float cpuUsage;

    long totTime = LinuxParser::Jiffies();
    long idle = LinuxParser::IdleJiffies();

    long totDiff = totTime - prevTot;
    long idleDiff = idle - prevIdle;

    cpuUsage = (totDiff - idleDiff)/(float)totDiff;

    prevTot = totTime;
    prevIdle = idle;

    return cpuUsage;
}