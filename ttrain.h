// typeTraining header
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include <fcntl.h>
#include <unistd.h>


#ifdef LIN
#   include "sys_depend/time_LIN.h"
#elif defined WIN
#   include "sys_depend/time_WIN.h"
#else

#endif

#include <sys/time.h>


#include <curses.h>


#include <deque>
#include <fstream>
#include <iostream>
#include <string>


class cursesException
{    
};

#define ex_ERR(x) if (x==ERR) throw cursesException()
