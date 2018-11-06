#include "cpprest/oauth2.h"
#include "cpprest/http_client.h"
#include "cpprest//http_listener.h"
#include <cpprest/filestream.h>
#include "Server.h"

pplx::task<void> request(web::uri address, method http_method = methods::GET);

void start_listener(web::uri address);

Server server;





