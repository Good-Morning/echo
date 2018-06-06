#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>
#include <set>

#include <string.h>

#include "utils.h"

int _main(int argc, char** argv) {
    sockaddr_in addr;

    u_int32_t addr_ip4;
    if (argc >= 2) {
        addr_ip4 = get_ip4(argv[1]);
    } else {
        std::cout << "address to listen to: ";
        std::string st;
        std::cin >> st;
        char* s = &st[0];
        addr_ip4 = get_ip4(s);
    }

    socket_t listener;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(1337);
    addr.sin_addr.s_addr = htonl(addr_ip4);
    if (bind(listener, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    listen(listener, 2);

    int indn = 5;
    std::set<int> inds = {1, 2, 3, 4};
    while(true) {
        std::cout << "listening" << std::endl; 
        int _sock = accept(listener, 0, 0);
        if (_sock < 0) {
            throw std::runtime_error("While accepting an error occurred: "_str + strerror(errno));
        }

        if (inds.size() == 0) {
            inds.insert(indn++);
        }
        int ind = *inds.begin();
        inds.erase(inds.begin());
        std::thread([_sock, ind, &inds]{
            socket_t sock(_sock);
            while(true) {
                std::cout << ind << " awaits" << std::endl;
                std::string st = blocking_recv(sock);
                if (st == "exit") {
                    std::cout << "connection #" << ind << " was closed\n";
                    break;
                }
                std::cout << ind << " has recieved: '" << st << "'" << std::endl;
                blocking_send(sock, st);
            }
            inds.insert(ind);
        }).detach();
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
