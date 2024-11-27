#include "processor.h"

#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

float Processor::Utilization()
{
    vector<string> cpu_string_values = LinuxParser::CpuUtilization();
    vector<long> cpu_values;
    for (int i = 0; i < (int)cpu_string_values.size(); i++)
    {
        cpu_values.push_back((long)stol(cpu_string_values[i]));
    }

    unsigned long long int now_idle_time = LinuxParser::IdleJiffies();
    unsigned long long int now_non_idle_time = LinuxParser::ActiveJiffies();

    unsigned long long int now_total_time = now_non_idle_time + now_idle_time;
    unsigned long long int prev_total_time = prev_non_idle_time + prev_idle_time;

    if (prev_total_time == 0U)
    {
        prev_non_idle_time = now_non_idle_time;
        prev_idle_time = now_idle_time;
        return 0.0;
    }

    if (now_total_time == prev_total_time)  return 0.0;

    float ret = (float)((float)(now_total_time - prev_total_time - now_idle_time + prev_idle_time) / (float)(now_total_time - prev_total_time));

    prev_non_idle_time = now_non_idle_time;
    prev_idle_time = now_idle_time;

    return ret;
}