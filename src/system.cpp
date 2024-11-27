#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
// DONE
Processor &System::Cpu()
{
    return cpu_;
}

// DONE
bool System::is_id_in_process_list(int pid)
{
    for (int i = 0; i < (int)processes_.size(); i++)
    {
        if (processes_[i].Pid() == pid) return true;
    }
    return false;
}

// DONE
bool System::is_process_in_list(int pid, std::vector<int> list)
{
    for (int i = 0; i < (int)list.size(); i++)
    {
        if (list[i] == pid) return true;
    }
    return false;
}

// DONE
static bool compareProcess(Process p1, Process p2)
{
    return (p2 < p1);
}

// DONE
vector<Process> &System::Processes()
{
    vector<int> all_process = LinuxParser::Pids();
    for (int i = 0; i < (int)all_process.size(); i++)
    {
        if (is_id_in_process_list(all_process[i]) == false)
        {
            Process temp(all_process[i]);
            processes_.push_back(temp);
        }
    }

    for (int i = 0; i < (int)processes_.size(); i++)
    {
        if (is_process_in_list(processes_[i].Pid(), all_process) == false)
        {
            processes_.erase(processes_.begin() + i);
            i = 0;
        }
    }

    sort(processes_.begin(), processes_.end(), compareProcess); 

    return processes_;
}

std::string System::Kernel() 
{
    return LinuxParser::Kernel();
}

float System::MemoryUtilization() 
{ 
    return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() 
{
    return LinuxParser::OperatingSystem(); 
}

int System::RunningProcesses()
{
    return LinuxParser::RunningProcesses();
}

int System::TotalProcesses()
{
    return LinuxParser::TotalProcesses();
}

long int System::UpTime() 
{ 
    return LinuxParser::UpTime(); 
}
