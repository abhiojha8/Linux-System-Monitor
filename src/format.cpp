#include <string>
#include <iomanip>
#include <sstream>

#include "format.h"

using std::string;
using std::stringstream;
using std::setfill;

string Format::ElapsedTime(long seconds) { 
    // INPUT: Long int measuring seconds
    // OUTPUT: HH:MM:SS
    int hrs, mins;
    long secs;
    hrs = seconds / 3600;
    mins = (seconds / 60) % 60;
    secs = seconds % 60;

    stringstream output_str;

    output_str << setfill('0') << std::setw(2) << hrs << ":"
               << setfill('0') << std::setw(2) << mins << ":"
               << setfill('0') << std::setw(2) << secs;

    return output_str.str(); 
    }