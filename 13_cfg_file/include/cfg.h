/*
    Compile with:
        g++ -g -std=c++17 -o test test.cpp

    2024.02.11 - Added support for user default in array items
    2024.02.11 - Improved number formatting error detection
    2024.02.28 - Added workaround to the 'Static Initialization Order Fiasco
    2024.03.01 - Added support for cfg_vector
    2024.10.03 - Added size(), addItem() and eraseItem() to cfg_vector
    2024.10.09 - Added size(), cfg_array
    2024.12.31 - Added support for range (Es.: 0-5) in array index
*/
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stack>
#include <array>
#include <vector>
#include <regex>
#include <sstream>

#include "mexception.h"
#include "pstream.h"

#ifndef CFG_H
#define CFG_H

// Debug
// #define CFG_DEBUG
#ifdef CFG_DEBUG
    #define CFG_PRINT(...) __VA_ARGS__
#else
    #define CFG_PRINT(...)
#endif

// =====================================================================
// Shortocut to create a configuration struct
// usage: CFG_STRUCT ( name,
//              CFG_ITEM();           
//              CFG_ARRAY();
//        );

#define CFG_STRUCT(X, ...)                       \
  struct X : cfg_struct {                        \
    X(std::string name): cfg_struct(name) {}     \
    X(): cfg_struct("") {}                       \
        cfgFieldListStart dummy_start{&fieldList};      \
        __VA_ARGS__                                     \
        cfgFieldListEnd   dummy_end;                    \
  }                                                     



// =====================================================================
// Configuration item definition to be used in CFG_RECORD
//      Convert CFG_ITEM(int, pippo, def) to cfg_<int> pippo {"pippo", 1}
//          and CFG_ITEM(bool, pluto)     to cfg_<bool> pluto {"pluto"}
//           
#define CFG_ITEM(type, name, ...)      \
    cfg_<type> name {#name, __VA_ARGS__}    
// =====================================================================
// Configuration array of itemes or record
//      Converts CFG_ARRAY(int, 4, clara) to cfg_array<int, 4> clara ("clara")
#define CFG_ARRAY(type, size, name)      \
    cfg_array<type, size> name {#name}
// =====================================================================
// Configuration array of itemes or record
//      Converts CFG_VECTOR(int, clara) to cfg_vector<int> clara ("clara")
#define CFG_VECTOR(type, name)      \
    cfg_vector<type> name {#name}
// =====================================================================
// Instance of configuration struct inside a configuration struct
#define CFG_STRUCT_ITEM(type, name, ...)      \
    type name {#name, __VA_ARGS__}    
// =====================================================================



class ConfigReadError : public MException
{
public:
	explicit ConfigReadError(const string& __arg) {msg = "Configuration file read error: " + __arg;}
};

class InvalidData : public MException
{
public:
	explicit InvalidData(const string& __arg) {msg = "Invalid data: " + __arg;}
};



class cfg
{
template<typename T, int N> friend class cfg_array;
template<typename T> friend class cfg_vector;
typedef std::stack<std::vector<cfg*>*> fieldListStack_t;

public:
    cfg(std::string name): pIsItem(true), pName(name), pIsArrayElement(false) {
        if (!cfg::fieldListStack().empty() && name != "") {
                cfg::fieldListStack().top()->push_back(this); 
        }
    }

    std::string             getName() { return pName; }
    bool                    isItem() {return pIsItem;}
    bool                    isArrayElement() {return pIsArrayElement;}
    int                     arrayIndex() {return pArrayIndex;}
    virtual void            print(std::ostream&, std::string prefix="")  {}
    virtual bool            strim_in(pstream&, bool ignoreIndex=false)  {return false;}
    virtual std::string     getValue()=0;

protected:
//    static inline std::stack<std::vector<cfg*>*> fieldListStack;
    bool                    pIsItem;
    int                     pArrayIndex;
	std::string 	        pName;
    bool                    pIsArrayElement;
    Mark                    tokenStart;

protected:
    fieldListStack_t& fieldListStack();
    // Read a tocken from stream
    std::string get_tocken(pstream& ps);

    // Return true if it match 
    bool match_tocken(pstream& ps, std::string tocken) {
        return (tocken == get_tocken(ps));
    }

    bool match_tocken_or_fail(pstream& ps, std::string tocken);

public:
    void writeFile(std::string fileName);
    bool readFile(std::string fileName);
};



/*
    Funzioni per inizializzare le liste di campi nei record
*/
// Start Of Record marker 
class cfgFieldListStart: public cfg
{
public:
    cfgFieldListStart(std::vector<cfg*> *fieldList): cfg("") { fieldListStack().push(fieldList); }
    std::string getValue() { return ""; }
};

// End Of Record Marker 
class cfgFieldListEnd: public cfg
{
public:
    cfgFieldListEnd(): cfg("") { fieldListStack().pop(); }
    std::string getValue() { return ""; }
};

//
// Base class for configuration items
//
class cfg_item: public cfg
{
public:
	cfg_item(std::string name) : cfg (name) {
        pIsItem=true; 
    }
};


//
//  Configuration data types for fields
//
template<class T>
class cfg_: public cfg_item
{
public:
	cfg_(std::string name, T defVal={}) : cfg_item (name), val(defVal) {}
    cfg_() : cfg_item (""), val(0) {}
 
    // Overloaded copy operator (Used to write data to class)
    void operator= (T c){ val = c; }

    // Overloaded int cast (Used to read data from class)
    operator T() const { return val; }

    // prefix increment
    cfg_<T>& operator++()
    {
        val++;          // actual increment takes place here
        return *this;   // return new value by reference
    }
 
    // postfix increment
    cfg_<T> operator++(int)
    {
        cfg_<T> old = *this; // copy old value
        operator++();  // prefix increment
        return old;    // return old value
    }

    // prefix decrement
    cfg_<T>& operator--()
    {
        val--;      // actual decrement takes place here
        return *this; // return new value by reference
    }
 
    // postfix decrement
    cfg_<T> operator--(int)
    {
        cfg_<T> old = *this; // copy old value
        operator--();  // prefix decrement
        return old;    // return old value
    }

    cfg_<T>& operator+=(const T& rhs) // compound assignment (does not need to be a member,
    {                           // but often is, to modify the private members)
        val += rhs;    /* addition of rhs to *this takes place here */
        return *this; // return the result by reference
    }

    cfg_<T>& operator-=(const T& rhs) // compound assignment (does not need to be a member,
    {                           // but often is, to modify the private members)
        val -= rhs;    /* subtraction of rhs to *this takes place here */
        return *this; // return the result by reference
    }

    cfg_<T>& operator*=(const T& rhs) // compound assignment (does not need to be a member,
    {                           // but often is, to modify the private members)
        val *= rhs;    /* multiply of rhs to *this takes place here */
        return *this; // return the result by reference
    }

    // String access to internal value
    std::string getValue() { return std::to_string (val); } 

    // Set value from string. This implementation works for floatings and all integers but int8 and uint8
    void setValue(std::string valString) {  
        std::istringstream ss(valString);
        ss.unsetf(std::ios::basefield);     // Set base to automatic
        ss >> val;
        if (ss.fail() || !ss.eof()) {
            // Error
            std::string ErrString = "Invalid number '" + valString + "' in " + __FUNCTION__;
            throw ConfigReadError(ErrString);
        }

        CFG_PRINT( std::cout << "cfg_<T>::setValue (" << valString << ") set to " << val << "\n"; )
        if (ss.fail()) {
            throw InvalidData(valString);
        }
    }

    void print(std::ostream& ostr, std::string prefix="") {
        ostr << prefix << getName() << " ";
        if (isArrayElement())
            ostr << arrayIndex() << " ";
        ostr << "= " << getValue() << ";" << std::endl;
    }

    // read configuration from input stream
    bool strim_in(pstream& ps, bool ignoreIndex)  {
        std::string valString;
        if (!match_tocken(ps, getName()))
            return false;

        if (isArrayElement()){
            std::string t = get_tocken(ps);
            if (!ignoreIndex && t!=std::to_string(arrayIndex()))
                return false;   
        }

        match_tocken_or_fail(ps, "=");

        valString = get_tocken(ps); 

        try {
            setValue( valString ); 
        } catch (...) {
            ps.SetFail(tokenStart);
            throw;
        }

        match_tocken_or_fail(ps, ";");
        return true;
    }

private:
    T                  val;                 
};

// Specilizations of setValue for int8 and uint8
template <> inline
    void cfg_<int8_t>::setValue(std::string valString) { 
        int v = stoi(valString, 0, 0);
        if (v > 127 || v < -128)
            throw  InvalidData("Out of range"); 
        val = v;   
    }

template <> inline
    void cfg_<uint8_t>::setValue(std::string valString) { 
        unsigned long v = stoul(valString, 0, 0);
        if (v >255)
            throw  InvalidData("Out of range"); 
        val = v;   
    }

template <> inline
    void cfg_<bool>::setValue(std::string valString) { 
        for (auto & c: valString) c = toupper(c);
        if (valString=="TRUE" || valString == "1") 
            val = true;
        else if (valString=="FALSE" || valString == "0") 
            val = false;
        else {
            throw  InvalidData("Expected bool value"); 
        }
    }

template <> inline
    void cfg_<std::string>::setValue(std::string valString) { 
        val = valString;   
    }

// Explicit specialization for string type of getValue()
template <> inline
    std::string cfg_<std::string>::getValue() { 
        return "\"" + val + "\""; 
    } 

// Explicit specialization for bool type of getValue()
template <> inline
    std::string cfg_<bool>::getValue() { 
        return (val ? "true": "false"); 
    } 


//
// Base class for configuration record
//
class cfg_container: public cfg
{
public:
    cfg_container(std::string name): cfg(name) { 
        pIsItem=false; 
    }

    std::string     getValue() {
        return "";
    }

public:
    // Copy operator overloading
//    void operator=(const cfg_container& src);
    cfg_container& operator=(const cfg_container& src);

    std::vector<cfg*> fieldList;
};



// Configuration record
class cfg_struct: public cfg_container
{
public:
    cfg_struct(std::string name): cfg_container(name) {   
    } 

public:
    // Print configuration to output stream
    void print(std::ostream& ostr, std::string prefix="");

    // read configuration from input stream
    bool strim_in(pstream& ps, bool ignoreIndex=false);
};


//
// Array of items or records
//
template<typename T, int N>
class cfg_array: public cfg_container
{
public:
    cfg_array(std::string name): cfg_container(name) {
        for (int i=0; i<N; i++) {
            itemArray[i].pName = name;
            itemArray[i].pIsArrayElement = true;
            itemArray[i].pArrayIndex = i;
        } 
    } 

    T& operator [](int idx) {
        return itemArray[idx];
    }

    T operator [](int idx) const {
        return itemArray[idx];
    }

    T& operator= (const T& src)
    {
        // Copy only base class vars, preserving "itemArray" pointers    
        cfg_container::operator= (src);  // Not realy needed. Compiler calls copy operator for the base class before call this function!
        return *this;
    }

    std::string     getValue() {
        return "";
    }

public:
    std::array<T, N> itemArray;

public:
    size_t size() {return N;}

    void print(std::ostream& ostr, std::string prefix) {
        for (int i=0; i<N; i++)
            itemArray[i].print(ostr, prefix);
    }

    // read configuration from input stream
    bool strim_in(pstream& ps, bool ignoreIndex)  {
        int idx, idxMin=0, idxMax=0;
        Mark origin;
        bool found = false;
        bool parsingUserDefault = false;

        CFG_PRINT( std::cout << "cfg_array::strim_in (" << getName() << ") "  << "ENTER\n"; )

        while (true) {
            origin = ps.GetMark();
            if (!match_tocken(ps, getName())){
                ps.SetMark(origin);
                return found;
            }
            found = true;

            std::string t = get_tocken(ps);
            
            if (t == "default" || t=="-1") {            // User default index
                parsingUserDefault = true;
                idxMin = 0;
                idxMax = 0;
            } else {      // check for range
                size_t found = t.find("-");
                if (found != string::npos){
                    string strIdxMin = t.substr(0, found);
                    string strIdxMax = t.substr(found+1);
                    idxMin = stoi(strIdxMin);
                    idxMax = stoi(strIdxMax);
                } else {
                    idxMin = stoi(t);
                    idxMax = idxMin;
                    if (std::to_string(idxMin) != t){
                        ps.SetFail(tokenStart);
                        throw  InvalidData("Invalid array index"); 
                    }
                }
            }
            if (idxMin>idxMax || idxMin<0 || idxMax>=N){
                ps.SetFail(tokenStart);
                throw  InvalidData("Invalid array index"); 
            }

            for (idx=idxMin; idx<=idxMax; idx++){
                // set position for each index
                ps.SetMark(origin);
                CFG_PRINT( std::cout << "cfg_array::strim_in (" << getName() << ") "  << "calling item " << itemArray[idx].getName() << "[" << idx  << "]\n"; )
                itemArray[idx].strim_in(ps, true); // ignoreIndex = true. ori:parsingUserDefault
            }

            if (parsingUserDefault) {
                parsingUserDefault = false;

                // Copy user default to others items
                for (int i=1; i<N; i++){
                    CFG_PRINT( std::cout << "cfg_array::strim_in (" << getName() << ") "  << "Coping items " << itemArray[i].getName() << endl; )
                    itemArray[i] = itemArray[0];
                    itemArray[i].pArrayIndex = i;
                }
            }
        }
    }

};

//
// Vector of items or records
//
template<typename T>
class cfg_vector: public cfg_container
{
public:
    cfg_vector(std::string name): cfg_container(name) {
        itemVector.reserve(1000);                             // FIXME: Non e' la soluzione corretta, ma funziona.
    } 

    std::string     getValue() {
        return "";
    }

    void setMaxSize (size_t n) { 
        size_t currentSize = itemVector.size();
        if (currentSize < n) {
            itemVector.resize(n);
            for (size_t i=currentSize; i<n; i++){
                itemVector[i] = userDefault();
                itemVector[i].pName = getName();
                itemVector[i].pIsArrayElement = true;
                itemVector[i].pArrayIndex = i; 
            }
        }
    }

    size_t size() {return itemVector.size();}

    void addItem(T &newItem) {
        size_t i = size();
        itemVector.resize(i+1);
        itemVector[i] = newItem;
        itemVector[i].pName = getName();
        itemVector[i].pIsArrayElement = true;
        itemVector[i].pArrayIndex = i; 
    }    

    void eraseItem(int i) {
        itemVector.erase(itemVector.begin()+i);
    }

    T& operator [](int idx) {
        return itemVector[idx];
    }

    T operator [](int idx) const {
        return itemVector[idx];
    }

    T& operator= (const T& src)
    {
        // Copy only base class vars, preserving "itemVector" pointers    
        cfg_container::operator= (src);  // Not realy needed. Compiler calls copy operator for the base class before call this function!
        return *this;
    }

    
private:
    std::vector<T> itemVector {};
    // T userDefault {"userDefault"};

    T& userDefault() {
        static T *userDefaultPtr = new T {"userDefault"};
        return *userDefaultPtr;
    }


public:
     void print(std::ostream& ostr, std::string prefix) {
        for (auto item : itemVector) {
            item.print(ostr, prefix);
        }
    }

    // read configuration from input stream
    bool strim_in(pstream& ps, bool ignoreIndex)  {
        int idx = -1;
        Mark origin;
        bool found = false;
        bool parsingUserDefault = false;

        CFG_PRINT( std::cout << "cfg_vector::strim_in (" << getName() << ") "  << "ENTER\n"; )

        while (true) {
            origin = ps.GetMark();
            if (!match_tocken(ps, getName())){
                ps.SetMark(origin);
                return found;
            }
            found = true;

            std::string t = get_tocken(ps);
            
            if (t == "default" || t=="-1") {            // User default index
                parsingUserDefault = true;
            } else {
                idx = stoi(t);
                if (std::to_string(idx) != t || idx<0){
                    ps.SetFail(tokenStart);
                    throw  InvalidData("Invalid vector index"); 
                }
                // Eventualy resize the vector, filling with user Default
                setMaxSize(idx+1);
            }
        
            ps.SetMark(origin);
            if (parsingUserDefault){
                userDefault().pName = getName();
                userDefault().pIsArrayElement = true;
                CFG_PRINT( std::cout << "cfg_vector::strim_in (" << getName() << ") "  << "calling item " << userDefault().getName() << "\n"; )
                userDefault().strim_in(ps, parsingUserDefault);
            } else {
                CFG_PRINT( std::cout << "cfg_vector::strim_in (" << getName() << ") "  << "calling item " << itemVector[idx].getName() << "[" << idx  << "]\n"; )
                itemVector[idx].strim_in(ps, parsingUserDefault);
            }

            parsingUserDefault = false;
        }
    }

};




#endif // CFG_H

