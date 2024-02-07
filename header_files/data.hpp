#pragma once

#include <iostream>
#include <queue>
#include <string> 
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace std; 
using json = nlohmann::json; 

template<typename T>
class BlockingQueue {

    private:
        mutex _mutex;
        queue<T> private_std_queue;
        condition_variable condNotEmpty;
        condition_variable condNotFull;
        int count; // Guard with Mutex
        int max_queue_size = 10000;
        int MAX{max_queue_size}; 

    public:

        BlockingQueue(int queue_size) { 

            max_queue_size = queue_size;

        }

        int size() {

            return count; 
        }

        T front() { 

            return private_std_queue.front();
        }

        T back() { 

            return private_std_queue.back(); 

        }

        void pop() { 

            private_std_queue.pop();
            count--; 

        }

        void put(T new_value) {

            unique_lock<mutex> _lock(_mutex);
            //Condition takes a unique_lock and waits given the false condition
            condNotFull.wait(_lock,[this]{
                if (count == MAX) {
                    return false;
                } else {
                    return true;
                }
                
            });

            private_std_queue.push(new_value);
            count++;
            condNotEmpty.notify_one();
        }

        void take(T& value) {
            unique_lock<mutex> _lock(_mutex);
            //Condition takes a unique_lock and waits given the false condition
            condNotEmpty.wait(_lock,[this]{return !private_std_queue.empty();});
            value=private_std_queue.front();
            private_std_queue.pop();
            count--;
            condNotFull.notify_one();
        }

        vector<T> get_all_values() { 

            vector<T> output; 

            for (int i = 0; i < private_std_queue.size(); i++){ 

                output.push_back(private_std_queue.back()); 
                private_std_queue.pop();
                count--;
                condNotFull.notify_one();

            }

            return output;

        }


};

class DataUtilities { 

    public:

        static double logistic_regression_prediction(vector<double> model_coefficients, 
                                                            json observation); 


}; 
