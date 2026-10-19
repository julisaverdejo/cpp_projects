#ifndef pstream_First
#define pstream_First

#include <iostream>
#include <vector>

typedef int Mark;

class pstream
{
private:
	enum State { PS_GOOD = 0, PS_FAIL = 1, PS_BAD = 2, PS_EOF = 4 };

	State state; 

	std::string data;  		// STORES ACCUMULATED CHARS
	int nextchar;		    // INDEX OF NEXT CHAR TO BE RETURNED BY >>

	std::vector<std::istream*> isp;	// STACK OF OPTIONAL CHAR SOURCES
	std::vector<int> firstchar;	    // STACK OF FIRST CHAR POSITIONS FOR CHAR SRCS
	std::vector<int> returnfirstchar;	// STACK OF FIRST CHAR POSITIONS AFTER return from include file FOR CHAR SRCS
	std::vector<int> returnlinenum;	// STACK OF FIRST LINE NUMBER FOR CHAR SRCS
	std::vector<std::string> filename;	// STACK OF FILENAMES FOR CHAR SRCS

	std::string lastread;	// STORES THE TEXT OF THE LAST SUCCESSFUL READ
	int lastchar;			// position of last char read from stream
	Mark includeTokenStart;	// Position of include directive 

// DEACTIVATE ASSIGNMENT
	pstream& operator= (const pstream & p);

public:

// BY DEFAULT A pstream DOES NOT READ FROM ANY SOURCE (THINKS IT'S AT eof)
	pstream(void);
	pstream(std::string str);

// CAN BIND A pstream TO AN EXISTING istream, ifstream, istrstream, ETC
	pstream(std::istream & m);			 

	~pstream(void);

// CAN OPEN SOURCES HIERARCHICALLY
// RETURNS 0 ON FAILURE AND RETAINS EXISTING SOURCE IN THAT CASE
// OTHERWISE RETURNS LEVEL OF SOURCE (STARTING AT 1 FOR THE FIRST OPENED)
// autoclose INDICATES IF CURRENT SOURCE SHOULD AUTOMATICALLY CLOSE (AND
// REVERT) ON EOF, NOTE: IGNORED IF LEVEL == 1
// NOTE THAT THESE FUNCTIONS PARALLEL THE THREE NON-DEFAULT CTORS ABOVE
	int Open(std::string str);
	int Open(std::istream & m);			 

// CLOSING SOURCES REVERTS pstream TO PREVIOUS SOURCES (IF ANY)
// RETURNS THE LEVEL OF THE SOURCE REVERTED TO (0 INDICTES NO SOURCE)
// NOTE, CAN STILL BACKTRACK INTO CLOSED SOURCES (!)
	int Close(void);

// REPORT CURRENT SOURCE LEVEL
	int SourceLevel(void);

// Marks INDICATE THE CURRENT (get) POSITION OF THE pstream
// LIKE SEEKING ONLY WITH BETTER NAMES :-)
	Mark GetMark(void);
	Mark FirstMark(void);
	Mark SetMark(Mark m);

// INDICATE FAILURE OR SUCCESS, OPTIONALLY ALTERING THE get POSITION
	pstream& SetFail(Mark m);
	pstream& SetFail(void);

	pstream& SetGood(Mark m);
	pstream& SetGood(void);

// CONVERSION TO ALLOW "if (ps>>var)..."
	operator void*(void);

// CURRENT STATE OF pstream
	int eof(void);
	int good(void);
	int bad(void);
	int fail(void);


// READ THE NEXT CHARACTER (DOES _NOT_ SKIP WHITESPACE)
	pstream& get(char& c);

	int get(void);
    bool get_from_isp(char &c);
    pstream& get_word_from_isp(std::string &str);
    void parseDirective();

// SKIP WHITESPACE
	pstream& eatwhite(void);

// THIS putback ALLOWS ARBITRARY AMOUNTS OF PUTTING BACK
// (THE PARAMETER IS FOR UNIFORMITY WITH OTHER STREAMS AND IS IGNORED)
	pstream& putback(char c);

// EXAMINE THE CONTENTS OF THE pstream
// 1. RETURN A STRING CONTAINING ANY REMAINING DATA (UP TO THE FIRST CHAR
//    APPEARING IN tochars IF IT IS SPECIFIED)
//	std::string NextData(void);
//	std::string NextData(std::string topattern); 

// 2. LineNumber     RETURNS THE NUMBER OF THE CURRENT LINE, STARTING AT 1
//                         (IE: RETURN 1 + THE NUMBER OF '\n's BEFORE THE CURRENT get POSITION)
//    FileLineNumber GIVES LINE NUMBER WITHIN CURRENT INPUT SOURCE
//    FileName       RETURNS NAME OF CURRENT FILE
	int LineNumber(void);
	int FileLineNumber(void);
	int ErrorFileLineNumber(void);

	std::string FileName(void);
	std::string ErrorFileName(void);

// 3. RETURN A STRING CONTAINING THE TEXT OF THE CURRENT LINE
//    (IE: THE TEXT BETWEEN THE PREVIOUS '\n' (OR THE START OF FILE)
//         AND THE NEXT '\n' (OR EOF))
	std::string Line(void);

// 4. RETURN A STRING CONTAINING THE TEXT OF THE LINE lineno
	std::string Line(unsigned int lineno);

// 5. RETURN A STRING CONTAINING THE TEXT OF THE LAST SUCCESSFUL >> OPERATION
//	std::string LastRead(void);

// 6. RETURN THE NUMBER OF THE LINE READ WITH ERROR
	int ErrorLineNum(void);

// Print a message with error line number, line content and error position marker	
	void PrintErrorLine();
};

extern pstream pin;

#endif /* pstream_First */
