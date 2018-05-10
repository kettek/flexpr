#include "Expressionator.hpp"
#include <iostream>
#include <cmath>
#include <stdlib.h> // srand, rand
#include <time.h> // time

namespace Expressionator {
  bool TokenStack::empty() {
    return (stack.size() == 0 ? true : false);
  }
  size_t TokenStack::size() {
    return stack.size();
  }
  void TokenStack::push(Token token) {
    stack.push_back(token);
  }
  Token TokenStack::pop() {
    if (empty()) return Token();
    Token tok = stack.back();
    stack.pop_back();
    return tok;
  }
  void TokenStack::unshift(Token token) {
    stack.insert(stack.begin(), token);
  }
  Token TokenStack::shift() {
    if (empty()) return Token();
    Token tok = stack.front();
    stack.erase(stack.begin());
    return tok;
  }
  Token TokenStack::peek() {
    if (empty()) return Token();
    return stack.back();
  }

  long long rollDice(int count, int pips) {
    long long value = 0;
    srand(time(NULL));
    if (count == 0 || pips == 0) return value;
    for (int i = 0; i < count; i++) {
      value += rand() % pips + 1;
    }
    return value;
  }
  
  long long doOperation(Token token, Token a, Token b) {
    switch (token.v) {
      case '+':
        return a.v + b.v;
      case '-':
        return a.v - b.v;
      case '*':
        return a.v * b.v;
      case '/':
        return a.v / b.v;
      case '^':
        return a.v ^ b.v;
      case 'd':
        return rollDice(a.v, b.v);
    }
    return 0;
  }
  
  int getTokenPrecedence(Token token) {
    switch (token.v) {
      case '+':
      case '-':
        return 2;
      case '/':
      case '*':
        return 3;
      case '^':
        return 4;
      case 'd':
        return 5;
    }
    return 1;
  }
  
  bool compareTokenPrecedence(Token a, Token b) {
    if (getTokenPrecedence(a) < getTokenPrecedence(b)) return true;
    return false;
  }
  
  TokenStack infixToPostfix(std::string text) {
    TokenStack output;
    TokenStack stack;
    std::string number;
    Token tok;
  
    for (size_t i = 0; i < text.length(); i++) {
      tok.v = text[i];
      switch (tok.v) {
        case '0': case '1': case '2':
        case '3': case '4': case '5':
        case '6': case '7': case '8':
        case '9': case '.':
          number += tok.v;
          while (i+1 < text.length()) {
            tok.v = text[++i];
            if (tok.v == '0' || tok.v == '1' || tok.v == '2'
            || tok.v == '3' || tok.v == '4' || tok.v == '5'
            || tok.v == '6' || tok.v == '7' || tok.v == '8'
            || tok.v == '9' || tok.v == '.') {
              number += tok.v;
            } else {
              tok.v = text[--i];
              break;
            }
          }
          try {
            output.push(Token(NUMBER, std::stoll(number, nullptr)));
          } catch (...) {
            output.push(Token(NUMBER, 0));
          }
          number = "";
        break;
        // operators
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
        case 'd':
          while (!stack.empty() && stack.peek().v != '(') {
            if (compareTokenPrecedence(stack.peek(), tok))  // if top < token
              break;
            output.push(stack.pop());
          }
          stack.push(tok);
          break;
        // parenthesis
        case '(':
          stack.push(tok);
          break;
        case ')': // for ')', pop stack until '('
          while (!stack.empty() && stack.peek().v != '(') {
            output.push(stack.pop());
          }
          stack.pop(); // remove '('
          break;
      }
    }
    while (!stack.empty()) {
      output.push(stack.pop());
    }
  
    return output;
  }
  
  std::string evaluateInfix(std::string text) {
    std::string out_string;
    TokenStack output;
    TokenStack expression = infixToPostfix(text);
  
    while (!expression.empty()) {
      Token tok = expression.shift();
      if (tok.t == NUMBER) {
        output.push(tok);
      } else {
        output.push(Token(NUMBER, doOperation(tok, output.pop(), output.pop())));
      }
    }
    for (size_t i = 0; i < output.size(); i++) {
      out_string += std::to_string(output.pop().v);
    }
    return out_string;
  }
}
