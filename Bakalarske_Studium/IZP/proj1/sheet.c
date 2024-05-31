#include <stdio.h>   //input output
#include <stdlib.h>  //zakladne funkcie
#include <string.h>  //na pouzivanie stringov
#include <ctype.h>   //tolower a toupper funkcie
#include <stdbool.h> //bool funkcie
#include <limits.h>  //INT_MAX hlavne

#define ROWLEN 10242 //definicia dlzky riadku
#define COLLEN 102   // definicia dlzky stlpca

void irow(int r, int n, int ncols, char *delim);                 // pridanie konkretneho prazdneho riadku r = riadok
bool drow(int r, int n);                                         // vymazanie konkretneho riadku
bool drows(int m, int n, int o);                                 // vymazanie viacero riadkov m = prvy riadok n = posledny riadok
void icol(int c, char *delim, char *tabulka, int ncols);         // pridanie konkretneho stlpca c = stlpec
void acol(char *delim, char *tabulka);                           // pridanie noveho posledneho stlpca
void dcol(int c, char *delim, char *tabulka, int ncols);         // vymazanie konkretneho stlpca
void dcols(int m, int n, char *delim, char *tabulka, int ncols); // vymazanie viacero stlpcov m = prvy stlpec n = posledny stlpec

void setdelim(char *delim, char *tabulka);                                                        //nastavenie jednotneho delimetra v riadku (pomocna funkcia)
void findCol(char *tabulka, char *delim, int *idcolOne, int *idcolTwo, int tablen, int m, int n); //najdenie id zaciatku a konca stlpca (pomocna funkcia)

void rows(int *x, int *y, char n[COLLEN], char m[COLLEN], bool *LastRow);                     // vybranie konkretneho riadku na ktorych sa ma vykonat nasledujuca funkcia
void beginswith(int c, char s[COLLEN], char *tabulka, char *delim, int ncols, bool *begwith); // najdenie len tych stlpcov, ktore sa zacinaju zadanym stringom
void contains(int c, char s[COLLEN], char *tabulka, char *delim, int ncols, bool *cont);      // najdenie len tych stlpcov, ktore obsahuju zadany string

void cset(char *tabulka, char *delim, int c, char *setstr, int ncols); // nastavit konkretny stlpec na hodnotu urcenu stringom
void nizc(char *tabulka, char *delim, int c, int ncols);               // v konkretnom stlpci da vsetky znaky na male
void vysc(char *tabulka, char *delim, int c, int ncols);               // v konkretnom stlpci da vsetky znaky na velke
void zaok(char *tabulka, char *delim, int c, int ncols);               // v konkretnom stlpci zaokruhli cisla
void celc(char *tabulka, char *delim, int c, int ncols);               // v konkretnom stlpci pretypuje cislo na int
void copy(char *tabulka, char *delim, int n, int m, int ncols);        //prekopiruje jeden stlpec do druheho
void swap(char *tabulka, char *delim, int n, int m, int ncols);        // prehodi 2 stlpce
void move(char *tabulka, char *delim, int n, int m, int ncols);        // presunie jeden stlpec pred druhy

int main(int argc, char *argv[])
{
    char delim[COLLEN];   // ukladanie delimetrov
    char uloha[COLLEN];   // ukladanie argumentu, ktory sa nasledne porovnava s funkciami
    char tabulka[ROWLEN]; // ulozenie riadka tabulky
    char oldp;            // char pouzivany pri kontrole posledneho riadka
    char *odpad1;         // pointre pouzivane pri kontrole vlozeneho cisla
    char *odpad2;

    int ncols = 1;    //int na scitanie stlpcov v prvom riadku
    int n = 0;        // int na pocitanie riadkov
    int ar = 0;       // int na pocitanie mnozstva prikazov arow
    int x = 0, y = 0; // ints na urcenie riadkov pouzivane funciou rows

    bool tlac = true;     // bool na urcenie ci ma program tlacit riadok (drow a drows)
    bool robn = true;     // bool na zaistenie ze sa vykona pocitanie stlpcov len raz za riadok
    bool LastRow = false; // bool na urcenie posledneho riadku
    bool uprhod = true;   // bool na zaistenie ze po funkciach na upravu tabulky sa nespustia funcie na spracovanie dat

    long nmb1 = 0, nmb2 = 0; // pouzivane na jednoduchsiu pracu s strtol

    while (fgets(tabulka, ROWLEN, stdin) != NULL) //nacitanie zo suboru az pokym nenacita nic
    {
        if (strlen(tabulka) == ROWLEN - 1 && tabulka[strlen(tabulka)] != '\n') // preverenie dlzky riadku
        {
            return 0;
        }

        int ncolsp = 1; // int na porovnanie poctu stlpcov v tabulke

        bool robnp = true;   // bool aby sa pocital pocet slpcov len raz za riadok
        bool begwith = true; // bool funkcie beginswith
        bool cont = true;    // bool funkcie contains

        tlac = true; //reset v pripade ze predchadzajuci riadok nebol vytlaceny

        char p = fgetc(stdin); // kontrola posledneho riadku
        if (n >= 1)
        {
            memmove(tabulka + 1, tabulka, ROWLEN - 1);
            tabulka[0] = oldp;
        }
        oldp = p;
        if (p == EOF)
        {
            LastRow = true;
        }

        if (argc >= 2) // nastavenie jednotneho delimetra plus zaistenie dostatocneho poctu argumentov na pracu programu
        {
            if (strcmp(argv[1], "-d") == 0 && argc >= 3)
            {
                strcpy(delim, argv[2]);
                setdelim(delim, tabulka);
            }
            else
            {
                delim[0] = ' ';
            }
        }
        else
        {
            printf("nedostatocny pocet argumentov");
            return 0;
        }

        for (int i = 1; i < argc; i++) // for, ktory prechadza argumentami programu
        {
            strcpy(uloha, argv[i]); // argument sa zapise do premenej pre zjednodusenie

            if (n == 0 && robn) // pocitanie slpcov v prvom riadku plus kontrola dlzky stlpcov
            {
                int collen = 0;
                int tablen = strlen(tabulka);
                for (int i = 0; i < tablen; i++)
                {
                    if (tabulka[i] == delim[0])
                    {
                        ncols = ncols + 1;
                        collen = 0;
                    }
                    else
                    {
                        collen++;
                    }
                    if (collen > COLLEN)
                    {
                        printf("dlzka stlpca je dlhsia ako maximalna povolena (%i)", COLLEN);
                        return 0;
                    }
                }
                robn = false;
            }
            else if (n != 0 && robnp) // pocitanie v stlpcov v nasledujucich riadkoch plus kontrola dlzky stlpcov
            {
                int collen = 0;
                int tablen = strlen(tabulka);
                for (int i = 0; i < tablen; i++)
                {
                    if (tabulka[i] == delim[0])
                    {
                        ncolsp = ncolsp + 1;
                    }
                    else
                    {
                        collen++;
                    }
                    if (collen > COLLEN)
                    {
                        printf("dlzka stlpca je dlhsia ako maximalna povolena (%i)", COLLEN);
                        return 0;
                    }
                    robnp = false;
                }
                /*if (ncols != ncolsp) // kontrola aby kazdy riadok mal rovnaky pocet stlpcov
                {
                    printf("nasiel sa riadok s rozdielnym poctom stlpcov ako ma prvy riadok = tabulka je neplatna");
                    return 0;
                }*/
            }

            bool rowsb;                     // pomocny bool pri funcii rows
            if (strcmp(uloha, "rows") == 0) //porovnanie argumentu
            {
                rows(&x, &y, argv[i + 1], argv[i + 2], &LastRow);
                rowsb = true;
            }
            else if (rowsb == false) // v pripade ze rows nebol zadany tak sa do x,y zada nekonecny cyklus
            {
                x = n + 1; // n je vzdy o 1 mensie ako riadok, pretoze pocitadlo zacina nulov
                y = n + 1;
            }

            if (strcmp(uloha, "beginswith") == 0)
            {
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                if (*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0) //skuska spravnosti zadanych cisiel v argumentoch
                {
                    beginswith((int)nmb1, argv[i + 2], tabulka, delim, ncols, &begwith);
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }

            if (strcmp(uloha, "contains") == 0)
            {
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                if (*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0)
                {
                    contains((int)nmb1, argv[i + 2], tabulka, delim, ncols, &cont);
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }

            if (strcmp(uloha, "cset") == 0 && ((n + 1 >= x && n + 1 <= y) || LastRow) && begwith && cont && uprhod) // pridane aj podmienky rows, beginswith a contains
            {                                                                                                       // a kontrola ci sa pred nim nevykonala funkcia na upravu tabulky
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                if (*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0)
                {
                    if (strlen(tabulka) + strlen(argv[i + 2]) < ROWLEN - 1) // kontrola dlzky tabulky po pridani zmeneni hodnoty stlpca
                    {
                        cset(tabulka, delim, (int)nmb1, argv[i + 2], ncols);
                        break; // zaistenie ze vsetky prikazy po tomto budu ignorovane
                    }
                    else
                    {
                        printf("riadok tabulky by po pouziti prikazu %s prekrocil maximalny povoleny pocet znakov (%i)", uloha, ROWLEN);
                        return 0;
                    }
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }

            if (strcmp(uloha, "tolower") == 0 && ((n + 1 >= x && n + 1 <= y) || LastRow) && begwith && cont && uprhod)
            {
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                if (*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0)
                {
                    nizc(tabulka, delim, (int)nmb1, ncols);
                    break;
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }

            if (strcmp(uloha, "toupper") == 0 && ((n + 1 >= x && n + 1 <= y) || LastRow) && begwith && cont && uprhod)
            {
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                if (*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0)
                {
                    vysc(tabulka, delim, (int)nmb1, ncols);
                    break;
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }

            if (strcmp(uloha, "round") == 0 && ((n + 1 >= x && n + 1 <= y) || LastRow) && begwith && cont && uprhod)
            {
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                if (*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0)
                {
                    zaok(tabulka, delim, (int)nmb1, ncols);
                    break;
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }

            if (strcmp(uloha, "int") == 0 && ((n + 1 >= x && n + 1 <= y) || LastRow) && begwith && cont && uprhod)
            {
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                if (*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0)
                {
                    celc(tabulka, delim, (int)nmb1, ncols);
                    break;
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }

            if (strcmp(uloha, "copy") == 0 && ((n + 1 >= x && n + 1 <= y) || LastRow) && begwith && cont && uprhod)
            {
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                nmb2 = strtol(argv[i + 2], &odpad2, 10);
                if ((*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0) && (*odpad2 == '\0' && nmb2 <= INT_MAX && nmb2 > 0))
                {
                    copy(tabulka, delim, (int)nmb1, (int)nmb2, ncols);
                    break;
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }

            if (strcmp(uloha, "swap") == 0 && ((n + 1 >= x && n + 1 <= y) || LastRow) && begwith && cont && uprhod)
            {
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                nmb2 = strtol(argv[i + 2], &odpad2, 10);
                if ((*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0) && (*odpad2 == '\0' && nmb2 <= INT_MAX && nmb2 > 0))
                {
                    swap(tabulka, delim, (int)nmb1, (int)nmb2, ncols);
                    break;
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }

            if (strcmp(uloha, "move") == 0 && ((n + 1 >= x && n + 1 <= y) || LastRow) && begwith && cont && uprhod)
            {
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                nmb2 = strtol(argv[i + 2], &odpad2, 10);
                if ((*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0) && (*odpad2 == '\0' && nmb2 <= INT_MAX && nmb2 > 0))
                {
                    move(tabulka, delim, (int)nmb1, (int)nmb2, ncols);
                    break;
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }

            if (strcmp(uloha, "irow") == 0) //funcie na opravu tabulky nepotrebuju kontrolu rows,beginswith a contains
            {
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                if (*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0)
                {
                    irow((int)nmb1, n, ncols, delim);
                    uprhod = false; // zaistenie toho ze nasledujuce prikazy na spracovanie dat budu ignorovane
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }

            if (strcmp(uloha, "arow") == 0)
            {
                if (n == 0) // v pripade ze bude vyvolana funkcia arow tak sa ar zvysi o jeden
                {           // n == 0 sluzi na to ze sa to spocita len raz pocas prveho cyklu
                    ar += 1;
                }
                uprhod = false;
            }

            if (strcmp(uloha, "drow") == 0)
            {
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                if (*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0)
                {
                    tlac = drow((int)nmb1, n); // drow a drows sa priamo vkladaju hodnoty do boolu
                    uprhod = false;
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }

            if (strcmp(uloha, "drows") == 0)
            {
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                nmb2 = strtol(argv[i + 2], &odpad2, 10);
                if ((*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0) && (*odpad2 == '\0' && nmb2 <= INT_MAX && nmb2 > 0))
                {
                    tlac = drows((int)nmb1, (int)nmb2, n);
                    uprhod = false;
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }

            if (strcmp(uloha, "icol") == 0)
            {
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                if (*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0)
                {
                    if (strlen(tabulka) + 1 < ROWLEN - 1) // znova porovnanie dlzky tabulky po pridani delimetru
                    {
                        icol((int)nmb1, delim, tabulka, ncols);
                        uprhod = false;
                    }
                    else
                    {
                        printf("riadok tabulky by po pouziti prikazu %s prekrocil maximalny povoleny pocet znakov (%i)", uloha, ROWLEN);
                        return 0;
                    }
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }

            if (strcmp(uloha, "acol") == 0)
            {
                if (strlen(tabulka) + 1 < ROWLEN - 1)
                {
                    acol(delim, tabulka);
                    uprhod = false;
                }
                else
                {
                    printf("riadok tabulky by po pouziti prikazu %s prekrocil maximalny povoleny pocet znakov (%i)", uloha, ROWLEN);
                    return 0;
                }
            }

            if (strcmp(uloha, "dcol") == 0)
            {
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                if (*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0)
                {
                    dcol((int)nmb1, delim, tabulka, ncols);
                    uprhod = false;
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }

            if (strcmp(uloha, "dcols") == 0)
            {
                nmb1 = strtol(argv[i + 1], &odpad1, 10);
                nmb2 = strtol(argv[i + 2], &odpad2, 10);
                if ((*odpad1 == '\0' && nmb1 <= INT_MAX && nmb1 > 0) && (*odpad2 == '\0' && nmb2 <= INT_MAX && nmb2 > 0))
                {
                    dcols((int)nmb1, (int)nmb2, delim, tabulka, ncols);
                    uprhod = false;
                }
                else
                {
                    printf("zle zapisany argument funkcie %s", uloha);
                    return 0;
                }
            }
        }
        n++; // pripocinanie riadku do dalsieho while

        if (tlac) // tlaci riadok len ak je bool tlac = true
            printf("%s", tabulka);
        else
        {
        }
    }

    if (ar >= 1) // kontrola poctu arow
    {
        for (int i = 0; i < ar; i++) // for na vypis poctu arow
        {
            for (int i = 1; i < ncols; i++) // for na vypis spravneho poctu delimetrov do noveho riadku
            {
                printf("%c", delim[0]);
            }
            printf("\n");
        }
    }

    return 0;
}

void irow(int r, int n, int ncols, char *delim)
{
    if (n + 1 == r) // kontrola ci sme na spravnom riadku
    {
        for (int i = 1; i < ncols; i++)
        {
            printf("%c", delim[0]);
        }
        printf("\n");
    }
}

bool drow(int r, int n)
{
    if (n + 1 == r) //kontrola spravnosti riadku
    {
        return false;
    }
    else
        return true;
}

bool drows(int m, int n, int o) // int o je int n z mainu ale aby sa nebili premenne tak je zmeneny
{
    if (o + 1 >= m && o + 1 <= n) // kontrola spravnosti riadku
    {
        return false;
    }
    else
        return true;
}

void icol(int c, char *delim, char *tabulka, int ncols)
{
    int tablen = strlen(tabulka); // prepisanie dlzky riadku do int pre lepsie pocitanie
    int ccount = 1;               // int pre spravne vlozenie delimetra
    if (c <= ncols)               // kontrola ci vlozeny pocet stlpcov z argumentu nieje vacsi ako celkovy pocet stlpcov
    {
        for (int i = 0; i < tablen; i++) //for na najdenie spravneho id stlpca
        {
            if (ccount == c)
            {
                memmove(tabulka + i + 1, tabulka + i, ROWLEN - (i + 1)); //po najdeni sa zvysok riadka posunie o jednu doprava
                tabulka[i] = delim[0];                                   // a na prazdne miesto sa vlozi delim
                break;                                                   // ukoncenie foru
            }
            if (tabulka[i] == delim[0]) // ak sa znak v riadku rovna prvemu znaku v delimetri tak sa pripocita stlpec
            {
                ccount++;
            }
        }
    }
    else // ak nebola splnena podmienka funkcia sa nevykona
    {
        return;
    }
}

void acol(char *delim, char *tabulka)
{
    int tablen = strlen(tabulka);
    memmove(tabulka + tablen, tabulka + tablen - 1, ROWLEN - tablen); // presunutie /n o poziciu dalej
    tabulka[tablen - 1] = delim[0];                                   // vlozenie delimetra do prazdneho miesta
}

void dcol(int c, char *delim, char *tabulka, int ncols)
{
    int tablen = strlen(tabulka);
    int idcolOne = 0;          // zaciatok hladaneho stlpca
    int idcolTwo = tablen - 1; // koniec hladaneho stlpca automaticky nastavene na koniec znakov v stringu
    if (c <= ncols)
    {
        findCol(tabulka, delim, &idcolOne, &idcolTwo, tablen, c, c + 1); // funkcia na najdenie idcolOne a idcolTwo
        if (c == ncols)                                                  // pre c == ncols musi byt zmeneni memmove kvoli delimetru
        {
            memmove(tabulka + idcolOne - 1, tabulka + idcolTwo, ROWLEN - idcolOne + 1); // podobne ako nizsie ale vymaze to prvy delimeter a nevymaze /n
        }
        else
        {
            memmove(tabulka + idcolOne, tabulka + idcolTwo + 1, ROWLEN - idcolOne); // presunie znaky po idcolTwo na idcolOne cim prepise stlpec na vymazanie
        }
    }
    else
        return;
}

void dcols(int m, int n, char *delim, char *tabulka, int ncols)
{
    int tablen = strlen(tabulka);
    int idcolOne = 0;
    int idcolTwo = tablen - 1;
    if (n >= m) // kontrola ci prve cislo nieje vacsie ako druhe
    {
        if (n <= ncols)
        {
            findCol(tabulka, delim, &idcolOne, &idcolTwo, tablen, m, n + 1); // podobne ako dcol ale do idcolOne sa nacita zaciatok prveho stlpca
            if (n == ncols)                                                  // a do idcolTwo koniec posledneho
            {
                memmove(tabulka + idcolOne - 1, tabulka + idcolTwo, ROWLEN - idcolOne + 1);
            }
            else
                memmove(tabulka + idcolOne, tabulka + idcolTwo + 1, ROWLEN - idcolOne);
        }
        else
            return;
    }
    else
        return;
}

void setdelim(char *delim, char *tabulka) // funkcia na nastavenie jednotneho delimetru
{
    int tablen = strlen(tabulka); // dlzky stringov pre zjednodusenie pouzivania
    int demlen = strlen(delim);
    for (int i = 0; i < tablen; i++) // porovna kazdy znak v riadku
    {
        for (int d = 0; d < demlen; d++) // s kazdym znakom v delimetri
        {
            if (tabulka[i] == delim[d]) // a ak sa rovna
            {
                tabulka[i] = delim[0]; // tak ho zmeni na prvy znak v delimetri
            }
        }
    }
}

void cset(char *tabulka, char *delim, int c, char *setstr, int ncols)
{
    int tablen = strlen(tabulka);
    int lenstr = strlen(setstr);
    int idcolOne = 0;
    int idcolTwo = tablen - 1;

    if (c <= ncols)
    {
        findCol(tabulka, delim, &idcolOne, &idcolTwo, tablen, c, c + 1);

        char buff[ROWLEN]; // pomocny string na ulozenie zvysku riadku

        memmove(buff, tabulka + idcolTwo, ROWLEN - idcolTwo);
        memmove(tabulka + idcolOne, setstr, lenstr);                              // vlozenie stringu do riadka
        memmove(tabulka + idcolOne + lenstr, buff, ROWLEN - (idcolOne + lenstr)); // vlozenie zvysku riadku
    }
    else
        return;
}

void nizc(char *tabulka, char *delim, int c, int ncols)
{
    int tablen = strlen(tabulka);
    int idcolOne = 0;
    int idcolTwo = tablen - 1;

    if (c <= ncols)
    {
        findCol(tabulka, delim, &idcolOne, &idcolTwo, tablen, c, c + 1);
        char buff[COLLEN];

        memmove(buff, tabulka + idcolOne, idcolTwo - idcolOne); // vlozenie stlpca do buffu
        for (int i = 0; i < (idcolTwo - idcolOne); i++)         // for na prejdenie celym buffom
        {
            buff[i] = tolower(buff[i]); // zmena znaku na maly znak
        }
        memmove(tabulka + idcolOne, buff, idcolTwo - idcolOne); // vlozenie buffu nazad do riadku
    }
    else
        return;
}

void vysc(char *tabulka, char *delim, int c, int ncols)
{
    int tablen = strlen(tabulka);
    int idcolOne = 0;
    int idcolTwo = tablen - 1;

    if (c <= ncols)
    {
        findCol(tabulka, delim, &idcolOne, &idcolTwo, tablen, c, c + 1);
        char buff[COLLEN];

        memmove(buff, tabulka + idcolOne, idcolTwo - idcolOne);
        for (int i = 0; i < (idcolTwo - idcolOne); i++)
        {
            buff[i] = toupper(buff[i]); // to iste ako vyssie len sa znaky menia na vyssie
        }
        memmove(tabulka + idcolOne, buff, idcolTwo - idcolOne);
    }
    else
        return;
}

void zaok(char *tabulka, char *delim, int c, int ncols)
{
    int tablen = strlen(tabulka);
    int idcolOne = 0;
    int idcolTwo = tablen - 1;
    char *odpad; // pointer na kontrolu spravneho formatu cisla v stlpci

    if (c <= ncols)
    {
        findCol(tabulka, delim, &idcolOne, &idcolTwo, tablen, c, c + 1);
        char buff[COLLEN];

        memmove(buff, tabulka + idcolOne, idcolTwo - idcolOne);
        buff[idcolTwo - idcolOne] = '\0';    // po prepisani vacsieho cisla mensim sa stavalo ze ostali cisla
                                             // zapisane, toto nastavuje pevny koniec stringu
        double cislo = strtod(buff, &odpad); //presunutie cisla v stlpci do premmenej
        if (*odpad == '\0')                  // skuska ci v stlpci bolo len cislo
        {
            int cislo2 = (int)cislo; // zapisanie doublu do integeru pre pocitanie
            if (cislo < 0.0)
            {
                if (cislo - cislo2 + 1 <= 0.5) // zaokruhlovanie negativnych cisiel 1 je pripocitana kvoli tomu ze sa zle porovnavaju cisla s minusom
                {
                    cislo2 = cislo2 - 1;
                }
            }
            else
            {
                if (cislo - cislo2 >= 0.5) // zaokruhlovanie kladneho cisla
                {
                    cislo2 = cislo2 + 1;
                }
            }

            sprintf(buff, "%d", cislo2); // zapisanie doublu do pomocneho stringu buff

            cset(tabulka, delim, c, buff, ncols); // vlozenie buffu do riadka pouzitie cset pre zjednodusenie vkladania
        }                                         // ak char na ktory ukazuje pointer nebude /0 prikaz sa nevykona
    }
    else
        return;
}

void celc(char *tabulka, char *delim, int c, int ncols)
{
    int tablen = strlen(tabulka);
    int idcolOne = 0;
    int idcolTwo = tablen - 1;
    char *odpad;

    if (c <= ncols)
    {
        findCol(tabulka, delim, &idcolOne, &idcolTwo, tablen, c, c + 1);
        char buff[COLLEN];

        memmove(buff, tabulka + idcolOne, idcolTwo - idcolOne);
        buff[idcolTwo - idcolOne] = '\0'; // po prepisani vacsieho cisla mensim sa stavalo ze ostali cisla
                                          // zapisane, toto nastavuje pevny koniec stringu
        double cislo = strtod(buff, &odpad);

        if (*odpad == '\0')
        {
            int cislo2 = (int)cislo; // podobne ako predtym len sa nezaokruhluje

            sprintf(buff, "%d", cislo2);

            cset(tabulka, delim, c, buff, ncols);
        }
    }
    else
        return;
}

void copy(char *tabulka, char *delim, int n, int m, int ncols)
{
    int tablen = strlen(tabulka);
    int idcolnOne = 0;
    int idcolnTwo = tablen - 1;

    if (m <= ncols && n <= ncols && m != n)                                // porovnanie ci obe cisla su mensie ako celkovy pocet stlpcov
    {                                                                      // a porovnanie ci niesu rovnake
        findCol(tabulka, delim, &idcolnOne, &idcolnTwo, tablen, n, n + 1); // majde id stlpca ktory ma kopirovat

        char buffn[COLLEN];

        memmove(buffn, tabulka + idcolnOne, idcolnTwo - idcolnOne); // ulozi ho do buffn

        buffn[idcolnTwo - idcolnOne] = '\0';

        cset(tabulka, delim, m, buffn, ncols); // vypise buffn do druheho stlpca
    }
    else
        return;
}

void swap(char *tabulka, char *delim, int n, int m, int ncols)
{
    int tablen = strlen(tabulka); // podobny princip ako copy ale spravi to s oboma stlpcami
    int idcolmOne = 0;
    int idcolmTwo = tablen - 1;
    int idcolnOne = 0;
    int idcolnTwo = tablen - 1;

    if (m <= ncols && n <= ncols && m != n)
    {
        findCol(tabulka, delim, &idcolmOne, &idcolmTwo, tablen, m, m + 1);
        findCol(tabulka, delim, &idcolnOne, &idcolnTwo, tablen, n, n + 1);

        char buffn[COLLEN];
        char buffm[COLLEN];

        memmove(buffn, tabulka + idcolnOne, idcolnTwo - idcolnOne);
        memmove(buffm, tabulka + idcolmOne, idcolmTwo - idcolmOne);

        buffm[idcolmTwo - idcolmOne] = '\0';
        buffn[idcolnTwo - idcolnOne] = '\0';

        cset(tabulka, delim, m, buffn, ncols);
        cset(tabulka, delim, n, buffm, ncols);
    }
    else
        return;
}

void move(char *tabulka, char *delim, int n, int m, int ncols)
{
    int tablen = strlen(tabulka);
    int idcolnOne = 0;
    int idcolnTwo = tablen - 1;

    if (m <= ncols && n <= ncols && m != n)
    {
        findCol(tabulka, delim, &idcolnOne, &idcolnTwo, tablen, n, n + 1);

        char buffn[COLLEN];

        memmove(buffn, tabulka + idcolnOne, idcolnTwo - idcolnOne); // nacitam len jeden stlpec
        buffn[idcolnTwo - idcolnOne] = '\0';

        dcol(n, delim, tabulka, ncols); // nasledne ho odstranim z riadka
        if (m == 1)                     // ak vkladam pred 1 tak nemozem vlozit nulty riadok
        {
            icol(m, delim, tabulka, ncols);        // pridam novy stlpec
            cset(tabulka, delim, m, buffn, ncols); // a vlozim donho buffn
        }
        else // ak stlpec pred ktoreho vkladam nieje 1 tak musim vlozit o buffn pred stlpec m
        {
            icol(m - 1, delim, tabulka, ncols);
            cset(tabulka, delim, m - 1, buffn, ncols);
        }
    }
    else
        return;
}

void rows(int *x, int *y, char n[COLLEN], char m[COLLEN], bool *LastRow)
{
    long nmb1 = 0, nmb2 = 0; // pre kontrolu inputu
    char *odpad1;
    char *odpad2;

    if ((n[0] == '-' && m[0] == '-') && (n[1] == '\0' && m[1] == '\0')) // ak sa obe vstupne stringy rovnaju - tak sa do x,y zapisu hodnoty 0
    {                                                                   // ktore sposobia ze sa prikaz spusti len na poslednom riadku skrz bool LastRow
        *x = (int)nmb1;
        *y = (int)nmb2;
    }

    else if (m[0] == '-' && m[1] == '\0') // ak sa len druhy string rovna - tak sa do x zapise cislo ktore je v prvom stringu
    {                                     // a do druheho sa da INT_MAX aby to robil do konca programu
        nmb1 = strtol(n, &odpad1, 10);
        if (*odpad1 == '\0' && nmb1 < INT_MAX && nmb1 > 0)
        {
            *x = (int)nmb1;
            *y = INT_MAX;
        }
        else
            printf("zle zadane hodnoty rows ");
    }
    else if (n[0] == '-') // pokial sa 1 string rovna - tak sa prikaz nevykona lebo nemoze zacinat od konca
    {
        printf("Prve cislo musi byt mensie ako druhe cislo rows ");
        *LastRow = false; // bool LastRow sa da ako false aby sa nevykonal posledny riadok
    }
    else
    {
        nmb1 = strtol(n, &odpad1, 10);
        nmb2 = strtol(m, &odpad2, 10); // inak sa hodnoty zapisu do premennych a ak v stringoch neboli len cisla tak sa funkcia nevykona
        if ((*odpad1 == '\0' && nmb1 < INT_MAX && nmb1 > 0) && (*odpad2 == '\0' && nmb2 < INT_MAX && nmb2 > 0))
        {
            *x = (int)nmb1;
            *y = (int)nmb2;
            if (*x > *y) // porovna ci prve cislo nieje vacsie ako druhe
            {
                printf("Prve cislo musi byt mensie ako druhe cislo rows ");
                *x = 0;
                *y = 0;
            }
            *LastRow = false; // nastavi aby sa funkcia nerobila na poslednom riadku ale len na konkretnych zvolenych
        }
        else
            printf("zle zadane hodnoty rows ");
    }
}

void beginswith(int c, char s[COLLEN], char *tabulka, char *delim, int ncols, bool *begwith)
{
    int tablen = strlen(tabulka); // int pre jednoduchost
    int slen = strlen(s);
    int idcolOne = 0;
    int idcolTwo = tablen - 1;

    if (c <= ncols)
    {
        findCol(tabulka, delim, &idcolOne, &idcolTwo, tablen, c, c + 1);

        char buff[COLLEN];
        memmove(buff, tabulka + idcolOne, idcolTwo - idcolOne); // stiahnem stlpec z riadka do noveho stringu
        int bufflen = strlen(buff);

        if (bufflen < slen) // ak je dlhsika stlpca mensia ako dlzka hladaneho stringu tak nemusim hladat
        {
            *begwith = false;
            return;
        }
        else
        {
            int j = 0;                     // spravim si premennu ktorou bude prechadzat cez hladany string
            for (int i = 0; i < slen; i++) // prejdem forom len tolko raz kolko ma hladany string
            {                              // znakov kedze hladam len zaciatok
                if (buff[i] == s[j])
                {
                    j++; // ak sa znaky rovnaju tak v dalsom cykle budeme porovnavat nasledujuce
                }
                else
                {
                    *begwith = false; // ak sa znaky nerovnaju tak na riadku sa nebude funkcia spracovania dat nevykona
                    return;
                }
            }
            *begwith = true; // ak uspesne prejde forom tak sa na riadku funkcia spracovania dat vykona
        }
    }
}

void contains(int c, char s[COLLEN], char *tabulka, char *delim, int ncols, bool *cont)
{
    int tablen = strlen(tabulka);
    int slen = strlen(s);
    int idcolOne = 0;
    int idcolTwo = tablen - 1;

    if (c <= ncols)
    {
        findCol(tabulka, delim, &idcolOne, &idcolTwo, tablen, c, c + 1);

        char buff[COLLEN];

        memmove(buff, tabulka + idcolOne, idcolTwo - idcolOne);

        int bufflen = strlen(buff);

        if (slen > bufflen) // ak je dlzka hladaneho stringu dlhsia ako stlpec tak nemusi hladat
        {
            *cont = false;
            return;
        }
        else
        {
            int j = 0;
            int zhoda = 0;                    // podobne ako beginswith ale s novou premennou
            for (int i = 0; i < bufflen; i++) // teraz prechadza cez cely stlpec
            {
                if (buff[i] == s[j]) // ak sa znaky rovnaju tak v dalsom cykle sa porovnavaju nasledujuce a k zhode sa pripocita 1
                {
                    j++;
                    zhoda++;
                }
                else // ak sa nerovnaju tak v dalsom cykle sa porovnava nasledujuci znak stlpca znova s prvym znakom hladaneho stringu
                {    // a zaroven sa resetuje counter zhod
                    j = 0;
                    zhoda = 0;
                }
                if (zhoda == slen) // ak sa zhoda rovna dlzke hladaneho stringu tak vychadzame s foru
                {
                    break;
                }
            }
            if (zhoda == slen) // ak sme vysli s breakom tak sa na danom riadku vykona funkcia spracovania dat
            {
                *cont = true;
            }
            else // ak nie tak sa nevykona
                *cont = false;
        }
    }
}

void findCol(char *tabulka, char *delim, int *idcolOne, int *idcolTwo, int tablen, int m, int n)
{
    int cols = 1;  // int na pocitanie stlpcov
    bool b = true; // pomocny bool

    for (int i = 0; i < tablen; i++) // prejde cez vsetky znaky
    {
        if (cols == m && b) // ak sa cols rovna hladanemu stlpcu tak sa zapise hodnota i ako prve id
        {
            *idcolOne = i;
            b = false; // bool bude false aby uz nemohli znova zadat inu hodnotu
        }
        if (cols == n) // pokial sa druhy zadany stlpec rovna cols tak sa zapise dalsie id a skonci for
        {
            *idcolTwo = i - 1;
            break;
        }
        if (tabulka[i] == delim[0]) // ak sa znak rovna delimetru tak sa ku cols pripocita 1
        {
            cols++;
        }
    }
}