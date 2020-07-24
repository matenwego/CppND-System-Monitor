#include <string>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    long hours = seconds / 3600;
    seconds = seconds % 3600;
    long minutes = seconds / 60;
    seconds = seconds % 60;

    return Padding(hours) + ":" + Padding(minutes) + ":" + Padding(seconds);
 }

 string Format::Padding(long number) {
     return (number < 10 ? "0" : "") + to_string(number);
 }