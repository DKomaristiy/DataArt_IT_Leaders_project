#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#pragma comment(lib, "cpprest_2_10")

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <thread>
using namespace std;


#define TRACE(msg)            wcout << msg

class RServer 
{
public:
	RServer();
	RServer(utility::string_t url);
	~RServer();
	void startServer();
	void stopServer();
	void change_data(json::value jval, vector <string> &TableUrl, uint32_t cnt);
	ofstream file_out;
	ifstream file_in;

	//pplx::task<void>open(){return m_listener.open();}
	//pplx::task<void>close(){return m_listener.close();}
	http_listener m_listener;
	
private:
	void handle_get(http_request request);
	void handle_post(http_request request);
	void handle_put(http_request request);
	void handle_del(http_request request);
	bool Fl_client_created = false;

};
