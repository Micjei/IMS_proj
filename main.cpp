/**
 * @file main.cpp
 * @authors Matej Smida (xsmida06), Michael Babusik (xbabus01)
 * @brief 
 * @version 0.1
 * @date 2023-12-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "cell.cpp"
#include "grid.cpp"
#include <getopt.h>

#define OK 0
#define ERR_PARAMS -2

using namespace std;

/**
 * @brief Deallocks all allocated cells in grid
 * 
 * @param grid Grid of cells
 */
void cleanup(vector<vector<Cell*>> grid){
    for(auto& row : grid){
        for(Cell* cell : row){
            delete cell;
        }
    }
}

/**
 * @brief Checks if argument is unsigned int
 * 
 * @param argument Program input arguments
 * @return u_int Value
 */
u_int isInt(string argument){
    u_int val;

    try{
        for(u_int i = 0; i < argument.size(); i++){
            if(!isdigit(argument[i])){
                throw invalid_argument("Argument must be unsigned integer");
            }
        }

        val = stoi(argument);
    }
    catch(invalid_argument& e){
        cerr << "Error: " << e.what() << endl;
        exit(ERR_PARAMS);
    }
    catch(out_of_range& e){
        cerr << "Error: Index out of range while parsing arguments" << endl;
        exit(ERR_PARAMS);
    }
    catch(...){
        cerr << "Unknown error occured while parsing arguments" << endl;
        exit(ERR_PARAMS);
    }

    return val;
}

/**
 * @brief Creates grid and interates through it
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char* argv[]){
    u_int rows = 10;
    u_int cols = 10;
    float habit_ratio = 0.8;
    float population_density = 0.5;
    float warming_raise = 0.0;
    u_int years = 50;
    u_int output = 0;

    int opt;

    // Optional arguments
    struct option long_options[] = {
        {"row", required_argument, 0, 'R'},
        {"col", required_argument, 0, 'C'},
        {"habit", required_argument, 0, 'H'},
        {"popul", required_argument, 0, 'P'},
        {"years", required_argument, 0, 'Y'},
        {"output", required_argument, 0, 'O'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while((opt = getopt_long(argc, argv, "R:C:H:P:Y:O:h", long_options, NULL)) != -1){
        switch(opt){
            case 'R':
                rows = isInt(optarg);
                break;

            case 'C':
                cols = isInt(optarg);
                break;

            case 'H':
                habit_ratio = isInt(optarg);

                if(habit_ratio > 100){
                    cerr << "Value must be in range 0 - 100" << endl;
                    exit(ERR_PARAMS);
                }

                habit_ratio /= 100;
                break;

            case 'P':
                population_density = isInt(optarg);

                if(population_density > 100){
                    cerr << "Value must be in range 0 - 100" << endl;
                    exit(ERR_PARAMS);
                }
                population_density /= 100;
                break;
            
            case 'Y':
                years = isInt(optarg);
                break;
            
            case 'O':
                output = isInt(optarg);
                if(output != 0 and output != 1){
                    cout << "output must be either 0 for short output or 1 for full output" << endl;
                    exit(ERR_PARAMS);
                }

                break;

            case 'h':
                cout << "Simulation model tracking migratory and non-migratory birds in relation to global warming using cellular automaton." << endl
                << "arguments:" << endl
                << "    -R, --row:" << endl
                << "        Rows for grid of CA (default is 10)" << endl
                << "    -C, --col:" << endl
                << "        Collumns of grid (default is 10)" << endl
                << "    -H, --habit:" << endl
                << "        Percentage of how much of area can be inhabitable by birds. Number must be in range from 0 to 100 (default is 80%)" << endl
                << "    -P, --popul:" << endl
                << "        Percentage of how much of the area is actually populated by birds. Number must be in range from 0 to 100 (default is 50%)" << endl
                << "    -Y, --years:" << endl
                << "        How many years the simulation will take (default is 50)" << endl
                << "    -O, --output:" << endl
                << "        0 for short output or 1 for full output" << endl;
                exit(OK);

            case '?':
                cout << "Wrong argument. Try running the program with --help" << endl;
                exit(ERR_PARAMS);
        }
    }

    vector<vector<Cell*>> grid = generateGrid(rows, cols, habit_ratio, population_density);

    // Prints short output
    int print_years = 0;

    if(!output){
        printGrid(grid);
        cout << endl << "Year: " << print_years << endl
        << "_____________________________________________________________________" << endl;

        for(u_int i = 0; i < years; i++){
            iterateGrid(grid, warming_raise, rows, cols);

            warming_raise += 0.01;
            print_years++;
        }

        printGrid(grid);
        cout << endl << "Year: " << print_years << endl
        << "_____________________________________________________________________" << endl;

    }else{
        // Long output
        for(u_int i = 0; i < years; i++){
            printGrid(grid);
            cout << endl << "Year: " << print_years << endl
            << "_____________________________________________________________________" << endl;

            iterateGrid(grid, warming_raise, rows, cols);
            sleep(0.5);

            warming_raise += 0.01;
            print_years++;
        }
    }
    
    cleanup(grid);

    return 0;
}
