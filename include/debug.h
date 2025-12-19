#pragma once

#if _DEBUG_COFFLOADER == 1

#include <stdio.h>

#define DBG "DBG"
#define ERR "ERR"
#define WRN "WRN"
#define INF "INF"

#define _log_no_format_coffloader(format, ...) printf(format, ## __VA_ARGS__);
#define _log_coffloader(level, format, ...) printf("[%s] %s:%d - " format "\n", level, __FUNCTION__, __LINE__, ## __VA_ARGS__);
#define _dbg_coffloader(format, ...) _log_coffloader(DBG, format, ## __VA_ARGS__)
#define _err_coffloader(format, ...) _log_coffloader(ERR, format, ## __VA_ARGS__)
#define _wrn_coffloader(format, ...) _log_coffloader(WRN, format, ## __VA_ARGS__)
#define _inf_coffloader(format, ...) _log_coffloader(INF, format, ## __VA_ARGS__)

#define _log_no_newline_coffloader(level, format, ...) printf("[%s] %s:%d - " format, level, __FUNCTION__, __LINE__, ## __VA_ARGS__);
#define _inf_no_newline_coffloader(format, ...) _log_no_newline_coffloader(INF, format, ## __VA_ARGS__)

#else

#define _log_no_format_coffloader(format, ...)
#define _log_coffloader(level, format, ...)
#define _dbg_coffloader(format, ...)
#define _err_coffloader(format, ...)
#define _wrn_coffloader(format, ...)
#define _inf_coffloader(format, ...)

#define _log_no_newline_coffloader(level, format, ...) 
#define _inf_no_newline_coffloader(format, ...) 

#endif