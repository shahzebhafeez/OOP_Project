#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cpprest/ws_client.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <thread>
#include <algorithm>
#include <chrono>
#include "matplotlibcpp.h"
#include <Python.h>

using namespace web;
using namespace web::websockets::client;
using namespace boost::property_tree;
namespace plt = matplotlibcpp;

class datafeed {
    private:
        ptree JSON(std::string message){
            std::stringstream ss(message);
            ptree result;
            read_json(ss, result);
            return result;
        }

        void CYCLONE(ptree df, std::map<double, double> & bids, std::map<double, double> & asks){
            bool snapshot = false;
            bool l2update = false;
            for(ptree::const_iterator it = df.begin(); it != df.end(); ++it){
                if(l2update == true && it->first == "changes"){
                    for(ptree::const_iterator jt = it->second.begin(); jt != it->second.end(); ++jt){
                        std::vector<std::string> hold;
                        for(ptree::const_iterator kt = jt->second.begin(); kt != jt->second.end(); ++kt){
                            hold.push_back(kt->second.get_value<std::string>().c_str());
                        }

                        double price = atof(hold[1].c_str());
                        double volume = atof(hold[2].c_str());

                        if(hold[0] == "buy"){
                            if(volume == 0){
                                bids.erase(price);
                            } else {
                                bids[price] = volume;
                            }
                        } else {
                            if(volume == 0){
                                asks.erase(price);
                            } else {
                                asks[price] = volume;
                            }
                        }
                    }
                }
                if(snapshot == true && it->first == "bids"){
                    for(ptree::const_iterator jt = it->second.begin(); jt != it->second.end(); ++jt){
                        std::vector<double> hold;
                        for(ptree::const_iterator kt = jt->second.begin(); kt != jt->second.end(); ++kt){
                            hold.push_back(atof(kt->second.get_value<std::string>().c_str()));
                        }
                        bids[hold[0]] = hold[1];
                    }
                }
                if(snapshot == true && it->first == "asks"){
                    for(ptree::const_iterator jt = it->second.begin(); jt != it->second.end(); ++jt){
                        std::vector<double> hold;
                        for(ptree::const_iterator kt = jt->second.begin(); kt != jt->second.end(); ++kt){
                            hold.push_back(atof(kt->second.get_value<std::string>().c_str()));
                        }
                        asks[hold[0]] = hold[1];
                    }
                }

                if(it->first == "type"){
                    if(it->second.get_value<std::string>() == "l2update"){
                        l2update = true;
                    }
                    if(it->second.get_value<std::string>() == "snapshot"){
                        snapshot = true;
                    }
                }
            }
        }

        

    public:
        
        static void Socket(datafeed dx, std::map<double, double> & bids, std::map<double, double> & asks){
            std::string url = "wss://ws-feed.exchange.coinbase.com";
            std::string msg = "{\"type\":\"subscribe\",\"product_ids\":[\"BTC-USD\"],\"channels\":[\"level2_batch\"]}";

            websocket_client client;
            client.connect(url).wait();
            websocket_outgoing_message outmsg;
            outmsg.set_utf8_message(msg);
            client.send(outmsg);

            while(true){
                client.receive().then([](websocket_incoming_message inmsg){
                    return inmsg.extract_string();
                }).then([&](std::string message){
                    dx.CYCLONE(dx.JSON(message), std::ref(bids), std::ref(asks));
                }).wait();
            }

            client.close().wait();
        
        }
};

std::map<std::string, std::vector<double>> Extract(std::map<double, double> bids, std::map<double, double> asks)
{
    int depth = 60;
    std::map<std::string, std::vector<double>> result;

    int count = 0;
    double bidvol = 0;
    for(auto it = bids.rbegin(); it != bids.rend(); ++it){
        bidvol += it->second;
        result["bidPrice"].push_back(it->first);
        result["bidSize"].push_back(bidvol);
        count += 1;
        if(count >= depth){
            break;
        }
    }

    count = 0;
    double askvol = 0;
    for(auto it = asks.begin(); it != asks.end(); ++it){
        askvol += it->second;
        result["askPrice"].push_back(it->first);
        result["askSize"].push_back(askvol);
        count += 1;
        if(count >= depth){
            break;
        }
    }

    std::reverse(result["bidPrice"].begin(), result["bidPrice"].end());
    std::reverse(result["bidSize"].begin(), result["bidSize"].end());
    

    return result;
}

double order_bid_ask_mid(std::map<std::string, std::vector<double>> X){
    int n = X["bidPrice"].size();
    int j = 0;
    double bidP = 0, bidV = 0, askP = 0, askV = 0;
    for(int i = n - 1; i >= 0; --i){
        bidP += X["bidPrice"][i]*X["bidSize"][i];
        bidV += X["bidSize"][i];
        askP += X["askPrice"][j]*X["askSize"][j];
        askV += X["askSize"][j];
        j += 1;
    }

    return 0.5*(bidP/bidV + askP/askV);
}

std::map<std::string, std::vector<double>> frequency(std::vector<double> x){
    std::map<std::string, std::vector<double>> result;
    int bins = 20;
    std::sort(x.begin(), x.end());
    double x0 = x[0];
    double x1 = x[x.size() - 1];
    double dX = (x1 - x0)/((double) bins);
    double count = 0;
    for(int i = 0; i < bins; ++i){
        double a = x0 + i*dX;
        double b = x0 + (i+1)*dX;
        count = 0;
        for(int j = 0; j < x.size(); ++j){
            if(i == bins - 1){
                if(x[j] >= a && x[j] <= b){
                    count += 1;
                }
            } else {
                if(x[j] >= a && x[j] < b){
                    count += 1;
                }
            }
        }
        result["x"].push_back((a + b)/2.0);
        result["y"].push_back(count);
    }
    return result;
}

int main()
{
    PyObject * ax = plt::chart2D(111);

    std::map<double, double> bids, asks;
    std::map<std::string, std::vector<double>> preprice, z;

    datafeed wsfeed;
    std::thread feed(wsfeed.Socket, wsfeed, std::ref(bids), std::ref(asks));

    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::vector<double> historical;
    std::string position = "neutral";
    double lastPrice = 0;

    while(true){
        preprice = Extract(bids, asks);
        double midPrice = order_bid_ask_mid(preprice);
        double bid = preprice["bidPrice"][preprice["bidPrice"].size() - 1];
        double ask = preprice["askPrice"][0];

        if(ask > midPrice && position == "long"){
            double spread = ask - lastPrice;
            std::cout << "Spread: " << spread << std::endl;
            historical.push_back(spread);
            position = "neutral";
        }

        if(bid < midPrice && position == "neutral"){
            lastPrice = bid;
            position = "long";
        }

        if(historical.size() > 3){
            z = frequency(historical);
            plt::Clear3DChart(ax);
            plt::bar2D(ax, z["x"], z["y"], "blue");
            plt::pause(0.0001);
        }

    }

    plt::show();
    feed.join();
    return 0;
}
