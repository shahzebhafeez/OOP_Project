#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cpprest/ws_client.h>
#include <sstream>
#include <algorithm>
#include <thread>
#include <math.h>
#include <cmath>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <Python.h>
#include "matplotlibcpp.h"

using namespace web;
using namespace web::websockets::client;
using namespace boost::property_tree;

namespace plt = matplotlibcpp;


std::string JoinTicks(std::vector<std::string> tickers){
    std::string result = "[";
    for(auto & tick : tickers){
        result += "\"" + tick + "\",";
    }
    result.pop_back();
    result += "]";
    return result;
}

std::string BuildMsg(std::map<std::string, std::string> the_input){
    std::string result = "{";
    for(auto & entry : the_input){
        result += "\"" + entry.first + "\":\"" + entry.second + "\",";
    }
    result.pop_back();
    result += "}";
    return result;
}

void Hurricane(std::string message, std::map<std::string, double> & priceData){
    ptree data;
    std::stringstream ss(message);
    read_json(ss, data);
    bool goPrice = false;
    for(ptree::const_iterator it = data.begin(); it != data.end(); ++it){
        std::string ticker = "";
        double currentPrice = 0;
        for(ptree::const_iterator jt = it->second.begin(); jt != it->second.end(); ++jt){
            if(goPrice == true){
                if(jt->first == "S"){
                    ticker = jt->second.get_value<std::string>();
                }
                if(jt->first == "p"){
                    currentPrice = atof(jt->second.get_value<std::string>().c_str());
                    goPrice = false;
                }
            }
            if(jt->first == "T"){
                if(jt->second.get_value<std::string>() == "t"){
                    goPrice = true;
                }
            }
        }
        priceData[ticker] = currentPrice;
    }
}

static void Socket(std::vector<std::string> tickers, std::map<std::string, double> & priceData){

    std::string ticklist = JoinTicks(tickers);
    std::string url = "wss://stream.data.alpaca.markets/v2/iex";
    
    // ALPACA API KEY AND SECRET
    std::map<std::string, std::string> auth = {
        {"action", "auth"},
        {"key", ""},
        {"secret",""}
    };
    
    std::string authenticate = BuildMsg(auth);
    std::string message = "{\"action\":\"subscribe\", \"trades\":" + ticklist + "}";
    
    websocket_client client;
    client.connect(url).wait();

    websocket_outgoing_message outmsg;
    outmsg.set_utf8_message(authenticate);
    client.send(outmsg);

    outmsg.set_utf8_message(message);
    client.send(outmsg);

    while(true){
        client.receive().then([](websocket_incoming_message inmsg){
            return inmsg.extract_string();
        }).then([&](std::string message){
            std::cout << message << std::endl;
            Hurricane(message, std::ref(priceData));
        }).wait();

    }

    client.close().wait();
    
}

std::vector<double> RateOfReturn(std::vector<double> x){
    std::vector<double> y;
    for(int i = 1; i < x.size(); ++i){
        y.push_back(x[i]/x[i-1] - 1.0);
    }
    return y;
}

std::vector<double> MuSd(std::vector<double> x){
    std::vector<double> result;
    double average = 0, stdev = 0;
    int N = x.size();
    for(auto & i : x){
        average += i;
    }
    average /= (double) N;
    for(auto & i : x){
        stdev += pow(i - average, 2);
    }
    stdev /= (double) (N - 1);
    stdev = pow(stdev, 0.5);
    result.push_back(stdev);
    result.push_back(average);

    return result;
}

double HurstExponent(std::vector<double> stats, std::vector<double> returns){
    double result = 0;
    int N = returns.size();

    double csum = 0;
    std::vector<double> Y;
    for(int i = 0; i < N; ++i){
        csum += returns[i];
        Y.push_back(csum - stats[1]);
    }

    std::sort(Y.begin(), Y.end());
    double minY = Y[0];
    double maxY = Y[Y.size() - 1];
    
    result = log((maxY - minY)/stats[0])/log(N);

    return result;
}

int main(){

    PyObject * ax = plt::chart2D(111);

    std::vector<std::string> stocks = {"SPY","NVDA","MSFT","AAPL","GOOGL","JPM","MS","AMZN"};
    std::map<std::string, double> priceData;
    std::map<std::string, std::vector<double>> history;

    std::thread WebSocket(Socket, stocks, std::ref(priceData));
    int limit = 2;
    bool checkOn = false;

    std::vector<double> xp, yp, ror, musd;
    std::vector<std::string> lp, color;
    

    while(true){
        int SN = stocks.size();
        int SO = priceData.size();
        if(SN <= SO){
            xp.clear();
            yp.clear();
            lp.clear();
            ror.clear();
            musd.clear();
            color.clear();
            plt::Clear3DChart(ax);
            for(auto & stock : stocks){
                std::cout << stock << std::endl;
                history[stock].push_back(priceData[stock]);
                if(history[stock].size() > 1){
                    ror = RateOfReturn(history[stock]);
                    musd = MuSd(ror);
                    double hexp = HurstExponent(musd, ror);
                    xp.push_back(musd[0]);
                    yp.push_back(musd[1]);
                    if(hexp > 0.5){
                        lp.push_back(stock + " Trending");
                        color.push_back("green");
                    } else if(hexp < 0.5) {
                        lp.push_back(stock + " Mean-Reverting");
                        color.push_back("blue");
                    } else {
                        lp.push_back(stock + " Effecient");
                        color.push_back("red");
                    }
                }
            }
            
            for(int k = 0; k < lp.size(); ++k){
                plt::scatter2DX(ax, xp[k], yp[k], color[k]);
                plt::annotateGraph(ax, lp[k], xp[k], yp[k]);
            }
            plt::pause(2);
        } else {    
            std::cout << SN - SO << " left to load" << std::endl;
            
        }
    }

    WebSocket.join();
    return 0;
}