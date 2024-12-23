#pragma once
/**
// Created by andyh on 12/22/24.
// Copyright (c) 2024 Andy Heilveil, (github/980f). All rights reserved.
*/

#pragma once

/** how tokenizing should be done, no statics under the hood like using strtok. **/
class Tokenizer {
  char *cursor;
  char *end;
  char terminator = 0;

public:
  Tokenizer(char *starting): cursor{starting}, end{nullptr} {}

  /** @returns whether another token found, call @see fetch to get it.
   * next char from delim found after moving starting past anything in @param whitespace (which can be whatever you like, just typically whitespace and CRLF  */
  bool hasNext(const char *delim, const char *whitespace = nullptr);

  /** after hasNext returns true, this @returns whether the token was framed by the given @param which */
  bool endedWith(char which) const {
    return terminator == which;
  }

  /** must be called once each time that hasNext returns true in order to move the parser along */
  char *fetch();
};
