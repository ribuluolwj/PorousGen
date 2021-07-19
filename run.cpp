#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<cstring>

#include"Porous2D.h"

using namespace std;

void Generate2D();
int main()
{
    clock_t startTime, endTime;
    startTime = clock();


//    std::cout << porous.PNUM << endl;

    Generate2D();

    endTime = clock();
    std::cout << "Total Time : " << (double)(endTime - startTime) /CLOCKS_PER_SEC << "s" << std::endl;
    return 0;
}
 
void Generate2D()
{
    int pnum = 100;
    double * cx = new double[pnum];
    double * cy = new double[pnum];
    memset(cx, 0, sizeof(double)*pnum);
    memset(cy, 0, sizeof(double)*pnum);
    Porous2D porous(0.0,0.0,20.0,4.0,100,0.2);
    porous.ReadInit("Porous2D.init");
    porous.Generation(cx,cy);
    porous.output2tecplot(pnum, cx, cy, "porous2D.plt");
    delete[] cx;
    delete[] cy;

}