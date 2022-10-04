set(NET_SRCS 
	netfd.cpp 
	polldesc.cpp 
	netpoll.cpp 
	scheduler.cpp 
	processor.cpp 
	timer.cpp
)


add_library(jiangRpc_net ${NET_SRCS})
target_link_libraries(jiangRpc_net pthread boost_context jiangRpc_log)
