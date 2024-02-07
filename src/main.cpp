#include <string> 
#include <thread>
#include <iostream>
#include "../header_files/websocket.hpp"
#include "../header_files/data.hpp"

int main(int argc, char* argv[]) { 
    
    string currency = argv[1]; 
    string exchange = argv[2];
    string bar_type = argv[3];

    cout << ".config/"+exchange+"_"+currency+".json" << endl;
    ifstream config_file("./config/"+exchange+"_"+currency+".json", ifstream::binary); 
    json config = json::parse(config_file); 
    
    int max_queue_size = config["max_queue_size"]; 
    double dollar_bar_size = config["dollar_bar_size"];

    cout << "Trade execution system\nInitial Params: \n"; 
    cout << "Dollar bar size: " + to_string(dollar_bar_size) << "\n"; 
    cout << "Currency: " << currency << "\n"; 
    cout << "Exchange: " << exchange << "\n"; 

    json current_trade;
    BlockingQueue<json> trade_message_queue(max_queue_size); 
    double notional_count = 0; 
    client websocket_client;
    vector<thread> _threads; 
    json observation; 

    cout << "Initializing websocket client and connecting to " + exchange + "...\n";

    thread t1(WebsocketUtilities::start_websocket, 
                                    config, 
                                    &trade_message_queue, 
                                    &websocket_client);   
    t1.detach(); 
    
    websocket_client.get_alog().write(websocketpp::log::alevel::app, "Starting queue management thread..."); 
    
    if (bar_type == "dollar") {

        thread t2(WebsocketUtilities::create_dollar_bars, 
                                        &websocket_client, 
                                        &trade_message_queue, 
                                        &notional_count, 
                                        &dollar_bar_size, 
                                        &observation);

        t2.join();

    }
    


}   

