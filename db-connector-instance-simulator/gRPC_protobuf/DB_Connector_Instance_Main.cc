// C++ header include
#include <iostream>
#include <memory>
#include <string>

// db connect instance include
#include "stdafx.h"
#include "DB_Connector_Instance.h"
#include "Query_Planner.h"
#include "Meta_Data_Manager.h"
#include "Plan_Executer.h"
#include "Storage_Engine_Interface.h"
#include "Parsed_Query.h"

std::unique_ptr<DB_Connector_Instance> g_httpHandler;

void on_shutdown()
{
	g_httpHandler->close().wait();
    return;
}

int main(int argc, char *argv[])
{
    utility::string_t port = U("40000");
    if(argc == 2)
    {
        port = argv[1];
    }

    utility::string_t address = U("http://0.0.0.0:");
    address.append(port);

    web::uri_builder uri(address);  

    auto addr = uri.to_uri().to_string();
     g_httpHandler = std::unique_ptr<DB_Connector_Instance>(new DB_Connector_Instance(addr));
     g_httpHandler->open().wait();

    ucout << utility::string_t(U("Listening for requests at: ")) << addr << std::endl;

    std::cout << "Press ENTER to exit." << std::endl;

    std::string line;
    std::getline(std::cin, line);

    on_shutdown();
    cout << "end " << endl;
    return 0;
}
