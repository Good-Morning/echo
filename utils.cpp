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

std::string operator ""_str(const char* st, const size_t size) {
    return std::string(st);
}

u_int8_t get_byte(const std::string& st, int& index) {
    if (index == st.size()) {
        throw std::invalid_argument("Wrong address format");
    }
    u_int8_t res = 0;
    while(true) {
        if (st[index] != 0) {
            char c = st[index];
            index++;
            if (c >= '0' && c <= '9') {
                res = res * 10;
                res += c - '0';
            } else {
                if (c != '.') {
                    throw std::invalid_argument("Wrong address format");
                } else {
                    return res;
                }
            }
        } else {
            return res;
        }
    }
}

u_int32_t get_ip4(const std::string& st) {
    int index = 0;
    u_int32_t res = 0;
    for (int i = 0; i < 4; i++) {
        res <<= 8;
        res |= get_byte(st, index);
    }
    return res;
}

void _send(int socket, const std::string& st) {
    size_t left = st.size();
    size_t sent = 0;
    while (left) {
        const char* current_start = st.data() + sent;
        ssize_t res = send(socket, current_start, left, 0);
        if (-1 == res) {
            throw std::runtime_error("While sending: '"_str + std::string(st, 0, sent) + "' an error occured: " + strerror(errno));
        } else {
            sent += res;
            left -= res;
        }
    }
}

void blocking_send(int socket, const std::string& st) {
    if (st.size() > 120) {
        throw std::runtime_error("'"_str + st + "' is too large to be sent");
    }
    u_int8_t size = st.size();
    const char* size_data = (const char*)(void*)&size;
    _send(socket, std::string(size_data, 0, sizeof(u_int8_t)));
    _send(socket, st);
}

socket_t::socket_t() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        throw std::runtime_error("While setting socket an error occurred: "_str + strerror(errno));
    }
}

socket_t::socket_t(int _sock) {
    sock = _sock;
}

socket_t::~socket_t() {
    close(sock);
}

socket_t::operator int() {
    return sock;
}

const char* _recv(int socket, size_t size) {
    static char buffer[4096];
    if (size > 4000) {
        throw std::invalid_argument("inner error");
    }
    size_t received = 0;
    size_t left = size;
    while (left) {
        ssize_t res = recv(socket, buffer + received, left, 0);
        if (-1 == res) {
            if (errno == EAGAIN || errno == EWOULDBLOCK){
                throw 42;
            }
            throw std::runtime_error("While receiving: '"_str + std::string(buffer, 0, received) + "' an error occured: " + strerror(errno));
        } else {
            received += res;
            left -= res;
        }
    }
    return buffer;
}

std::string blocking_recv(int socket) {
    size_t left = 0;
    size_t received = 0;
    u_int32_t size;
    while (true) {
        try {
            size = *(u_int8_t*)_recv(socket, sizeof(u_int8_t));
            break;
        } catch (int) {}
    }
    left = size;
    std::string res;
    while (left) {
        size_t cur_size = std::min(left, (size_t)4000);
        left -= cur_size;
        received += cur_size;
        const char* bit = _recv(socket, cur_size);
        res += std::string(bit, 0, cur_size);
    }
    return res;
}
