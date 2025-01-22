#include "charscanner.h"
#include "cstr.h"
#include "char.h"
#include "onexit.h"

//this one is sharable, with care! You should never be calling wrap or clone on a reference.
CharScanner CharScanner::Null;


int ourStrncmp(const char *one, const char *two, unsigned length) {
  for (unsigned i = 0; i < length; ++i) { //# in order
    if (one[i] > two[i]) {
      return 1;
    }
    if (one[i] < two[i]) {
      return -1;
    }
    //char are equal to get here. If they are null then we return equal, if not then we already returned.
    if (one[i] == 0) { //only need to test one here.
      return 0;
    }
  }
  return 0;
} // ourStrncmp

bool ByteScanner::putBytes(unsigned value, unsigned numBytes) {
  if (stillHas(numBytes)) {
    const uint8_t *p = reinterpret_cast<const uint8_t *>(&value);
    if (bigendian) {
      p += numBytes; //past the msb of interest
      while (numBytes-- > 0) {
        next() = *--p;
      }
    } else {
      while (numBytes-- > 0) {
        next() = *p++;
      }
    }
    return true;
  } else {
    return false;
  }
} /* putBytes */

uint32_t ByteScanner::getU(unsigned numBytes, uint32_t def) {
  //using a pointer to a local precludes compiler optimizing for register use.
  if (stillHas(numBytes)) {
    uint32_t acc = 0;
    if (bigendian) {
      uint8_t *pun = reinterpret_cast<uint8_t *>(&acc) + numBytes;
      while (numBytes-- > 0) {
        *--pun = next();
      }
    } else {
      copyObject(&peek(), &acc, numBytes);
      skip(numBytes);
    }
    return acc;
  } else {
    return def;
  }
} // ByteScanner::getU

void ByteScanner::getTailOf(const CharScanner &other) {
  //#invalid cast:    grab(reinterpret_cast<ByteScanner>(other));
  buffer = reinterpret_cast<uint8_t *>(other.buffer);
  if (other.ordinal() > 0) { //want front end.
    length = other.pointer;
    pointer = 0;
  } else { //was already rewound and truncated
    length = other.length;
    pointer = other.pointer;
  }
} // ByteScanner::grab

ByteScanner::ByteScanner() {
  //#nada
}

ByteScanner::ByteScanner(uint8_t *content, unsigned size) : Indexer(content, size) {
  //#nada
}

ByteScanner::ByteScanner(const ByteScanner &other, int clip) : Indexer(other, clip) {
  //#nada
}

ByteScanner::ByteScanner(const CharScanner &other) : //choices herein are for first use which is in type casting a ready-to-send string.
  Indexer<uint8_t>(reinterpret_cast<uint8_t *>(other.internalBuffer()), other.used()) {
  //#nada
}


uint16_t ByteScanner::getuint16_t(uint16_t def) {
  return uint16_t(getU(2, def));
}

uint32_t ByteScanner::getU24(uint32_t def) {
  return getU(3, def);
}

uint32_t ByteScanner::getuint32_t(uint32_t def) {
  return getU(4, def);
}

bool ByteScanner::putuint16_t(unsigned value) {
  return putBytes(value, 2);
}

bool ByteScanner::putU24(unsigned value) {
  return putBytes(value, 3);
}

bool ByteScanner::putuint32_t(unsigned value) {
  return putBytes(value, 4);
}

ByteScanner ByteScanner::subset(unsigned fieldLength, bool removing) {
  Indexer punter(Indexer::subset(fieldLength, removing));
  return ByteScanner(punter.internalBuffer(), punter.allocated());
}

bool ByteScanner::chuckSpaces() {
  while (hasNext()) {
    if (next() != ' ') {
      rewind(1);
      break;
    }
  }
  return hasNext();
}

///////////////////
CharScanner::CharScanner() {
  //#nada
}

CharScanner CharScanner::infer(TextKey content) {
  Cstr wrap(content);
  return CharScanner(wrap.violated(), wrap.length());
}

CharScanner::CharScanner(char *content, unsigned size) : Indexer<char>(content, size) {
  //#nada
}

CharScanner::CharScanner(const CharScanner &other, int clip) : Indexer<char>(other, clip) {
  //#nada
}

/** grabs used part */
CharScanner::CharScanner(const ByteScanner &other, int clip) : //
  Indexer<char>(reinterpret_cast<char *>(other.internalBuffer()), clip ? other.used() : other.allocated()) {
  //#nada
  pointer = 0;
}

CharScanner::CharScanner(const CharScanner &other) : CharScanner(other, 0) {
  //#nada
}

CharScanner::CharScanner(const Indexer<char> &other): Indexer<char>(other, 0) {
  //#nada
}

CharScanner::CharScanner(const Indexer<uint8_t> &other): Indexer<char>(reinterpret_cast<char *>(other.internalBuffer()), other.allocated()) {
  //#nada
}


/** ensure content is null terminated at present pointer.
 * maydo: return null if we can't put a null at the end
 * maydo: add argument for 'urgent' or not, and if not urgent see if there is a null before the end, not just at the end
 */
TextKey CharScanner::asciiz() {
  if (length == 0) { //then we don't have a place for a terminating null
    return ""; //so point to a universal empty string.
  }
  if (!hasPrevious() || previous() != 0) { //if not already terminated nicely
    if (hasNext()) { //add a terminator if we can
      next() = 0;
    } else { //else destroy the last char, termination if far more important then completeness of the string.
      //if we were prone to throw exceptions we would do that here
      previous() = 0;
    }
  }
  return reinterpret_cast<TextKey>(internalBuffer());
} /* asciiz */

bool CharScanner::isTerminal() {
  return !hasNext() || buffer[pointer] == 0;
}

bool CharScanner::operator ==(const CharScanner &rhs) const {
  CharScanner me(*this);
  CharScanner other(rhs);
  while (me.hasNext()) {
    if (other.hasNext()) {
      if (me.next() != other.next()) {
        return false; //char mistmatched
      }
    } else {
      return !me.next(); //this startswith literal but  is longer
    }
  }
  return !other.hasNext() || !other.next(); //this is shorter
} // ==

/** for use with trusted rhs strings */
bool CharScanner::operator ==(const char *literal) const {
  if (!literal) {
    return used() == 0; //null pointer matches empty string
  }
  /** todo: get rid of the following strncmp */
  if (0 == ourStrncmp(internalBuffer(), literal, used())) { //then we MIGHT have a match
    //strncmp stops at first null in either string, or after 'used' items
    if (buffer[pointer - 1]) { //no null terminator
      return literal[pointer] == 0;
    } else {
      return literal[pointer - 1] == 0;
    }
  } else {
    return false;
  }
} // ==

bool CharScanner::matches(const char *s) {
  if (!s) {
    return false; //never match a null pointer to anything herein, even an empty one.
  }
  CharScanner me(*this);

  while (me.hasNext() && *s) {
    if (*s++ != me.next()) {
      return false;
    }
  }
  return !me.hasNext() || me.next() == 0; //occasionally we get a null term on our counted string.
} // CharScanner::matches

void CharScanner::operator =(CharScanner &&other) noexcept{
  this->buffer = other.buffer;
  this->length = other.allocated();
  this->pointer = other.used();
}

void CharScanner::trimNulls(void) {
  while (hasPrevious() && previous() == 0) {
    unget();
  }
}

bool CharScanner::isBlank() const {
  if (length == 0) {
    return true;
  }
  CharScanner content(*this);
  while (content.hasNext()) {
    char ch(content.next());
    if (!ch) {
      return true; //c string rule
    }
    if (!isPresent("\t\n ", ch)) {
      return false;
    }
  }
  return true;
}

CharScanner CharScanner::cut(char separator) {
  if (hasNext()) {
    Index termlocation(findInTail(separator));
    if (termlocation.isValid()) { //return from pointer to termlocation
      AssignOnExit<unsigned> aoe(pointer, termlocation + 1); //move past terminator, but not until we've grabbed our reference
      buffer[termlocation] = 0;
      unsigned pallocated = termlocation - pointer; //if pointer was the separator then termlocation ==separator and we pass back a zero length buffer point at a null, should the caller forget to check the length.
      return CharScanner(&peek(), pallocated);
    } else {
      AssignOnExit<unsigned> aoe(pointer, allocated()); //consume remainder
      return CharScanner(&peek(), freespace());
    }
  } else {
    return CharScanner();
  }
}

bool CharScanner::putBytes(unsigned value, unsigned numBytes) {
  if (stillHas(numBytes)) {
    const uint8_t *p = reinterpret_cast<const uint8_t *>(&value);
    if (bigendian) {
      p += numBytes; //past the msb of interest
      while (numBytes-- > 0) {
        next() = *--p;
      }
    } else {
      while (numBytes-- > 0) {
        next() = *p++;
      }
    }
    return true;
  } else {
    return false;
  }
} /* putBytes */

uint32_t CharScanner::getU(unsigned numBytes, uint32_t def) {
  //using a pointer to a local precludes compiler optimizing for register use.
  if (stillHas(numBytes)) {
    uint32_t acc = 0;
    if (bigendian) {
      uint8_t *pun = reinterpret_cast<uint8_t *>(&acc) + numBytes;
      while (numBytes-- > 0) {
        *--pun = next();
      }
    } else {
      copyObject(&peek(), &acc, numBytes);
      skip(numBytes);
    }
    return acc;
  } else {
    return def;
  }
} // ByteScanner::getU

uint16_t CharScanner::getU16(uint16_t def) {
  return getU(2, def);
}

uint32_t CharScanner::getU24(uint32_t def) {
  return getU(3, def);
}

uint32_t CharScanner::getU32(uint32_t def) {
  return getU(4, def);
}

bool CharScanner::putU16(unsigned value) {
  return putBytes(value, 2);
}

bool CharScanner::putU24(unsigned value) {
  return putBytes(value, 3);
}

bool CharScanner::putU32(unsigned value) {
  return putBytes(value, 4);
}

///////////////////
ByteLooker::ByteLooker(const uint8_t *content, unsigned size) : Indexer<const uint8_t>(content, size) {
  //#nada
}

uint32_t ByteLooker::getU(unsigned numBytes, uint32_t def) {
  //using a pointer to a local precludes compiler optimizing for register use.
  if (stillHas(numBytes)) {
    uint32_t acc = 0;
    copyObject(&peek(), &acc, numBytes);
    skip(numBytes);
    return acc;
  } else {
    return def;
  }
} /* getU */


bool ByteScanner::nextWord(Indexer<const char> other) {
  while (other.hasNext() && this->hasNext()) {
    if (this->peek() == other.next()) {
      this->next();
    } else {
      rewind(other.used());
      return false;
    }
  }
  return true; //
}

bool ByteScanner::find(Indexer<const char> other) {
  unsigned mark = used();
  if (other.hasNext()) {
    char first = other.next();
    while (hasNext()) {
      if (next() == first) {
        if (nextWord(other)) { //rest of chars match
          return true;
        }
      }
    }
    rewind(used() - mark);
    return false;
  }
  return false; //never find 'nothing'
}
