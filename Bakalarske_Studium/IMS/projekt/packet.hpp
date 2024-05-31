#ifndef PACKET_HPP
#define PACKET_HPP

#include "sortedpacket.hpp"
#include "facilities.hpp"
#include <simlib.h>

#define VAN_CAPACITY 300

float DockUnload;
float SorterTime;

class Packet : public Process
{
    void Behavior()
    {
        Docker.Enter(this, 1);
        Wait(Exponential(30));
        Docker.Leave(1);

        Seize(RewriteDockUnload);
        DockUnload = (float)(Time / 60 / 60);
        Release(RewriteDockUnload);

        Sorter.Enter(this, 1);
        Wait(Exponential(30));
        Sorter.Leave(1);

        Seize(RewriteSorter);
        SorterTime = (float)(Time / 60 / 60);
        Release(RewriteSorter);

        Seize(checkPacked);
        if ((int)queueSorted.Length() == nOfPackets - 1)
        {
            while (!queueSorted.Empty())
            {
                queueSorted.GetFirst()->Cancel();
            }
            for (int i = 0; i < (nOfPackets / VAN_CAPACITY); i++)
            {
                (new SortedPackets)->Activate();
            }
            if (nOfPackets % VAN_CAPACITY != 0)
            {
                (new SortedPackets)->Activate();
            }
            Release(checkPacked);
            Cancel();
        }
        else
        {
            queueSorted.Insert(this);
        }
        Release(checkPacked);

        Passivate();
    }

public:
    int nOfPackets = -1;
};
#endif