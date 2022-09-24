set(HTTP_SRCS 
  http_server.cpp 
  http_handler.cpp 
  http_context.cpp
  )


add_library(dbserver_http ${HTTP_SRCS})
target_link_libraries(dbserver_http dbserver_net)

