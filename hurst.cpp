#include <iostream>
#include <string>
#include <sstream>
#include <thread> // for sleep_for
#include <chrono> // for chrono literals   
#include <curl/curl.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <vector>
#include <map>
#include <math.h>
#include <cmath>
#include <Python.h>
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

using namespace boost::property_tree;

std::string address(std::string ticker){
    std::string url = "https://api.polygon.io/v2/aggs/ticker/" + ticker + "/range/1/day/2024-01-09/2024-09-07?adjusted=true&sort=asc&limit=300&apiKey=_qfnK4nx068Hwzfer53IJWlZ_YpF97cM";
    return url;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t totalSize = size * nmemb;
    response->append((char*)contents, totalSize);
    return totalSize;
}

std::string RequestData(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init(); // Initialize CURL
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // Set the URL
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // Set the callback function
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response); // Pass the response string to the callback
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Follow redirects if necessary

        // Perform the request
        res = curl_easy_perform(curl);
        
        // Check for errors
        if(res != CURLE_OK) {
            std::cerr << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }

    return response; // Return the response
}

std::vector<double> ROR(std::vector<double> x){
    std::vector<double> y;
    for(int i = 1; i < x.size(); ++i){
        y.push_back(x[i]/x[i-1] - 1.0);
    }
    return y;
}

std::map<std::string, std::vector<double>> ImportHistoricalData(std::vector<std::string> tickers){
    std::map<std::string, std::vector<double>> result;
    std::cout << "Stock data is loading" << std::endl;
    for(auto & stock : tickers){
        std::this_thread::sleep_for(std::chrono::seconds(10));
        std::cout << stock << " is imported" << std::endl;
        std::string response = RequestData(address(stock));
        ptree data;
        std::stringstream ss(response);
        read_json(ss, data);
        for(ptree::const_iterator it = data.begin(); it != data.end(); ++it){
            if(it->first == "results"){
                for(ptree::const_iterator jt = it->second.begin(); jt != it->second.end(); ++jt){
                    for(ptree::const_iterator kt = jt->second.begin(); kt != jt->second.end(); ++kt){
                        if(kt->first == "l"){
                            result[stock].push_back(atof(kt->second.get_value<std::string>().c_str()));
                        }
                    }
                }
            }
        }
    }

    return result;
}

double Average(std::vector<double> x){
    double total = 0;
    for(auto & i : x){
        total += i;
    }
    total /= (double) x.size();
    return total;
}

double StandardDeviation(std::vector<double> x){
    double total = 0;
    double mu = Average(x);
    for(int i = 0; i < x.size(); ++i){
        total += pow(x[i] - mu, 2);
    }
    total /= ((double) x.size() - 1);
    total = pow(total, 0.5);
    return total;
}

double hurstExponent(std::vector<double> stockData){
    int N = stockData.size();
    std::vector<double> Y;
    double xmu = Average(stockData);
    double y = 0;
    for(int i = 0; i < N; ++i){
        y += (stockData[i] - xmu);
        Y.push_back(y);
    }
    std::sort(Y.begin(), Y.end());
    double R = Y[N - 1] - Y[0];
    double S = StandardDeviation(stockData);
    return log(R/S)/log(N);

}

int main()
{

    PyObject * ax = plt::chart2D(111);

    std::vector<std::string> tickers = {"NVDA","MSFT","AAPL","AMZN","META","GOOGL","IBM", "GS", "JPM", "MS"};

    std::map<std::string, std::vector<double>> df = ImportHistoricalData(tickers);
    
    for(auto & ticker : tickers){
        std::vector<double> rate_of_return = ROR(df[ticker]);
        double hexp = hurstExponent(rate_of_return);
        double risk_ = StandardDeviation(rate_of_return);
        double reward_ = Average(rate_of_return);
        if(hexp == 0.5){
            plt::scatter2DX(ax, risk_, reward_, "black");
        } else if(hexp < 0.5) {
            plt::scatter2DX(ax, risk_, reward_, "red");
        } else {
            plt::scatter2DX(ax, risk_, reward_, "blue");
        }
        plt::annotateGraph(ax, ticker, risk_, reward_);
    }
    
    plt::show();

    return 0;
}