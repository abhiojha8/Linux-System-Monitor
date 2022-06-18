#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
    float utilization = 0;

    long active_ticks = LinuxParser::ActiveJiffies();
    long idle_ticks = LinuxParser::IdleJiffies();

    long active_time = active_ticks - cached_active_ticks_;
    long idle_time = idle_ticks - cached_idle_ticks_;

    long total_time = active_time + idle_time;

    utilization = static_cast<float> (active_time) / total_time;
    cached_active_ticks_ = active_ticks;
    cached_idle_ticks_ = idle_ticks;
    return utilization;
 }