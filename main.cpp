#define _USE_MATH_DEFINES
#define _WEBSOCKETPP_CPP11_THREAD_
#define _WEBSOCKETPP_CPP11_CHRONO_
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#define ASIO_STANDALONE
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_STD_ATOMIC
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

#include <asio.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/config/core.hpp>
#include <websocketpp/server.hpp>
#include <unordered_map>
#include <iostream>
typedef websocketpp::server<websocketpp::config::asio> server;
using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

class WS{
private:
    server m_server;
    std::map<connection_hdl, int, std::owner_less<connection_hdl>> holdThreads;
public:
	WS(){
		m_server.init_asio();
		m_server.set_open_handler(bind(&WS::on_open,this,::_1));
		m_server.set_close_handler(bind(&WS::on_close,this,::_1));
		m_server.set_message_handler(bind(&WS::on_message,this,::_1,::_2));
	}
    void on_open(connection_hdl hdl) {
        holdThreads[hdl]=0;//just an example.  Creates a new item for every connection.  Gets deleted on close
        m_server.send(hdl,"message", websocketpp::frame::opcode::text);
	}
	void on_close(connection_hdl hdl) {
        holdThreads.erase(hdl); //remove connection from map
	}
	void on_message(connection_hdl hdl, server::message_ptr msg) {//make sure to start a new thread for long running processes!
        std::cout<<msg->get_payload()<<std::endl;
        //std::thread myThread(&somefunction, this, hdl, msg->get_payload());
    }
	void run(uint16_t port) {
		m_server.listen(port);
		m_server.start_accept();
		m_server.run();
	}
};
int main(){
    WS server;
    server.run(9000);//give port to the program
}