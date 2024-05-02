#include <stdio.h>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <random>

#ifndef cell_cpp
#define cell_cpp

#define NESTLE               1
#define DONT_NESTLE          2
#define MIG_SOON             3
#define MIG_LATE             4

using namespace std;

class Cell{
    public:
        bool mig;
        bool occupied;
        int x_pos;
        int y_pos;
        int lifeYear = 0;

        virtual ~Cell(){};
};

class Habitable : public Cell{
    public:
        Habitable(int x, int y){
            x_pos = x;
            y_pos = y;
        }
};

class NonMigrating : public Cell{
    public:
        NonMigrating(int x, int  y){
            x_pos = x;
            y_pos = y;
        }

        ~NonMigrating(){};

        int nestle_in(float warming_raise){
            random_device device;
            mt19937 generator(device());

            uniform_real_distribution<double> distribution(0.0, 1.0);
            double random_nestle = distribution(generator);

            // Birds have 60% chance that nesting will be succesfull
            if(random_nestle < (0.6 - warming_raise)){
                return NESTLE;
            }else{
                return DONT_NESTLE;
            }
        }
};

class Migrating : public Cell {
    public:
        bool nestle;

        Migrating(int x, int y){
            x_pos = x;
            y_pos = y;
        }

        ~Migrating(){};
        
        int migrate(float warming_raise){
            random_device device;
            mt19937 generator(device());

            uniform_real_distribution<double> distribution(0.0, 1.0);
            double random_migrate = distribution(generator);

            // If bird will start migrating soon they will very propably die
            if(random_migrate < 0.05 + warming_raise){
                return MIG_SOON;
            }
            // Optimal migration start, parent bird will return to their original spot
            else if(random_migrate >= (0.05 + warming_raise) and random_migrate < (0.95 - warming_raise)){
                double random_nestle = distribution(generator);
                
                if(random_nestle < (0.6 - warming_raise)){
                    return NESTLE;
                }else{
                    return DONT_NESTLE;
                }
            }
            // If bird will start migrating too lately they will also return later and there is high chance that there will be no optimal habitats for them
            // so they will be forced to move somewhere else
            else{
                return MIG_LATE;
            }
        }
};

#endif