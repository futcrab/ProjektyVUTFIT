#ifndef FACILITIES_HPP
#define FACILITIES_HPP

#include <simlib.h>

Queue queueSorted("queueSorted");

Queue ErrorPackets("ErrorPackets");

Store Sorter("Sorter", 1);

Store Docker("Docker", 1);

Store Van("Van", 1);

Facility checkPacked("checkPacked");

Facility SentVan("SentVan");

Facility ReturnedVan("ReturnedVan");

Facility RewriteDockUnload("RewriteDockUnload");

Facility RewriteDockLoad("RewriteDockLoad");

Facility RewriteSorter("RewriteSorter");

#endif