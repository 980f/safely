#ifndef ARTPROTOCOLPROCESSOR_H
#define ARTPROTOCOLPROCESSOR_H

#include "hassettings.h"
#include "charformatter.h"
#include "asciiframer.h"
#include "art.h" //to get include sigcuser stuff stable

class ArtProtocolProcessor {
protected:
  /** link to command interpreter, this class just slices up the comm stream into command packets */
  SettingsGrouper &sg;
public:
  /** frame recognizer */
  AsciiFramer framer; //public so we can stuff commands into it to send them to ourself.
  /** @param sg is the command interpreter that this class parses for, @param buf is a workspace allocated by someone else the framer's use*/
  ArtProtocolProcessor(SettingsGrouper &sg, char *buf, int sizeofbuf);
  void init();
  /** you must push incoming data to this server: */
  bool onReception(int incoming);
  /** use by firmware: must be called after any interrupt, @returns whether settings have changed since this last returned that settings have changed*/
  bool doLogic(void);
private:
  /** called when some failure occurs, makes a handy berakpoint */
  void failed();  
public:
  /** @return whether at least one packet has been seen by framer.*/
  bool havaPacket()const;
  
  int successes;
  int failures;
  
#ifdef SIGCUSER_H
  sigc::signal<void,bool /*outgoing*/, ID /*unitId*/,ID /*fieldId*/,ArgSet&/*args*/> packetWatcher;
#endif
};

#endif // ARTPROTOCOLPROCESSOR_H
