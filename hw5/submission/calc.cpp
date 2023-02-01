/*
* CSF Assignment 5
* Network Calculator
* Function implementations
* Simone Bliss & Danny Lee
* sbliss5@jhu.edu, jlee692@jhu.edu
*/

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cctype>
#include <climits>
#include "calc.h"

using std::cout; using std::cin; using std::cerr; 
using std::endl; using std::stringstream;
using std::map; using std::string; using std::vector;


//global constant indicator variables
const int INVALID = 0; const int SUCCESS = 1;
const int INTEGER = 2; const int VARIABLE = 3;

struct Calc {
};

class CalcImpl : public Calc {
    public:
        map<string, int> dictionary;
        int evalExpr(const char *expr, int &result);
        
        //helper functions 
        vector<string> tokenize(const string &expr);
        bool changeStrToNum(string str, int& arg);
        int variableCheck(string operand);
        int integerCheck(string opernad);

        int operation(string op, int arg1, int arg2, int& error);
        int assignment(string var, int arg);

        int oneArg(vector<string> tokens, int& fail);
        int threeArgs(vector<string> tokens, int& fail);
        int fiveArgs(vector<string> tokens, int& fail);
};

/*
 * Purpose: create an instance of a CalcImpl object
 * Parameters:
 * 		void
 * Returns:
 * 		an instance of a CalcImpl object
 */
extern "C" struct Calc *calc_create(void) {
    return new CalcImpl();
}

/*
 * Purpose: delete an instance of a CalcImpl object
 * Parameters:
 * 		calc - a pointer to a Calc struct 
 * Returns:
 * 		void
 */
extern "C" void calc_destroy(struct Calc *calc) {
    CalcImpl *obj = static_cast<CalcImpl *>(calc);
    delete obj;
}

/*
 * Purpose: evaluate the user-entered expression
 * Parameters:
 * 		calc - a pointer to a Calc struct
 *      expr - a const char pointer to the user-entered expression
 *      result - an pointer for the result of the calculation
 * Returns:
 * 		int, whether or not the calcuation was successful or invalid
 */
extern "C" int calc_eval(struct Calc *calc, const char *expr, int *result) {
    CalcImpl *obj = static_cast<CalcImpl *>(calc);
    return obj->evalExpr(expr, *result);
}

/*
 * Purpose: evaluate the user-entered expression (helper for cal_eval)
 * Parameters:
 * 		expr - a const char pointer for the user-entered expression
 *      result - an integer reference for the result of the calculation
 * Returns:
 * 		int, the code for the whether the calculation was successful or invalid
 */
int CalcImpl::evalExpr(const char * expr, int &result) {

    vector<string> tokens = tokenize(expr);
    int len = tokens.size();
    int fail = 0;

    if (len == 1) {
        result = oneArg(tokens, fail);
        if (fail) {
            return INVALID;
        }
        return SUCCESS;
    } else if (len == 3) {
        result = threeArgs(tokens, fail);
        if (fail) {
            return INVALID;
        }
        return SUCCESS;
    } else if (len == 5) {
        result = fiveArgs(tokens, fail);
        if (fail) {
            return INVALID;
        }
        return SUCCESS;
    }
    return INVALID;
}

/*
 * Purpose: tokenize the user entered string
 * Parameters:
 * 		&expr - a reference to the string expression entered by the user
 * Returns:
 * 		vec - a vector containing the tokenized string
 */
vector<string> CalcImpl::tokenize(const string &expr) { 
    vector<string> vec;
    stringstream s(expr);
    string token; 
    while (s >> token) {
        vec.push_back(token); 
    }
    return vec; 
}

/*
 * Purpose: check whether or not the user entered argument is an integer or a variable and convert to int or lookup in dictionary
 * Parameters:
 * 		str - a string representing the user input for operand
 *      arg - integer reference to be updated with converted number or the number found in the dictionary
 * Returns:
 * 		boolean, whether or not the operation could be performed
 */
bool CalcImpl::changeStrToNum(string str, int& arg) {
    if (integerCheck(str) == INTEGER) {                 // A) INTEGER Case
        arg = atoi(str.c_str());
        return true;
    } else if (variableCheck(str) == VARIABLE) {        // B) VARIABLE Case
        auto it = dictionary.find(str);
        if (it != dictionary.end()) {
            arg = it->second;
            return true;                                    // a) Defined
        }
        return false;                                       // b) Undefined
    } else {
        return false;                                   // C) INVALID Case
    }
}

/*
 * Purpose: check that the operand the user entered is a valid variable name
 * Parameters:
 * 		operand - a string representing the user input for operand
 * Returns:
 * 		int, the code for the whether the input was a variable or invalid
 */
int CalcImpl::variableCheck(string operand) {
    char firstChar = operand.at(0);
    int len = operand.length();

    //first char is A-z, we expect a variable name
    if (isalpha(firstChar)) { 
        for (int i = 1; i < len; i++) {
            if (!isalpha(operand.at(i))) {
                return INVALID;
            }
        }
        return VARIABLE;
    }
    return INVALID;
}

/*
 * Purpose: check that the number entered by the user is an integer
 * Parameters:
 * 		operand - string containing the user input for the operand
 * Returns:
 *  	int, the code for the whether the input was an integer, or invalid
 */
int CalcImpl::integerCheck(string operand) {
    errno = 0;
    long lnum;
    char *endptr;

    lnum = strtol(operand.c_str(), &endptr, 10);
    if (endptr == operand) {
        return INVALID;
    }
    if ((lnum == LONG_MAX || lnum == LONG_MIN) && errno == ERANGE) {
        return INVALID;
    }
    if ((lnum > INT_MAX) || (lnum < INT_MIN)) {
        return INVALID;
    }
    return INTEGER;
}

/*
 * Purpose: do division, multiplication, addition or subtraction
 * Parameters:
 * 		op - string containing the operation to be done
 *      arg1 - the first integer operand
 *      arg2 - the second integer operand
 *      error - integer reference, marker for whether or not the operation could be performed
 * Returns:
 * 		result - the result of the calculation
 */
int CalcImpl::operation(string op, int arg1, int arg2, int& error) {
    char symbol = op.at(0);
    int arg;
    switch(symbol) {
        case '*':
            arg = arg1 * arg2;
            return arg;
        case '/':
            if (arg2 == 0) {
                error = 1;
                return 0;
            }
            arg = arg1 / arg2;
            return arg;
        case '+':
            arg = arg1 + arg2;
            return arg;
        case '-':
            arg = arg1 - arg2;
            return arg;
        default:
            error = 1;
            return 0;
    }
}

/*
 * Purpose: add or update a  variable in the dictionary when assignment is necessary
 * Parameters:
 * 		var - string containing the variable to be added or updated in the dictionary
 *      arg - the integer value to be assigned to  var
 * Returns:
 * 		arg - the integer value assigned
 */
int CalcImpl::assignment(string var, int arg) {
    dictionary[var] = arg;
    return arg;
}

/*
 * Purpose: perform the calculation for the case 'operand'
 * Parameters:
 * 		tokens - the vector containing the tokenized user input
 *      fail - integer reference to an integer to mark whether or not the operation failed
 * Returns:
 * 		result - the integer result of the calculation
 */
int CalcImpl::oneArg(vector<string> tokens, int& fail) {
    string operand = tokens.at(0);
    int arg;
    if (changeStrToNum(operand, arg)) {
        return arg;
    } else {
        fail = 1;
        return fail;
    }
}

/*
 * Purpose: perform the calculation for the case 'operand op operand' or 'var = operand'
 * Parameters:
 * 		tokens - the vector containing the tokenized user input
 *      fail - integer reference to an integer to mark whether or not the operation failed
 * Returns:
 * 		result - the integer result of the calculation
 */
int CalcImpl::threeArgs(vector<string> tokens, int& fail) {
    int result = 0;
    string op = tokens.at(1);           // ASSIGNMENT vs. COMPUTATION
    string operand1 = tokens.at(0);     // VARIABLE vs. OPERAND
    string operand2 = tokens.at(2);     // MUST be defined
    int arg1;
    int arg2;

    if (!changeStrToNum(operand2, arg2)) {
        fail = 1;
        return result;
    }

    // CASE 1: ASSINGMENT
    if (op.at(0) == '=' && variableCheck(operand1) == VARIABLE) {
        result = assignment(operand1, arg2);
        return result;
    }

    // CASE 2: OPERATION
    if (!changeStrToNum(operand1, arg1)) {
        fail = 1;
        return result;
    } else {                            // operand1 must be defined for op
        int error = 0;
        result = operation(op, arg1, arg2, error);
        if (error) {
            fail = 1;
        }
        return result;
    }

    fail = 1;
    return result;
}

/*
 * Purpose: perform the calculation for the case 'var = operand op operand'
 * Parameters:
 * 		tokens - the vector containing the tokenized user input
 *      fail - integer reference to an integer to mark whether or not the operation failed
 * Returns:
 * 		result - the integer result of the calculation
 */
int CalcImpl::fiveArgs(vector<string> tokens, int& fail) {
    int result = 0;
    string eq = tokens.at(1);
    string op = tokens.at(3);
    string var = tokens.at(0);
    string operand1 = tokens.at(2);
    string operand2 = tokens.at(4);
    int arg1;
    int arg2;
    int arg;

    // operand1 and operand2 must be well-formed and/or defined
    if (!changeStrToNum(operand1, arg1) || !changeStrToNum(operand2, arg2)) {
        fail = 1;
        return result;
    }

    // Must be a variable assignment
    if (eq.at(0) != '=' || variableCheck(var) != VARIABLE) {
        fail = 1;
        return result;
    }

    int error = 0;
    arg = operation(op, arg1, arg2, error);
    if (error) {
        fail = 1;
        return result;
    }
    result = assignment(var, arg);
    return result;
}