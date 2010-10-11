int main(int argc, char *argv[])
{
    {
	{
	    {
	    }
	    else if (tqueue->size()-currLine)
		tqueue->pop_front();
	    if (symb==10) // ret-and-indent
	    {
		currChar = 0;
		while((*tqueue)[currLine][currChar]==' ')
		    currChar++;
	    }
	    else
		currChar = 0;
	}
	else
	    keyError(' ');
    }
    else if (symb==9)
    {
	bool first_indent = true;
	for (int i=currChar; i>=0; i++)
	    if ((*tqueue)[currLine][i]!=' ') first_indent = false;
	if (first_indent)
	    while((*tqueue)[currLine][currChar]==' ')
		currChar++;
    }
    else if (symb==-1)  // initialize: skip typed char control
    {
    }
}
