#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm> 

class SenseAmplifier {
public:
    double width, length, supplyVoltage, inputVoltage, currentDrive;
    SenseAmplifier(double w, double l, double vdd, double vin, double id)
        : width(w), length(l), supplyVoltage(vdd), inputVoltage(vin), currentDrive(id) {}
    double evaluate() const {
        double power = supplyVoltage * currentDrive;
        double speed = (width * length) / (currentDrive * supplyVoltage); 
        double noiseMargin = (width / length) - (currentDrive / supplyVoltage);
        double area = width * length;
        double cost = power + 1.0 / speed + (100.0 / noiseMargin) + area * 0.0001; 
        return cost;
    }
};

class DEOptimizer {
public:
    int populationSize;
    int dimensions;
    double crossoverRate;
    double mutationFactor;
    int generations;
    std::vector<SenseAmplifier> population;
    DEOptimizer(int popSize, int dims, double crossover, double mutation, int gens)
        : populationSize(popSize), dimensions(dims), crossoverRate(crossover),
          mutationFactor(mutation), generations(gens) {
        srand(time(0));  
        }
    void initializePopulation() {
        for (int i = 0; i < populationSize; ++i) {
            double width = 100 + rand() % 1000;  
            double length = 50 + rand() % 500;   
            double supplyVoltage = 0.9 + static_cast<double>(rand()) / RAND_MAX * 0.3; 
            double inputVoltage = 0.0 + static_cast<double>(rand()) / RAND_MAX * 1.2;  
            double currentDrive = 10 + rand() % 100; 
            SenseAmplifier sa(width, length, supplyVoltage, inputVoltage, currentDrive);
            population.push_back(sa);
        }
    }
    SenseAmplifier mutate(const SenseAmplifier &target, int idx1, int idx2, int idx3) {
        SenseAmplifier &p1 = population[idx1];
        SenseAmplifier &p2 = population[idx2];
        SenseAmplifier &p3 = population[idx3];
        double newWidth = target.width + mutationFactor * (p1.width - p2.width);
        double newLength = target.length + mutationFactor * (p1.length - p3.length);
        double newSupplyVoltage = target.supplyVoltage + mutationFactor * (p2.supplyVoltage - p3.supplyVoltage);
        double newInputVoltage = target.inputVoltage + mutationFactor * (p1.inputVoltage - p3.inputVoltage);
        double newCurrentDrive = target.currentDrive + mutationFactor * (p2.currentDrive - p1.currentDrive);
        return SenseAmplifier(newWidth, newLength, newSupplyVoltage, newInputVoltage, newCurrentDrive);
    }
    SenseAmplifier crossover(const SenseAmplifier &target, const SenseAmplifier &mutant) {
        double newWidth = target.width;
        double newLength = target.length;
        double newSupplyVoltage = target.supplyVoltage;
        double newInputVoltage = target.inputVoltage;
        double newCurrentDrive = target.currentDrive;
        if (static_cast<double>(rand()) / RAND_MAX < crossoverRate) {
            newWidth = mutant.width;
        }
        if (static_cast<double>(rand()) / RAND_MAX < crossoverRate) {
            newLength = mutant.length;
        }
        if (static_cast<double>(rand()) / RAND_MAX < crossoverRate) {
            newSupplyVoltage = mutant.supplyVoltage;
        }
        if (static_cast<double>(rand()) / RAND_MAX < crossoverRate) {
            newInputVoltage = mutant.inputVoltage;
        }
        if (static_cast<double>(rand()) / RAND_MAX < crossoverRate) {
            newCurrentDrive = mutant.currentDrive;
        }
        return SenseAmplifier(newWidth, newLength, newSupplyVoltage, newInputVoltage, newCurrentDrive);
    }
    void selectNewGeneration(int idx, const SenseAmplifier &candidate) {
        double candidateFitness = candidate.evaluate();
        double targetFitness = population[idx].evaluate();
        if (candidateFitness < targetFitness) {
            population[idx] = candidate;  
        }
    }
    void optimize() {
        initializePopulation();
        for (int gen = 0; gen < generations; ++gen) {
            for (int i = 0; i < populationSize; ++i) {
                int idx1, idx2, idx3;
                do { idx1 = rand() % populationSize; } while (idx1 == i);
                do { idx2 = rand() % populationSize; } while (idx2 == i || idx2 == idx1);
                do { idx3 = rand() % populationSize; } while (idx3 == i || idx3 == idx1 || idx3 == idx2);
                SenseAmplifier mutant = mutate(population[i], idx1, idx2, idx3);
                SenseAmplifier candidate = crossover(population[i], mutant);
                selectNewGeneration(i, candidate);
            }
            double bestFitness = population[0].evaluate();
            int bestIndex = 0;
            for (int i = 1; i < populationSize; ++i) {
                double fitness = population[i].evaluate();
                if (fitness < bestFitness) {
                    bestFitness = fitness;
                    bestIndex = i;
                }
            }
            std::cout << "Generation " << gen << " Best Fitness: " << bestFitness << std::endl;
        }
        double bestFitness = population[0].evaluate();
        int bestIndex = 0;
        for (int i = 1; i < populationSize; ++i) {
            double fitness = population[i].evaluate();
            if (fitness < bestFitness) {
                bestFitness = fitness;
                bestIndex = i;
            }
        }
        std::cout << "Optimized Best Fitness: " << bestFitness << std::endl;
        std::cout << "Best Parameters: " << std::endl;
        std::cout << "Width: " << population[bestIndex].width << " nm" << std::endl;
        std::cout << "Length: " << population[bestIndex].length << " nm" << std::endl;
        std::cout << "Supply Voltage: " << population[bestIndex].supplyVoltage << " V" << std::endl;
        std::cout << "Input Voltage: " << population[bestIndex].inputVoltage << " V" << std::endl;
        std::cout << "Current Drive: " << population[bestIndex].currentDrive << " uA" << std::endl;
    }
};


int main() {
    int populationSize = 50;        
    int dimensions = 5;             
    double crossoverRate = 0.9;     
    double mutationFactor = 0.8;    
    int generations = 100;        
    DEOptimizer optimizer(populationSize, dimensions, crossoverRate, mutationFactor, generations);
    optimizer.optimize();
    return 0;
}
