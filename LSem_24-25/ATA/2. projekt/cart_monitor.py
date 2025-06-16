#!/usr/bin/env python3
"""
Runtime verifier of the cart controller.
syntax:
    $0 <log_file     or
    $0 log_file
"""

import enum

class RequestStatus(enum.Enum):
    WAITING = 0
    LOADED = 1
    UNLOADED = 2

class CartStatus(enum.Enum):
    ON_STATION = 0
    MOVING = 1

class CoverageStatus:
    def __init__(self, num_slots, num_stations):
        self.coverage = [[0 for _ in range(num_stations)] for _ in range(num_slots)]

    def calculate_coverage(self):
        final_sum = sum(sum(self.coverage, []))
        total_slots = len(self.coverage) * len(self.coverage[0])
        return (final_sum / total_slots) * 100
            

class Cargo:
    def __init__(self, content, weight, destination):
        self.content = content
        self.destination = destination
        self.weight = weight

class Request:
    def __init__(self, content, weight, time, source, destination):
        self.status = RequestStatus.WAITING
        self.content = content
        self.source = source
        self.destination = destination
        self.weight = weight
        self.spawn_time = time

class Cart:
    def __init__(self, slots, max_weight):
        self.status = CartStatus.ON_STATION
        self.position = None
        self.slots = slots * [None]
        self.loaded_cargo = 0
        self.requests = []
        self.total_weight = 0
        self.max_weight = max_weight
        self.last_event_time = 0
        self.coverage = CoverageStatus(slots, 4)

first_loading = True
monitor_cart = Cart(4, 150)
error_check = False

def report_coverage():
    global monitor_cart, error_check
    
    # Ak vznikne poziadavok na presun tak naklad sa musi nalozit (4)
    if not error_check:
        for request in monitor_cart.requests:
            if request.status == RequestStatus.WAITING:
                print(f'{monitor_cart.last_event_time}:ERROR:Request for {request.content} from {request.source} to {request.destination} is not loaded (rule 4)')
                error_check = True

        # Kazdy nalozeny naklad sa musi vylozit (8)
        for request in monitor_cart.requests:
            if request.status == RequestStatus.LOADED:
                print(f'{monitor_cart.last_event_time}:ERROR:Request for {request.content} from {request.source} to {request.destination} is not unloaded (rule 8)')
                error_check = True
    
    if not error_check:
        print('All properties hold.')
    "Coverage reporter"
    # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    # Zde nahradte vypocet/vypis aktualne dosazeneho pokryti
    print('CartCoverage %d%%' % monitor_cart.coverage.calculate_coverage())

def has_active_requests(monitor_cart: Cart):
    for request in monitor_cart.requests:
        if request.status == RequestStatus.WAITING or request.status == RequestStatus.LOADED:
            return True
    return False

def onmoving(time, pos1, pos2):
    global first_loading, monitor_cart, error_check
    time = int(time)
    
    # Vozik zostane na stanici aj ked ma aktivnu poziadavku (9)
    if has_active_requests(monitor_cart) and monitor_cart.status == CartStatus.ON_STATION and time - monitor_cart.last_event_time > 2:
        print(f'{time}:ERROR:Cart was idle at station while having active requests (rule 9)')
        error_check = True
    
    monitor_cart.last_event_time = time
    # Naklad se musi vylozit pokud je vozik v stanici jejiho cile (3)
    third_rule = True
    for cargo in monitor_cart.slots:
        if cargo is not None and cargo.destination == pos1:
            third_rule = False
            break

    if third_rule:
        monitor_cart.status = CartStatus.MOVING
        monitor_cart.position = pos2
    else:
        error_check = True
        print(f'{time}:ERROR: Cargo not unloaded at station {pos1} (rule 3)')



def onrequesting(time, pos1, pos2, content, weight):
    global monitor_cart, error_check
    time = int(time)
    weight = int(weight)

    # Vozik zostane na stanici aj ked ma aktivnu poziadavku (9)
    if has_active_requests(monitor_cart) and monitor_cart.status == CartStatus.ON_STATION and time - monitor_cart.last_event_time > 2:
        print(f'{time}:ERROR:Cart was idle at station while having active requests (rule 9)')
        error_check = True

    monitor_cart.last_event_time = time
    monitor_cart.requests.append(Request(content, weight, time, pos1, pos2))

def onloading(time, pos, content, weight, slot):
    global first_loading, monitor_cart, error_check
    
    time = int(time)
    weight = int(weight)
    slot = int(slot)
    station_parser = {'A': 0, 'B': 1, 'C': 2, 'D': 3}

    if first_loading:
        first_loading = False
        monitor_cart.position = pos
    
    monitor_cart.status = CartStatus.ON_STATION
    monitor_cart.last_event_time = time

    # Vozik nesmi nakladat naklad na ktery neni pozadavek (5)
    fifth_rule = False
    destination = None
    for request in monitor_cart.requests:
        if request.content == content and request.weight == weight and request.source == pos and request.status == RequestStatus.WAITING:
            fifth_rule = True
            request.status = RequestStatus.LOADED
            destination = request.destination
            break
    # Vozik nesmi nakladat na obsadeny slot alebo ked su vsetky sloty obsadene (6) (1)
    sixth_rule = False
    first_rule = False

    if monitor_cart.slots[slot] is None:
        first_rule = True

    if monitor_cart.loaded_cargo < len(monitor_cart.slots):
        sixth_rule = True
    
    # Vozik ani naklad nesmie prekrocit maximalnu hmotnost (7)

    seventh_rule = False
    if monitor_cart.total_weight + weight <= monitor_cart.max_weight:
        seventh_rule = True

    # Kontrola pravidiel
    if first_rule and sixth_rule and fifth_rule and seventh_rule:
        monitor_cart.slots[slot] = Cargo(content, weight, destination)
        monitor_cart.loaded_cargo += 1
        monitor_cart.total_weight += weight
        monitor_cart.coverage.coverage[slot][station_parser[pos]] = 1
    elif not first_rule:
        print(f'{time}:ERROR:Loading to slot {slot} which is already occupied (rule 1)')
        error_check = True
    elif not sixth_rule:
        print(f'{time}:ERROR:Loading to all occupied slots (rule 6)')
        error_check = True
    elif not fifth_rule:
        print(f'{time}:ERROR:Loading with no request for {content} with weight {weight} from {pos} (rule 5)')
        error_check = True
    elif not seventh_rule:
        print(f'{time}:ERROR:Total weight of loaded cargo {monitor_cart.total_weight + weight} exceeds maximum weight {monitor_cart.max_weight} rule 7)')
        error_check = True

def onunloading(time, pos, content, weight, slot):
    global first_loading, monitor_cart, error_check
    time = int(time)
    weight = int(weight)
    slot = int(slot)

    monitor_cart.status = CartStatus.ON_STATION
    monitor_cart.last_event_time = time
    # Vozik nesmie vykladat z prazdneho slotu (2)
    second_rule = False
    if monitor_cart.slots[int(slot)] is not None:
        second_rule = True

    if second_rule:
        for request in monitor_cart.requests:
            if request.content == content and request.weight == weight and request.destination == pos and request.status == RequestStatus.LOADED:
                request.status = RequestStatus.UNLOADED
                monitor_cart.slots[int(slot)] = None
                monitor_cart.loaded_cargo -= 1
                monitor_cart.total_weight -= weight
                break
    else:
        print(f'{time}:ERROR:Unloading empty slot {slot} (rule 2)')
        error_check = True
    

def onevent(event):
    "Event handler. event = [TIME, EVENT_ID, ...]"
    # !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    # ZDE IMPLEMENTUJTE MONITORY
    if error_check:
        return
    # vyjmeme identifikaci udalosti z dane n-tice
    event_id = event[1]
    del(event[1])
    # priklad predani ke zpracovani udalosti moving
    if event_id == 'moving':
        # predame n-tici jako jednotlive parametry pri zachovani poradi
        onmoving(*event)
    elif event_id == 'requesting':
        onrequesting(*event)
    elif event_id == 'loading':
        onloading(*event)
    elif event_id == 'unloading':
        onunloading(*event)

###########################################################
# Nize nemenit.

def monitor(reader):
    "Main function"
    for line in reader:
        line = line.strip()
        onevent(line.split())
    report_coverage()

if __name__ == "__main__":
    import sys
    if len(sys.argv) > 1:
        with open(sys.argv[1], 'r') as istream:
            monitor(istream)
    else:
        monitor(sys.stdin)
