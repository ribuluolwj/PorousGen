#include<iostream>
using namespace std;
std::string  Get_Current_Date()
{
    time_t nowtime;  
    nowtime = time(NULL); //获取日历时间   
    char tmp[64];   
    strftime(tmp,sizeof(tmp),"%Y%m%d-%H%M%S",localtime(&nowtime));   
    return tmp;
}