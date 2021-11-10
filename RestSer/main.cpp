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
//#include <cstring>
using namespace std;

#define TRACE(msg)            wcout << msg

map <string, int> TableOfNumber; 

map<utility::string_t, utility::string_t> dictionary;

void json_calc(json::value const & jvalue)
{
   
 auto numberOne = jvalue.at(U("oneParam")).as_integer();
 auto numberTwo = jvalue.at(U("threeParam")).as_integer();
 auto action = jvalue.at(U("twoParam")).as_string();
 std::cout << numberOne << " " << action << " " <<  numberTwo << endl;
 
 auto answer = 0;

  if(action=="+")
     answer = numberOne + numberTwo; 
  else if (action=="-")
     answer = numberOne - numberTwo; 
  else if (action=="*")
     answer = numberOne * numberTwo;    
  else if (action=="/")
     answer = numberOne / numberTwo; 
   else
     std::cout << "Неопределенное действие";

/*
 auto putvalue = json::value::object();
 putvalue["Answer"] = json::value(answer);
*/
TableOfNumber.at(U("Answer")) = answer;

 std::cout << answer << endl;
  
}
void display_json(json::value const & jvalue,utility::string_t const & prefix)
{
   
 
std::cout << "Json disp" << endl;
std::cout << prefix << endl;
std::cout << jvalue.serialize() << endl ;
/*
std::cout << "One=" << TableOfNumber["oneParam"];
std::cout << "Two=" << dictionary["twoParam"];
std::cout << "Three=" << TableOfNumber["threeParam"] << endl ;
 */
}

void handle_get(http_request request)
{
   TRACE(L"\nhandle GET\n");

   auto answer = json::value::object();


   for (auto const & p : TableOfNumber)
   {
      answer[p.first] = json::value(p.second);
   }
   for (auto const & p : dictionary)
   {
      answer[p.first] = json::value::string(p.second);
   }

   display_json(json::value::null(), "R: ");
   display_json(answer, "S: ");

   request.reply(status_codes::OK, answer);
}

void handle_request( http_request request, function<void(json::value const &, json::value &)> action)
{
   auto answer = json::value::object();

   request
      .extract_json()
      .then([&answer, &action](pplx::task<json::value> task) {
         try
         {
            auto const & jvalue = task.get();
            display_json(jvalue, "R: ");
            

            if (!jvalue.is_null())
            {
               action(jvalue, answer);
            }
         }
         catch (http_exception const & e)
         {
            wcout << e.what() << endl;
         }
      })
      .wait();

 
   display_json(answer, "S: ");
   

   request.reply(status_codes::OK, answer);
}

void handle_post(http_request request)
{
   TRACE("\nhandle POST\n");

   handle_request(request,[](json::value const & jvalue, json::value & answer)
   {
      for (auto const & e : jvalue.as_array())
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

void handle_put(http_request request)
{
   TRACE("\nhandle PUT\n");

   handle_request( request,[](json::value const & jvalue, json::value & answer)
   {
      
      for (auto const & e : jvalue.as_object())
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
           
          if( TableOfNumber.find(key) == TableOfNumber.end())           
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
      json_calc(jvalue);
   });
}

void handle_del(http_request request)
{
   TRACE("\nhandle DEL\n");

   handle_request(request, [](json::value const & jvalue, json::value & answer)
   {
      set<utility::string_t> keys;
      for (auto const & e : jvalue.as_array())
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

      for (auto const & key : keys)
         dictionary.erase(key);
   });
}

int main()
{
  
   std::cout << "main" << endl; 
   http_listener listener("http://localhost:2424");
   listener.support(methods::GET,  handle_get);
   listener.support(methods::POST, handle_post);
   listener.support(methods::PUT,  handle_put);
   listener.support(methods::DEL,  handle_del);

   wcout << L"\nStart Server" << endl; 

   try
   {
      listener
         .open()
         .then([&listener]() {TRACE(L"\nstarting to listen\n"); })
         .wait();

      while (true);
   }
   catch (exception const & e)
   {
      wcout << e.what() << endl;
   }

   return 0;
}
