#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>
#include <exception>
#include <algorithm>

#include "utils.h"

int _main(int argc, char** argv) {
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
    socket_t sock;
    sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(1337);
    addr.sin_addr.s_addr = htonl(addr_ip4);
    timeval timeout{2, 0};
    if (-1 == setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) {
        throw std::runtime_error("While setting socket an error occurred: "_str + strerror(errno));
    }
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        throw std::runtime_error("While connecting an error occurred: "_str + strerror(errno));
    }
    while(true) {
        std::cout << "enter message: ";
        std::string st;
        std::getline(std::cin, st);

        std::cout << "sending" << std::endl;
        blocking_send(sock, st);
        if (st == "exit") {
            break;
        }
        std::cout << "receiving..." << std::endl;
        st = blocking_recv(sock);
        std::cout << "received: " << st;
    }
    return 0;
}

int main(int argc, char** argv) {
    try {
        return _main(argc, argv);
    } catch (std::runtime_error e) {
        std::cerr << e.what();
        return -1;
    } catch (std::invalid_argument e) {
        std::cerr << e.what();
        return -1;
    }
}

