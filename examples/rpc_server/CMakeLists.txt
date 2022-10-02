

add_custom_command(
	OUTPUT test.pb.h test.pb.cc 
	COMMAND protoc -I=./ --cpp_out=./ test.proto 
	DEPENDS test.proto 
)



add_executable(rpc_server server.cpp test.pb.cc)

target_link_libraries(rpc_server jiangRpc_http protobuf)
