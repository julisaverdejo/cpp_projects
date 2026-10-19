/*
    Compile with:
        g++ -g -std=c++17 -o test test.cpp

    2024.02.11 - Added support for user default in array items
    2024.02.21 - Added check of file open
    2024.02.28 - Added workaround to the 'Static Initialization Order Fiasco
    2024.03.19 - Added check on empty file o similar issuese in readFile
*/

#include <iostream>
#include <fstream>

#include "cfg.h"

//
// workaround to the 'Static Initialization Order Fiasco' on the needs static std::map
//
cfg::fieldListStack_t& cfg::fieldListStack() {
    static cfg::fieldListStack_t *fieldListStackPtr = new cfg::fieldListStack_t();
    return *fieldListStackPtr;
}

//
// Read a tocken from stream
//
std::string cfg::get_tocken(pstream& ps) {
    // skip spaces and non printable chars
    char c;
    do {
        if (ps.eof())
            return ("");
        ps.get(c);
    } while (isblank(c) || !isprint(c) || ps.eof());

    tokenStart = ps.GetMark()-1;

    bool tocken_is_string = (c=='"') ? true : false;   
    std::string tocken = "";

    if (!tocken_is_string)       // if is string, remove leading '"'
        ps.putback(c);          

    do {
        ps.get(c);
        tocken += c ;
        if (c=='"')
            break;
    } while( isalnum(c) || c=='_' || c=='-' || c=='.' || (tocken_is_string && (isblank(c) || std::ispunct(c) )));

    if (tocken.length()>1){     // Return at least one char
        if (!tocken_is_string) 
            ps.putback(c);
        tocken.pop_back();
    }
            
    CFG_PRINT( std::cout << "get_tocken return <" << tocken << ">" << std::endl; )
    return (tocken); 
}

//
//  read a tocken from stream and compare with expected
//
bool cfg::match_tocken_or_fail(pstream& ps, std::string tocken) {
    std::string tk = get_tocken(ps);
    if (tocken == tk)
        return true;
    else {
        std::string ErrString = "Unexpected token '" + tk + "'";
        ps.SetFail(tokenStart);
        throw ConfigReadError(ErrString);
    }
}

//
//  Write configuration to file
//
void cfg::writeFile(std::string fileName){
	std::ofstream oFile;

	oFile.open (fileName);
    print(oFile);
    oFile.close();
}

//
//  Read configuration from file
//
bool cfg::readFile(std::string fileName){
    pstream ps;
    std::string errString = "";

    try {
        ps.Open(fileName);
    }     
    catch (const runtime_error & e) {           // catch "File not found" error
        cerr << "**** Error: " << e.what() << endl;
        return false;
    }

    try {
        if (!strim_in(ps))
           errString = "Nothing read from input file";  
    }
    catch (const ConfigReadError & e) {
        errString = e.what();
    }
    catch (const InvalidData & e) {
        errString = e.what();
    }
    catch (const std::runtime_error & e) {
        errString = std::string("Runtime Error: ") + e.what();
    }
    // Standard exceptions for stoi
    catch (const std::invalid_argument & e) {
        errString = "Invalid number";
    }
    catch (const std::out_of_range & e) {
        errString = "Number out of range";
    }

    if (errString != ""){
        cerr << endl;
        ps.PrintErrorLine();
        cerr << "**** " << errString << endl;
        ps.Close();
        return false;
    }

    ps.Close();
    return true;
}



/*******************************************************************************/
//    CFG_STRUCT related methods
/*******************************************************************************/
//
// Overloaded copy operator (Used to write data to class)
//
// void cfg_container::operator=(const cfg_container& src)
cfg_container& cfg_container::operator=(const cfg_container& src)
{
    // Copy only base class vars, preserving "fieldList" pointers    
    cfg::operator= (src);  // Not realy needed. Compiler calls copy operator for the base class before call this function!

    return *this;
}



//
//  print out the struct to output stream
//
void cfg_struct::print(std::ostream& ostr, std::string prefix) {
    std::string indent="    ";
    ostr << prefix << getName() << " ";
    if (isArrayElement())
        ostr << std::to_string(arrayIndex()) << " ";
    ostr << "{" << std::endl;
    for (auto element :fieldList){
            element->print(ostr, prefix + indent);
        }
    ostr << prefix << "}" << std::endl << std::endl;
}

//
//  read struct content from input stream
//
bool cfg_struct::strim_in(pstream& ps, bool ignoreIndex)  {

    CFG_PRINT( std::cout << "cfg_struct::strim_in (" << getName() << " " << arrayIndex() << ") "  << "ENTER\n"; )

    Mark origin = ps.GetMark();
    if (!match_tocken(ps, getName()))
        goto unmatch;
        
    if (isArrayElement()) {
        std::string tocken = get_tocken(ps);
        if (!ignoreIndex && tocken!=std::to_string(arrayIndex()))
            goto unmatch;
    }

    match_tocken_or_fail(ps, "{");

    CFG_PRINT( std::cout << "cfg_struct::strim_in (" << getName() << ") "  << "Start loop\n"; )

    while (true) {
        Mark item = ps.GetMark();

        if (match_tocken(ps, "}")){      // End of record 
            CFG_PRINT( std::cout << "cfg_struct::strim_in (" << getName() << ") "  << "EOR" << std::endl; )
            return true;
        }
        ps.SetMark(item);

        // search over items
        bool found=false;
        std::string tocken = get_tocken(ps);
        CFG_PRINT( std::cout << "cfg_struct::strim_in (" << getName() << ") token is now " << tocken << "\n"; )

        ps.SetMark(item);

        for (auto element :fieldList){
            CFG_PRINT( std::cout << "cfg_struct::strim_in (" << getName() << ") "  << element->getName() << " " << tocken << std::endl; )
            if (element->getName() != tocken){
                continue;
            }
            found = true;
            if (element->strim_in(ps)){
                break;
            } else 
                ps.SetMark(item);
        }
        if (!found){
            std::string ErrString = "Unknow item name '" + tocken + "' in cfg_struct::strim_in";
            ps.SetFail(tokenStart);
            throw ConfigReadError(ErrString);
        }
    }

    return true;
unmatch:
    ps.SetMark(origin);
    return false;
}


