#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <string.h>

u_int8_t get_byte(char*& st) {
    u_int8_t res = 0;
    while(true) {
        if (*st != 0) {
            char c = *st;
            st++;
            if (c >= '0' && c <= '9') {
                res = res * 10;
                res += c - '0';
            } else {
                return res;
            }
        } else {
            return res;
        }
    }
}

u_int32_t get_ip4(char* st) {
    u_int32_t res = 0;
    res |= get_byte(st);
    res <<= 8;
    res |= get_byte(st);
    res <<= 8;
    res |= get_byte(st);
    res <<= 8;
    res |= get_byte(st);
    return res;
}

int main(int argc, char** argv) {
    char buffer[4096];
    char buf[4096];
    u_int32_t addr_ip4;
    if (argc >= 2) {
        addr_ip4 = get_ip4(argv[1]);
    } else {
        std::cout << "address to connect to: ";
        std::string st;
        std::cin >> st;
        char* s = &st[0];
        addr_ip4 = get_ip4(s);
    }
    int sock;
    struct sockaddr_in addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(1337);
    addr.sin_addr.s_addr = htonl(addr_ip4);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connecting");
        exit(EXIT_FAILURE);
    }
    while(true) {
        std::cout << "enter message: ";
        std::string st;
        std::cin >> st;
        if (st == "exit") {
            close(sock);
            std::cout << "connection is closed\n";
            break;
        }

        std::cout << "sending" << std::endl;
        send(sock, st.data(), st.size(), 0);
        int read = 0;
        st.resize(4096, '\0');
        std::cout << "receiving..." << std::endl;
        read = recv(sock, &st[0], 4096, 0);
        st[read] = 0;
        std::cout << "received: " << st; 
    }
    return 0;
}