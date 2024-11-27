#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem()
{
    string line;
    string key;
    string value;
    std::ifstream filestream(kOSPath);
    if (filestream.is_open())
    {
        while (std::getline(filestream, line))
        {
            std::replace(line.begin(), line.end(), ' ', '_');
            std::replace(line.begin(), line.end(), '=', ' ');
            std::replace(line.begin(), line.end(), '"', ' ');
            std::istringstream linestream(line);
            while (linestream >> key >> value)
            {
                if (key == "PRETTY_NAME")
                {
                    std::replace(value.begin(), value.end(), '_', ' ');
                    return value;
                }
            }
        }
    }
    return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel()
{
    string os, kernel, version;
    string line;
    std::ifstream stream(kProcDirectory + kVersionFilename);
    if (stream.is_open())
    {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> os >> version >> kernel;
    }
    return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids()
{
    vector<int> pids;
    DIR *directory = opendir(kProcDirectory.c_str());
    struct dirent *file;
    while ((file = readdir(directory)) != nullptr)
    {
        // Is this a directory?
        if (file->d_type == DT_DIR)
        {
            // Is every character of the name a digit?
            string filename(file->d_name);
            if (std::all_of(filename.begin(), filename.end(), isdigit))
            {
                int pid = stol(filename);
                pids.push_back(pid);
            }
        }
    }
    closedir(directory);
    return pids;
}

// DONE
float LinuxParser::MemoryUtilization()
{
    string line;
    string key;
    string value;
    int mem_total;
    int mem_free;
    int mem_buffer;
    int mem_cache;
    int mem_reclaimable;
    int mem_shmem;
    std::ifstream filestream(kProcDirectory + kMeminfoFilename);
    if (filestream.is_open())
    {
        while (std::getline(filestream, line))
        {
            std::istringstream linestream(line);
            linestream >> key >> value;
            if (key == "MemTotal:")
                mem_total = stol(value);
            else if (key == "MemFree:")
                mem_free = stol(value);
            else if (key == "Buffers:")
                mem_buffer = stol(value);
            else if (key == "Cached:")
                mem_cache = stol(value);
            else if (key == "SReclaimable:")
                mem_reclaimable = stol(value);
            else if (key == "Shmem:")
                mem_shmem = stol(value);
        }
    }
    else
        return 0.0;

    return (float)((float)(mem_total - mem_free - mem_buffer - mem_cache - mem_reclaimable + mem_shmem) / mem_total);
}

// DONE
long LinuxParser::UpTime()
{
    string line;
    string secs;
    std::ifstream filestream(kProcDirectory + kUptimeFilename);
    if (filestream.is_open())
    {
        std::getline(filestream, line);
        std::replace(line.begin(), line.end(), '.', ' ');
        std::istringstream linestream(line);
        linestream >> secs;
        return (long)stol(secs);
    }
    else
        return 0;
    return 0;
}

// DONE
long LinuxParser::Jiffies()
{
    return (long)(ActiveJiffies() + IdleJiffies());
}

// DONE
long LinuxParser::ActiveJiffies(int pid)
{
    string line;
    string word;
    vector<string> values;
    std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
    if (filestream.is_open())
    {
        while (std::getline(filestream, line))
        {
            std::istringstream linestream(line);
            while (linestream >> word)
            {
                values.push_back(word);
            }
            return (int)(stol(values[13]) + stol(values[14]) + stol(values[15]) + stol(values[16]));
        }
    }
    else
        return 0;
    return 0;
}

// DONE
long LinuxParser::ActiveJiffies()
{
    string line;
    string temp;
    vector<long> cpu_values;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open())
    {
        std::getline(filestream, line);
        std::istringstream linestream(line);
        linestream >> temp;
        for (int i = 0; i < 10; i++)
        {
            linestream >> temp;
            cpu_values.push_back((long)stol(temp));
        }
        cpu_values[kUser_] -= cpu_values[kGuest_];
        cpu_values[kNice_] -= cpu_values[kGuestNice_];
        return (cpu_values[kUser_] + cpu_values[kNice_] + cpu_values[kSystem_] + cpu_values[kIRQ_] + cpu_values[kSoftIRQ_] + cpu_values[kSteal_]);
    }
    else
        return 0;
    return 0;
}

// DONE
long LinuxParser::IdleJiffies()
{
    string line;
    string temp;
    vector<long> cpu_values;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open())
    {
        std::getline(filestream, line);
        std::istringstream linestream(line);
        linestream >> temp;
        for (int i = 0; i < 10; i++)
        {
            linestream >> temp;
            cpu_values.push_back((long)stol(temp));
        }
        cpu_values[kUser_] -= cpu_values[kGuest_];
        cpu_values[kNice_] -= cpu_values[kGuestNice_];
        return (cpu_values[LinuxParser::kIdle_] + cpu_values[LinuxParser::kIOwait_]);
    }
    else
        return 0;
    return 0;
}

// DONE
vector<string> LinuxParser::CpuUtilization()
{
    string line;
    string temp;
    vector<string> values;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open())
    {
        std::getline(filestream, line);
        std::istringstream linestream(line);
        linestream >> temp;
        for (int i = 0; i < 10; i++)
        {
            linestream >> temp;
            values.push_back(temp);
        }
        return values;
    }
    else
        return {};
    return {};
}

// DONE
int LinuxParser::TotalProcesses()
{
    string line;
    string key;
    string value;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open())
    {
        while (std::getline(filestream, line))
        {
            std::istringstream linestream(line);
            linestream >> key >> value;
            if (key == "processes")
                return stol(value);
        }
    }
    else
        return 0;
    return 0;
}

// DONE
int LinuxParser::RunningProcesses()
{
    string line;
    string key;
    string value;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open())
    {
        while (std::getline(filestream, line))
        {
            std::istringstream linestream(line);
            linestream >> key >> value;
            if (key == "procs_running")
                return stol(value);
        }
    }
    else
        return 0;
    return 0;
}

// DONE
string LinuxParser::Command(int pid)
{
    string line;
    std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
    if (filestream.is_open())
    {
        if (std::getline(filestream, line))
        {
            std::replace(line.begin(), line.end(), '\000', ' ');
            return line;
        }
    }
    else
        return string();
    return string();
}

// DONE
string LinuxParser::Ram(int pid)
{
    string line;
    string key;
    string value;
    std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
    if (filestream.is_open())
    {
        while (std::getline(filestream, line))
        {
            std::istringstream linestream(line);
            linestream >> key >> value;
            if (key == "VmSize:")
                return to_string((long)(stol(value) / 1024));
        }
    }
    else
        return string();
    return string();
}

// DONE
string LinuxParser::Uid(int pid)
{
    string line;
    string key;
    string value;
    std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
    if (filestream.is_open())
    {
        while (std::getline(filestream, line))
        {
            std::istringstream linestream(line);
            linestream >> key >> value;
            if (key == "Uid:")
                return value;
        }
    }
    else
        return string();
    return string();
}

// DONE
string LinuxParser::User(int pid)
{
    string line;
    string key;
    string value;
    string id = Uid(pid);
    string x;
    std::ifstream filestream(kPasswordPath);
    if (filestream.is_open())
    {
        while (std::getline(filestream, line))
        {
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream linestream(line);
            linestream >> key >> x >> value;
            if (value == id)
                return key;
        }
    }
    else
        return string();
    return string();
}

// DONE
long LinuxParser::UpTime(int pid)
{
    string line;
    string word;
    vector<string> values;
    long time = 0;
    std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
    if (filestream.is_open())
    {
        std::getline(filestream, line);
        std::istringstream linestream(line);
        while (linestream >> word)  {values.push_back(word);}
        time = stol(values[21]) / sysconf(_SC_CLK_TCK);
        return time;
    }
    else
        return 0;

    return 0;
}