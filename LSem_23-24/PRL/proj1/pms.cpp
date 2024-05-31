#include <mpi.h> // OpenMPI
#include <vector> // std::vector
#include <fstream> // reading files
#include <cmath> // for calculating power of number

// Macros for identifying position of a core
#define MIDDLE_RANK 1
#define LAST_RANK 2

std::vector<unsigned char> insertNumbers; // Vector for numbers to be sorted
std::vector<unsigned char> outNumbers; // Vector for sorted numbers

bool readNumbers(){
    //Open file numbers as binary
    std::ifstream file("numbers", std::ios::binary);
        
    //check if properly opened
    if (!file.is_open()) {
        std::cerr << "Error while reading numbers" << std::endl;
        return false;
    }else{
        // read from file byte by byte and push to in vector
        unsigned char number;

        while (file.read(reinterpret_cast<char*>(&number), sizeof(number))){
            insertNumbers.push_back(number);
        }
        file.close();
    }
    // Print out unsorted numbers
    for(int i = 0; i<insertNumbers.size(); i++){
        std::cout << +insertNumbers[i] << " ";
    }
    std::cout << std::endl;

    return true;
}

// print numbers from sorted vector line by line
void printSortedNumbers(){
    while(!(outNumbers.empty())){
        // reading from back because numbers are pushed to out vector from back (smallest numbers are last)
        std::cout << +outNumbers.back() << std::endl;
        outNumbers.pop_back();
    }
}

// function for rank 0 core
// read number from in vector and send to next rank
void processFirstRank(int rank, std::vector<unsigned char> in){
    unsigned char number;
    while(!in.empty()){
        number = in.front();

        MPI_Send(&number, 1, MPI_UNSIGNED_CHAR, rank+1, 0, MPI_COMM_WORLD);

        // after successfull send erase number from in vector
        in.erase(in.begin());
    }
}

// function for sending number to next rank and incrementing control variables
// if last rank send number to out vector
void sendNumberToNext(int dst, unsigned char value, int& sideVectorSend, int& sendNumbers, std::vector<unsigned char>& sideVector, int position){
    if(position == MIDDLE_RANK){
        MPI_Send(&value, 1, MPI_UNSIGNED_CHAR, dst, 0, MPI_COMM_WORLD);
    }else{
        outNumbers.push_back(value);
    }
    sideVectorSend++; // increment cortrol variable which checks how many number have been send from either queue in sequence (either leftSend of rightSend)
    sendNumbers++; // increment control variable which checks number of all send numbers
    sideVector.erase(sideVector.begin()); // erase first number from either vector (left or right)
}

// function for Other rank cores
void processOtherRank(int rank, int size, std::vector<unsigned char>& left, std::vector<unsigned char>& right, int inSequenceLength, int sizeOfNumbers){
    //control variables
    int receivedNumbers = 0, sendNumbers = 0; // for checking if i send or received all numbers
    unsigned char leftNumber = 0, rightNumber = 0; // for loading numbers from 2 insert queues
    int leftSend = 0, rightSend = 0; // for checking how much numbers where send from either side in sequence
    unsigned char recvData; // variable for receiving data
    bool sendData = false; // variable which is used to postpone sending data at the start when not enough numbers received
    int position = MIDDLE_RANK; // variable for determining position of core
    bool Side = true; // variable for selecting to which queue save the received number

    // if last rank change position to last rank
    if(rank == size - 1){
        position = LAST_RANK;
    }
    
    // this while end when all numbers have been sent
    while(sendNumbers != sizeOfNumbers){
        // if not all numbers received then receive next number
        if(receivedNumbers != sizeOfNumbers){
            // receive number from the previous core
            MPI_Recv(&recvData, 1, MPI_UNSIGNED_CHAR, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // check Side variable and save number to either queue based on it
            if(Side){
                left.push_back(recvData);
            }else{
                right.push_back(recvData);
            }

            // increment received numbers for if statement in next while cycle
            receivedNumbers++;

            // if whole sequence is send then change queue to which received number is send
            if(!(receivedNumbers % inSequenceLength)){
                Side = !Side;
            }
        }

        // if enough data is send from previous core start sending data to next core
        if ((left.size() == (size_t) inSequenceLength) && right.size() == 1){
            sendData = true;
        }

        // logic for determining from which queue number is send
        if(sendData){
            // check if left queue is empty
            // if not read from front if yes read load 0
            if (!left.empty()){
                leftNumber = left.front();
            }else{
                leftNumber = 0;
            }

            // same for right queue
            if (!right.empty()){
                rightNumber = right.front();
            }else{
                rightNumber = 0;
            }

            // check which number is larger
            if(leftNumber >= rightNumber){
                // if left number is larger and i havent already send all numbers from left sequence => send left number
                if (leftSend < inSequenceLength){
                    sendNumberToNext(rank+1, leftNumber, leftSend, sendNumbers, left, position);
                // if left number is larger but i already send all numbers from left sequence => send right number
                }else{
                    sendNumberToNext(rank+1, rightNumber, rightSend, sendNumbers, right, position);
                }
            // same but right number is bigger
            }else{
                if(rightSend < inSequenceLength){
                    sendNumberToNext(rank+1, rightNumber, rightSend, sendNumbers, right, position);
                }else{
                    sendNumberToNext(rank+1, leftNumber, leftSend, sendNumbers, left, position);
                }
            }
            // if all numbers from both sequence have been send reset values in control variables
            if ((leftSend == inSequenceLength) && (rightSend == inSequenceLength)){
                leftSend = rightSend = 0;
            }
        }
    }
    // after sending all numbers if last rank print sorted numbers
    if (position == LAST_RANK){
        printSortedNumbers();
    }
}

int main(int argc, char** argv){
    // Init of MPI with declaration of variables for rank and size of cores
    MPI_Init(&argc, &argv);
    int rank, size;
    
    // control variable declaration
    bool run = false;
    int sizeOfNumbers = 0;
    
    // setting rank and size for all cores
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // if first core
    if (rank == 0){
        //read numbers if error end program
        if(!readNumbers()){
            MPI_Finalize();
            return 0;
        }
        // if single number on input just print it and end
        if(insertNumbers.size() == 1){
            std::cout << +insertNumbers[0] << std::endl;
        }
        else{ // if everything went ok set run to true and set size of numbers
              // after that run function for rank 0
            run = true;
            sizeOfNumbers = insertNumbers.size();
            processFirstRank(rank, insertNumbers);
        }
    }
    // broadcast for run and size of number variable
    MPI_Bcast(&run, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
    MPI_Bcast(&sizeOfNumbers, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //if not first rank and run is true
    if (rank != 0 && run){
        //declare 2 input vectors and calculate length of in sequence
        std::vector<unsigned char> left, right;
        int inSequenceLenght = pow(2, rank-1);
        // run function for other ranks
        processOtherRank(rank, size, left, right, inSequenceLenght, sizeOfNumbers);
    }

    //end core
    MPI_Finalize();
}