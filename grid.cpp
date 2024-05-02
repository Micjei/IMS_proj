#include "cell.cpp"
#include <unistd.h>
#include <vector>

using namespace std;

vector<Cell*> birds;
vector<Cell*> newBirds;

/**
 * @brief Generates grid and fills in randomly with migrating birds, non migrating birds, habitable and un habitable cells
 * 
 * @param rows Rows of grid
 * @param columns Columns of grid
 * @param habitable_ratio How many % of grid are habitable
 * @param population_density How many % will be populated with birds in the start
 * @return vector<vector<Cell*>> Grid of cells
 */
vector<vector<Cell*>> generateGrid(u_int rows, u_int columns, float habitable_ratio, float population_density){
    // Randomizing seed
    srand(static_cast<unsigned>(time(0)));

    vector<vector<Cell*>> grid(rows, vector<Cell*>(columns, 0));

    for(u_int i = 0; i < rows; ++i){
        for(u_int j = 0; j < columns; ++j){
            double random_habit = static_cast<double>(rand()) / RAND_MAX;
            double random_density = static_cast<double>(rand()) / RAND_MAX;
            double random_migrate = static_cast<double>(rand()) / RAND_MAX;

            if(random_habit < habitable_ratio){
                
                if(random_density < population_density){
                    // 50% chance that bird will either migrate or not
                    if(random_migrate < 0.5){
                        Migrating* bird = new Migrating(i, j);

                        bird->lifeYear = (rand() % 5) + 1;
                        bird->mig = true;
                        bird->occupied = true;

                        birds.push_back(bird);
                        grid[i][j] = bird;
                    }
                    else{
                        NonMigrating* bird = new NonMigrating(i, j);

                        bird->lifeYear = (rand() % 5) + 1;
                        bird->mig = false;
                        bird->occupied = true;

                        birds.push_back(bird);
                        grid[i][j] = bird;
                    }
                }else{
                    Habitable* habit = new Habitable(i, j);
                    habit->occupied = false;
                    grid[i][j] = habit;
                }
            }
        }
    }

    return grid;
}

/**
 * @brief Creates habitable cell in grid and deletes bird cell
 * 
 * @param grid Grid of cells
 * @param x X position of bird to delete
 * @param y Y position of bird to delete
 * @param bird pointer to bird cell
 */
void freeLand(vector<vector<Cell*>>& grid, int x, int y, Cell* bird){
    Habitable* habit = new Habitable(x, y);
    habit->occupied = false;

    grid[x][y] = habit;
    delete bird;
}

/**
 * @brief Searches for habitable cells in Moore surroundings
 * 
 * @param grid Grid of cells
 * @param x X position of bird
 * @param y Y position of bird
 * @param columns Columns of grid
 * @param rows Rows of grid
 * @return vector<vector<int>> vector of habitable positions in "infinite" Grid
 */
vector<vector<int>> searchGrid(vector<vector<Cell*>>& grid, int x, int y, int columns, int rows){
    vector<vector<int>> positions;

    for(int check_x = -1; check_x <= 1; ++check_x){
        for(int check_y = -1; check_y <= 1; ++check_y){
            int new_x = x + check_x;
            int new_y = y + check_y;

            if(new_x < 0){
                new_x = rows-1;
            }
            if(new_x == rows){
                new_x = 0;
            }
            if(new_y < 0){
                new_y = columns-1;
            }
            if(new_y == columns){
                new_y = 0;
            }

            if(grid[new_x][new_y] != nullptr and not grid[new_x][new_y]->occupied){
                vector<int> vect = {new_x, new_y};
                positions.push_back(vect);
            }
        }
    }

    return positions;
}

/**
 * @brief Populates habitable cell (if possible) in grid
 * 
 * @param grid Grid of cells
 * @param x X position of bird
 * @param y Y positions of bird
 * @param columns Columns of grid
 * @param rows Rows of grid
 * @param migrates Bool value of bird migrates
 * @param mig_bird pointer to migrating bird
 */
void occupyLand(vector<vector<Cell*>>& grid, int x, int y, int columns, int rows, bool migrates, Cell* mig_bird = NULL){
    
    random_device device;
    mt19937 generator(device());
    uniform_real_distribution<double> distribution(0.0, 1.0);

    if(!migrates){
        // Searches grid for habitable positions
        vector<vector<int>> positions = searchGrid(grid, x, y, columns, rows);

        if(positions.size() > 0){
            // Randomly selecting habitable place in Moore surroundings
            int random_pos = rand() % positions.size();
            vector<int> new_pos = positions[random_pos];
            Cell* cell_delete = grid[new_pos[0]][new_pos[1]];

            // Creating new bird
            NonMigrating* new_nonMig = new NonMigrating(new_pos[0], new_pos[1]);

            new_nonMig->mig = false;
            new_nonMig->occupied = true;

            grid[new_pos[0]][new_pos[1]] = new_nonMig;
            // Deleting original habitable cell
            delete cell_delete;

            newBirds.push_back(new_nonMig);
        }
    }
    else{
        vector<vector<int>> positions = searchGrid(grid, x, y, columns, rows);

        if(positions.size() > 0){
            // Parents are migrating back with their younglings
            int random_pos = rand() % positions.size();
            vector<int> new_pos = positions[random_pos];
            Cell *cell_delete = grid[new_pos[0]][new_pos[1]];

            Migrating* new_mig = new Migrating(new_pos[0], new_pos[1]);

            new_mig->mig = true;
            new_mig->occupied = true;

            grid[new_pos[0]][new_pos[1]] = new_mig;
            delete cell_delete;

            mig_bird->lifeYear++;
            newBirds.push_back(mig_bird);
            newBirds.push_back(new_mig);
        }
        else{
            // Random number in range 0 - 1
            double parent_leaves = distribution(generator);

            // 50% chance that parent will stay or leave their younglings if there is no habitable cell for younglings
            if(parent_leaves < 0.5){
                freeLand(grid, x, y, mig_bird);
            }
            else{
                mig_bird->lifeYear++;
                newBirds.push_back(mig_bird);
            }
        }
    }
}

/**
 * @brief Randomly shuffles List of birds in grid and performs their reproductive or migrating actions
 * 
 * @param grid Grid of cells
 * @param warming_raise Raising temperature increment
 * @param rows Rows of grid
 * @param columns Columns of grid
 */
void iterateGrid(vector<vector<Cell*>>& grid, float warming_raise, int rows, int columns){
    unsigned seed = chrono::steady_clock::now().time_since_epoch().count();

    shuffle(birds.begin(), birds.end(), default_random_engine(seed));

    for(Cell* cell : birds){
        if(cell->mig){
            Migrating* mig = dynamic_cast<Migrating*>(cell);

            int x = mig->x_pos;
            int y = mig->y_pos;

            // Bird is too old
            if(mig->lifeYear > 5){
                freeLand(grid, x, y, mig);
            }
            else{
                int mig_state = mig->migrate(warming_raise);

                if(mig_state == MIG_SOON){
                    freeLand(grid, x, y, mig);
                }
                else if(mig_state == NESTLE){
                    occupyLand(grid, x, y, columns, rows, true, mig);
                }
                else if(mig_state == DONT_NESTLE){
                    mig->lifeYear++;
                    newBirds.push_back(mig);
                }
                else if(mig_state == MIG_LATE){
                    freeLand(grid, x, y, mig);
                }        
            }

        }else{
            NonMigrating* nonMig = dynamic_cast<NonMigrating*>(cell);

            int x = nonMig->x_pos;
            int y = nonMig->y_pos;

            if(nonMig->lifeYear > 5){
                freeLand(grid, x, y, nonMig);
            }
            else{
                if(nonMig->nestle_in(warming_raise) == NESTLE){
                    occupyLand(grid, x, y, columns, rows, false);
                }

                nonMig->lifeYear++;
                newBirds.push_back(nonMig);
            }
        }
    }

    birds.clear();
    birds = newBirds;
    newBirds.clear();
}

/**
 * @brief Prints grid
 * 
 * @param grid Grid of cells
 */
void printGrid(vector<vector<Cell*>> grid){
    string grey = "\033[90m";
    string green = "\033[32m";
    string red = "\033[31m";
    string blue = "\033[34m";
    string reset = "\033[0m";

    for(const auto& row : grid){
        for(Cell* cell : row){
            if(dynamic_cast<NonMigrating*>(cell)){
                cout << blue << "N" << reset << " ";
            }

            else if(dynamic_cast<Migrating*>(cell)){
                cout << red << "M" << reset << " ";
            }

            else if(dynamic_cast<Habitable*>(cell)){
                cout << green << "O" << reset << " ";
            }

            else{
                cout << grey << "X" << reset << " ";
            }
        }

        cout << endl;
    }

    int mig = 0;
    int nonMig = 0;

    for(Cell* cell : birds){
        if(dynamic_cast<NonMigrating*>(cell)){nonMig++;}
        else if(dynamic_cast<Migrating*>(cell)){mig++;}
    }

    cout << endl
    << blue << "N" << reset << " : Non migrating birds | " << nonMig << endl
    << red << "M" << reset << " : Migrating birds     | " << mig << endl
    << green << "O" << reset << " : Habitable places" << endl
    << grey << "X" << reset << " : Non habitable places" << endl << endl;
}