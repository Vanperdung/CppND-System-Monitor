#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

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
	DIR* directory = opendir(kProcDirectory.c_str());
	struct dirent* file;
	while ((file = readdir(directory)) != nullptr)
	{
		// Is this a directory?
		if (file->d_type == DT_DIR)
		{
			// Is every character of the name a digit?
			string filename(file->d_name);
			if (std::all_of(filename.begin(), filename.end(), isdigit))
			{
				int pid = stoi(filename);
				pids.push_back(pid);
			}
		}
	}
	closedir(directory);
	return pids;
}

float LinuxParser::MemoryUtilization() 
{
    std::ifstream mem_path;
    std::string line;

    std::regex header_pattern(R"(^\w+)");
    std::regex info_pattern(R"(\d+)");
    std::smatch header_match;
    std::smatch info_match;

    std::map<std::string, int> mem_infos = {
        {"MemTotal", 0}, \
        {"MemFree", 0}, \
        {"MemAvailable", 0}, \
        {"Buffers", 0}, \
		{"Cached", 0}, \
        {"SReclaimable", 0}, \
        {"Shmem", 0}
    };

    mem_path.open(kProcDirectory + kMeminfoFilename, std::ios::in);
    if (!mem_path)
    {
        return 0.0;
    }

    // get line from mem_path
    while (getline(mem_path, line))
    {
        // get the header from this line
        if (std::regex_search(line, header_match, header_pattern))
        {
            // check if the header exists in map
            if (mem_infos.find(header_match.str()) != mem_infos.end())
            {
                // get the value of this header and store to map
                std::regex_search(line, info_match, info_pattern);
                mem_infos[header_match.str()] = std::stoi(info_match.str());
            }
        }
    }

    return ((float)(mem_infos["MemTotal"] - mem_infos["MemFree"] - mem_infos["Buffers"] - mem_infos["Cached"] + mem_infos["Shmem"])) / ((float)mem_infos["MemTotal"]);
}

long LinuxParser::UpTime() 
{
	string line;
	int uptime;
	std::ifstream stream(kProcDirectory + kUptimeFilename);
	if (stream.is_open())
	{
		std::getline(stream, line);
		std::istringstream linestream(line);
		linestream >> uptime;
	}
	return uptime;
}

long LinuxParser::Jiffies()
{
	long jiffiesTotal;
	std::vector<std::string> jiffies = LinuxParser::CpuUtilization();

	for (auto jiffy : jiffies)
	{
		jiffiesTotal += std::stol(jiffy);
	}
	
	return jiffiesTotal;
}

long LinuxParser::ActiveJiffies(int pid)
{
	vector<string> jiffies;
	string jiffy, line;
	std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
    if (stream.is_open())
    {
        while (std::getline(stream, line))
        {
            std::istringstream linestream(line);
            while (linestream >> jiffy)
            {
                jiffies.push_back(jiffy);
            }
            return (int)(std::stol(jiffies[13]) + std::stol(jiffies[14]) + std::stol(jiffies[15]) + std::stol(jiffies[16]));
        }
    }
    else
        return 0;
    return 0;
}

long LinuxParser::ActiveJiffies()
{
	std::vector<std::string> cpu_values = CpuUtilization();
	return std::stol(cpu_values[LinuxParser::kUser_]) +
           std::stol(cpu_values[LinuxParser::kNice_]) +
           std::stol(cpu_values[LinuxParser::kSystem_]) +
           std::stol(cpu_values[LinuxParser::kIRQ_]) +
           std::stol(cpu_values[LinuxParser::kSoftIRQ_]) +
           std::stol(cpu_values[LinuxParser::kSteal_]);
}

long LinuxParser::IdleJiffies()
{
	std::vector<std::string> cpu_values = CpuUtilization();
	return std::stol(cpu_values[LinuxParser::kIdle_]) +
           std::stol(cpu_values[LinuxParser::kIOwait_]);
}

vector<string> LinuxParser::CpuUtilization() 
{ 
	std::string line, key, value;
	std::vector<std::string> values;

	std::ifstream stream(kProcDirectory + kStatFilename);
	if (stream.is_open())
	{
		std::getline(stream, line);
		std::istringstream linestream(line);
		
		linestream >> key;
		while (linestream >> value)
		{
			values.emplace_back(value);
		}	
	}

	return values;
}

int LinuxParser::TotalProcesses()
{
	std::string line, key;
	int value = 0;
	std::ifstream stream(kProcDirectory + kStatFilename);
	if (stream.is_open())
	{
		while (std::getline(stream, line))
		{
			std::istringstream linestream(line);
			linestream >> key >>value;
			if (key == "processes")
			{
				return value;
			}
		}
	}
	return 0;
}

int LinuxParser::RunningProcesses()
{
	std::string line, key;
	int value = 0;
	std::ifstream stream(kProcDirectory + kStatFilename);
	if (stream.is_open())
	{
		while (std::getline(stream, line))
		{
			std::istringstream linestream(line);
			linestream >> key >>value;
			if (key == "procs_running")
			{
				return value;
			}
		}
	}
	return 0;
}


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

string LinuxParser::Ram(int pid)
{
	std::string line, key, ram;
	std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if (stream.is_open()) 
	{
        while (std::getline(stream, line)) 
		{
            std::istringstream linestream(line);
            linestream >> key >> ram;
            if (key == "VmSize:") {
                return std::to_string(std::stol(ram) / 1024);
            }
        }
    }
    return string();
}

string LinuxParser::Uid(int pid)
{
	std::string line, key, uid;
	std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if (stream.is_open()) 
	{
        while (std::getline(stream, line)) 
		{
            std::istringstream linestream(line);
            linestream >> key >> uid;
            if (key == "Uid:") {
                return uid;
            }
        }
    }
    return string();
}

string LinuxParser::User(int pid)
{
	std::string uid = LinuxParser::Uid(pid);
    std::ifstream stream(kPasswordPath);
    std::string line, user, x, id;
    if (stream.is_open()) 
	{
        while (std::getline(stream, line)) 
		{
            std::replace(line.begin(), line.end(), ':', ' ');
            std::istringstream linestream(line);
            linestream >> user >> x >> id;
            if (id == uid) 
			{
                return user;
            }
        }
    }
    return string();
}

long LinuxParser::UpTime(int pid)
{
    std::string line;
    long start_time;
	std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
    if (stream.is_open()) 
	{
        std::getline(stream, line);
        std::istringstream linestream(line);
        std::vector<std::string> values((std::istream_iterator<std::string>(linestream)),
                                        std::istream_iterator<std::string>());
		start_time = std::stol(values[21]) / sysconf(_SC_CLK_TCK);
    }
    return UpTime() - start_time;
}
