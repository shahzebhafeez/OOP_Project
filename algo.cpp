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
    cout << "5. 3D\n";
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

            vector<double> x_points, y_points;
            int n = 100;
            double t0 = -3.0, t1 = 3.0;  

            double dT = (t1 - t0) / (n - 1);

            for (int i = 0; i < n; ++i) {
                double t = t0 + i * dT;
                x_points.push_back(t);
                y_points.push_back(object.equation(t));
            }

            plt::plot(x_points, y_points, "b-");
            plt::xlabel("X-Axis");
            plt::ylabel("Y-Axis");
            plt::title("Polynomial");
            plt::grid(true);
            plt::show();
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
                plt::title("Circle: " + equation);
                plt::xlabel("X-axis");
                plt::ylabel("Y-axis");
                plt::grid(true);
                plt::show();
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
                plt::title(": " + equation);
                plt::xlabel("X-axis");
                plt::ylabel("Y-axis");
                plt::grid(true);
                plt::show();

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
            plt::title("Hyperbola");
            plt::xlabel("X-axis");
            plt::ylabel("Y-axis");
            plt::grid(true);
            plt::show();
            break;
            }
        case 5:
            // 3D graphing
            break;;
        case 0:
            break;
    }
    return 0;
}