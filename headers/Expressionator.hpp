#pragma once
#include <string>
#include <vector>

namespace Expressionator {
  struct Token {
    char t = 0;
    int v;
    Token() {}
    Token(char type, int value) : t(type), v(value) { }
  };
  #define NUMBER 1
  #define CHAR 2
  
  class TokenStack {
    private:
      std::vector<Token> stack;
    public:
      bool empty();
      size_t size();
      void push(Token token);
      Token pop();
      void unshift(Token token);
      Token shift();
      Token peek();
  };
  
  int doOperation(Token token, Token a, Token b);
  int getTokenPrecedence(Token token);
  bool compareTokenPrecedence(Token a, Token b);
  TokenStack infixToPostfix(std::string text);
  std::string evaluateInfix(std::string text);

  int rollDice(int count, int pips);
}
