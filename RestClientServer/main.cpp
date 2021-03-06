#include "R_server.h"
#include <csignal>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
namespace bpt = boost::property_tree;

RServer *serversig;

void signalHandler( int signum ) {

   auto id = getpid();
   http::uri url =  serversig->m_listener.uri();
   cout << "Id procces(" << id  << ") URL-" << url.to_string() << " Interrupt signal (" << signum << ") received.\n";

   serversig->stopServer();
  

   exit(signum);  
}


void thread_start_server( RServer *server)
{
   server->startServer();
}
void fork_create_server(string url, vector <string> &TableUrl, uint32_t cnt)
{
   RServer server(url);
   serversig = &server;  
   signal(SIGINT, signalHandler);  

   std::ifstream ifs{"../in.json"};
   bpt::ptree pt;
   bpt::read_json(ifs, pt);      
   int valSleep = pt.get<int>("Port.Sleep_"+url,2);
   sleep(valSleep);

   std::thread tser(thread_start_server, &server); 
   
   auto putvalue = json::value::object();
   putvalue["fork"] = json::value(getpid());
  // server.change_data(putvalue, TableUrl, cnt);
   while(1);
   sleep(30);
   server.stopServer();

   tser.join();

}

int main()
{
   string action;
   vector <string> TableUrl(5);
   pid_t ser_1, ser_2, ser_3;
   
   std::ifstream file("../urlAddr.txt");
   std::string line;
   uint32_t cnt = 0;
   while (std::getline(file, line))
   {
      TableUrl[cnt] = line;
      cnt++;
   }
   cnt--;

   pid_t  ser[cnt];


   
   for (int i = cnt ; i >= 0; i--)
   {
      ser[i] = fork();
      switch (ser[i])
      { 
      case -1:
         std::cout << "fork error" << endl;
         return -1;
      case 0:
         if (i != 0)
         {
            fork_create_server(TableUrl[i],TableUrl,cnt);
         }
         break;
      default:
         if (i == 0)
         {
            RServer server(TableUrl[i]);
            serversig = &server;
            signal(SIGINT, signalHandler);
            std::ifstream ifs{"../in.json"};
            bpt::ptree pt;
            bpt::read_json(ifs, pt);
            int valSleep = pt.get<int>("Port.Sleep_" + TableUrl[i], 2);
            sleep(valSleep);

            std::thread tser(thread_start_server, &server);

            auto putvalue = json::value::object();
            /*       putvalue["one"] = json::value(455);
            putvalue["two"] = json::value(222);
            putvalue["three"] = json::value(333);
            server.change_data(putvalue, TableUrl, cnt);*/
            struct timeval tBegin, tEnd;
            gettimeofday(&tBegin, NULL);
            
            auto timebegin = tBegin.tv_sec;
           

            putvalue["Street"] = json::value(12);
            putvalue["home"] = json::value(15);

            for (uint32_t i = 0; i < 1000; i++)
            {
               server.change_data(putvalue, TableUrl, cnt);
               putvalue["Street"] = json::value( i);
               putvalue["home"] = json::value( i);
            }
            gettimeofday(&tEnd, NULL);
            auto timeend = tEnd.tv_sec;

            std::cout << "Time sending = " << (timeend - timebegin) << endl;

            for (;;)
            {
               std::cout << "Enter K to complete fork server" << endl;
               std::cin >> action;
               if (action == "K" || action == "k")
                  break;
            }

            for (int j = cnt; j > 0; j--)
               kill(ser[j], SIGINT); // kill(ser[j], SIGKILL);
            std::cout << "Servers were closed" << endl;
            
            tser.join();
         }
         break;
      }
   }

   return 0;
}
