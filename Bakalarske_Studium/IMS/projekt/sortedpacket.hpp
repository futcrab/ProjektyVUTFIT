#ifndef SORTEDPACKET_HPP
#define SORTEDPACKET_HPP

#include "packet.hpp"
#include "facilities.hpp"
#include <simlib.h>

#define ERROR 0.05

class ErrPacket : public Process
{
    void Behavior()
    {

        Wait(Exponential(20));

        ErrorPackets.Insert(this);

        Passivate();
    }
};
int VansReturned = 0;
int VansSent = 0;
float SumOfTimeAway = 0;

float DockLoad;
float VanSent;
float VanReturn;

class SortedPackets : public Process
{
    void Behavior()
    {
        Van.Enter(this, 1);

        Docker.Enter(this, 3);
        Wait(Exponential(20 * 60));
        Docker.Leave(3);

        Seize(RewriteDockLoad);
        DockLoad = (float)(Time / 60 / 60);
        Release(RewriteDockLoad);

        double leaveTime = Time;

        Seize(SentVan);
        VansSent++;
        VanSent = (float)(Time / 60 / 60);
        Release(SentVan);

        Wait(Uniform(2 * 60 * 60, 4 * 60 * 60));

        Van.Leave(1);

        Seize(ReturnedVan);
        VansReturned++;
        SumOfTimeAway += (float)((Time - leaveTime) / 60 / 60);
        VanReturn = (float)(Time / 60 / 60);
        Release(ReturnedVan);

        float rand = Random();
        if (rand <= ERROR)
        {
            for (int i = 0; i < (int)(1 + Random() * 40); i++)
            {
                (new ErrPacket)->Activate();
            }
        }
        Cancel();
    }
};
#endif