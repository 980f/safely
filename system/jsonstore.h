#ifndef JSONSTORE_H
#define JSONSTORE_H

#include "storable.h"

#include "utf8.h"
#include <string>
class ByteScanner;//forward ref to speed build

/**
  * parse and print Storable to json-like file.
  * (recovered from code that was once inside Storable itself)
  * Each object parses one node, there is a static member that parses a tree.
  */
class JsonStore {

  class Parser {
    bool notStarted;
    /** Since we need to be able to interrupt parsing and come back later we can't use actual function call recursion to parse wads.
  * Therefore we make a stack of parents, which is all we need to maintain recursion-wise as we descend and ascend only at known states.
  * It happens that Storable is inherently a stack, popping the stack is replacing our local pointer with a pointer to the parent member of said local item.
  * We would like to have some parallel state for item to be matched such as type of quote or brace. For the brace we could add a boolean member to the Storable (indicating wad vs array) which would be a good thing for writing stuff back out (strictlyOrdered has been added but not yet honored).
  * State as to whether we are looking for a name or value does not need stacking.
  */
    /** we are parsing a wad, unless node is null in which case we are parsing the root item of a tree.
    ArtTree needs to be reworked to use a pointer unless we can invoke this parser in its constructor.
    Note:  if node->parent == null and node->numChildren() == 0, this is the root and we should not create a wad  child upon seeing an open brace.*/
    StorageWalker node;
    /** stores the name of the current node */
    Text name;
    /** Adds child by current name and sets value */
    void addNode(std::string &tokenImage);
  public:
    Parser(Storable &rootNode);
    /** the lexer will call this.
  * Our use of type char for the term precludes using non-ascii separators.*/
    bool acceptToken(std::string&tokenImage, UTF8 term);
  };

  /** breaks up byte stream into strings and the character that separates them.*/
  class Lexer {
    /** fixed rules for now:*/
    static const char *separator;
    enum LexicalState {
      PreWhite,
      InQuote,
      InText, //we don't distinguish numbers here, not our problem
      PostWhite
    };
    /** whether the next char is NOT to be investigated as a potential separator*/
    bool escaped;
    LexicalState state;
    /** we need our own copy of incoming text, so source doesn't have to keep it around.*/
    std::string accumulator;
    Parser&parser;
  public:
    Lexer(Parser&parser);
    /** forget all state, good for error recovery*/
    void start();
    /** push bytes at this parser. utf8 sequences pass on through uninterpreted and not converted.
  * C-escapes also pass through unanalyzed, the only escaping is of quote but the complete escape sequence is passed through. */
    void accept(char ch);

  };

  /**
  *  We might change lexers in the future so we use two objects rather than merging the two into one object.
  */
  Parser parser;
  Lexer lexer;
public:
  JsonStore(Storable &rootNode);
  /** file reader or socket receiver calls this. @returns whether state is hopelessly mangled. */
  bool parse(ByteScanner&is);

  class Printer {
    unsigned tablevel;
    std::ostream&os;
    void indent();

    /** print text that will parse properly on way back in */
    void printText(const char *p, bool forceQuote = false);

    /** we recurse via the parent pointer of the Storable*/
    StorageWalker node;
    /** @returns whether a 'name:' was emitted == name is not empty.*/
    bool printName();
<<<<<<< HEAD
=======

>>>>>>> dp5qcu
    /** print children */
    void printWad();

  public:
    bool printValue();
    Printer(Storable&node, std::ostream&os);
  };

};



#endif // JSONSTORE_H
