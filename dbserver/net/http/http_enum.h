#pragma once 

#include <map>
enum HeaderState {
 	H_KEY = 0,
  	H_COLON,
  	H_VALUE,
  	H_CR,
  	H_LF,
  	H_END_CR,
  	H_END_LF
};

enum ParseState {
  	AGAIN = 0,
  	SUCCESS,
  	ERROR
};


enum ProcessState {
	PARSE_LINE,
	PARSE_HEADER,
	PARSE_BODY,
	PARSE_SUCCESS
};


enum HttpMethod {
	HTTP_GET = 0,
	HTTP_POST,
	HTTP_HEAD
};

const std::map<std::string, HttpMethod> g_HttpMethodMap
{
	{"GET", HTTP_GET},
	{"POST", HTTP_POST},
	{"HEAD", HTTP_HEAD}
};

enum HttpVersion {
	HTTP_10 = 0,
	HTTP_11
};

const std::map<HttpVersion, std::string> g_HttpVersionMap
{
	{HTTP_10, "HTTP/1.0"},
	{HTTP_11, "HTTP/1.1"}
};


enum HttpContentType {
	HTTP_FORM = 0,
	HTTP_MULTI_FORM,
	HTTP_TEXT_PLAIN,
	HTTP_TEXT_HTML,
	HTTP_JSON
};

const std::map<HttpContentType, std::string> g_HttpContentTypeMap
{
	{HTTP_FORM, "application/x-www-form-urlencoded"},
	{HTTP_MULTI_FORM, "multipart/form-data"},
	{HTTP_TEXT_PLAIN, "text/plain; charset=UTF-8"},
	{HTTP_TEXT_HTML, "text/html"},
	{HTTP_JSON, "application/json"}
};

enum HttpStatus {
	HTTP_OK = 0,
	HTTP_NOT_FOUND,
	HTTP_BAD_REQUEST,
	HTTP_BAD_GATEWAY
};


const std::map<HttpStatus, std::string> g_HttpStatusMap
{
	{HTTP_OK, "200 OK"},
	{HTTP_NOT_FOUND, "404 NOT FOUND"},
	{HTTP_BAD_REQUEST, "400 BAD REQUEST"},
	{HTTP_BAD_GATEWAY, "502 BAD GATEWAY"}
};


