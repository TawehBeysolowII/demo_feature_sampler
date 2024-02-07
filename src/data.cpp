#include <iostream>
#include <string> 
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include "../header_files/data.hpp" 

using namespace std; 
using json = nlohmann::json; 

double DataUtilities::logistic_regression_prediction(vector<double> model_coefficients, json observation) {

    vector<double> observation_values; 
    double linear_sum = 0; 

    for (json::iterator it = observation.begin(); it != observation.end(); ++it) { 

        observation_values.push_back(*it);

    }

    for (int i = 0; i < model_coefficients.size(); i++) { 
        
        linear_sum += model_coefficients[i]*observation_values[i]; 

    }

    if (1/(1 - exp(linear_sum)) >= .5){ 
        return 1;
    } else { 
        return 0;
    }
}
