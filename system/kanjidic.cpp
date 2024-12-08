#include <cppext/utf8.h>
#include "kanjidic.h"

#include "storable.h"
#include "storednumeric.h"

#include "safely.h"
//#include "l10n.h" //for options that must persist when this guy's artfile is lost.
#include <cstdio>
#include <filename.h>

#include "range.h"

#include "chainsorter.h"    //for unihan filtering
#include "char.h"
/**
 *  lookup list of kanji unicodes given hiragana or pinyin string.
 *  the kanji's are not unique, nor are the text keys.
 */


/** parse 4 hex digits as unsigned integer, moving @param mark textpointer past last hex digit*/
static Unichar parse4hex(const char *&mark){
  Unichar kanjiuni(0);
  while(Char ch=*mark++ &&!ch.isWhite()) {
    kanjiuni *= 16;
    kanjiuni += ch.hexDigit();
  }
  return kanjiuni;
}

/** skip two characters then parse 4 hex digits as unsigned integer, moving @param mark textpointer past last hex digit*/
static Unichar parseUPlus(const char *&mark){
  mark += 2;//skip "U+"
  return parse4hex(mark);
}

/**
 *  lookup list of kanji unicodes given hiragana or pinyin string.
 *  the kanji's are not unique, nor are the text keys.
 */

/** unicode range for cjk unified characters, we wish to exclude the appendices */
static const Range<Unichar> CjkUnified(0x9fff,0x4e00);

struct KanjiLookup : public ArtFile {
  StoredBoolean haveCached;

  /** hiragana index. Note: this pushes on our Art file parser as we now have utf8 child names. */
  Storable &KUN;
  /** katakana index, @see enableON */
  Storable &ON;
  /** zh-zh */
  Storable &traditional;
  /** zh-tw */
  Storable &simplified;
  /** ascii index */
  Storable &Korean;


  Storable &set(CJK nameset){
    switch(nameset) {
    default: //pathological error, lump in with first set of interest.
    case CJK::Kunyomi:
      return KUN;
    case CJK::Onyomi:
      return ON;
    case CJK::Traditional:
      return traditional;
    case CJK::Simplified:
      return simplified;
    case CJK::Korean:
      return Korean;
    } // switch
  } // set

  /** add a new reading for a latinized knaji name, often creating a new entry*/
  void addReading(Storable&set,const char *textkey,Unichar kanjicode){
    Storable &index(set.child(textkey));
    index.setType(Storable::Textual);//todo:1 add to storable a setting for 'type for new children'
    //# Storable does not offer a reference into its image as it is designed to notify on changes. A reference access to its values would break that contract.
    Text readings=index.image();
    if(readings.find(kanjicode)==BadIndex) {
      readings.append(1,kanjicode);
      index.setImage(readings);
    } else {
      //we were getting multiple identical kanji's
      dbg("duplicate kanji ignored.");
    }
  } // addReading

  /** add a new reading for a hiragana name, often creating a new entry*/
  void addReading(Storable&set,const Glib::ustring &yomi,Unichar kanjicode){
    addReading(set,yomi.c_str(),kanjicode);
  }

  KanjiLookup() : ArtFile("kanji","l10n"),  //during debug put kanji table in root of l10n. Might use links to share from jp and cn directories.
    ConnectChild(haveCached,false),
    ConnectChild(KUN),
    ConnectChild(ON),
    ConnectChild(traditional),
    ConnectChild(simplified),
    ConnectChild(Korean),
    hanziSorter(hanziGroup,&HanziGroup::compare){

  }

  /// kanjidic specific
  /** @returns whether @param euc is strictly a katakana char */
  static bool isEucKatakana(u16 euc){
    return euc>=0xa5a1 && euc<=0xa5f6;
  }

  /** @returns whether @param euc is strictly a hiragana char */
  static bool isEucHiragana(u16 euc){
    return euc>=0xa4a1 && euc<=0xa4f3;
  }

  /** @returns unicode for either katakana or hiragana, but not yet punctuation or other operators.*/
  static Unichar euc2uni(u16 euc){
    if(isEucHiragana(euc)) {
      return euc - 0xa4a1 + 0x3041;
    }
    if(isEucKatakana(euc)) {
      return euc - 0xa5a1 + 0x30a1;
    }
    //might translate punctuation someday. also double wide ascii and such.
    return 0;
  }

  /** @returns whether @param c is strictly a hiragana character */
  static bool isHiragana(Unichar c){//duplicate in Romaji
    return c>0x03040 && c<0x3097; //todo:0 add iteration mark
  }

  /** @returns whether @param c is strictly a katakana character.
   *  //todo:1 debate whether punctuation should be included. */
  static bool isKatakana(Unichar c){
    return c>0x030A0 && c<0x30F7;  //todo:0 add iteration mark
  }

  /** @returns a utf8 string for a set of euc-jp characters (jis-208) stopping at either end-of-string OR a period.
   *@deprecated limited for use in this module, see single char translators for details.*/
  static Text euc2unis(const char* eucp,int length){
    Glib::ustring utf8;
    utf8.reserve(length / 2);//length is odd only when some non-jis 208 punctuation is present, in which case we stop early
    while(*eucp && length-->0) {//length is guard against failure to null terminate string
      if(*eucp=='.') {
        break;//special rule for kanjidic, separates okurigani from name of kanji
      }
      u16 euchar = 256 * *eucp++;
      euchar += *eucp++;
      utf8.append(1,euc2uni(euchar));
    }
    return utf8;
  } // euc2unis

  /** parse one line of kanjidic */
  void parseLine(char *mark){
    char *term(mark);
    Unichar kanjiuni(0);
    if(*mark=='#') {
      return;//comment line.
    }
    while(1) {
      for(term = mark; *term&&*term!=' '; ++term) {//find space or end of line
      }
      *term = 0;//terminates the field with a null.
      switch(*mark) {
      case 'U'://if mark is 'U' then remainder is hex digits of unicode char
        kanjiuni = 0;
        while(++mark<term) {
          kanjiuni *= 16;
          kanjiuni += hexValue(*mark);
        }
        break;
      case 'T': //if mark is 'T' discard rest
        return;
      case '{': //if mark is '{' discard rest, FYI: is an actual definition
        return;
      // we only take chinese from unihan files, not kanjidic.gz
      //      case 'Y': //pinyin
      //        if(l10n->jp.enablePinyin){
      //          addReading(traditional,++mark,kanjiuni);
      //        }
      //        break;
      //      case 'W'://korean
      //        if(l10n->jp.enableKorean){
      //          addReading(Korean,++mark,kanjiuni);
      //        }
      //        break;
      default:
        if(!isascii(*mark)) {//tihs test saves some time
          Glib::ustring euc(euc2unis(mark,term - mark));//convert from euc-jp to utf8
          Unichar first(euc.at(0));
          if(kanjiuni==0x4e94) {
            wtf("don't pass go");
          }
          if(isHiragana(first)) {//if mark ishiragana save as KUN reading.
            addReading(KUN,euc,kanjiuni);
          } else if(l10n->jp.enableON && isKatakana(first)) {//if katalookup enabled and mark iskatakana save as ON reading (if exists unicode char)
            addReading(ON,euc,kanjiuni);
          }
        }
      } // switch
      mark = ++term;
    }//parseField
  } // parseLine

  /** from unicode website tr38:
   *
   *  X is used in both SC and TC and is unchanged when mapping between them. An example would be 井 U+4E95. This is the most common case, and is indicated by both the
   * kSimplifiedVariant and kTraditionalVariant fields being empty.
   *  X is used in TC but not SC, that is, it is changed when converting from TC to SC, but not vice versa. In this case, the kSimplifiedVariant field lists the
   * character(s) to which it is mapped and the kTraditionalVariant field is empty. An example would be 書 U+66F8 whose kSimplifiedVariant field is 书 U+4E66.
   *  X is used in SC but not TC, that is, it is changed when converting from SC to TC, but not vice versa. In this case, the kTraditionalVariant field lists the
   * character(s) to which it is mapped and the kSimplifiedVariant field is empty. An example would be 学 U+5B66 whose kTraditionalVariant field is 學 U+5B78.
   *  X is used in both SC and TC and may be changed when mapping between them. This is the most complex case, because there are two distinct sub-cases:
   *     X may be mapped to itself or to another character when converting between SC and TC. In this case, the character is its own simplification as well as the
   * simplification for other characters. An example would be 后 U+540E, which is the simplification for itself and for 後 U+5F8C. When mapping TC to SC, it is left
   * alone,
   * but when mapping SC to TC it may or may not be changed, depending on context. In this case, both kTraditionalVariant and kSimplifiedVariant fields are defined and
   * X
   * is included among the values for both.
   *     X is used for different words in SC and TC. When converting between the two, it is always changed. An example would be 苧 U+82E7. In traditional Chinese, it is
   * pronounced zhù and refers to a kind of nettle. In simplified Chinese, it is pronounced níng and means limonene (a chemical found in the rinds of lemons and other
   * citrus fruits). When converting TC to SC it is mapped to 苎 U+82CE, and when converting SC to TC it is mapped to 薴 U+85B4. In this case, both kTraditionalVariant
   * and
   * kSimplifiedVariant fields are defined but X is not included in the values for either.
   *
   *  all we need is:
   *  if exists kTraditionalVariant then remove from simplified
   *  if exists kSimplifiedVariant then remove from traditional
   *  but if both then the name is probably wrong! not sure what to do at the moment
   *
   *  file fragment:
   *  U+4E0D	kZVariant	U+F967
   *  U+4E0E	kSemanticVariant	U+8207<kMatthews,kMeyerWempe
   *  U+4E0E	kTraditionalVariant	U+8207
   *  U+4E10	kSemanticVariant	U+5303<kMatthews
   *  U+4E11	kTraditionalVariant	U+919C
   *  U+4E13	kTraditionalVariant	U+5C08
   *  U+4E15	kSpecializedSemanticVariant	U+4EF3<kMeyerWempe
   *  U+4E16	kZVariant	U+4E17
   *  U+4E17	kZVariant	U+4E16
   *  U+4E18	kSemanticVariant	U+3400 U+5775<kMatthews
   *  U+4E18	kZVariant	U+4E20
   *  U+4E1F	kSimplifiedVariant	U+4E22
   *
   */
  struct HanziGroup {
    Unichar unicode;
    /** has kTraditionalVariant record */
    bool hasTrad;
    /** has kSimplifedVariant record */
    bool hasSimp;
    /** @returns whether this unicode is only in simplified set */
    bool isSimple() const {
      return hasSimp;
    }

    /** @returns whether this unicode is only in traditional set */
    bool isTraditional() const {
      return hasTrad && !hasSimp;
    }

    HanziGroup(Unichar unicode) :
      unicode(unicode),
      hasTrad(false),
      hasSimp(false){
      //default is 'belongs to either group'
    }

    /** chainsorter lambda for finding by 'unicode' */
    static int find(const HanziGroup*oldone,Unichar unicode){
      return oldone->unicode - unicode;
    }

    /** chainsorter lambda for sorting by 'unicode' */
    static int compare(const HanziGroup*newer,const HanziGroup*oldone){
      return oldone->unicode - newer->unicode;
    }

  };

  Chain<HanziGroup> hanziGroup;
  ChainSorter<HanziGroup> hanziSorter;
  /** @return autocreated unique record */
  HanziGroup &groupFor(Unichar hanzi){
    int index = hanziSorter.indexFor(sigc::bind(&HanziGroup::find,hanzi));
    if(index>=0) {//exists
      return *hanziGroup[index];
    } else {//doesn't exist, build one.
      return *hanziGroup.insert(new HanziGroup(hanzi),~index);// prechecked for uniqueness.
    }
  }

  /** set flag on group @param hanzi to @param traditional else simplified */
  void flagGroup(Unichar hanzi,bool traditional){
    HanziGroup &group(groupFor(hanzi));
    if(traditional) {
      group.hasTrad = true;
    } else {
      group.hasSimp = true;
    }
  }

  void parseHanziList(bool traditional,const FileName &unicodeList){
    const char *cname = unicodeList.c_str();
    FILE *fp = fopen(cname,"r");
    try {
      if(fp) {
        char line[2 + 4 + 2];//0x 4 digit unicodes \r\n
        while (fgets(line,sizeof(line),fp)) {//for each line
          const char *mark = &(line[2]);//skip the 0x
          flagGroup(parse4hex(mark),traditional);
        }
        fclose(fp);
      } else {
        dbg("%s not found",cname);
      }
    } catch(...) {
      wtf("exception parsing %s list",cname);
      fclose(fp);
    }
  } // parseHanziList

  void parseUnihan(FILE *fp,int maxline,sigc::slot<void(Unichar, const char *)> action){
    char line[maxline * 2 + 1];//factor of 2 in case we blew the manual search via wc for the longest line.
    while (fgets(line,sizeof(line),fp)) {//for each line
      const char *mark(line);
      if(*mark=='#') {//comment line
        continue;
      }
      Unichar kanjiuni = parseUPlus(mark);

      if(CjkUnified.contains(kanjiuni)) { //group of interest, performance tweak
        action(kanjiuni,mark);
      }
    }
  } // parseUnihan

  /** lambda for per-line actions of parsing unihan variants*/
  void variantsAction(Unichar kanjiuni, const char *mark){
    switch(mark[3]) {//taking advantage of fixed finite set of keywords
    case 'r': //kTraditionalVariant
      flagGroup(kanjiuni, false);
      while(*++mark!='\t') {
      }
      do {
        ++mark;
        Unichar traditional = parseUPlus(mark);
        flagGroup(traditional,true);
      } while(*mark==' ');
      break;
    case 'i': //kSimplifiedVariant
      flagGroup(kanjiuni,true);
      while(*++mark!='\t') {
        //#nada
      }
      do {
        ++mark;
        Unichar simple = parseUPlus(mark);
        if(simple==0x4E07) {
          wtf("Things!");
        }
        flagGroup(simple,false);
      } while(*mark==' ');
      break;
    } // switch
  } // variantsAction

  /** @returns new string created via removing accents from values and concatenating tone mark at the end.
   *  bar:1  \u0304
   *  tick:2  \u0301
   *  cup:3   \u0306
   *  backtick:4  \u0300
   *  <placeholder for no tone>:5
   *  \u0308  umlaut, apparently only used with u and converts to v
   */
  static int tonecode(Unichar &c){
    switch(c) {
    case 0x0304: return -1;//combining char just gets put at end
    case 0x0301: return -2;//ditto
    case 0x0306: return -3;//ditto
    case 0x0300: return -4;//ditto
    case 0x0308: return -5;//combining umlaut is presumed to be preceded by u and becomes a v
    case 0xFC: //composed u umlaut
      c = 'v'; return 0;
    case 0x1d6: case 0x1d8: case 0x1da: case 0x1dc: //composed u umlaut and a mark
    {
      int tone = 1 + ((c - 0x1d6) / 2);
      c = 'v'; return tone;
    }
    case 0x1e3f://=m2
      c = 'm'; return 2 + 5; //+5 for debug
    case 0x144: //n2
      c = 'n'; return 2 + 5; //+5 for debug
    } // switch

    //scan lut for composed chars since some tool is doing that conversion on us:
    static const Unichar lut[] = {
      //bar,tick,caron,back,breve         fyi: cup=bar+2  tick=back+1 and breve shouldn't even be present!
      0x101,0xe1,0x1ce,0x103,0xe0, //a
      0x113,0xe9,0x11b,0x115,0xe8, //e
      0x12b,0xed,0x1d0,0x12d,0xec, //i
      0x14d,0xf3,0x1d2,0x14f,0xf2, //o
      0x16b,0xfa,0x1d4,0x16d,0xf9, //u
    }; // aeiou by 12343
    for(int lute = countof(lut); lute-->0; ) {
      if(c==lut[lute]) {
        c = "aeiou"[lute / 5];
        return 1 + (lute % 5);//2 lsbs as 1..4
      }
    }
    wtf("pinyuck: %x",c);
    return 0;
  } // tonecode

  static Glib::ustring accentsToDigits(const Glib::ustring &pinyin){
    Glib::ustring flat;
    flat.reserve(2 * pinyin.length());
    int finaltone(0);
    for(auto it = pinyin.begin(); it!=pinyin.end(); ++it) {
      Unichar c = *it;
      if(!c||c=='\n') {//moved line clipping here from caller.
        break;//end of readable line.
      }
      int tone = (c<128) ? 0 : tonecode(c);//#the ternary here is a performance/debug tweak
      if(tone<0) {//composing tone mark
        if(tone==-5) {//special for u umlaut
          flat.replace(flat.length() - 1,1,1,'v');
        } else {
          finaltone = -tone;
          //and flat is untouched
        }
      } else if(tone>0) {
        if(tone==5) {//unihan authors usedundocumented tone mark, our expert says "same as descending"
          tone = 4; //per Yilin, there were very few 4's before this and they were tied to u umlaut's.
        }
        finaltone = tone;
      }
      //if tone==0 we leave finaltone as is.
      flat.append(1,c);
    }
    if(finaltone) {
      flat.append(1,Unichar('0' + finaltone));//cast needed to pick unambiguous overload
    } else {//separate for debug, could just init to 5 and remove the conditional.
      flat.append(1,Unichar('5'));//cast needed to pick unambiguous overload
    }
    return flat;
  } // accentsToDigits

  static Glib::ustring accentsToDigits(const char *utf8s){
    return accentsToDigits(Glib::ustring(utf8s));//leveraging Glib::ustring to convert utf8 into unicode.
  }

  //extract from kMandarin field of Unihan_Reading.txt
  void parseMandarinField(Unichar kanjiuni, const char *mark){
    mark += 1 /*tab*/ + (sizeof("kMandarin") - 1) + 1 /*tab*/;//mark becomes -> to pinyin, which is null terminated.

    Glib::ustring decimalated(accentsToDigits(mark));
    HanziGroup &group(groupFor(kanjiuni));

    if(group.isTraditional()) {
      addReading(traditional,decimalated,kanjiuni);
    }
    if(group.isSimple()) {
      addReading(simplified,decimalated,kanjiuni);
    }
  } // parseMandarinField

  /** call to preprocess any files, will also be called automatically on first use.*/
  void init(bool forceful = false){
    if(!haveCached||forceful) { //parse kanjidic file
      if(l10n->jp.enable) {
        FileName kanjidic(filename.c_str());
        kanjidic.ext("raw");
        FILE *file = fopen(kanjidic.c_str(),"r");
        try {
          if(file) {
            if(forceful) {
              KUN.filicide();
              ON.filicide();
            }
            //wc reports longest line has 397 something in it, let us go overboard
            char line[397 * 3 + 1];
            while (fgets(line,sizeof(line),file)) {//for each line
              parseLine(line);
            }//have lines
            fclose(file);
          }
        } catch(...) {
          wtf("exception in kanjidic parser");
          fclose(file);
        }
      }
      if(l10n->jp.enablePinyin) {
        hanziGroup.clear();//remove last attempt

        //load list of simples
        parseHanziList(false,FileName("l10n/gb2312.txt")); //file created by finding unicode consortium's gb2312.txt file and running through grep <CJK> | cut -f2
        //load list of traditionals
        parseHanziList(true,FileName("l10n/big5.txt"));  //file created by finding unicode consortium's BIG5.TXT file and running through grep <CJK> | cut -f2

        //associate pinyin with chars
        FileName unihan("l10n/mandarin.txt");//grepped from Unihan/Unihan_Readings
        FILE *file = fopen(unihan.c_str(),"r");
        try {
          if(file) {
            if(forceful) {
              traditional.filicide();
              simplified.filicide();
            }
            //wc reports longest line has 450 something in it
            parseUnihan(file,450,MyHandler(KanjiLookup::parseMandarinField));
            fclose(file);
          } else {
            wtf("%s not found, hanzi may be missing",unihan.c_str());
          }
        } catch(...) {
          wtf("exception parsing unihan readings");
          fclose(file);
        }
      }
      haveCached = true;
      save(false);
    }
  } // init

  /** @deprecated was used to test parsing, not useful to our customers.
   * @return single result for hiragana string, sets length to how much of the trailing part of @param hiraganaFragment was actually part of the name (may drop this if
   * we choose a strict lookup)*/
//  Unichar charFor(const Glib::ustring &hiraganaFragment,int &length){
//    init();
//    Storable *entry=KUN.existingChild(hiraganaFragment.c_str());
//    if(entry){
//      length=hiraganaFragment.length();
//      return entry->getNumber<Unichar>();
//    } else {
//      length=0;
//      return 0;
//    }
//  }

  /** @returns list of kanji whose Kun reading contains hiraganaFragment. Might chose to do 'startsWith' */
  Glib::ustring candidates(const Glib::ustring &name,CJK nameset){
    init();
    if(name.length()>0) {//testing for debug
      Storable *kanjiset = set(nameset).existingChild(name.c_str());
      if(kanjiset) {
        return kanjiset->image();
      } else {
        return Glib::ustring();
      }
    }
    return Glib::ustring();
  } // candidates

};

static KanjiLookup *lookup(nullptr);

void KanjiDic::init(bool forceful){
  if(!lookup) {
    lookup = new KanjiLookup();
  }
  lookup->init(forceful);
}

Unichar KanjiDic::charFor(const Glib::ustring &hiraganaFragment, int & /*length*/){
  KanjiDic::init();
  if(hiraganaFragment.length()>0) {//pretest is for debug
    //return lookup->charFor(hiraganaFragment,length);
    return 0;
  } else {
    return 0;
  }
}

Glib::ustring KanjiDic::candidates(const Glib::ustring &name, CJK nameset){
  KanjiDic::init();
  return lookup->candidates(name,nameset);
}

Storable &KanjiDic::debugNode(){
  KanjiDic::init();
  return lookup->node;
}
