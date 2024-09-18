#define USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <math.h>
#include <cmath>
#include <Python.h>
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

double JointDist(double x, double y, double rho){
    double factor = 1.0/(2.0*M_PI*sqrt(1 - pow(rho, 2)));
    double exponent = (pow(x, 2) - 2*x*y*rho + pow(y, 2))/(2.0*(1 - pow(rho, 2)));
    return factor*exp(-exponent);
}

std::map<std::string, std::vector<std::vector<double>>> Graph(double rho){
    std::map<std::string, std::vector<std::vector<double>>> result;
    std::vector<double> tx, ty, tz;
    double a0 = -3.5, a1 = 3.5;
    double an = 60;
    double da = (a1 - a0)/(an - 1);
    for(int i = 0; i < an; ++i){
        tx.clear();
        ty.clear();
        tz.clear();
        double xi = a0 + i*da;
        for(int j = 0; j < an; ++j){
            double yi = a0 + j*da;
            tx.push_back(xi);
            ty.push_back(yi);
            tz.push_back(JointDist(xi, yi, rho));
        }
        result["x"].push_back(tx);
        result["y"].push_back(ty);
        result["z"].push_back(tz);
    }
    return result;
}

int main()
{
    std::map<std::string, std::vector<std::vector<double>>> graph;

    PyObject * ax = plt::chart(121);
    PyObject * ay = plt::chart2D(122);

    double r0 = -0.99, r1 = 0.99;
    int rn = 150;
    double dr = (r1 - r0)/(rn - 1);

    plt::pause(4);
    for(int i = 0; i < rn; ++i){
        double rho = r0 + i*dr;
        plt::Clear3DChart(ax);
        plt::Clear3DChart(ay);
        graph = Graph(rho);
        plt::surface3DMap(ax, graph["x"], graph["y"], graph["z"], "hsv", 1.0);
        plt::contour3D(ay, graph["x"], graph["y"], graph["z"], "jet");
        plt::pause(0.001);
    }

    plt::show();

    return 0;
}