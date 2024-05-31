# Hardware Info Server

Tento projekt je server v jazyku C, ktory komunikuje pomocou protokolu HTML a poskytuje informacie o systeme na ktorom bezi.

## Uvod

Server dokaze odpovedat na 3 vyzvy a to:

- Vypise hostname servera
- Vypise nazov procesoru servera
- Vypise aktualnu zataz procesora na serveri

Server je mozne vypnut pomocou interupt signalu a teda bud CTRC+C alebo prikazom `kill -2 [PID]`

### Prerekvizity

GCC prekladac pre prelozenie zdrojoveho kodu.

### Instalacia

Pomocou makefile prelozime projekt na serveri.

```
$ make
```

Potom spustime program s parametrom portu pomocou ktoreho bude server komunikovat

```
$ ./hinfosvc [PORTNUMBER]
```

## Pouzitie

Ukazka pouzivania programu aj s ilustracnymi odpovedami servera

Server side:

```
$ ./hinfosvc [PORTNUMBER]
```

Client side:

```
$ curl http://[SERVER_IP]:[PORTNUMBER]/hostname
Linux-server
$ curl http://[SERVER_IP]:[PORTNUMBER]/cpu-name
AMD Threadripper 3990X
$curl http://[SERVER_IP]:[PORTNUMBER]/load
15%
```
