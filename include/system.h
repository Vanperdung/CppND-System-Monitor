#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System
{
  public:
    Processor &Cpu();
    std::vector<Process> &Processes();
    float MemoryUtilization();         
    long UpTime();                     
    int TotalProcesses();              
    int RunningProcesses();            
    std::string Kernel();              
    std::string OperatingSystem();     

  private:
    Processor cpu_ = {};
    std::vector<Process> processes_ = {};
    bool is_id_in_process_list(int pid);
    bool is_process_in_list(int pid, std::vector<int> list);
};

#endif