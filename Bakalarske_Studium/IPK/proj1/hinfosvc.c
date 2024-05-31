#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int get_port(int argc, char const *argv[])
{
    if (argc == 2)
    {
        return atoi(argv[1]);
    }
    else
    {
        return -1;
    }
}
void get_hostname(char *answer)
{
    FILE *p = popen("cat /proc/sys/kernel/hostname", "r");
    if (p != NULL)
    {
        char hostname[50];
        fgets(hostname, sizeof(hostname), p);
        strcat(answer, hostname);
        pclose(p);
    }
}

void get_load(char *answer)
{
    long int values[2][10];

    FILE *p = popen("cat /proc/stat", "r");

    char line[100];

    if (p != NULL)
    {
        fgets(line, sizeof(line), p);

        strtok(line, " ");

        for (int i = 0; i < 10; i++)
        {
            values[0][i] = strtol(strtok(NULL, " "), NULL, 10);
        }
        pclose(p);
    }

    sleep(1);

    FILE *f = popen("cat /proc/stat", "r");

    if (f != NULL)
    {
        fgets(line, sizeof(line), f);

        strtok(line, " ");

        for (int i = 0; i < 10; i++)
        {
            values[1][i] = strtol(strtok(NULL, " "), NULL, 10);
        }
        pclose(f);
    }

    long int PrevIdle = values[0][3] + values[0][4];
    long int Idle = values[1][3] + values[1][4];

    long int PrevNonIdle = values[0][0] + values[0][1] + values[0][2] + values[0][5] + values[0][6] + values[0][7];
    long int NonIdle = values[1][0] + values[1][1] + values[1][2] + values[1][5] + values[1][6] + values[1][7];

    long int PrevTotal = PrevIdle + PrevNonIdle;
    long int Total = Idle + NonIdle;

    float totald = Total - PrevTotal;
    float idled = Idle - PrevIdle;

    float load = (float)((totald - idled) / totald) * 100.0;

    char str_load[10];

    sprintf(str_load, "%.0f", load);

    strcat(answer, str_load);
    strcat(answer, "%\n");
}

void get_modelname(char *answer)
{
    FILE *p = popen("cat /proc/cpuinfo | grep \"model name\" | head -n 1 | awk 'BEGIN{FS = \": \"}{print $2}'", "r");
    if (p != NULL)
    {
        char modelname[50];
        fgets(modelname, sizeof(modelname), p);
        strcat(answer, modelname);
        pclose(p);
    }
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int port = 0;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((port = get_port(argc, argv)) < 0)
    {
        fprintf(stderr, "Need to choose port: ./hinfosvc [PORTNUMBER]\n");
        return EXIT_FAILURE;
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0)
    {
        fprintf(stderr, "Internal Error\n");
        return EXIT_FAILURE;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        fprintf(stderr, "Internal Error\n");
        return EXIT_FAILURE;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        fprintf(stderr, "Internal Error\n");
        return EXIT_FAILURE;
    }
    if (listen(server_fd, 3) < 0)
    {
        fprintf(stderr, "Internal Error\n");
        return EXIT_FAILURE;
    }
    char answer[100] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n";

    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) >= 0)
    {
        if ((read(new_socket, buffer, 1024)) < 0)
        {
            fprintf(stderr, "Internal Error\n");
            return EXIT_FAILURE;
        }

        strcpy(answer, "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n");

        if (!memcmp(buffer, "GET /cpu-name HTTP/1.1", 22))
        {
            get_modelname(answer);
        }
        else if (!memcmp(buffer, "GET /hostname HTTP/1.1", 22))
        {
            get_hostname(answer);
        }
        else if (!memcmp(buffer, "GET /load HTTP/1.1", 18))
        {
            get_load(answer);
        }
        else
        {
            strcat(answer, "400 Bad Request\n");
        }
        send(new_socket, answer, strlen(answer), 0);

        close(new_socket);
    }

    fprintf(stderr, "Internal Error\n");
    return EXIT_FAILURE;
}