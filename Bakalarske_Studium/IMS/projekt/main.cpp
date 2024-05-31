#include <simlib.h>
#include <iostream>
#include <stdio.h>
#include <getopt.h>

#include "packet.hpp"
#include "pause.hpp"

using namespace std;

void print_usage(char *name)
{
    std::cout << "Usage: " << name << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -c <number>    Number of cars" << std::endl;
    std::cout << "  -w <number>    Number of workers" << std::endl;
    std::cout << "  -s <number>    Number of sorters" << std::endl;
    std::cout << "  -o <number>    Number of orders" << std::endl;
    std::cout << "  -h             Print this help" << std::endl;
}

int main(int argc, char *argv[])
{
    Init(0, 60 * 60 * 10 /*8 hodin + 2 hodiny pre navrat dodavok*/);

    char *name;
    name = argv[0];
    unsigned long cars = 1;
    unsigned long dockers = 1;
    unsigned long sorters = 1;
    unsigned long orders = 0;
    bool pause = false;

    static const char *sOpts = "c:d:s:o:p";
    int opt;
    char *err;

    while ((opt = getopt(argc, argv, sOpts)) != -1)
    {
        switch (opt)
        {
        case 'p':
            pause = true;
            break;
        case 'c':
            cars = strtoul(optarg, &err, 10);
            if (*err != '\0' || cars < 1)
            {
                std::cerr << "Invalid number of cars: " << optarg << std::endl;
                return 1;
            }
            break;
        case 'd':
            dockers = strtoul(optarg, &err, 10);
            if (*err != '\0' || dockers < 1)
            {
                std::cerr << "Invalid number of workers: " << optarg << std::endl;
                return 1;
            }
            break;
        case 's':
            sorters = strtoul(optarg, &err, 10);
            if (*err != '\0' || sorters < 1)
            {
                std::cerr << "Invalid number of sorters: " << optarg << std::endl;
                return 1;
            }
            break;
        case 'o':
            orders = strtoul(optarg, &err, 10);
            if (*err != '\0' || orders < 1)
            {
                std::cerr << "Invalid number of orders: " << optarg << std::endl;
                return 1;
            }
            break;
        case 'h':
            print_usage(name);
            return 0;
        case '?':
        default:
            print_usage(name);
            return 1;
        }
    }

    Van.SetCapacity(cars);
    Docker.SetCapacity(dockers);
    Sorter.SetCapacity(sorters);

    RandomSeed(time(NULL));

    if (pause)
    {
        Pause *pa;
        pa = new Pause;
        pa->nOfDockers = dockers;
        pa->nOfSorters = sorters;
        pa->Activate(4 * 60 * 60);
    }

    EndofWork *end;
    end = new EndofWork;
    end->nOfDockers = dockers;
    end->nOfSorters = sorters;
    end->Activate(8 * 60 * 60);

    Packet *p;
    for (int i = 0; i < (int)orders; i++)
    {
        p = new Packet;
        p->nOfPackets = orders;
        p->Activate();
    }

    Run();

    cout << "Simulacia rozvozu objednavok" << endl;
    cout << "####################################################" << endl;
    cout << "Kompletne statistiky na konci pracovneho dna: " << endl;
    cout << "####################################################" << endl;
    Sorter.Output();
    Docker.Output();
    Van.Output();
    queueSorted.Output();
    ErrorPackets.Output();
    cout << "####################################################" << endl;
    cout << "" << endl;
    cout << "Prehladnejsie statistiky na konci pracovneho dna: " << endl;
    cout << "####################################################" << endl;
    cout << "Celkovy pocet objednavok: " << orders << endl;
    cout << "Celkovy pocet skladnikov: " << Docker.Capacity() << endl;
    cout << "Celkovy pocet triedicov: " << Sorter.Capacity() << endl;
    cout << "Celkovy pocet dodavok: " << Van.Capacity() << endl;
    string lunchbreak = (pause) ? "Ano" : "Nie";
    cout << "Bola umoznena obedova pauza: " << lunchbreak << endl;
    cout << "####################################################" << endl;
    cout << "Celkovy pocet objednavok, cakajucich na vylozenie/nalozenie: " << Docker.QueueLen() << endl;
    cout << "Posledna vylozena objednavka bola v case: " << DockUnload << " hodin" << endl;
    cout << "Posledna nalozena objednavka bola v case: " << DockLoad << " hodin" << endl;
    cout << "----------------------------------------------------" << endl;
    cout << "Celkovy pocet objednavok, cakajucich na triedenie: " << Sorter.QueueLen() << endl;
    cout << "Posledna triedena objednavka bola v case: " << SorterTime << " hodin" << endl;
    cout << "----------------------------------------------------" << endl;
    cout << "Celkovy pocet objednavok, cakajucich na dodavku: " << Van.QueueLen() << endl;
    cout << "Pocet odoslanych dodavok: " << VansSent << endl;
    cout << "Posledna odoslana dodavka bola v case: " << VanSent << " hodin" << endl;
    cout << "Pocet dodavok, ktore sa stihli vratit: " << VansReturned << endl;
    cout << "Posledna vratena dodavka bola v case: " << VanReturn << " hodin" << endl;
    cout << "Pocet dodavok, ktore sa nestihli vratit: " << VansSent - VansReturned << endl;
    cout << "Priemerny cas, kedy bola dodavka prec: " << SumOfTimeAway / VansReturned << " hodin" << endl;
    cout << "Celkovy pocet objednavok, ktore boli nespravne roztriedene: " << ErrorPackets.Length() << endl;
    cout << "####################################################" << endl;

    return 0;
}