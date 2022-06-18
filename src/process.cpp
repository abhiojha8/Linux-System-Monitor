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

// Return this process's ID
Process::Process(int pid) {
    pid_ = pid;
}

int Process::Pid() const { 
    return pid_; 
    }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const {
    return cpu_;
}

void Process::CpuUtilization(long system_ticks, long active_ticks) {
    long uptime, active_time;
    uptime = system_ticks - cached_system_ticks_;
    active_time = active_ticks - cached_active_ticks_;
    cpu_ = static_cast<float>(active_time) / uptime;
    cached_system_ticks_ = system_ticks;
    cached_active_ticks_ = active_ticks;
}

// TODO: Return the command that generated this process
string Process::Command() { return string(); }

// TODO: Return this process's memory utilization
string Process::Ram() { return string(); }

// TODO: Return the user (name) that generated this process
string Process::User() { return string(); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const { 
    return LinuxParser::UpTime(Pid()); 
    }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }