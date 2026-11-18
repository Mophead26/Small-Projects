#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>
#include <stack>
#include <limits>

enum class TokenType {
    START,
    VARIABLE,    // a, b, c
    BINARY_OP,   // &, |, ^
    UNARY_OP,    // ~
    OPEN_PAREN,  // (
    CLOSE_PAREN  // )
};

std::string colorBit(bool bit) {
    // Returns Green "1" if true, Red "0" if false, then resets color to normal
    return bit ? "\033[32m1\033[0m" : "\033[31m0\033[0m";
}

bool isValidLogicExpression(const std::string& expr) {
    int parenDepth = 0;
    TokenType lastType = TokenType::START;
    bool hasVariable = false;

    for (size_t i = 0; i < expr.length(); ++i) {
        char c = expr[i];

        // 1. Skip whitespace safely (in case this function is called directly)
        if (std::isspace(static_cast<unsigned char>(c))) {
            continue;
        }

        // 2. Validate against character whitelist and enforce state grammar
        if (c >= 'a' && c <= 'z') {
            // A variable cannot directly follow another variable or a closing parenthesis
            if (lastType == TokenType::VARIABLE || lastType == TokenType::CLOSE_PAREN) {
                return false;
            }
            lastType = TokenType::VARIABLE;
            hasVariable = true;
        }
        else if (c == '&' || c == '|' || c == '^') {
            // Binary operators cannot start the expression or follow an operator/open parenthesis
            if (lastType == TokenType::START || lastType == TokenType::BINARY_OP || 
                lastType == TokenType::UNARY_OP || lastType == TokenType::OPEN_PAREN) {
                return false;
            }
            lastType = TokenType::BINARY_OP;
        }
        else if (c == '~') {
            // Unary NOT cannot follow a variable or a closing parenthesis
            if (lastType == TokenType::VARIABLE || lastType == TokenType::CLOSE_PAREN) {
                return false;
            }
            lastType = TokenType::UNARY_OP;
        }
        else if (c == '(') {
            // Open parenthesis cannot follow a variable or closing parenthesis
            if (lastType == TokenType::VARIABLE || lastType == TokenType::CLOSE_PAREN) {
                return false;
            }
            parenDepth++;
            lastType = TokenType::OPEN_PAREN;
        }
        else if (c == ')') {
            // Close parenthesis cannot follow an operator or an open parenthesis
            if (lastType == TokenType::START || lastType == TokenType::BINARY_OP || 
                lastType == TokenType::UNARY_OP || lastType == TokenType::OPEN_PAREN) {
                return false;
            }
            parenDepth--;
            if (parenDepth < 0) return false; // Mismatched right parenthesis ')'
            
            lastType = TokenType::CLOSE_PAREN;
        }
        else {
            // Found a completely illegal character
            return false;
        }
    }

    // 3. Final structural requirements
    if (parenDepth != 0) return false; // Unclosed parenthesis
    if (!hasVariable) return false;    // An expression must contain at least one variable
    
    // An expression cannot end with an operator
    if (lastType == TokenType::BINARY_OP || lastType == TokenType::UNARY_OP) return false;

    return true;
}

bool isValid(std::string& expr) {
    // Strip all whitespace in-place
    expr.erase(std::remove_if(expr.begin(), expr.end(), 
               [](unsigned char c) { return std::isspace(c); }), expr.end());
    
    // Check that expr is not empty ("")
    if (expr.empty()) {
        std::cout << "The entered expression cannot be empty!\n";
        return false;
    }

    // Check expr is formed properly
    if (!isValidLogicExpression(expr)) {
        std::cout << "The entered expression is invalid.\n"
                  << "Make sure the expression contains only valid characters and is properly formed!\n";
        return false;
    }

    // expr is valid
    return true;
}

int getPrecedence(char op) {
    switch (op) {
        case '~':
            return 4;
        case '&':
            return 3;
        case '^':
            return 2;
        case '|':
            return 1;
        }
    return 0;
}

std::string getUniqueVars(const std::string& expr) {
    std::string vars;
    for (char c : expr) {
        if (c >= 'a' && c <= 'z' && vars.find(c) == std::string::npos) {
            vars += c;
        }
    }
    std::sort(vars.begin(), vars.end()); // Keeps columns in alphabetical order!
    return vars; // e.g., returns "ab" or "abc" or "pqz"
}

std::string infixToPostfix(std::string expr) {
    std::string postfix;
    std::stack<char> opStack;

    for (char c : expr) {
        // Case 1: 'c' is a variable
        if (c >= 'a' && c <= 'z') postfix += c;

        // Case 2: 'c' is '(' || 'c' is '~' 
        else if (c == '(' || c == '~') opStack.push(c);

        // Case 3: 'c' is close parenthesis
        else if (c == ')') {
            while (!opStack.empty() && opStack.top() != '(') {
                postfix += opStack.top();
                opStack.pop();
            }
            opStack.pop();
        }

        // Case 4: 'c' is a binary operator
        else if (c == '&' || c == '^' || c == '|') {
            while (!opStack.empty() && getPrecedence(opStack.top()) >= getPrecedence(c)){
                postfix += opStack.top();
                opStack.pop();
            }
            opStack.push(c);
        }
    }
    while (!opStack.empty()) {
        postfix += opStack.top();
        opStack.pop();
    }
    return postfix;
}

bool evaluatePostfix(const std::string& postfix, const bool varValues[26]) {
    std::stack<bool> valStack;

    for (char c : postfix) {
        if (c >= 'a' && c <= 'z') {
            valStack.push(varValues[c - 'a']);
        }

        else if (c == '~') {
            bool topVal = valStack.top();
            valStack.pop();
            valStack.push(!topVal);
        }
        else { // Must be a binary operator: '&', '|', or '^'
            bool rightVal = valStack.top();
            valStack.pop();
            bool leftVal = valStack.top();
            valStack.pop();

            if (c == '&')      valStack.push(leftVal && rightVal);
            else if (c == '|') valStack.push(leftVal || rightVal);
            else if (c == '^') valStack.push(leftVal != rightVal);
        }
    }
    return valStack.top();
}

std::string printTruthTable(const std::string& expr) {
    std::string truthVal = ""; // For comparison between expressions

    std::string vars = getUniqueVars(expr);
    std::string postfix = infixToPostfix(expr);
    int numVars = vars.length();
    int totalRows = 1 << numVars; // 2^numVars

    // Print Divider Line
    int totalWidth = (numVars * 4) + expr.length();
    std::cout << std::string(totalWidth, '-') << "\n";

    // Print Header
    for (char c : vars) {
        std::cout << c << " | ";
    }
    std::cout << expr << "\n";

    // Print Divider Line
    std::cout << std::string(totalWidth, '-') << "\n";

    // Padding to center the 0/1 result under the expression
    std::string centerPad((expr.length() - 1) / 2, ' ');

    // 3. Print Rows with Colored 1s and 0s
    bool varValues[26] = {false};
    for (int row = 0; row < totalRows; row++) {
        for (int i = 0; i < numVars; i++) {
            bool bit = (row >> (numVars - 1 - i)) & 1;
            char varChar = vars[i];
            varValues[varChar - 'a'] = bit;

            std::cout << colorBit(bit) << " | ";
        }

        bool result = evaluatePostfix(postfix, varValues);
        std::cout << centerPad << colorBit(result) << "\n";

        truthVal += (result ? "1" : "0");
    }
    // Print Divider Line
    std::cout << std::string(totalWidth, '-') << "\n\n";

    return truthVal;
}

std::string getInput() {
    std::string expr;

    std::cout << "\n" << "\033[37mEnter a Logical Expression\033[0m\n"
              << "\033[37mVariables: \033[32mAny lowercase letter\033[0m\n"
              << "\033[37mAllowed Characters:\033[32m & | ^ ~ ()\033[0m\n";

    // Safely read lines and validate without risking an infinite EOF loop
    while (std::getline(std::cin, expr)) {
        if (isValid(expr)) {
            break;
        }
        std::cout << "\nPlease try again:\n";
    }
    
    // Ensure we didn't exit the loop due to an unexpected EOF/closed input stream
    if (std::cin) {
        std::cout << "\nGenerating expression's truth table for: " << expr << "\n\n";
    }
    return expr;
}


int main() {
    std::string expr = getInput();
    std::string expr1val = printTruthTable(expr);

    std::string expr2;
    std::string expr2val;

    std::cout << "\n\033[37mWould you like to comapre to another expression? (y/n):\033[0m ";
    std::string response;
    std::cin >> response;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (response == "y") {
        expr2 = getInput();
        expr2val = printTruthTable(expr2);

        if (expr1val == expr2val) std::cout << "\n\033[32mBoth Logical Expressions are Logically Equivalent!\033[0m\n\n";
    }

    return 0;
}