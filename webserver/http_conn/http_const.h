#ifndef HTTP_CONST_H
#define HTTP_CONST_H
enum METHOD { GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT, PATCH };
enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER, CHECK_STATE_CONTENT };
enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOURCE, FORBIDDEN_REQUEST, FILE_REQUEST, INTERNAL_ERROR};
enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN };
const char* ok_200_title = "OK";
const char* error_400_title = "Bad Request";
const char* error_400_form1 = "Client does not exist.\n";
const char* error_400_form = "Your request has bad syntax or is inherently impossible to satisfy.\n";
const char* error_403_title = "Forbidden";
const char* error_403_form = "You do not have permission to get file from this server.\n";
const char* error_404_title = "Not Found";
const char* error_404_form = "The requested file was not found on this server.\n";
const char* error_500_title = "Internal Error";
const char* error_500_form = "There was an unusual problem serving the requested file.\n";
const char* doc_root = "/var/www/html";
static const int FILENAME_LEN = 200;
static const int READ_BUFFER_SIZE = 2048;
static const int WRITE_BUFFER_SIZE = 1024;
#endif