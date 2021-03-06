#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
using namespace boost::asio;
using namespace boost::posix_time;
using namespace std;
 io_service service;
void handle_connections() {
    char buff[1024];
    ip::udp::socket sock(service, ip::udp::endpoint(ip::udp::v4(), 8005));
    while ( true) {
        ip::udp::endpoint sender_ep;
        std::cout << "Wait Msg /n";
        int bytes = sock.receive_from(buffer(buff), sender_ep);
        std::string msg(buff, bytes);
        sock.send_to(buffer(msg), sender_ep);
    }
}
int main(int argc, char* argv[]) {
    std::cout << "Begin Server /n";
    handle_connections();
}


