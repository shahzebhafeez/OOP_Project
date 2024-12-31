#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <stdexcept>
#include <cmath>
#include <Python.h>
#include "matplotlibcpp.h"

using namespace std;
namespace plt = matplotlibcpp;

class ThreeD {
    private:
        string equation;
    public:

        ThreeD(){  // Constructor
            //equation = s;
        }

        virtual void input_equation() {};
        virtual void plot() {};
};


double fx(double x, double y)
{
    return -(pow(x, 2) + pow(y, 2));
}

double dfx(double x, double y)
{
    return -2.0*x;
}

double dfy(double x, double y)
{
    return -2.0*y;
}

double plane(double x, double y, double x0, double y0)
{
    return dfx(x0, y0)*(x - x0) + dfy(x0, y0)*(y - y0) + fx(x0, y0);
}

std::map<std::string, std::vector<std::vector<double>>> Tangent(double c, double x0, double y0)
{
    double m0 = x0 - c, m1 = x0 + c;
    double n0 = y0 - c, n1 = y0 + c;
    int o = 60;

    double dM = (m1 - m0)/(o - 1);
    double dN = (n1 - n0)/(o - 1);

    std::map<std::string, std::vector<std::vector<double>>> result;
    std::vector<double> tx, ty, tz;

    double rx, ry;

    for(int i = 0; i < o; ++i){
        tx.clear();
        ty.clear();
        tz.clear();
        rx = m0 + i*dM;
        for(int j = 0; j < o; ++j){
            ry = n0 + j*dM;
            tx.push_back(rx);
            ty.push_back(ry);
            tz.push_back(plane(rx, ry, x0, y0));
        }
        result["x"].push_back(tx);
        result["y"].push_back(ty);
        result["z"].push_back(tz);
    }
    return result;
}

class z1 : public ThreeD{
    private:
        vector<vector<double>> x,y,z; // Coordiante 2-D Matrices Storing coordinates
        vector<double> tx,ty,tz; // 1-D temporary coordinates matrices

        int n = 60; // No. of times points are generated
        double t0 = -4.0 , t1 = 4.0; // Range of points to be generated
        double dT = (t1 - t0) / (n - 1); // Small increment value 
        double rx, ry; // Temporary Coordinates

    public:
         z1 () { // Constructor

         }
        //Function to Calculate coordinates
        double func (double a , double b){
            return a*a + b*b;
        }

        void plot () {
        PyObject* ax = plt::chart(111);
        plt::Clear3DChart(ax);

        for (int i = 0; i < n; ++i) {
            tx.clear();
            ty.clear();
            tz.clear();
            rx = t0 + i * dT;

            for (int j = 0; j < n; ++j) {
                ry = t0 + j * dT;
                tx.push_back(rx);
                ty.push_back(rx);
                tz.push_back(func(rx,ry));
            }
            x.push_back(tx);
            y.push_back(ty);
            z.push_back(tz);
    }   
    plt::surface3D(ax, x, y, z, "red", 0.9);
    plt::show();
    }
};

class z2 : public ThreeD{
private:
    double t0 , t1 , t2 , t3;
    double dT1 , dT2;
    int n = 60;
    
public:
    int a , b , c ;
    vector<vector<double>> x,y,z,z2; // Coordiante 2-D Matrices Storing coordinates
    vector<double> tx,ty,tz,tz2; // 1-D temporary coordinates matrices
    double theta,phi;
    virtual double x_calc(int param, double theta, double phi) = 0;
    virtual double y_calc(int param, double theta, double phi) = 0;
    virtual double z_calc(int param, double phi) = 0;
    
//Function to plot
    void calc_bounds (double l , double m , double o , double p)
    {
     t0 = l, t1 = m;
     dT1 = (t1 - t0) / (n - 1);

     t2 = o , t3 = p;
     dT2 = (t2 - t3) / (n - 1);
    }

    void plot(double l , double m , double o , double p, int a , int b , int c){
        
        calc_bounds (l,m,o,p);

        PyObject* ax = plt::chart(111);
        plt::Clear3DChart(ax);
        for (int i = 0; i < n; ++i) {
            tx.clear();
            ty.clear();
            tz.clear();
            theta = t0 + i * dT1;

           for (int j = 0; j < n; ++j) {
                phi = t2 + j * dT2;
                tx.push_back(x_calc(a,theta,phi));
                ty.push_back(y_calc(b,theta,phi));
                tz.push_back(z_calc(c,phi));
            }
            x.push_back(tx);
            y.push_back(ty);
            z.push_back(tz);
        }
        // double Px = -3.5, Py = -3.5;
        // double learning = 0.15;
        
        // std::map<std::string, std::vector<std::vector<double>>> gd;

        // for(int i = 0; i < 100; ++i){
        //     plt::Clear3DChart(ax);
        //     gd = Tangent(2, Px, Py);

        //     plt::surface3D(ax, x, y, z, "red", 0.9);
        //     plt::surface3D(ax, gd["x"], gd["y"], gd["z"], "green", 0.9);

        //     Px = Px + learning*dfx(Px, Py);
        //     Py = Py + learning*dfy(Px, Py);

        //     plt::pause(0.5);
        // }
        plt::surface3D(ax, x, y, z, "red", 0.9);
        plt::show();
    }
};

class sphere : public z2{

public:

// Functions to calculate coordinates
    double x_calc(int ro, double theta, double  phi) {
        return ro*sin(phi)*cos(theta);
    }

    double y_calc(int ro, double theta, double  phi) {
        return ro*sin(theta)*sin(phi);
    }

    double z_calc(int ro, double  phi) {
        return ro*cos(phi);
    }


};

class ellipsoid : public z2{
     // Denominator values
    private:

    public:

    // Functions to calculate coordinates
        double x_calc(int denom, double theta, double  phi) {
            return denom*sin(phi)*cos(theta);
        }

        double y_calc(int denom, double theta, double  phi) {
            return denom*sin(theta)*sin(phi);
        }

        double z_calc(int denom, double  phi) {
            return denom*cos(phi);
        }

};

class cone : public z2{
    private:

    public:

    double x_calc(int denom, double theta, double  phi) {
        return denom*cosh(phi)*cos(theta);
    }

    double y_calc(int denom, double theta, double  phi) {
        return denom*sin(theta)*cosh(phi);
    }

    double z_calc(int denom, double  phi) {
        return denom*sinh(phi);
    }

};

class twoSheetHyperboloid : public z2{
    private:

    int a = 2;
    int b = 4;
    int c = 3;

    int n = 60;
    double t0 = 0, t1 = 2*3.1428;
    double dT1 = (t1 - t0) / (n - 1);

    double t2 = 4;
    double dT2 = (t2 - t0) / (n - 1);

    public:

    double x_calc(int denom, double theta, double  phi) {
        return denom*sinh(phi)*cos(theta);
    }

    double y_calc(int denom, double theta, double  phi) {
        return denom*sin(theta)*sinh(phi);
    }

    double z_calc(int denom, double  phi) {
        return denom*cosh(phi);
    }
    
    void plot(){
        PyObject* ax = plt::chart(111);
        plt::Clear3DChart(ax);

        

        for (int i = 0; i < n; ++i) {
            tx.clear();
            ty.clear();
            tz.clear();
            tz2.clear();
            theta = t0 + i * dT1;

            for (int j = 0; j < n; ++j) {
                phi = t0 + j * dT2;
                tx.push_back(x_calc(a,theta,phi));
                ty.push_back(y_calc(b,theta,phi));
                tz.push_back(z_calc(c,phi));
                tz2.push_back(z_calc(-c,phi));
            }
            x.push_back(tx);
            y.push_back(ty);
            z.push_back(tz);
            z2.push_back(tz2);
        }
        
        plt::surface3D(ax, x, y, z, "blue", 0.7);
        plt::surface3D(ax, x, y, z2, "blue", 0.7);
        plt::show();
    }
};

class rectangular2D {
protected:
    vector<double> x_coefficients,x_powers;
    double constants;
public:
    rectangular2D(vector<double> coef,vector<double> powers, double constant)
        : x_coefficients(coef),x_powers(powers),constants(constant){}

    double equation(double x_value) {
        double sum = 0;
        for (int i = 0; i < x_coefficients.size(); ++i) {
            sum += x_coefficients[i]*pow(x_value,x_powers[i]);
        }
        sum += constants;
        return sum;
    }
    double derivative(double x_value) {
        double sum = 0;
        for (int i = 0; i < x_coefficients.size(); ++i) {
            if (x_powers[i] != 0) {
                sum += x_coefficients[i] * x_powers[i] * pow(x_value, x_powers[i] - 1);
            }
        }
        return sum;
    }
    void plot_combined(rectangular2D& object) {
        vector<double> x_points, y_points_function, y_points_derivative;
        int n = 100;
        double t0 = -3.0, t1 = 3.0;

        double dT = (t1 - t0) / (n - 1);

        for (int i = 0; i < n; ++i) {
            double t = t0 + i * dT;
            x_points.push_back(t);
            y_points_function.push_back(object.equation(t));
            y_points_derivative.push_back(object.derivative(t));
        }

        plt::plot(x_points, y_points_function, "b-");
        plt::plot(x_points, y_points_derivative, "r--");
        plt::xlabel("X-Axis");
        plt::ylabel("Y-Axis");
        plt::title("Polynomial and Its Derivative");
        plt::grid(true);
        plt::legend();
        plt::show();
    }
};

class Hyperbola {
private:
    double a, b, h, k;

public:
    Hyperbola(double h_val, double k_val, double a_val, double b_val)
        : h(h_val), k(k_val), a(a_val), b(b_val) {}

    // Function to calculate the 'x' equation for the right branch
    double x_equation_right(double t) {
        return h + a * cosh(t); // Hyperbolic cosine for x
    }

    // Function to calculate the 'y' equation for the right branch
    double y_equation_right(double t) {
        return k + b * sinh(t); // Hyperbolic sine for y
    }

    // Function to calculate the 'x' equation for the left branch
    double x_equation_left(double t) {
        return h - a * cosh(t); // Negative x for the left branch
    }

    // Function to calculate the 'y' equation for the left branch
    double y_equation_left(double t) {
        return k - b * sinh(t); // Negative y for the left branch
    }
    void plot_graph(Hyperbola& hyperbola_eq){
        vector<double> x_right, y_right, x_left, y_left;
        int n = 1000; // Increase the number of points for smoother curves
        double t0 = -3.0, t1 = 3.0; // Extend the range of t

        double dT = (t1 - t0) / (n - 1);

        // Generating points for the right branch
        for (int i = 0; i < n; ++i) {
            double t = t0 + i * dT;
            x_right.push_back(hyperbola_eq.x_equation_right(t));
            y_right.push_back(hyperbola_eq.y_equation_right(t));
        }

        // Generating points for the left branch
        for (int i = 0; i < n; ++i) {
            double t = t0 + i * dT;
            x_left.push_back(hyperbola_eq.x_equation_left(t));
            y_left.push_back(hyperbola_eq.y_equation_left(t));
        }

        // Plot both branches of the hyperbola
        plt::plot(x_right, y_right, "b-");
        plt::plot(x_left, y_left, "r-");
        plt::title("Hyperbola: ");
        plt::xlabel("X-axis");
        plt::ylabel("Y-axis");
        plt::grid(true);
        plt::show();
    }
};

class Circle{
    protected:
    double h,k,Radius;
    public:
    Circle(double& h_input,double& k_input,double& r):h(h_input),k(k_input),Radius(r){}
    double x_point(double& theta){
        return h+Radius*cos(theta);
    }
    double y_point(double& theta){
        return h+Radius*sin(theta);
    }
    void plot_graph(Circle& circle1){
        // Generate points for the circle
            vector<double> x, y;
            int n = 100; // Number of points
            double t0 = 0, t1 = 2 * M_PI;
            double dT = (t1 - t0) / (n - 1);

            for (int i = 0; i < n; ++i) {
                double theta = t0 + i * dT;
                x.push_back(circle1.x_point(theta));
                y.push_back(circle1.y_point(theta));
            }

            // Plot the circle
            plt::plot(x, y, "blue");
            plt::title("Circle: ");
            plt::xlabel("X-axis");
            plt::ylabel("Y-axis");
            plt::grid(true);
            plt::show();
    }
};
class Ellipse{
    protected:
    double h,k,a,b;
    public:
    Ellipse(double& h_input,double& k_input,double& a_input,double& b_input):h(h_input),k(k_input),a(a_input),b(b_input){}
    double x_point(double& theta){
        return h+a*cos(theta);
    }
    double y_point(double& theta){
        return k+b*sin(theta);
    }
    void plot_graph(Ellipse& ellipse1){
        // Generate points for the shape
        vector<double> x, y;
        int n = 100; // Number of points
        double t0 = 0, t1 = 2 * M_PI;
        double dT = (t1 - t0) / (n - 1);

        for (int i = 0; i < n; ++i) {
            double theta = t0 + i * dT;
            x.push_back(ellipse1.x_point(theta));
            y.push_back(ellipse1.y_point(theta));
        }

        // Plot the shape
        plt::plot(x, y, "blue");
        plt::title("Ellipse: ");
        plt::xlabel("X-axis");
        plt::ylabel("Y-axis");
        plt::grid(true);
        plt::show();
    }
};

// function to extract circle equation 
void parse_circle_equation(const string& equation, double& h, double& k, double& r) {
    // Regular expression to match the general equation of a circle
    regex circle_regex(R"(\(x\s*-\s*([-\d.]+)\)\^2\s*\+\s*\(y\s*-\s*([-\d.]+)\)\^2\s*=\s*([-\d.]+))");
    smatch match;

    if (regex_search(equation, match, circle_regex)) {
        h = stod(match[1].str());
        k = stod(match[2].str());
        r = sqrt(stod(match[3].str())); // Extract r^2 and compute r
    } else {
        throw invalid_argument("Invalid circle equation format. Use the form: (x - h)^2 + (y - k)^2 = r^2");
    }
}

//function to extract ellipse 
void parse_ellipse_equation(const string& equation, double& h, double& k, double& a, double& b) {
    regex ellipse_regex(R"(\(\s*x\s*-\s*([-\d.]+)\)\^2\s*/\s*([-\d.]+)\^2\s*\+\s*\(\s*y\s*-\s*([-\d.]+)\)\^2\s*/\s*([-\d.]+)\^2\s*=\s*1)");
    // regex hyperbola_regex(R"(\(\s*x\s*-\s*([-\d.]+)\)\^2\s*/\s*([-\d.]+)\^2\s*-\s*\(\s*y\s*-\s*([-\d.]+)\)\^2\s*/\s*([-\d.]+)\^2\s*=\s*1)");
    smatch match;
    if (regex_search(equation, match, ellipse_regex)) {
        h = stod(match[1].str());
        k = stod(match[2].str());
        a = sqrt(stod(match[3].str()));
        b = sqrt(stod(match[4].str()));
    } 
}


void displayMenu() {
    cout << "\n--- Main Menu ---\n";
    cout << "1. Polynomial\n";
    cout << "2. Circle\n";
    cout << "3. Ellipse\n";
    cout << "4. Hyperbola\n";
    cout << "5. Sphere\n";
    cout << "6. Ellipsoid\n";
    cout << "7. Cone\n";
    cout << "8. Two Sheet Hyperboloid\n";
    cout << "0. Exit\n";
}
int main() {
    string input_str;
    regex coef_regex("\\d+(?:\\.\\d+)?");
    regex power_regex("\\^\\d+");
    regex constant_regex("\\d+(?:\\.\\d+)?");
    double matched_coef, matched_power, matched_constant;

    //Main Menu
    displayMenu();
    int choice;
    cin >> choice;
    switch(choice){
        case 1:
            {
            vector<double> x_coefs,x_powers;
            double x_constant;

            //1.polynomial equations
            cout << "Enter a polynomial equation of type alpha*x^beta + constant :\n";
            getline(cin >> ws,input_str);

            // Extract coefficients, powers, and constant from input string
            auto words_begin = sregex_iterator(input_str.begin(), input_str.end(), coef_regex);
            auto words_end = sregex_iterator();
            
            for (sregex_iterator i = words_begin; i != words_end; ++i) {
                smatch match = *i;
                x_coefs.push_back(stod(match.str()));
            }

            words_begin = sregex_iterator(input_str.begin(), input_str.end(), power_regex);
            words_end = sregex_iterator();
            
            for (sregex_iterator i = words_begin; i != words_end; ++i) {
                smatch match = *i;
                x_powers.push_back(stod(match.str().substr(1))); // Remove '^' symbol
            }

            words_begin = sregex_iterator(input_str.begin(), input_str.end(), constant_regex);
            words_end = sregex_iterator();
            
            for (sregex_iterator i = words_begin; i != words_end; ++i) {
                smatch match = *i;
                x_constant = stod(match.str());
            }

            rectangular2D object(x_coefs,x_powers,x_constant);
            object.plot_combined(object);
            break;
            }
        case 2:
            {
            //2.Cricle
            string equation;
            double h = 0, k = 0, r = 0;
            // Prompt the user to enter the equation of the circle
            cout << "Enter the equation of the circle in the form (x - h)^2 + (y - k)^2 = r^2:\n";
            getline(cin >> ws, equation);

            try {
                // Parse the input equation
                parse_circle_equation(equation, h, k, r);

                // Display the extracted values
                cout << "Center: (" << h << ", " << k << "), Radius: " << r << endl;

                Circle circle1(h,k,r);
                circle1.plot_graph(circle1);
            } catch (const exception& e) {
                cerr << "Error: " << e.what() << endl;
            }

            break;
            }
        case 3:
            {
            //3. Ellipse 
            
            string equation;
            cout << "Enter the equation of the shape in the form: "
                    "(x - h)^2 / a^2 + (y - k)^2 / b^2 = 1 for Ellipse or\n ";
            getline(cin >> ws, equation);
            try {
                double h,k,a,b;
                // Parse the input equation
                parse_ellipse_equation(equation, h, k, a, b);

                // Display the extracted values
                cout << " Center: (" << h << ", " << k << "), ";
                Ellipse ellipse1(h,k,a,b);
                ellipse1.plot_graph(ellipse1);

            } catch (const exception& e) {
                cerr << "Error: " << e.what() << endl;
            }
            break;
            }
        case 4:
            {
            //4.hyperbola
            double h,k,a,b;
            cout << "Enter values for h for hyperbola: ";
            cin >> h;
            cout << "Enter values for k for hyperbola: ";
            cin >> k;
            cout << "Enter values for a for hyperbola: ";
            cin >> a;
            cout << "Enter values for b for hyperbola: ";
            cin >> b;
            Hyperbola hyperbola_eq(h, k, a, b);
            hyperbola_eq.plot_graph(hyperbola_eq);
            break;
            }
        case 5:
            {
                sphere surface;
                surface.plot(0, M_PI, 0, 2 * M_PI, 1, 1, 1);  
                break;
            }
        case 6:
            {
                ellipsoid ell;
                ell.plot(0, M_PI, 0, 2 * M_PI, 2, 1, 1); // Plot an ellipsoid
                break;  
            }
        case 7:
            {
                cone c1;
                c1.plot(0, 2 * M_PI, -4, 4, 2, 4, 3);
                break;
            }
        case 8:
            {
                twoSheetHyperboloid hyperboloid;
                hyperboloid.plot();
                break;
            }
        case 0:
            break;
    }
    return 0;
}