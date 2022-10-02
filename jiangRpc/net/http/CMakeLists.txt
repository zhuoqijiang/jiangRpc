set(HTTP_SRCS 
  http_server.cpp 
  http_handler.cpp 
  http_context.cpp
  )


add_library(jiangRpc_http ${HTTP_SRCS})
target_link_libraries(jiangRpc_http jiangRpc_net)

