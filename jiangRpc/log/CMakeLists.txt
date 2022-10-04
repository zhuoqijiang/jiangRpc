set (LOG_SRC
	logger.cpp
	log_formatter.cpp
)


add_library(jiangRpc_log ${LOG_SRC})

target_link_libraries(jiangRpc_log pthread)
