#pragma once

#include <iostream>
#include <string>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp> 
#include <nlohmann/json.hpp>
#include <fstream>
#include "data.hpp"

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

class WebsocketUtilities { 

    public:

        static void on_open(client* c, 
                            string* sub_msg, 
                            websocketpp::connection_hdl hdl); 

        static void on_fail(client* c, 
                            websocketpp::connection_hdl hdl); 

        static void on_message(client* c, 
                               BlockingQueue<json>* q, 
                               string* vm, 
                               websocketpp::connection_hdl hdl, 
                               message_ptr msg); 

        static void on_close(client* c, 
                             websocketpp::connection_hdl hdl);

        static websocketpp::lib::shared_ptr<SslContext> on_tls_init();

        static void start_websocket(json config, 
                                    BlockingQueue<json>* trade_message_queue, 
                                    client* websocket_client);

        static void create_dollar_bars(client* c, 
                                        BlockingQueue<json>* q, 
                                        double* nc, 
                                        double* dbs, 
                                        json* observation); 



};

