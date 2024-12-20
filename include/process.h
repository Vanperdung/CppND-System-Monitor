#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process
{
  public:
    Process(int id);
    int Pid();
    std::string User();
    std::string Command();
    float CpuUtilization();
    std::string Ram();
    long int UpTime();
    bool operator<(Process &a);

  private:
    int _id{0};
    int prev_active_time{0};
    int prev_total_time{0};
};

#endif