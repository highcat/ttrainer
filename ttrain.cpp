
#include "ttrain.h"
///***************************** 09-11-2006: added real number of writed characters,
///***************************** not very exact - completed size of typed text ( because of "total symb count" )
///*****************************

#define END 1
#define END_ERR 2

//#define DEBUG

#ifdef DEBUG
#define DEB_HIDE 
#else
#define DEB_HIDE 
#endif

/// not used??:
#define  START_COUNTER_MAX 50 // take care of n first typed symbols for count current speed 


inline void drawOriginalText();
inline void drawTypedText();
inline void drawStatistics();
inline int handleTyped(int symb);
inline void initTyped();

    
bool auto_indent_after_error = true;


std::ifstream *inFile;
std::deque<std::string> *tqueue;


int indentSize = 4;


int currLine; // screen cursor coordinates
int currChar; // real cursor coordinates
int verticalFields = 7;
int rightField = 10;
int statisticsWidth = 13;


int errors = 0;
int maxErrors;
int totalSymbCount; // plain size of used file


double timeSum = 0; // summary typing time, excluding pauses, in milliseconds
int symbNum = 0; // number of typed symbols (including errors, symbols just before returning to the beginning of string)
int realTypedSymbNum = 0; // where you now - useful typed symbols
double nstopped_time_period = 4000; // after this period time counting stops


#define L_CH_D_SIZE 15
int l_ch_d_current = 0;
double l_ch_duration[L_CH_D_SIZE];


const int symbPerOneErrorCount(80*25); // symbols for one error: 80x25
//const int symbPerOneErrorCount(8); // symbols for one error: 80x25


int countSize(char *filename);

void draw_all();

int main(int args, char *argv[])
{
	// process starting errors
	if (argv[1]==NULL)
	{		
		fprintf(stderr, "no input file specified\n");
		exit(1);
	
	}

	//////
	// check file existence
	// 
	FILE *file = fopen(argv[1], "r");
	if ( file == NULL )
	{
		fprintf(stderr, "can't open file \"");
		fprintf(stderr, argv[1]);
		fprintf(stderr, "\": ");
		perror("");
		exit(1);
        
	}
	fclose(file);
	//
	//////


	totalSymbCount = countSize(argv[1]);
	maxErrors = totalSymbCount/symbPerOneErrorCount;
	
	std::string s;
    
	/////////
    
	try
	{   // curses start
		initscr();
		start_color();
		init_pair(1,COLOR_GREEN,COLOR_WHITE); // cursor color)
		init_pair(2,COLOR_RED,COLOR_WHITE); // cursor color)
		init_pair(3,COLOR_BLACK,COLOR_RED); // error color)
		init_pair(4,COLOR_WHITE,COLOR_BLACK); // common color)
		init_pair(5,COLOR_RED,COLOR_BLACK); // error2 color)

		cbreak();
		nodelay(stdscr, true); // non-blocking mode;
		intrflush(stdscr, true);
		noecho();
		curs_set(0);
	
		if (COLS-statisticsWidth-1-rightField<=0)
		{
			fprintf(stderr, "terminal length too small!\n");	    
			endwin();
		}

	
		inFile = new std::ifstream(argv[1]);
		tqueue = new std::deque<std::string>;

		initTyped();
		int state;


		////////////////////////////
		// main typing cycle
		// 
		double lastTime = get_time_milisec(); 
		do 
		{
			////////////////////////
			///// getting character
			/////
			int key;
			while (getch()!=ERR);  // read if there was a trash in buffer

			nodelay(stdscr, false); // blocking mode
			while( (key = getch()) == KEY_RESIZE ) // read a char
				draw_all();

			// NEXT: add window size change check
			nodelay(stdscr, true); // return to non-blocking
			/////
			/////
			////////////////////////

			////////////////////////
			///// handling time
			/////
			double time_ = get_time_milisec();
			if (time_-lastTime <= nstopped_time_period)
			{
				timeSum += time_-lastTime;
				l_ch_duration[l_ch_d_current] = time_-lastTime;
				if (l_ch_d_current==L_CH_D_SIZE-1)
					l_ch_d_current = 0;
				else
					l_ch_d_current++;
			}

			lastTime = time_;
			/////
			/////
			////////////////////////

			state = handleTyped(key);

			if (errors>maxErrors)
				state=END_ERR;
			
		} while(state!=END && state!=END_ERR);
		
	
		delete inFile;
		delete tqueue;
	
		endwin();


		///////
		// save log file
		//
#if defined LIN
		char *home = getenv("HOME");
#elif defined WIN
		char *home = getenv("HOMEPATH");
#else
#error unknown os!
#endif

		if (home!=NULL)
		{
			char *f_name = new char[strlen(home)+11+1];
			strcpy(f_name, home);
			strcat(f_name, "/ttrain.log");
			FILE *log = fopen(f_name, "a");
	
			if ( log == NULL )
			{
				fprintf(stderr, "can't open/create log file \"%s\": ", f_name);
				perror("");
			}
			else
			{
				char s[200]; // will contain log;  XXX: just big size
				struct tm *t_;
				time_t t_curr;
				time(&t_curr);
				t_ = localtime(&t_curr);
				char *c = asctime(t_);
	    	
				c[strlen(c)-1]='\0';    
				sprintf(s, "%s: ", c);
		
				char sp[200]; // XXX: just big size
		
				sprintf(sp, "%s", argv[1]);
				strcat(s, argv[1]); // add filename

				// add speed
				int x;
				if ((x = (int)(symbNum/(timeSum/60./1000.))) <0)
					x = 0;
				sprintf(sp, ": %4d sybm/sec", x);
				strcat(s, sp);
				strcat(s, ": ");
		
				sprintf(sp, "%6d symbols,", totalSymbCount);
				strcat(s, sp);
				if ( state==END_ERR )
				{
					printf("too many errors!\n");
					char err_s[20];
					sprintf(err_s, "%3d%% typed: ", (int)(100.*realTypedSymbNum/totalSymbCount));
					strcat(s, err_s);
					strcat(s, "too many errors ");
					sprintf(err_s, " (%d max)", maxErrors);
					strcat(s, err_s);		    
				}
				else
				{
#ifdef WIN
					printf("100% printed!\n");
#endif
					strcat(s, " ok");
				}
				
				strcat(s, "\n");

				fprintf(log, "%s", s);
				fclose(log);
			}
			delete f_name;
		}
		else
			fprintf(stderr, "can't open/create log file : environment variable \"HOME\" not exist\n");

	}
	catch (cursesException)
	{
		endwin();
		fprintf(stderr, "curses error\n");
		exit(1);
	}
        
	
	return 0;    
}



inline int countSize(char *filename)
{
    int sum=0;
    std::ifstream *inFile = new std::ifstream(filename);
    std::string str;
    std::string newStr;

    while (getline(*inFile, str))
    {
	newStr.erase();
	// removing spaces and tabs after lines!
	while (str[str.size()-1]==' ' || str[str.size()-1]==0x0a || str[str.size()-1]==0x09)
	    str.resize(str.size()-1);

	// replacing indents with indentSize spaces!
	std::string::iterator strIter = str.begin();
	while (strIter!=str.end())
	{
	    if (*strIter==0x0a || *strIter==0x09)
	    {
		newStr.append(indentSize*2, ' ');
	    }
	    else newStr.append(1, *strIter);
		
	    strIter++;
	}
	// add to length
	sum+=newStr.size()+1;
//	printf("string \"%s\"      size=%d\n", newStr.c_str(), newStr.size());
    }
    delete inFile;
//    printf("%d", sum);
//    fflush(stdout);
//    sleep(3);
    return sum;
}



inline int calcTextHeight()
{
    return (LINES-1)/2;
}

inline int calcTextWidth()
{
    return COLS-statisticsWidth;
}

inline int calcStatWidth()
{
    return COLS - calcTextWidth();
}



static std::string *currentString;
char currentSymb; // (wrong)symbol that you just have printed




inline void drawOriginalText()
{
	std::deque<std::string>::iterator tqueIter;
	tqueIter = tqueue->begin();
    

	int i=0;   

	int offset;
	if (currChar>=COLS-statisticsWidth-rightField)
		offset = currChar-(COLS-statisticsWidth-1-rightField);
	else
		offset = 0;
    
        // draw line before cursos 
	while(i!=currLine && tqueIter!=tqueue->end())
	{
	
		// string is longer then screen length
		if (tqueIter->size()>offset)
		{
			char *t = new char[COLS-statisticsWidth];
			const char *queMid = tqueIter->c_str() + offset;
			strncpy(t, queMid, COLS-statisticsWidth-1);
			t[COLS-statisticsWidth-1]='\0';  // solved: must place '\0' every time!: strange! seems like an error somewere before, I think I don't need this line
			mvaddstr(i, 0, t);
			delete t;
		}
		
		tqueIter++;
		i++;
	}

	{   // draw line with cursor
		const char *c = tqueIter->c_str() + offset;
	
		int j;
		for (j=0; j<currChar-offset; j++)
			mvaddch(i, j, c[j]);
		attron(COLOR_PAIR(1));
	
		if (currChar<currentString->size()) /// end of line - add empty char; spaces after every line must be deleted!
			mvaddch(i, j, c[j]);
		else
			mvaddch(i, j, ' ');
		j++;
		attroff(COLOR_PAIR(1));
	
		for (; j< ((currentString->size() < offset+COLS-statisticsWidth-1) ? currentString->size()-offset : COLS-statisticsWidth-1); j++)
			mvaddch(i, j, c[j]);
	
		tqueIter++;
		i++;
	}
        // draw lines after cursor
	while(tqueIter!=tqueue->end())
	{
		if (tqueIter->size()>offset)
		{
			char *t = new char[COLS-statisticsWidth];
			char *queMid = (char*)(tqueIter->c_str()) + offset;
			strncpy(t, queMid, COLS-statisticsWidth-1);
			t[COLS-statisticsWidth-1]='\0';  // solved: must place '\0' every time!: strange! seems like an error somewere before, I think I don't need this line
			mvaddstr(i, 0, t);
			delete t;
		}

		tqueIter++;
		i++;
	}
    
}

inline void drawTypedText()
{
	int voffset = calcTextHeight()+2;

	std::deque<std::string>::iterator tqueIter;
	tqueIter = tqueue->begin();

	int i=0;   

	int offset;
	if (currChar>=COLS-statisticsWidth-rightField)
		offset = currChar-(COLS-statisticsWidth-1-rightField);
	else
		offset = 0;
    
        // draw line before cursos 
	while(i!=currLine && tqueIter!=tqueue->end())
	{
	
		// string is longer then screen length
		if (tqueIter->size()>offset)
		{
			char *t = new char[COLS-statisticsWidth];
			const char *queMid = tqueIter->c_str() + offset;
			strncpy(t, queMid, COLS-statisticsWidth-1);
			t[COLS-statisticsWidth-1]='\0';  // solved: must place '\0' every time!: seems like an error somewere before, I think I don't need this line
			mvaddstr(i+voffset, 0, t);

			delete t;
		}
		
		tqueIter++;
		i++;
	}

	{   // draw line with cursor
		const char *c = tqueIter->c_str() + offset;
	
		int j;
		for (j=0; j<currChar-offset; j++)
			mvaddch(i+voffset, j, c[j]);
		attron(COLOR_PAIR(2));
	
		mvaddch(i+voffset, j, currentSymb);
		j++;
		attroff(COLOR_PAIR(2));
	}
    
}



inline void drawDelimiters()
{
    int voffset = calcTextHeight()+1;
    
    attron(COLOR_PAIR(1));
    for (int i=0; i<COLS-statisticsWidth; i++)
    {
	mvaddch(voffset, i, ' ');
    }

    for (int i=0; i<LINES; i++)
    {
	mvaddch(i, COLS-statisticsWidth-1, ' ');
    }
    
    attroff(COLOR_PAIR(1));
}


static double last_current_speed;
static int startCounter;



inline void drawStatistics()
{
    char s[50];
    sprintf(s, "errors: %3d", errors);
    mvaddstr(1, COLS-statisticsWidth+1, s);
    
    sprintf(s, "max: %6d", maxErrors);
    mvaddstr(3, COLS-statisticsWidth+1, s);
    
    //sprintf(s, "tsum:%7d", (long)timeSum);
    //mvaddstr(5, COLS-statisticsWidth+1, s);

    //

    mvaddstr(9, COLS-statisticsWidth, "current speed");
    // compute number of typed symbols for using in current speed
    static int num;
    static int numCounter;
    static int nPoint;
    static double current_sum;

    num = 5;

    // take care of first typed symbols
    if (startCounter<START_COUNTER_MAX)
    {
	if (num>startCounter-1)
	    num=startCounter;
	startCounter++;
    }
    
#ifdef DEBUG
    sprintf(s, "%d", num);
    mvaddstr(1, 80, s);
#endif

    if (num>0)
    {
	numCounter = num;
	
	if (l_ch_d_current!=0)
	    nPoint = l_ch_d_current-1;
	else
	    nPoint = L_CH_D_SIZE-1;
	
	current_sum = 0;
	
#ifdef DEBUG
	sprintf(s, "%d", numCounter);
	mvaddstr(4+num-numCounter, 80, s);

	sprintf(s, "nPoint=%d", nPoint);
	mvaddstr(4, 95, s);
	
	sprintf(s, "l_ch_d_current=%d", l_ch_d_current);
	mvaddstr(5, 95, s);

	sprintf(s, "numCounter=%d", numCounter);
	mvaddstr(6, 95, s);
#endif
	while (numCounter!=-1)
	{
	    if (l_ch_duration[nPoint]>0)
		current_sum += l_ch_duration[nPoint];
	    
#ifdef DEBUG
	    sprintf(s, "%f-%d", l_ch_duration[nPoint], nPoint);
	    mvaddstr(5+nPoint, 100, s);
	    sprintf(s, "nPoint=%d", nPoint);
	    mvaddstr(6, 50, s);
	    
	    sprintf(s, "arr[nPoint]=%f", l_ch_duration[nPoint]);
	    mvaddstr(7, 50, s);

	    refresh();
#endif 

	    if (nPoint)
		nPoint--;
	    else
		nPoint = L_CH_D_SIZE -1;
	    
	    numCounter--;
	}
	
#ifdef DEBUG
	sprintf(s, "%f", current_sum);
	mvaddstr(2, 80, s);

	for (int i=0; i<L_CH_D_SIZE; i++)
	{
	    sprintf(s, "%f", l_ch_duration[i]);
	    if (i==l_ch_d_current) strcat(s, " <");
	    mvaddstr(5+i, 80, s);
	}
#endif

	if (current_sum<0.1)
	{
	    last_current_speed = 0;
	}
	else
	    last_current_speed = (double)num/(current_sum/60./1000.);
    }
    else last_current_speed = 0;

    ////
    sprintf(s, "  %9d", (int)last_current_speed);
    mvaddstr(10, COLS-statisticsWidth+1, s);
    ///////////
    
    mvaddstr(12, COLS-statisticsWidth, "average speed");
    if (timeSum!=0)
	sprintf(s, "  %9d", (int)(symbNum/(timeSum/60./1000.)));
    else
	sprintf(s, "  %9d", 0);
    mvaddstr(13, COLS-statisticsWidth+1, s);

    sprintf(s, "typed: %5d", realTypedSymbNum);
    mvaddstr(21, COLS-statisticsWidth, s);
    sprintf(s, "out of %5d", totalSymbCount);
    mvaddstr(22, COLS-statisticsWidth, s);


    mvaddstr(24, COLS-statisticsWidth+1, "complete:");
    sprintf(s, "%d%%", (int)(100.*realTypedSymbNum/totalSymbCount));
    mvaddstr(25, COLS-statisticsWidth+7, s);
}

static bool endOfFile;




inline void initTyped()
{
    currLine = 0;
    currChar = 0;
    endOfFile = false;

    for (int i=0; i<L_CH_D_SIZE; i++)
	l_ch_duration[i] = -1.;
    
    startCounter=0; 

    handleTyped(-1);
}



void line_again()
{
    int last_realTypedSymbNum = currChar;
    currChar = 0;
    if (auto_indent_after_error)
	while((*tqueue)[currLine][currChar]==' ')
	    currChar++;

    realTypedSymbNum -= last_realTypedSymbNum - currChar;
}



void keyError(char symb)
{
    currentSymb = symb;
    errors++;

    drawOriginalText();
    drawDelimiters();
    drawTypedText();
    drawStatistics();
    refresh();
	
    overwrite(curscr,stdscr);
    bkgd(COLOR_PAIR(3));
    refresh();
    
    for (int i=0; i<10; i++)
    {
	beep();
//	usleep(10000); // TODO: use win version
    }

    if (errors<=maxErrors)
//	sleep(1); // TODO: use win version
    
    overwrite(curscr,stdscr);
    bkgd(COLOR_PAIR(5));
    refresh();

    if (errors<=maxErrors)
//	sleep(4); // TODO: use win version
    
    overwrite(curscr,stdscr);
    bkgd(COLOR_PAIR(4));
    refresh();
    
    line_again();
}


void draw_all()
{
    erase(); // maybe it's slow, but with it screen isn't blinking!

    drawOriginalText();
    drawDelimiters();
    drawTypedText();
    drawStatistics();

    
    refresh();
    
    overwrite(curscr, stdscr);
}


/**
*
* symb = -1 means to skip char handling
*
*/
inline int handleTyped(int symb)
{
                                           //* symbols handling
    currentSymb = ' '; // eraze wrong symbol you are typed
    
    if (symb==10 || symb==13)  // new line
    {
	if ((*tqueue)[currLine].size()==currChar)
	{
	    if (currLine<tqueue->size()-verticalFields)
	    {
		currLine++;
	    }
	    else if (tqueue->size()-currLine)
		tqueue->pop_front();
	    if (symb==10) // ret-and-indent
	    {
		currChar = 0;
		if (tqueue->size()-1>=currLine) //marginal condition 
		    while((*tqueue)[currLine][currChar]==' ') // skip indent at the beginning of line
		    {
			currChar++;
			realTypedSymbNum++;
		    }
	    }
	    else
		currChar = 0;
	    symbNum++;
	    realTypedSymbNum++;
	}
	else
	    keyError(' ');
    }
    else if (symb==9)
    {
	bool first_indent = true;
	for (int i=currChar; i>=0; i--)
	    if ((*tqueue)[currLine][i]!=' ') first_indent = false;
	
	if (first_indent)
	{
	    while((*tqueue)[currLine][currChar]==' ') // skip indent at the beginning of line
	    {
		currChar++;
		realTypedSymbNum++;
	    }
	    symbNum++;
	    realTypedSymbNum++;
	}
	else
	    keyError(' ');
    }
    else if (symb==-1)  // initialize: skip typed char control
    {}
    else // common character
    {
	if (currChar<currentString->size())
	{
	    // typed character compartion
	    if ((*currentString)[currChar]!=(char)symb)
		keyError((char)symb);
	    else
	    {
		currChar++;
		symbNum++;
		realTypedSymbNum++;
	    }
	}
	else if (currChar==currentString->size())
	    keyError((char)symb);
	else
	    keyError((char)symb);
    }
    
	
    // filling text buffer
    while(tqueue->size()<calcTextHeight() && !endOfFile)
    {
	char x[20];
	sprintf(x, "qsize %d, height %d", tqueue->size(), calcTextHeight());
	std::string str;
	std::string newStr;
	
	if (!getline(*inFile, str))
	    endOfFile = true;
	else
	{
	    // removing spaces and tabs after lines!
	    while (str[str.size()-1]==' ' || str[str.size()-1]==0x0a || str[str.size()-1]==0x09)
	    	str.resize(str.size()-1);

	    // replacing indents with indentSize spaces!
	    std::string::iterator strIter = str.begin();
	    while (strIter!=str.end())
	    {
		if (*strIter==0x0a || *strIter==0x09)
		{
			newStr.append(indentSize*2, ' ');
		}
		else newStr.append(1, *strIter);
		
		strIter++;
	    }
	    // push string
	    tqueue->push_back(newStr);
	}
    }
    if (endOfFile)
    {
	if (!(tqueue->size()-currLine))
	{
#ifdef DEBUG	    
	    printf("ret-end");
#endif
	    return END;
	}
	
    }
    
    /// saving current string
    currentString = &(*tqueue)[currLine];
    

    draw_all();
    
    while(getch()!=ERR);  // read if there was a trash in buffer
}
