#include <cpprest/http_client.h>
#include <cpprest/json.h>
#pragma comment(lib, "cpprest_2_10")

using namespace web;
using namespace web::http;
using namespace web::http::client;

#include <iostream>
using namespace std;


void display_json_get(   json::value const & jvalue)
{
   
 auto numberOne = jvalue.at(U("oneParam")).as_integer();
 auto numberTwo = jvalue.at(U("threeParam")).as_integer();
 auto action = jvalue.at(U("twoParam")).as_string();
 auto answer = jvalue.at(U("Answer")).as_string();
 std::cout << numberOne << " " << action << " " <<  numberTwo  <<"Answer=" << answer << endl;
 
}

void display_json(   json::value const & jvalue,    utility::string_t const & prefix)
{
   std::cout << prefix << jvalue.serialize() << endl;
 
}


pplx::task<http_response> make_task_request( http_client & client, method mtd, json::value const & jvalue)
{
   return (mtd == methods::GET || mtd == methods::HEAD) ?
      client.request(mtd, "/restdemo") :
      client.request(mtd, "/restdemo", jvalue);
}
method mtdGlob;
void make_request(   http_client & client,    method mtd,    json::value const & jvalue)
{ 
   mtdGlob = mtd;
   make_task_request(client, mtd, jvalue)
      .then([](http_response response)
      {
         if (response.status_code() == status_codes::OK)
         {
            return response.extract_json();
         }
         return pplx::task_from_result(json::value());
      })
      .then([](pplx::task<json::value> previousTask)
      {
         
         try
         {
           
           // if(mtdGlob==(methods::GET))
          //  display_json_get(previousTask.get());
           // else
             display_json(previousTask.get(), "R: ");
            
         }
         catch (http_exception const & e)
         {
            wcout << e.what() << endl;
         }
      })
     
      .wait();

  }

int main()
{
   http_client client(U("http://localhost:2424"));
   std::cout << "Start client" << endl;

   for(;;)
{
   std::cout << "Enter a number: " ;
   utility::stringstream_t ss1;
   int num, numTwo;
   string action;
   std::cin >> num;
   std::cout << "Enter action: " ;
   std::cin >> action;
   std::cout << "Enter a number: " ;
   std::cin >> numTwo;
   ss1 << num;
   json::value v1 = json::value::parse(ss1);
   ss1 << numTwo;
   json::value v2 = json::value::parse(ss1);

   auto putvalue = json::value::object();
   putvalue["oneParam"] = json::value(v1); // string("100");
   putvalue["twoParam"] = json::value::string(action);
   putvalue["threeParam"] = json::value(v2);
   putvalue["Answer"] = json::value();
   wcout << L"\nPUT (add values)\n";
   display_json(putvalue, "S: ");
   make_request(client, methods::PUT, putvalue);
  
  // GET
   auto nullvalue = json::value::null();

   wcout << L"\nGET (get all values)\n";
  // display_json(nullvalue, "S: ");
  
   make_request(client, methods::GET, nullvalue);

  //std::cout << "For resume, enter 'r' "  << endl;
   std::cout << "For exit, enter 'e'! Else enter any key "  << endl;
   std::cin >> action;
   if(action=="e")
    break;
      
}

 

/*
// Post
  /*auto getvalue = json::value::array();
   getvalue[0] = json::value::string("oneParam");
   getvalue[1] = json::value::string("twoParam");
   getvalue[2] = json::value::string("threeParam");

   wcout << L"\nPOST (get some values)\n";
   display_json(getvalue, "S: ");
   make_request(client, methods::POST, getvalue);
*/
/*
   auto putvalue = json::value::object();
   putvalue["one"] = json::value::string("100");
   putvalue["two"] = json::value::string("200");

   wcout << L"\nPUT (add values)\n";
   display_json(putvalue, "S: ");
   make_request(client, methods::PUT, putvalue);

   auto getvalue = json::value::array();
   getvalue[0] = json::value::string("one");
   getvalue[1] = json::value::string("two");
   getvalue[2] = json::value::string("three");

   wcout << L"\nPOST (get some values)\n";
   display_json(getvalue, "S: ");
   make_request(client, methods::POST, getvalue);

   auto delvalue = json::value::array();
   delvalue[0] = json::value::string("one");

   wcout << L"\nDELETE (delete values)\n";
   display_json(delvalue, "S: ");
   make_request(client, methods::DEL, delvalue);

   wcout << L"\nPOST (get some values)\n";
   display_json(getvalue, "S: ");
   make_request(client, methods::POST, getvalue);

   auto nullvalue = json::value::null();

   wcout << L"\nGET (get all values)\n";
   display_json(nullvalue, "S: ");
   make_request(client, methods::GET, nullvalue);
*/
   return 0;
}