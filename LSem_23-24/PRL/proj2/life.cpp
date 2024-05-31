//Impelmentácia s pevnými stenami

#include <mpi.h> // OpenMPI
#include <vector> // std::vector
#include <fstream> // reading files

// Macros for identifying position of a core
#define FIRST 0
#define MIDDLE 1
#define LAST 2

// Vector for saving first state from file
std::vector<std::vector<int>> firstState;

bool readFirstState(char* filename){
    //Open file with first step
    std::ifstream file(filename);
        
    //check if properly opened
    if (!file.is_open()) {
        std::cerr << "Error while opening firstState file" << std::endl;
        return false;
    }else{
        std::string line;
    
        // Read line by line
        while(std::getline(file, line)){
            std::vector<int> boolLine;
            // based on char value push integer to vector
            for(char c: line){
                if(c == '0'){
                    boolLine.push_back(0);
                }else if (c == '1'){
                    boolLine.push_back(1);
                }
            }
            // when full line is read push it to first state vector
            firstState.push_back(boolLine);
        }
        file.close();
    }

    return true;
}

void sendFirstState(int nOfProcessors, int sizeOfTable){
    int nOfLines;
    int sendLines = 0;
    // Loop for every core
    for(int i = 1; i < nOfProcessors + 1; i++){
        // Get nOfLines from procesor
        MPI_Recv(&nOfLines, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Send requested lines back
        for(int j = sendLines; j < sendLines + nOfLines; j++){
            MPI_Send(firstState[j].data(), sizeOfTable, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Keep count of send lines to know which ones to send next
        sendLines += nOfLines;
    }
}

void receiveFirstState(std::vector<std::vector<int>>& myLines, int nOfLines, int sizeOfTable){
    // Send nOfLines to first core for it to know how many to send
    MPI_Send(&nOfLines, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    
    // Insert empty first line 
    myLines.emplace_back();
    myLines.back().assign(sizeOfTable, 0);
    
    // Get lines from first core 
    for(int i = 0; i < nOfLines; i++){
        std::vector<int> line;
        line.assign(sizeOfTable, 0);
        
        MPI_Recv(line.data(), sizeOfTable, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        myLines.push_back(line);
    }

    // Insert empty last line
    myLines.emplace_back();
    myLines.back().assign(sizeOfTable, 0);

    // Empty lines are used to save neighbouring lines from other cores
}

void receiveLine(int* line, int src, int size){
    MPI_Recv(line, size, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void sendLine(int* line, int dst, int size){
    MPI_Send(line, size, MPI_INT, dst, 0, MPI_COMM_WORLD);
}

void runLife(std::vector<std::vector<int>>& myLines, int steps, int position, int rank, int sizeOfTable, int nOfLines){
    // Vector for new calculated values
    std::vector<std::vector<int>> newValues(myLines);
    
    // for loop trough steps
    for(int step = 0; step < steps; step++){
        // If not first, exchange lines with lesser rank
        if(position != FIRST){
            receiveLine(myLines[0].data(), rank - 1, sizeOfTable);
            sendLine(myLines[1].data(), rank - 1, sizeOfTable);
        }
        // If not last, exchange lines with greater rank
        if(position != LAST){
            sendLine(myLines[nOfLines].data(), rank + 1, sizeOfTable);
            receiveLine(myLines.back().data(), rank + 1, sizeOfTable);
        }
        // Loop trough each cell on my lines
        for(int i = 1; i <= nOfLines; i++){
            for(int j = 0; j < sizeOfTable; j++){
                int sum = 0;
                int val = myLines[i][j];

                // Loop trough all cells in Moore neighborhood
                for(int y = -1; y < 2; y++){
                    for(int x = -1; x < 2; x++){
                        // if at edge ignore pixel
                        if((j + x == -1) || (j + x == sizeOfTable)){
                            sum += 0;
                        }else{
                            sum += myLines[i + y][j + x];
                        }
                    }
                }
                // if live cell
                if(val == 1){
                    sum--; // Ignore self in sum
                    if((sum < 2) || (sum > 3)){
                        newValues[i][j] = 0;
                    }else{
                        newValues[i][j] = 1;
                    }
                // if dead cell
                }else{
                    if(sum == 3){
                        newValues[i][j] = 1;
                    }else{
                        newValues[i][j] = 0;
                    }
                }
            }
        }
        // copy new calculated cells to myLines
        std::copy(newValues.begin(), newValues.end(), myLines.begin());
    }
}

void sendFinalState(std::vector<std::vector<int>>& myLines, int nOfLines, int sizeOfTable){
    // Send number of lines to first core
    MPI_Send(&nOfLines, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    // Send individual lines
    for(int i = 1; i <= nOfLines; i++){
        MPI_Send(myLines[i].data(), sizeOfTable, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
}

void receiveFinalState(int sizeOfTable, int nOfProcessors){
    int nOfLines;

    std::vector<int> line;
    line.assign(sizeOfTable, 0);

    //Loop trough every procesor
    for(int i = 1; i <= nOfProcessors; i++){
        // Get nOfLines to know how many to expect from computing core
        MPI_Recv(&nOfLines, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int j = 0; j < nOfLines; j++){
            // Save line from computing core
            MPI_Recv(line.data(), sizeOfTable, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // Print line to stdout
            std::cout << i << ": ";
            for(int k = 0; k < sizeOfTable; k++){
                std::cout << line[k];
            }
            std::cout << std::endl;
        }
    }
}

// My implementation:
// 1. First core reads input file
// 2. Other (computing) cores calculate how much lines from grid they want based on their rank
// 3. First core sends lines to computing cores
// 4. At the beginning of step in game of life neighbouring cores send first and last lines between them
// 5. Calculate given steps
// 6. Computing cores send final grid to first core
// 7. First core prints result
int main(int argc, char** argv){
    // Init of MPI with declaration of variables for rank and size of cores
    MPI_Init(&argc, &argv);
    int rank, size;

    // control variables
    int position, sizeOfTable, steps, nOfLines;
    bool run = false;
    
    // setting rank and size for all cores
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Computing cores calculate their position
    if(rank != 0){
        if(rank == 1){
            position = FIRST;
        }else if(rank == size - 1){
            position = LAST;
        }else{
            position = MIDDLE;
        }
    }

    // First core reads input state and sets control variables
    if (rank == 0){
        if(!readFirstState(argv[1])){
            MPI_Finalize();
            return 0;
        }else{
            sizeOfTable = firstState.size();
            steps = atoi(argv[2]);
            run = true;
        }
    }

    // Broadcast control variables to computing cores
    MPI_Bcast(&run, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
    MPI_Bcast(&sizeOfTable, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&steps, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // If file was read succesfully first rank start sending lines to computing cores
    if((rank == 0) && run){
        sendFirstState(size - 1, sizeOfTable);
    }

    // If not first rank and file read succesfully
    if((rank != 0) && run){
        // calculate number of lines
        // if not last: nOfLines = (int) number of rows / number of procesors
        // if last: rest of the lines that earlier cores didnt get
        nOfLines = (int) sizeOfTable / (size-1);
        if(position == LAST){
            nOfLines = sizeOfTable - ((size - 2) * nOfLines);
        }
        // Create vector for computing core lines
        std::vector<std::vector<int>> myLines;
        
        // Receive first state from first rank
        receiveFirstState(myLines, nOfLines, sizeOfTable);
        
        // run life simulation
        runLife(myLines, steps, position, rank, sizeOfTable, nOfLines);
        
        // send final state back to first core
        sendFinalState(myLines, nOfLines, sizeOfTable);
    }

    if((rank == 0) && run){
        // receive final state from computing cores
        receiveFinalState(sizeOfTable, size - 1);
    }



    //end core
    MPI_Finalize();
}