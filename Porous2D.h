#ifndef POROUS2D_H
#define POROUS2D_H

#include <string>
class Porous2D
{
    public:
        double X0,Y0,XMAX,YMAX;  /// porous boundary
        int PNUM; /// particle number
        double PD; /// particle diameter

        /// Constructor
        Porous2D(double X0, double Y0, double XMAX, double YMAX, int PNUM, double PD);
        ~Porous2D();

        void ReadInit(std::string filename);
        void Generation(double* cx, double* cy);
        void output2tecplot(int pnum, double* cx, double* cy, std::string filename);
    private:
        double* CX; /// particle core x coordinate
        double* CY; /// particle core y coordinate

        

};

#endif