#pragma once
//(C) 2017 Andrew L. Heilveil
#include "charscanner.h"
#include "chain.h"
#include "textpointer.h"
#include "ftdi.h"
#include "logger.h"


/** FTDI C library wrapper.
 * Represents single FTDI device context.
 */
class Ftdi {
  bool opened;
  struct ftdi_context* ftdi;
  struct libusb_device* dev;
  /** returned value from last ftdi related function call*/
  int lastStatus = 0;//maydo: set to 'not open'
  /** records a return status and @returns whether it was == FT_OK (a.k.a 0)*/
  bool isOk(int fts);
public:
  Logger dbg;

  Ftdi();
  ~Ftdi();

  bool isOpen() const;
  bool open(struct libusb_device *dev = 0);
  bool open(uint16_t vendor, uint16_t product);
  bool open(uint16_t vendor, uint16_t product, TextKey description, TextKey serial = "", unsigned int index = 0);
  bool open(TextKey description);
  bool close();
  bool reset();

  bool flush(bool theInput,bool theOutput);
  bool setInterface(enum ftdi_interface interface);
  int &usbReadTtimeout() const;
  int &usbWriteTimeout() const;

  bool has(unsigned bytestoread) const;
  bool startRead(const ByteScanner &buf) const;

  /** blocking reads into the freespace */
  bool read(ByteScanner &buf);
  /** blocking send of the freespace, so the buf passed here must wrap what is to be sent, so that we can indicate partial sends and they can be retried */
  bool write(ByteScanner &buf);

  bool set_read_chunk_size(unsigned int chunksize);
  bool set_write_chunk_size(unsigned int chunksize);

  unsigned read_chunk_size();
  unsigned write_chunk_size();

  bool setLatency(unsigned char latency);
  unsigned latency() const;

  /** enable MPSSE */
  bool setBitmode(unsigned char bitmask, enum ftdi_mpsse_mode mode);
  bool bitbang_disable() const;
  int read_pins(unsigned char *pins) const;

  const char* errorString() const;

  bool getStrings();
  bool get_strings_and_reopen();

  void linkDevice(struct libusb_device_handle *devhandle);
  void linkDevice(struct libusb_device *dev);

  struct ftdi_context* context() const;
  void set_context(struct ftdi_context* context);

public:
  Text vendor;
  Text description;
  Text serial;

  unsigned findAll(ftdi_device_list** devlist, uint16_t vendor, uint16_t product);

}; // class Context

/** contains a list of devices that match the given vid, pid
 */
class FtdiDeviceList:public Chain<Ftdi> {
  struct ftdi_device_list* devlist;

public:
  FtdiDeviceList(Ftdi &context, uint16_t vendor, uint16_t product);
  virtual ~FtdiDeviceList();

}; // class List
