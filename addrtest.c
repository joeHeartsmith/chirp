#include "stdcntrl.h"


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char** argv) {
    struct ifaddrs *ifa;
    char address[NI_MAXHOST];
    getifaddrs(&ifa);

    while (ifa->ifa_next != NULL) {
        if (strncmp(argv[1], ifa->ifa_name, strlen(argv[1])) && ifa->ifa_addr->sa_family == AF_INET) {
            getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), address, NI_MAXHOST, NULL, NI_MAXHOST, NI_NUMERICHOST);
            printf("Found %s\n", ifa->ifa_name);
            //printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, address);
        }
        ifa = ifa->ifa_next;
    }
    return 0;
}