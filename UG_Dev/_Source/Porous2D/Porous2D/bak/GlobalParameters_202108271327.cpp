#include<iostream>
using namespace std;
std::string  Get_Current_Date()
{
    time_t nowtime;  
    nowtime = time(NULL); //��ȡ����ʱ��   
    char tmp[64];   
    strftime(tmp,sizeof(tmp),"%Y%m%d-%H%M%S",localtime(&nowtime));   
    return tmp;
}