#ifndef KANJIDIC_H
#define KANJIDIC_H

/** Kanji char lookup, with a bias towards japanese.
 *  This includes parsing the KANJIDIC file into something with which we can rapidly map hiragana Kun into Kanji chars.
 *  Depending upon speed we will cache that map in a file.
 */

#include "textpointer.h"  //unicode string used by our text editor

/** which  Kanji subset to scan for possible */
enum CJK {
  Kunyomi,Onyomi,Traditional,Simplified,Korean
};
class Storable;//for debug access
/**
 *  using an interface to hide the use of Storable in the implementation.
 */
class KanjiDic {
private: //this is a service, not an object
  KanjiDic();
public:
  /** call to preprocess any files, will also be called automatically on first use.*/
  static void init(bool forceful = false);
  /** @deprecated, most frequent cases have multiple candidates
   * @return single result for hiragana string, sets length to how much of the trailing part of @param hiraganaFragment was actually
   * part of the name (may drop this if we choose a strict lookup)*/
  static Unichar charFor(const Text &hiraganaFragment, int &);

  /** @returns fresh listing of kanji whose @param reading is @param name.
   * Someday might chose to do 'startsWith'
   * */
  static Text candidates(const Text &name,CJK nameset);

  /** factory access to guts of kanji tables*/
  static Storable &debugNode();
}; // class KanjiDic

#endif // KANJIDIC_H
