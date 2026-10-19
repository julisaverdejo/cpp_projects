// 2024.03.22 - Fixed crash on missinf include file


#include <stdlib.h>
#include <unistd.h>
#include "pstream.h"
#include <string.h>
#include <ctype.h>
#include <fstream>

#include <sstream>
#include <string>
#include <stdexcept>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::ifstream;
using std::istream;

Mark pstream::GetMark(void)
{
	return nextchar;
}

Mark pstream::FirstMark(void)
{
	return SetMark(0);
}

Mark pstream::SetMark(Mark m)
{
	Mark oldmark = nextchar;
	nextchar = m;
	return oldmark;
}

pstream& pstream::SetGood(void)
{
	state = PS_GOOD;
	return *this;
}

pstream& pstream::SetGood(Mark m)
{
	SetMark(m);
	SetGood();
	return *this;
}

pstream& pstream::SetFail(void)
{
	state = PS_FAIL;
	return *this;
}

pstream& pstream::SetFail(Mark m)
{
	SetMark(m);
	SetFail();

	if (m>lastchar)
		lastchar = m;

	return *this;
}

pstream& pstream::operator= (const pstream &)
{
	/* DEACTIVATED */
	return *this;
}

pstream::pstream(void)
:state(PS_EOF), data(""), nextchar(0), lastchar(0)
{
}

pstream::pstream(string str)
:state(PS_GOOD), lastchar(0)
{
    nextchar = 0;
	isp.push_back(new ifstream(str));
	firstchar.push_back(nextchar);
	returnfirstchar.push_back(nextchar);
    returnlinenum.push_back(1);
	filename.push_back(str);
}

pstream::pstream(istream & m)
:state(PS_GOOD), lastchar(0)
{
    nextchar = 0;
	isp.push_back(new istream(m.rdbuf()));
	firstchar.push_back(nextchar);
    returnfirstchar.push_back(nextchar);
    returnlinenum.push_back(1);
	filename.push_back("");
}


pstream::~pstream(void)
{
    for (auto & i : isp)
        delete i; 
}

// CAN OPEN SOURCES HIERARCHICALLY
// RETURNS LEVEL OF SOURCE (STARTING AT 1 FOR THE FIRST OPENED)
// autoclose INDICATES IF CURRENT SOURCE SHOULD AUTOMATICALLY CLOSE (AND
// REVERT) ON EOF, NOTE: IGNORED IF LEVEL == 1

	int pstream::Open(string str)
	{
		isp.push_back(new ifstream(str));
		firstchar.push_back(nextchar);
        returnfirstchar.push_back(nextchar);
        returnlinenum.push_back(1);
		filename.push_back(str);
        if (isp.back()->fail()){
			SetFail(includeTokenStart);
            throw std::runtime_error("File not found"); 
		}
		state = PS_GOOD;

		return SourceLevel();
	}

	int pstream::Open(istream & m)
	{
		isp.push_back( new istream(m.rdbuf()));
		state = PS_GOOD;
		firstchar.push_back(nextchar);
    	returnfirstchar.push_back(nextchar);
        returnlinenum.push_back(1);
		filename.push_back("");
		return SourceLevel();
	}


// CLOSING SOURCES REVERTS pstream TO PREVIOUS SOURCES (IF ANY)
// RETURNS THE LEVEL OF THE SOURCE REVERTED TO (0 INDICTES NO SOURCE)
// NOTE, CAN STILL BACKTRACK INTO CLOSED SOURCES

	int pstream::Close(void)
	{
        int includeLine=0;

		if (!isp.empty()) {
            int includeFirstChar = firstchar.back(); // First char position in included file to be close

			delete isp.back();
			isp.pop_back();
			firstchar.pop_back();
			filename.pop_back();
            returnlinenum.pop_back();
            returnfirstchar.pop_back();

            // find line number of file when it open included file     
            if (!isp.empty()){
                includeLine = returnlinenum.back();

		        for (int i=firstchar.back(); i<includeFirstChar; i++) {
			        if (data[i]=='\n') {
				        includeLine++;
				        }
			        }
                returnfirstchar.back() = nextchar;
                returnlinenum.back() = includeLine;
            }
			if (!filename.empty())
				cout << "Back to file " << filename.back() << "\n";
		}
		state = PS_GOOD;

		return SourceLevel();
	}


// REPORT CURRENT SOURCE LEVEL

	int pstream::SourceLevel(void)
	{
		return isp.size();
	}

int pstream::good(void)
{
	return (state==PS_GOOD);
}

int pstream::bad(void)
{
	return (state==PS_BAD);
}

int pstream::fail(void)
{
	return (state==PS_FAIL);
}

int pstream::eof(void)
{
	return (state==PS_EOF);
}

pstream::operator void*(void)
{
	return (void*)(state==PS_GOOD);
}


int pstream::get(void)
{
	char c;
	if (get(c)) {
		return (int)c;
		}
	else {
		return -1;
		}
}

pstream& pstream::get(char& c)
{ 
// DEAD STREAM? ABORT AT ONCE
	if (!good())
		return *this;

// DO WE ALREADY HAVE THE DATA?
	if (nextchar < (int) data.length()) {
		c = data[nextchar++];
		state = PS_GOOD;
		}

// CAN WE GET IT? CHECK FOR PROMPTING TOO
	else if (!isp.empty() && get_from_isp(c)) {
		data += c;
		nextchar++;
		state = PS_GOOD;
		}

// NO LUCK? MUST BE END OF FILE, TRY BACK UP THE HIERARCHY
	else {
		if (Close()!=0) {
			return this->get(c);
			}
		else {
			state = PS_EOF;
			}
		}
	return *this;
}

//
//  Low level function to get char from input file
//  Preprocessing is implemented here
//
bool pstream::get_from_isp(char &outc)
{
    char c;

    if (!isp.back()->get(c))                   // End of file
        return false;

    if (c=='#'){                                // Comments
        while (!isp.empty()>0 && c!='\n'){
            if (!isp.back()->get(c))
                return false;
        }
    } else if (c=='$'){                         // Directive
        parseDirective();
        c = ' ';    
    } 

    outc = c;
    return true;
}

pstream& pstream::get_word_from_isp(string &str)
{
    char c;

    // skip initial spaces and TABs
    do {
        if (isp.empty() || !isp.back()->get(c))
            return *this;
    } while (isblank(c));
    str = c;

    for (;;) {
        if (isp.empty() || !isp.back()->get(c))
            return *this;
        if (isspace(c))
            break;
        str += c;
    } 

    isp.back()->putback(c);

    return *this;
}

void pstream::parseDirective()
{
    std::string directive, fileName;

	includeTokenStart = GetMark()-1;
    if (!get_word_from_isp(directive)){
        cerr << "Error reading directive" << endl;
        exit(0);
    }

    if (directive == "include"){
        if (!get_word_from_isp(fileName)){
            cerr << "Error reading filename in include $directive" << endl;
            exit(0);
        }
        if ( fileName.front() == '"' && fileName.back() == '"') {
            fileName.erase( 0, 1 ); // erase the first character
            fileName.erase( fileName.size() - 1 ); // erase the last character
        }
        cout << "Opening include file: " << fileName << endl;
        Open(fileName);        

        if (errno!=0){
            cerr << "Error opening file "<< fileName << ": " << strerror(errno) << endl;
            exit(0);
        }

    } else {
        cerr << FileName() << ":" << ErrorFileLineNumber() << " Unknow directive $" << directive << "\"" << endl;
        exit(0);
    }

}


pstream& pstream::putback(char /*unused*/)
{
	if (--nextchar<0) {
		nextchar=0;
		}

	SetGood();

	return *this;
}


pstream& pstream::eatwhite(void)
{
	char nextchar;
	while (get(nextchar)) {
		if (!isspace(nextchar)) {
			putback(nextchar);
			break;
			}
		}
	return *this;
}


// RETURN THE NUMBER OF THE CURRENT LINE, STARTING AT 1
// (IE: RETURN 1 + THE NUMBER OF '\n's BEFORE THE CURRENT get POSITION)

	int pstream::LineNumber(void)
	{
		int i, linecount = 1;

		for (i=0;i<nextchar;i++) {
			if (data[i]=='\n') {
				linecount++;
				}
			}
		return linecount;
	}

	int pstream::FileLineNumber(void)
	{
        size_t currfile;
		int i, linecount = 1;

		for (currfile=0;currfile<firstchar.size() && firstchar[currfile]<nextchar; currfile++);
		currfile--;

		for (i=firstchar.back();i<nextchar;i++) {
			if (data[i]=='\n') {
				linecount++;
				}
			}
		return linecount;
	}

	string pstream::FileName(void)
	{
		if (firstchar.empty())
		{
			return "";
		}

		size_t currfile;
		for (currfile=0; currfile<firstchar.size() && firstchar[currfile]<=nextchar; currfile++);
		currfile--;

		if (currfile>=0)
		{
			return filename[currfile];
		}
		else
		{
			return "";
		}
	}

// RETURN A STRING CONTAINING THE TEXT OF THE CURRENT LINE
// (IE: THE TEXT BETWEEN THE PREVIOUS '\n' (OR THE START OF FILE)
// AND THE NEXT '\n' (OR EOF))

	string pstream::Line(void)
	{
		int startpos = nextchar-1;
		while (startpos>=0&&data[startpos]!='\n') {
			startpos--;
			}
	
		if (++startpos<0)
			return "";

		string line;
		while (startpos<(int)data.length() && data[startpos]!='\n') {
			line += data[startpos++];
			}
		return line;
	}

// RETURN A STRING CONTAINING THE TEXT OF THE LINE lineno

	string pstream::Line(unsigned int lineno)
	{
		if (!data.length()) {
			return "";
			}
		int startpos = 0;
		lineno--;
		while (lineno) {
			if (data[startpos]=='\n') {
				lineno--;
				}
			startpos++;
			if (startpos==(int) data.length()) {
				return "";
				}
			}
		string line;
		while (startpos<(int)data.length()&&data[startpos]!='\n') {
			line += data[startpos++];
			}
		return line;
	}

/*
    Error condition report functions
*/
	int pstream::ErrorLineNum(void)
	{
		int i, linecount = 1;

		for (i=0;i<lastchar;i++) {
			if (data[i]=='\n') {
				linecount++;
				}
			}
		return linecount;
	}

	int pstream::ErrorFileLineNumber(void)
	{
        size_t currfile;
		int i, linecount;

		for (currfile=0;currfile<firstchar.size() && firstchar[currfile]<lastchar; currfile++);
		if (currfile>0)
			currfile--;

        linecount = returnlinenum[currfile];
		for (i=returnfirstchar[currfile];i<lastchar;i++) {
			if (data[i]=='\n') {
				linecount++;
				}
			}
		return linecount;
	}

	string pstream::ErrorFileName(void)
	{
		if (firstchar.empty())
		{
			return "";
		}

		size_t currfile;
		for (currfile=0;currfile<firstchar.size() && firstchar[currfile]<=lastchar; currfile++);
		currfile--;

		if (currfile>=0)
		{
			return filename[currfile];
		}
		else
		{
			return "";
		}
	}


	void pstream::PrintErrorLine()
	{
		int oldpos = nextchar;
		State originalstate = state;
		string line;
		char c;
		int errLineStart = 0;
		int i, linecount = 1;
		int addTabSpaces = 0;

		nextchar = 0;
		state = PS_GOOD;

		for (i=0;i<lastchar;i++) {
			if (data[i]=='\n') {
				linecount++;
				nextchar = i+1;
				errLineStart = nextchar;
				}
			}

		do {
			c = -1;
			get(c);
			if (c=='\t'){
				line += "    ";
				addTabSpaces += 3;
			} else if (c!='\n')
				line += c;
		} while (c!='\n' && c!=-1);

		nextchar = oldpos;
		state = originalstate;

		cerr << ErrorFileName() << ":" << ErrorFileLineNumber() << endl;
		cerr << line << endl;
		cerr << string(lastchar-errLineStart+addTabSpaces, '_') << "^" << endl;
		return;
	}

// STANDARD ACCESS pstream
//	pstream pin(cin);



