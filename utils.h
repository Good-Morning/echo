#include <sys/types.h>

#include <string>

std::string operator ""_str(const char*, const size_t);

u_int32_t get_ip4(const std::string& st);

void blocking_send(int socket, const std::string& st);

std::string blocking_recv(int socket);

class socket_t {
    int sock;
public:
    socket_t();
    socket_t(int sock);
    ~socket_t();
    operator int();
};