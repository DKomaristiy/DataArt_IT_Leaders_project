
#include "R_server.h"
#include <cpprest/http_client.h>
#include <sys/time.h>

using namespace web::http::client;

pplx::task<http_response> make_task_request(http_client &client, method mtd, json::value const &jvalue)
{
   return (mtd == methods::GET || mtd == methods::HEAD) ? client.request(mtd, "") : client.request(mtd, "", jvalue);
}

void make_request(http_client &client, method mtd, json::value const &jvalue)
{
   struct timeval tBegin, tEnd;
   gettimeofday(&tBegin, NULL);
   gettimeofday(&tEnd, NULL);
   auto timebegin = tBegin.tv_sec;
   auto timeend = tEnd.tv_sec;
   uint32_t status = 0;
   int timeout;

   while (status != status_codes::OK && (timeout < 25))
   {
      gettimeofday(&tEnd, NULL);
      timeend = tEnd.tv_sec;
      timeout = timeend - timebegin;
      try
      {
         sleep(1);
         make_task_request(client, mtd, jvalue)

             .then([&status](http_response response)
                   {
                      status = response.status_code();
                      if (status == status_codes::OK)
                      {
                         return response.extract_json();
                      }
                      return pplx::task_from_result(json::value());
                   })

             .wait();
      }
      catch (web::http::http_exception &e)
      {
         std::cout << "HTTP Exception ::" << e.what() << "\nCode ::" << e.error_code();
      }
   }
}

map<utility::string_t, int> TableOfNumber;

map<utility::string_t, utility::string_t> dictionary;


void display_json(json::value const &jvalue, utility::string_t const &prefix)
{

   std::cout << "Json  disp" << endl;
   std::cout << prefix << endl;
   std::cout << jvalue.serialize() << endl;

}

void RServer::handle_get(http_request request)
{
   TRACE(L"\nhandle GET\n");

   auto answer = json::value::object();

   for (auto const &p : TableOfNumber)
   {
      answer[p.first] = json::value(p.second);
   }
   for (auto const &p : dictionary)
   {
      answer[p.first] = json::value::string(p.second);
   }

   display_json(json::value::null(), "R_get: ");
   display_json(answer, "S_get: ");

   request.reply(status_codes::OK, answer);
}

void handle_request(http_request request, function<void(json::value const &, json::value &)> action)
{
   auto answer = json::value::object();

   request
       .extract_json()
       .then([&answer, &action](pplx::task<json::value> task)
             {
                try
                {
                   auto const &jvalue = task.get();
                   display_json(jvalue, "R: ");

                   if (!jvalue.is_null())
                   {
                      action(jvalue, answer);
                   }
                }
                catch (http_exception const &e)
                {
                   wcout << e.what() << endl;
                }
             })
       .wait();

   display_json(answer, "S: ");

   request.reply(status_codes::OK, answer);
}

void RServer::handle_post(http_request request)
{
   TRACE("\nhandle POST\n");

   handle_request(request, [](json::value const &jvalue, json::value &answer)
                  {
                     for (auto const &e : jvalue.as_array())
                     {
                        if (e.is_string())
                        {
                           auto key = e.as_string();
                           auto pos = dictionary.find(key);

                           if (pos == dictionary.end())
                           {
                              answer[key] = json::value::string("<nil>");
                              json::value::string("dff");
                           }
                           else
                           {
                              answer[pos->first] = json::value::string(pos->second);
                           }
                        }
                     }
                  });
}

void RServer::handle_put(http_request request)
{
  // TRACE("\nhandle PUT\n");


   handle_request(request, [](json::value const &jvalue, json::value &answer)
                  {
                     for (auto const &e : jvalue.as_object())
                     {

                        if (e.second.is_string())
                        {
                           auto key = e.first;
                           auto value = e.second.as_string();

                           if (dictionary.find(key) == dictionary.end())
                           {
                              answer[key] = json::value::string("<put>");
                           }
                           else
                           {
                              answer[key] = json::value::string("<updated>");
                           }

                           dictionary[key] = value;
                        }
                        else if (e.second.is_integer())
                        {
                           auto key = e.first;
                           auto value = e.second.as_integer();

                           if (TableOfNumber.find(key) == TableOfNumber.end())
                           {
                              answer[key] = json::value::string("<put>");
                           }
                           else
                           {
                              answer[key] = json::value::string("<updated>");
                           }

                           TableOfNumber[key] = value;
                        }
                     }
                     display_json(jvalue,"Put"); //json_calc(jvalue); display_json
                  });
}

void RServer::handle_del(http_request request)
{
   TRACE("\nhandle DEL\n");

   handle_request(request, [](json::value const &jvalue, json::value &answer)
                  {
                     set<utility::string_t> keys;
                     for (auto const &e : jvalue.as_array())
                     {
                        if (e.is_string())
                        {
                           auto key = e.as_string();

                           auto pos = dictionary.find(key);
                           if (pos == dictionary.end())
                           {
                              answer[key] = json::value::string("<failed>");
                           }
                           else
                           {
                              answer[key] = json::value::string("<deleted>");
                              keys.insert(key);
                           }
                        }
                     }

                     for (auto const &key : keys)
                        dictionary.erase(key);
                  });
}

RServer::RServer()
{
}

RServer::RServer(utility::string_t url):m_listener(url)
{
    m_listener.support(methods::GET, std::bind(&RServer::handle_get, this, std::placeholders::_1));
    m_listener.support(methods::PUT, std::bind(&RServer::handle_put, this, std::placeholders::_1));
    m_listener.support(methods::POST, std::bind(&RServer::handle_post, this, std::placeholders::_1));
    m_listener.support(methods::DEL, std::bind(&RServer::handle_del, this, std::placeholders::_1));

   std::cout <<"RestServer " << "URL " << url <<  " Startup succes" << endl;

   std::string nameFileSer(1,url[url.size() - 1]);

           
}

RServer::~RServer()
{
}

void RServer::startServer()
{   
    std::string nameFileSer(1, m_listener.uri().to_string()[m_listener.uri().to_string().size() - 2]);
    file_in.open("../File" + nameFileSer + ".txt", std::ios::app);
    std::string line_1, line_2;
    if (file_in.is_open())
    {
       while (getline(file_in, line_1))
       {
          if (getline(file_in, line_2))
             TableOfNumber[line_1] = std::stoi(line_2);
       }      
    }

   m_listener.open().wait();
   while(true);
}
void RServer::stopServer()
{
   map<string, int>::iterator it;
   std::string nameFileSer(1, m_listener.uri().to_string()[m_listener.uri().to_string().size() - 2]);
   file_out.open("../File" + nameFileSer + ".txt"); //  file_out.open("../File" + nameFileSer + ".txt",std::ios::app);

   if (!file_out.is_open())
      cout << "Файл не открыт!\n\n"
           << endl;
   else
   {
      for (it = TableOfNumber.begin(); it != TableOfNumber.end(); it++)
      {
         file_out << (*it).first << endl
                  << (*it).second << endl;
         // cout<<(*it).first<<" -> "<<(*it).second<<endl;
      }
      file_out.close();
   }
   m_listener.close().wait();
}
vector <http_client> client_http;

void RServer::change_data(json::value jval, vector<string> &TableUrl, uint32_t cnt)
{
   for (auto const &e : jval.as_object())
   {
      auto key = e.first;
      auto value = e.second.as_integer();
      TableOfNumber[key] = value;
   }

   web::http::client::http_client_config cfg;
   cfg.set_timeout(std::chrono::seconds(30));
   std::vector<std::thread> t;

   if (cnt > 0 && (!Fl_client_created))
   {
      Fl_client_created = true;
      for (int i = cnt; i >= 0; i--)
      {
         if (m_listener.uri().to_string() != TableUrl[i])
         {
            http_client client(U(TableUrl[i]), cfg);

            client_http.push_back(client);
         }
      }
   }

   if (cnt > 0 && (Fl_client_created))
   {
      for (int i = cnt; i >= 0; i--)
      {
         if (m_listener.uri().to_string() != TableUrl[i])
         {                      
            std::thread th = std::thread(make_request,std::ref(client_http[i]), methods::PUT,std::ref(jval));
            t.push_back(std::move(th));


         }
      }

      for(auto& th : t)
      {              
         th.join();
      }

   }
}
