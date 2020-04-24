#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    
    string Time;

    int hours = seconds/3600;
    int remSeconds = seconds - (hours*3600);
    int minutes = remSeconds/60;
    int second = remSeconds - (minutes*60);
    
    string hr = std::to_string(hours);
    if(hours<10){
        hr = "0" + hr;
    }

    string mi = std::to_string(minutes);
    if(minutes<10){
        mi = "0" + mi;
    }

    string se = std::to_string(second);
    if(second<10){
        se = "0" + se;
    }

    string time = hr + ":" + mi + ":" + se;

    return time;
}