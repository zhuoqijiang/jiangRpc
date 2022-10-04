#include "../logger.h" 


void test()
{
	for (int i = 0; i < 50000; i++) {
		LOG_DEBUG("DEBUG %d", i);
	}
}

int main()
{
	g_logger.setLevel(jiangRpc::Logger::Level::DEBUG);
	g_logger.setFile("./test.txt");
	
	std::thread threadA(test), threadB(test), threadC(test);
	
	threadA.join();
	threadB.join();
	threadC.join();
	while(1);
}
