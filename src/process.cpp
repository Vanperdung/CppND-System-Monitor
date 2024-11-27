#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor
Process::Process(int id)
{
    _id = id;
}

// DONE
int Process::Pid()
{
    return _id;
}

// DONE
float Process::CpuUtilization()
{
    long now_active_time = LinuxParser::ActiveJiffies(_id) / sysconf(_SC_CLK_TCK);
    long now_total_time = Process::UpTime();

    return (float)now_active_time / (float)now_total_time;
}

// DONE
string Process::Command()
{
    return LinuxParser::Command(_id);
}

// DONE
string Process::Ram()
{
    return LinuxParser::Ram(_id);
}

// DONE
string Process::User()
{
    return LinuxParser::User(_id);
}

// DONE
long int Process::UpTime()
{
    return LinuxParser::UpTime(_id);
}

// DONE
bool Process::operator<(Process &a)
{
    if (CpuUtilization() - a.CpuUtilization() < 0)
        return true;
    else
        return false;
    return false;
}