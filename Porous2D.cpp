#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<cstdlib>
#include<ctime>
#include<omp.h>
#include<sstream>
//#include<random>

#include"Porous2D.h"
//#include"gnuplot-iostream.h"

using namespace std;

Porous2D::Porous2D(double x0=0.0, double y0=0.0, double xmax=20.0, double ymax=4.0, int pnum=100, double d=0.2):X0(x0), Y0(y0), XMAX(xmax), YMAX(ymax), PNUM(pnum), PD(d)
{
}

void Porous2D::ReadInit(std::string filename)
{
    std::string line;
    std::string data;
    ifstream readin;
    readin.open(filename, ios::in);
    if (readin.fail())  //open file failed
    {
        std::cout << "Open file failed.";
    }
    else //open file successfully
    {
        //skip 7 lines in the head
        for (int i = 0; i < 7; i++) readin.ignore(1000, '\n');

        while(getline(readin, line))
        {
            std::vector<double> x;
            std::istringstream record(line);
            for (double j; record >> j;)
            {
                x.push_back(j);
                if (record.peek() ==',') record.ignore();
            }
            if (x.size() == 6)
            {
                this->X0 = x[0];
                this->Y0 = x[1];
                this->XMAX = x[2];
                this->YMAX = x[3];
                this->PNUM = x[4];
                this->PD = x[5];
            }
        }
    }
}
void Porous2D::Generation(double* cx, double* cy) 
{
    // Generate particle core coordination

    // Generate random seed
    std::srand((unsigned)std::time(0));

    // 
    this->CX = new double[PNUM]{};
    this->CY = new double[PNUM]{};

    // Generate random core coordination
    for (int i = 0; i < this->PNUM; i++)
    {
        this->CX[i] = double(rand()) / double(RAND_MAX) * (this->XMAX - this->X0) + this->X0;
        this->CY[i] = double(rand()) / double(RAND_MAX) * (this->YMAX - this->Y0) + this->Y0;
        std::cout << i << '\t' << this->CX[i] << '\t' << this->CY[i] << endl;
    } 

}

void Porous2D::output2tecplot(int pnum, double* cx, double* cy, std::string filename)
{
    ofstream outfile;
    string title = "2D porous media";
    outfile.open(filename);
    
    outfile << "TITLE = \"" << title << "\"" << endl;
    
    /// output solid position
//    outfile << "VARIABLES = \"X\", \"Y\",\"value\" " << endl;
    outfile << "VARIABLES = \"X\", \"value\" " << endl;
//    outfile << "ZONE  t = \"solid\" " << " I = " << pnum << ", J = " << pnum << ", F = point" << endl;
    outfile << "ZONE  t = \"solid\" " << " I = " << pnum << ", F = point" << endl;
    for (int i = 0; i < pnum; i++)
    {
        cx[i] = this->CX[i];
        cy[i] = this->CY[i];
//        const int cell = CellIndex(i, j, Total_M, Total_N);
        outfile << cx[i] << "," << cy[i] << endl;
    }
    
    outfile.close();
    std::cout << "Output completed for tecplot." << endl;
}

Porous2D::~Porous2D() /// 
{
}