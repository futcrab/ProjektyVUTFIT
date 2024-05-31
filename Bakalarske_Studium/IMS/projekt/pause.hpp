#include "packet.hpp"
#include "sortedpacket.hpp"
#include "facilities.hpp"
#include <simlib.h>

class Pause : public Process
{
    void Behavior()
    {
        Priority = 10;
        Sorter.Enter(this, nOfSorters);
        Docker.Enter(this, nOfDockers);
        Wait(30 * 60);
        Docker.Leave(nOfDockers);
        Sorter.Leave(nOfSorters);
    }

public:
    int nOfDockers = -1;
    int nOfSorters = -1;
};

class EndofWork : public Event
{
    void Behavior()
    {
        Priority = 10;
        Sorter.Enter(this, nOfSorters);
        Docker.Enter(this, nOfDockers);
        Passivate();
    }

public:
    int nOfDockers = -1;
    int nOfSorters = -1;
};