#include <thread>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>
#include "stdafx.hpp"
#include "QueryParser.hpp"


class Rest_Handler_Instance{
    public:
        Rest_Handler_Instance(utility::string_t url);
        pplx::task<void> open(){return m_listener.open();}
        pplx::task<void> close(){return m_listener.close();}

    private:
        void handle_get(web::http::http_request message);
        void handle_put(web::http::http_request message);
        void handle_post(web::http::http_request message);
        void handle_delete(web::http::http_request message);
        void handle_error(pplx::task<void>& t);
        web::http::experimental::listener::http_listener m_listener;
};