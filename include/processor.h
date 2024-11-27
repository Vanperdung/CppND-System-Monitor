#ifndef PROCESSOR_H
#define PROCESSOR_H



class Processor
{
  public:
    float Utilization();

  private:
    unsigned long long int  prev_non_idle_time{0};
    unsigned long long int  prev_idle_time{0};
};

#endif