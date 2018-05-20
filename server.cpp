#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>
#include <set>

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
    int sock, listener;
    sockaddr_in addr;

    u_int32_t addr_ip4;
    if (argc >= 2) {
        addr_ip4 = get_ip4(argv[1]);
    } else {
        std::cerr << "address to listen to: ";
        std::string st;
        std::cin >> st;
        char* s = &st[0];
        addr_ip4 = get_ip4(s);
    }

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

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
        sock = accept(listener, 0, 0);
        if (sock < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        if (inds.size() == 0) {
            inds.insert(indn++);
        }
        int ind = *inds.begin();
        inds.erase(inds.begin());
        std::thread([sock, ind]{
            while(true) {
                int read;
                std::cout << ind << " awaits" << std::endl;
                std::string st;
                st.resize(4096, '\0');
                read = recv(sock, &st[0], 4096, 0);
                if (read <= 0) {
                    std::cout << "connection #" << ind << " was closed";
                    break;
                }
                st[read] = 0;
                std::cout << ind << " has recieved: '" << st << "'" << std::endl;
                send(sock, st.data(), read, 0);
            }
            close(sock);
        }).detach();
    }    
    return 0;
}