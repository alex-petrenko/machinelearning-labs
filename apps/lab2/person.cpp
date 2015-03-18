#include <regex>
#include <sstream>
#include <iostream>

#include <person.hpp>


void parseNumericalFeature(std::istringstream &input, double &feature)
{
    std::string value;
    std::getline(input, value, ',');
    std::istringstream(value) >> feature;
}


Person::Person(const std::string &csv)
{
    std::string s("there is a subsequence in the string\n");
    try{
    std::regex e("\\b(sub)([^ ]*)");   // matches words beginning by "sub"
        std::string replaced = std::regex_replace (s,e,std::string("$1-$2"));
    }catch(std::regex_error ex){
        std::cerr <<ex.what();
    }

//    std::istringstream values(csv);
//    std::string token;
//    const char comma = ',';

//    std::getline(values, token, comma);
//    std::istringstream(token) >> id;

//    std::getline(values, token, comma);
//    std::istringstream(token) >> survived;

//    parseNumericalFeature(values, numericalFeatures["pclass"]);

//    std::getline(values, token, comma);  // name
//    std::getline(values, token, comma);  // sex

//    parseNumericalFeature(values, numericalFeatures["age"]);
//    parseNumericalFeature(values, numericalFeatures["sibsp"]);
//    parseNumericalFeature(values, numericalFeatures["parch"]);
}
