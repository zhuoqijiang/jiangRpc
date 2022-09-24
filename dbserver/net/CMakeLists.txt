set(NET_SRCS 
	netfd.cpp 
	polldesc.cpp 
	netpoll.cpp 
	scheduler.cpp 
	processor.cpp 
	timer.cpp
)


add_library(dbserver_net ${NET_SRCS})
target_link_libraries(dbserver_net pthread boost_context tcmalloc)
