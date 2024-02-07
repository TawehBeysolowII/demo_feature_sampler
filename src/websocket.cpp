#include <iostream>
#include <string>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp> 
#include <nlohmann/json.hpp>
#include <fstream>
#include "../header_files/websocket.hpp"
#include "../header_files/data.hpp"

typedef websocketpp::client<websocketpp::config::asio_tls_client> client; 
typedef websocketpp::config::asio_tls_client::message_type::ptr message_ptr;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

using websocketpp::lib::placeholders::_1; 
using websocketpp::lib::placeholders::_2; 
using websocketpp::lib::placeholders::_3;
using websocketpp::lib::bind; 
using SslContext = websocketpp::lib::asio::ssl::context;

using namespace std;
using json = nlohmann::json;

void WebsocketUtilities::on_open(client* c, string* sub_msg, websocketpp::connection_hdl hdl){
    c->get_alog().write(websocketpp::log::alevel::app, "Sending connection message..."); 
    cout << *sub_msg << "\n";  
    c->send(hdl, *sub_msg, websocketpp::frame::opcode::text);
    c->get_alog().write(websocketpp::log::alevel::app, "Sent connection message!"); 
}

void WebsocketUtilities::on_fail(client* c, websocketpp::connection_hdl hdl) {
    c->get_alog().write(websocketpp::log::alevel::app, "Connection Failed");
}

void WebsocketUtilities::on_message(client* c, BlockingQueue<json>* q, string* vm, websocketpp::connection_hdl hdl, message_ptr msg) {
    
    if (json::parse(msg->get_payload())!= NULL 
    && json::parse(msg->get_payload()).contains(*vm)) {
        q->put(json::parse(msg->get_payload())); 
    } 
}

void WebsocketUtilities::on_close(client* c, websocketpp::connection_hdl hdl) {
    c->get_alog().write(websocketpp::log::alevel::app, "Connection Closed");
}


websocketpp::lib::shared_ptr<SslContext> WebsocketUtilities::on_tls_init() {
auto ctx = websocketpp::lib::make_shared<SslContext>(
    boost::asio::ssl::context::sslv23);
return ctx;
}


void WebsocketUtilities::start_websocket(json config, 
                    BlockingQueue<json>* trade_message_queue, 
                    client* websocket_client) {

    string subscribe_msg = config["subscribe_msg"].dump(); 
    string uri = config["uri"]; 
    string valid_message = config["trade_msg_format"]["size"];


    try {
        
        websocket_client->set_open_handler(bind(&on_open, websocket_client, &subscribe_msg, ::_1)); 
        websocket_client->set_fail_handler(bind(&on_fail, websocket_client, ::_1));
        websocket_client->set_message_handler(bind(&on_message, websocket_client, trade_message_queue, &valid_message, ::_1, ::_2));
        websocket_client->set_close_handler(bind(&on_close, websocket_client, ::_1));
        websocket_client->set_tls_init_handler(bind(&on_tls_init)); 

        // Start the ASIO io_service run loop
        websocket_client->init_asio(); 

        websocketpp::lib::error_code ec;
        client::connection_ptr connection = websocket_client->get_connection(uri, ec);
        websocket_client->connect(connection);

        if (ec) {
            websocket_client->get_alog().write(websocketpp::log::alevel::app, 
                    "Get Connection Error: "+ec.message());

        }

        websocket_client->run();

    } catch (const std::exception & e) {
        cout << e.what() << endl;
    } catch (websocketpp::lib::error_code e) {
        cout << e.message() << endl;
    } catch (...) {
        cout << "other exception" << endl;
    }

}

void WebsocketUtilities::create_dollar_bars(client* c, 
                                            BlockingQueue<json>* q, 
                                            double* nc, 
                                            double* dbs,
                                            json* observation) { 
    
    vector<json> trades;
    vector<json> raw_trades;
    vector<double> price; 
    double volume = 0; 
    vector<string> side; 
    json obs;
    json trade_data;
    string trade_size; 
    string _price;

    while (1) {
    
        if (*nc >= *dbs) { 
            
            obs["timestamp"] = time(0);
            obs["open"] = price[0]; 
            obs["high"] = *max_element(price.begin(), price.end());
            obs["low"] = *min_element(price.begin(), price.end());
            obs["close"] = price[price.size()-1]; 
            obs["n_trades"] = price.size();
            *observation = obs;

            raw_trades.clear();
            price.clear(); 
            side.clear();
            volume = 0; 
            *nc = 0; 

            c->get_alog().write(websocketpp::log::alevel::app, observation->dump()); 

        } else if (*nc < *dbs && q->size() > 0) { 
            
            trades = q->get_all_values();

            for (int i = 0; i < trades.size(); i++) { 
                
                trade_data = trades[i];
                raw_trades.push_back(trades[i]);
                trade_size = trade_data["last_size"]; 
                _price = trade_data["price"];
                price.push_back(stod(_price)); 
                volume += stod(trade_size)*stod(_price); 
                side.push_back(trade_data["side"]); 
                *nc += stod(trade_size)*stod(_price);

            }

            c->get_alog().write(websocketpp::log::alevel::app, 
            "N Trades: " + to_string(raw_trades.size()) 
            + ", " + "Count: " + to_string(*nc) 
            + ", Dollar Bar Size: " + to_string(*dbs)); 

        }
        
    }


}



