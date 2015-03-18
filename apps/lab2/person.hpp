#pragma once


#include <map>
#include <string>


class Person
{
public:
    Person(const std::string &csv);

public:
    int id;
    bool survived;

    std::map<std::string, double> numericalFeatures;
};
