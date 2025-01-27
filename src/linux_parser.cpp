#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;
using std::ifstream;

// An example of how to read data from the filesystem
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

// An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  ifstream stream(kProcDirectory + kVersionFilename);
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float m_free{0}, m_total{1};
  string token;
  ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (stream >> token) {
      if (token == "MemTotal:") {
        if (stream >> token) {
          m_total = stof(token);
        }
      } 
      else if (token == "MemFree:") {
        if (stream >> token) {
          m_free = stof(token);
        }
      }
    }
  }
  return 1 - m_free / m_total;
}

// Read and return the system uptime
long int LinuxParser::UpTime() { 
  string token;
  string line;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream stream(line);
    while (stream >> token) {
      long token_int = stol(token);
      return token_int;
    }
  }
  return 0;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return UpTime() * sysconf(_SC_CLK_TCK);
 }

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line, val;
  vector<string> values;
  long jiffies = 0;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream line_stream(line);
    while (line_stream >> val) {
      values.push_back(val);
    }
  }

  if (values.size() > 21) {
    long user = stol(values[13]);
    long children_user = stol(values[15]);
    long kernel = stol(values[14]);
    long children_kernel = stol(values[16]);
    jiffies = user + kernel + children_user + children_kernel;
  }
  return jiffies;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> time{CpuUtilization()};
  auto active_jiffies = (stol(time[CPUStates::kUser_]) + stol(time[CPUStates::kNice_]) +
          stol(time[CPUStates::kSystem_]) + stol(time[CPUStates::kIRQ_]) +
          stol(time[CPUStates::kSoftIRQ_]) + stol(time[CPUStates::kSteal_]) +
          stol(time[CPUStates::kGuest_]) + stol(time[CPUStates::kGuestNice_]));
  
  return active_jiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> time{CpuUtilization()};
  auto idle_jiffies = (stol(time[CPUStates::kIdle_]) + stol(time[CPUStates::kIOwait_]));
  return idle_jiffies;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line, val, key;
  vector<string> values;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while (linestream >> val) {
      values.push_back(val);
    }
  }
  return values;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, key, val;
  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream line_stream(line);
      while (line_stream >> key >> val) {
        if (key == "processes") {
          return stoi(val);
        }
      }
    }
  } 
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line, key, val;
  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream line_stream(line);
      while (line_stream >> key >> val) {
        if (key == "procs_running") {
          return stoi(val);
        }
      }
    }
  }
  return 0;
 }

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string cmd;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, cmd);
    return cmd;
  }
  return "";
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string val;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (stream >> val) {
      if (val == "VmSize:") {
        if (stream >> val) {
          return std::to_string(stoi(val) / 1024);
        }
      }
    }
  }
  return "0";
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string val;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (stream >> val) {
      if (val == "Uid:") {
        if (stream >> val) {
          return val;
        }
      }
    }
  }
  return string("");
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  std::ifstream stream(LinuxParser::kPasswordPath);
  if (stream.is_open()) {
    string line, val{"x:" + Uid(pid)};
    while (std::getline(stream, line)) {
      auto pos = line.find(val);
      if (pos != string::npos) {
        return line.substr(0, pos - 1);
      }
    }
  }
  return "NA";
}

// Read and return the uptime of a process
long int LinuxParser::UpTime(int pid) {
  string val;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    for (int i = 0; stream >> val; ++i)
      if (i == 13) {
        long int time = stol(val);
        time /= sysconf(_SC_CLK_TCK);
        return time;
      }
  }
  return 0;
}
