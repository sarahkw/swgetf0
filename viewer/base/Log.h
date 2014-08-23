#ifndef __BASE__LOG_H__
#define __BASE__LOG_H__

#include <iostream>

#define VIEWER_LOG_DEBUG std::cout << "[VIEWER:DEBUG " __FILE__ ":" << __LINE__ << "] "
#define VIEWER_LOG_INFO std::cout << "[VIEWER:INFO " __FILE__ ":" << __LINE__ << "] "
#define VIEWER_LOG_ERROR std::cerr << "[VIEWER:ERR " __FILE__ ":" << __LINE__ << "] "
#define VIEWER_LOG_END std::endl

#endif
