

include_directories(${PROJECT_SOURCE_DIR})
add_executable(logger_test logger_test.cpp ../logger.cpp ../log_formatter.cpp)

target_link_libraries(logger_test pthread)

