#include <fstream>
#include <iostream>

#include <person.hpp>


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <datasetPath>" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string datasetPath(argv[1]),
                      trainFile(datasetPath + "/train.csv");

    std::ifstream input(trainFile);
    std::string line;
    std::getline(input, line);  // read first line with csv format description

    while (std::getline(input, line))
    {
        Person person(line);
    }

    return EXIT_SUCCESS;
}
