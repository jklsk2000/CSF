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

//can we expect that they won't enter non-integers?

//should keep getting input until the user quits
//consider refactoring to pass in references to the token vector (avoid copies)

//make all codes final variables
const int MULTIPLY = 1; const int DIVIDE = 2;
const int ADD = 3; const int SUBTRACT = 4; const int EQUALS = 5; 
const int INVALID = 0; //consider changing this to -1?
const int VARIABLE = 1; const int INTEGER = 2; const int SUCCESS = 1;

struct Calc {
};

class CalcImpl : public Calc {
    public:
        map<string, int> dictionary; //check syntax
        int evalExpr(const char *expr, int *result);
        
        //helper functions 
        vector<string> tokenize(const string &expr);
        int * oneArg(vector<string> tokens, int &fail);
        int * threeArgs(vector<string> tokens, int &fail);
        int * fiveArgs(vector<string> tokens, int &fail);
        bool variableCheck(string variable, int &arg);
        int operandCheck(string operand);
        int symbolCheck(string symbol);
        bool changeStrToNum(string str, int &arg);
        int operation(string op, int arg1, int arg2);
        int assignment(vector<string> tokens, int arg2);
};

/*
 * Purpose: evaluate the user's entered expression
 * Parameters:
 * 		*expr - a const char pointer to the user input expression representation
 *      *result - pointer to hold the result of the calculation
 * Returns:
 * 		int, whether or not the calculation was successful(1) or invalid(0)
 */
int CalcImpl::evalExpr(const char *expr, int *result) {
    (void) result;
    vector<string> tokens = tokenize(expr);
    int fail = 0;
    //check tokens helper function (in the actual thing)
    int len = tokens.size();
    if (len == 1) {;
        *result = *oneArg(tokens, fail);
        if (fail) {
            cerr << "Error" << endl;
            return INVALID;
        }
        return SUCCESS;
    }
    else if (len == 3) {
        *result = *threeArgs(tokens, fail);
        if (fail) {
            cerr << "Error" << endl;
            return INVALID;
        }
        return SUCCESS;
    }
    else if (len == 5) {
        *result = *fiveArgs(tokens, fail);
        if (fail) {
            cerr << "Error" << endl;
            return INVALID;
        }
        return SUCCESS;
    }
    cerr << "Error" << endl;
    return INVALID;
}

/*
 * Purpose: create an instance of a CalcImpl object
 * Parameters:
 * 		void
 * Returns:
 * 		a new CalcImpl object
 */
extern "C" struct Calc *calc_create(void) {
    return new CalcImpl();
}

/*
 * Purpose: destroy an instance of a CalcImpl object
 * Parameters:
 * 		*calc - a pointer to a Calc struct
 * Returns:
 * 		void
 */
extern "C" void calc_destroy(struct Calc *calc) {
    CalcImpl *obj = static_cast<CalcImpl *>(calc);
    delete obj;
}

/*
 * Purpose: destroy an instance of a CalcImpl object
 * Parameters:
 * 		*calc - a pointer to a Calc struct
 *      *expr - a const char pointer to the user entered expression
 *      *result - a pointer to point to the result of the calculation
 * Returns:
 * 		an int, the return value of evalExpr that indicates whether or not a calculation was successful
 */
extern "C" int calc_eval(struct Calc *calc, const char *expr, int *result) {
    CalcImpl *obj = static_cast<CalcImpl *>(calc);
    return obj->evalExpr(expr, result);
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
 * Purpose: check that the operand the user entered is a valid variable name or integer
 * Parameters:
 * 		operand - a string representing the user input for operand
 * Returns:
 * 		int, the code for the whether the input was a variable, integer, or invalid
 */
int CalcImpl::operandCheck(string operand) {
    char firstChar = operand.at(0);
    int len = operand.length();
    int result; 

    //first char is A-z, we expect a variable name
    if (isalpha(firstChar)) { 
        result = VARIABLE;
        for (int i = 1; i < len; i++) {
            if (!isalpha(operand.at(i))) {
                return INVALID;
            }
        }
        return result;
    }

    //if it's not a variable, we expect an integer
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
    result = INTEGER;
    return result;
}

/*
 * Purpose: check that the symbol the user entered is *,+,-,/, or =
 * Parameters:
 * 		symbol - a string representing the user input for symbol
 * Returns:
 * 		int, the code for the resulting operation or invalid
 */
int CalcImpl::symbolCheck(string symbol) {

    //make sure it's just an operator
    if (symbol.length() != 1) {
        return INVALID;
    }

    char csymbol = symbol.at(0);

    //may not be necessary
    switch(csymbol) {
        case '*':
            return MULTIPLY;
        case '/':
            return DIVIDE;
        case '+':
            return ADD;
        case '-':
            return SUBTRACT;
        case '=':
            return EQUALS;
        default: 
            return INVALID;
    }
    return INVALID;
}

/*
 * Purpose: check that the symbol the user entered is *,+,-,/, or =
 * Parameters:
 * 		symbol - a string representing the user input for symbol
 * Returns:
 * 		int, the code for the resulting operation or invalid
 */
bool CalcImpl::variableCheck(string variable, int &arg) {
    map<string, int>::iterator it;
    it = dictionary.find(variable);
    if (it != dictionary.end()) {
        arg = it->second;
        return true;
    } else {
        return false;
    }
}

/*
 * Purpose: perform the calculation for when input is of the form 'operand'
 * Parameters:
 * 		tokens - a reference to a vector of Plugin structs (struct of function pointers for a plugin)
 * Returns:
 * 		result - an integer pointer to the result of the calculation
 */
bool CalcImpl::changeStrToNum(string str, int &arg) {
    int operandType = operandCheck(str);

    //convert well formed integer to a string
    if (operandType == INTEGER) {
        arg = atoi(str.c_str()); 
        return true;
        
    //check if the variable is in the dictionary (we know its value)
    } else if (operandType == VARIABLE) { 
        if (variableCheck(str, arg)) {
            return true;
        }
        return false;
    } else {
        return false; //dont 
    }
}

/*
 * Purpose: perform the calculation for when input is of the form 'operand'
 * Parameters:
 * 		tokens - a reference to a vector of Plugin structs (struct of function pointers for a plugin)
 * Returns:
 * 		result - an integer pointer to the result of the calculation
 */
int * CalcImpl::oneArg(vector<string> tokens, int &fail) {
    int * result;
    int dummy = 0;
    result = &dummy;    // avoid getting seg faults

    int arg;
    if (changeStrToNum(tokens.at(0), arg)) {
        result = &arg;
        return result; 
    } else {
        fail = 1;                            
        return result;
    }
}

/*
 * Purpose: perform the calculation for when input is of the form 'operand op operand' or 'var = operand'
 * Parameters:
 * 		tokens - a reference to a vector of Plugin structs (struct of function pointers for a plugin)
 * Returns:
 * 		result - an integer pointer to the result of the calculation
 */
int * CalcImpl::threeArgs(vector<string> tokens, int &fail) {
    int * result;
    int dummy = 0;
    result = &dummy;    // avoid getting seg faults

    int res;
    int symbol = symbolCheck(tokens.at(1)); 
    int arg1;
    int arg2;

    //check if well-formed
    if (operandCheck(tokens.at(0)) == INVALID || symbol == INVALID || operandCheck(tokens.at(2)) == INVALID) {
        fail = 1;
        return result;
    }

    if (!changeStrToNum(tokens.at(2), arg2)) {  // arg2 has to be a valid integer in both cases
        fail = 1;
        return result;  
    } 

    // Case 1 Assignment (variable = operand)
    if (symbol == EQUALS) {
        res = assignment(tokens, arg2);
        if (res == INVALID) {
            fail = 1;
            return result;
        }
        result = &res;
        return result;
    }

    //since it's not assignment, it's regular calculation, so check for malformed integer?
    if (!changeStrToNum(tokens.at(0), arg1)) {
        fail = 1;
        return result;  
    } 
    
    //Case 2 Operation (operand op operand)
    res = operation(tokens.at(1), arg1, arg2);
    if (res == INVALID) {
        fail = 1;
        return result;
    }
    result = &res;
    return result;
}

int CalcImpl::operation(string op, int arg1, int arg2) {
    int result;
    int r;
    char arg = op.at(0); 
    switch(arg) {
        case '*':
            r = arg1 * arg2;
            //*result = arg1 * arg2;
            result = r;
            return result;
        case '/':
            if (arg2 == 0) { // can't divide by 0
                return INVALID;
            }
            r = arg1 / arg2;
            //*result = arg1 / arg2;
            result = r;
            return result;
        case '+':
            r = arg1 + arg2;
            //*result = arg1 + arg2;
            result = r;
            return result;
        case '-':
            r = (arg1 - arg2);
            //*result = arg1 - arg2;
            result = r;
            return result;
        default: 
            result = INVALID;
            return result;
    }
    return result;
}

int CalcImpl::assignment(vector<string> tokens, int arg2) {
    int result;
    if (operandCheck(tokens.at(0)) != VARIABLE) {
        return INVALID;
    }
    dictionary[tokens.at(0)] = arg2;
    result = arg2;
    return result;
}


/*
 * Purpose: perform the calculation for when input is of the form 'var = operand op operand'
 * Parameters:
 * 		tokens - a reference to a vector of Plugin structs (struct of function pointers for a plugin)
 * Returns:
 * 		result - an integer pointer to the result of the calculation
 */
int * CalcImpl::fiveArgs(vector<string> tokens, int &fail) {
    int * result;
    int dummy = 0;
    result = &dummy;    // avoid getting seg faults

    string equal = tokens.at(1);
    string op = tokens.at(3);
    string operand1 = tokens.at(2);
    string operand2 = tokens.at(4);
    int arg1;
    int arg2;
    int argAfterOp;
    int varAfterAssign;

    // arg1 and arg2 must be well-formed and/or defined
    if (!changeStrToNum(operand1, arg1) || !changeStrToNum(operand2, arg2)) {
        fail = 1;
        return result;
    }

    if (symbolCheck(equal) != EQUALS) {
        fail = 1;
        return result;
    }

    if (symbolCheck(op) == INVALID) {
        fail = 1;
        return result;
    }

    argAfterOp = operation(op, arg1, arg2);
    varAfterAssign = assignment(tokens, argAfterOp);
    result = &varAfterAssign;
    return result;
}