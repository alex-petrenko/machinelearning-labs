#include <fstream>
#include <iomanip>
#include <iostream>
#include <algorithm>

#include <opencv2/opencv.hpp>


namespace
{
    const int numOfClusters = 10,
              numOfSamples = 2000,
              kFold = 15;
}


struct Neighbor
{
    Neighbor()
    {
    }

    Neighbor(double dist, int idx)
    :   dist(dist)
    ,   idx(idx)
    {
    }

    bool operator<(const Neighbor &neighbor) const
    {
        return dist != neighbor.dist ? dist < neighbor.dist : idx < neighbor.idx;
    }

    double dist;
    int idx;
};


void loadSamples(const std::string &path, std::vector<cv::Mat> &samples);

void calcNeigbors(const std::vector<cv::Mat> &testSamples,
                  const std::vector<cv::Mat> &trainSamples,
                  std::vector<std::vector<Neighbor>> &neighbors);

double evaluate(int k,
                const std::vector<std::vector<Neighbor>> &distances,
                const std::vector<int> &labels);


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <datasetPath>" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string datasetPath(argv[1]),
                      testPath(datasetPath + "/test"),
                      trainPath(datasetPath + "/train"),
                      labelsPath(trainPath + "/labels.txt"),
                      outLabelsPath(testPath + "/outlabels.txt");

    std::vector<cv::Mat> trainSamples(numOfSamples), testSamples(numOfSamples);
    std::vector<int> labels(numOfSamples), outLabels(numOfSamples);

    loadSamples(trainPath, trainSamples);
    loadSamples(testPath, testSamples);

    std::ifstream labelsFile(labelsPath);
    for (int i = 0; i < numOfSamples; ++i)
        labelsFile >> labels[i];

    std::vector<std::vector<Neighbor>> neighborsTrain(numOfSamples,
                                                      std::vector<Neighbor>(numOfSamples));
    calcNeigbors(trainSamples, trainSamples, neighborsTrain);

    const int minK = 1, maxK = numOfSamples / 2;
    std::vector<double> errorRates(maxK);

#pragma omp parallel for
    for (int k = minK; k <= maxK; ++k)
        errorRates[k] = evaluate(k, neighborsTrain, labels);

    auto minErrorRate = std::min_element(errorRates.begin() + 1, errorRates.end());
    int bestK = std::distance(errorRates.begin(), minErrorRate);

    std::cout << "Best value of K is: " << bestK << std::endl;
    std::cout << "With error rate of: " << *minErrorRate * 100 << "%" << std::endl;

    std::vector<std::vector<Neighbor>> neighborsTest(numOfSamples,
                                                     std::vector<Neighbor>(numOfSamples));
    calcNeigbors(testSamples, trainSamples, neighborsTest);

#pragma omp parallel for
    for (int i = 0; i < numOfSamples; ++i)
    {
        std::vector<int> voting(numOfClusters);
        for (int j = 0; j < bestK; ++j)
            ++voting[labels[neighborsTest[i][j].idx]];
        outLabels[i] = std::distance(voting.begin(), std::max_element(voting.begin(), voting.end()));
    }

    std::ofstream outLabelsFile(outLabelsPath);
    for (auto label : outLabels)
        outLabelsFile << label << std::endl;

    return EXIT_SUCCESS;
}

void loadSamples(const std::string &path, std::vector<cv::Mat> &samples)
{
    for (int i = 0; i < numOfSamples; ++i)
    {
        std::ostringstream sampleName(path + "/", std::ios::ate);
        sampleName << "image_" << std::setw(5) << std::setfill('0') << i << ".pgm";
        samples[i] = cv::imread(sampleName.str());
    }
}

void calcNeigbors(const std::vector<cv::Mat> &testSamples,
                  const std::vector<cv::Mat> &trainSamples,
                  std::vector<std::vector<Neighbor>> &neighbors)
{
#pragma omp parallel for
    for (int i = 0; i < numOfSamples; ++i)
    {
        for (int j = 0; j < numOfSamples; ++j)
            neighbors[i][j] = Neighbor(cv::norm(trainSamples[j], testSamples[i]), j);
        std::sort(neighbors[i].begin(), neighbors[i].end());
    }
}

double evaluate(int k,
                const std::vector<std::vector<Neighbor>> &neighbors,
                const std::vector<int> &labels)
{
    int numOfErrors = 0;

    for (int fold = 0; fold < kFold; ++fold)
    {
        for (int i = fold; i < numOfSamples; i += kFold)
        {
            int numNeighbors = 0;
            std::vector<int> voting(numOfClusters);
            for (int j = 0; numNeighbors < k; ++j)
            {
                const Neighbor &neigbor = neighbors[i][j];
                if (neigbor.idx % kFold == fold)
                    continue;

                ++numNeighbors;
                ++voting[labels[neigbor.idx]];
            }

            int label = std::distance(voting.begin(),
                                      std::max_element(voting.begin(), voting.end()));
            if (label != labels[i])
                ++numOfErrors;
        }
    }

    int numSamplesPerStep = static_cast<double>(numOfSamples) / kFold;
    return (static_cast<double>(numOfErrors) / kFold) / numSamplesPerStep;
}
