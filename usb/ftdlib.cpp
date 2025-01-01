/* (C) 2017 Andrew L. Heilveil
 * significantly rewritten but based on source that was  Copyright (C) 2008-2017 by Marek Vavruša / libftdi developers
 *
 */
#include "libusb/libusb.h"
#include "ftdlib.h"
// #include "ftdi_i.h"
#include "ftdi.h"

#include "cheaptricks.h"

bool Ftdi::isOk(int fts){
  if(changed(lastStatus,fts)) {
    if(fts!=0) {
      dbg("libftdi error %d:%s",fts,errorString());
    }
  }
  return fts>=0;
}

Ftdi::Ftdi() :
  opened(false),
  ftdi(ftdi_new()),
  dev(nullptr),
  dbg("FTDI"){
  //#nada
}

Ftdi::~Ftdi(){
  if (opened) {
    ftdi_usb_close(ftdi);
  }
  ftdi_free(ftdi);
}

bool Ftdi::isOpen(){
  return opened;
}

bool Ftdi::open(uint16_t vendor, uint16_t product){
  if(isOk(ftdi_usb_open(ftdi, vendor, product))) {
    return get_strings_and_reopen();
  } else {
    return false;
  }
}

bool Ftdi::open(uint16_t vendor, uint16_t product, TextKey description, TextKey serial, unsigned int index){
  // empty strings are supposedly not the same as nullptr to this method so ...
  if(isOk( ftdi_usb_open_desc_index(ftdi, vendor, product, nonTrivial(description)?description:nullptr, nonTrivial(serial)?serial:nullptr, index))){
    return get_strings_and_reopen();
  } else {
    return false;
  }
}

bool Ftdi::open(TextKey description){
  if(isOk(ftdi_usb_open_string(ftdi, description))){
    return get_strings_and_reopen();
  } else {
    return false;
  }
}

bool Ftdi::open(struct libusb_device *dev){
  if (dev != 0) {
    this->dev = dev;
  }
  if (this->dev ) {
    return get_strings_and_reopen();
  } else {
    return false;
  }
}

bool Ftdi::close(){
  opened = false;
  dev = nullptr;//?these don't leak?
  return isOk(ftdi_usb_close(ftdi));
  //don't null the ftdi, wse may reopen.
}

bool Ftdi::reset(){
  return isOk(ftdi_usb_reset(ftdi));
}

/* the reference code I cribbed from mismanaged the mask concept*/
bool Ftdi::flush(bool theInput, bool theOutput){
  bool ok = true;
  if (theInput) {
    ok &= isOk( ftdi_usb_purge_rx_buffer(ftdi));
  }
  if (theOutput) {
    ok &= isOk(ftdi_usb_purge_tx_buffer(ftdi));
  }
  return ok;
}

bool Ftdi::setInterface(enum ftdi_interface interface){
  return isOk(ftdi_set_interface(ftdi, interface));
}

void Ftdi::linkDevice(struct libusb_device_handle *devhandle){
  ftdi_set_usbdev(ftdi, devhandle);
  this->dev = libusb_get_device(devhandle);
}

int &Ftdi::usbReadTtimeout(){
  return ftdi->usb_read_timeout;
}

int &Ftdi::usbWriteTimeout(){
  return ftdi->usb_write_timeout;
}

bool Ftdi::has(unsigned bytestoread){
  return ftdi&&ftdi->readbuffer_remaining>=bytestoread;
}

bool Ftdi::startRead(ByteScanner &buf){
  int size=buf.freespace();
  return ftdi_read_data_submit(ftdi, &buf.peek(), size)!=nullptr;
}

bool Ftdi::read(ByteScanner &buf){
  int desired=buf.freespace();
  int actual= ftdi_read_data(ftdi, &buf.peek(), desired);
  if(actual>=desired){
    buf.skip(desired);
    return true;
  } if(actual>=0) {
    return false;//data not here yet.
  } else {//is error code, not quantity
    return isOk(actual);
  }
}

bool Ftdi::set_read_chunk_size(unsigned int chunksize){
  return isOk(ftdi_read_data_set_chunksize(ftdi, chunksize));
}

unsigned Ftdi::read_chunk_size(){
  unsigned chunk = 0;
  if (isOk(ftdi_read_data_get_chunksize(ftdi, &chunk))) {
    return chunk;
  } else {
    return BadLength;
  }
}

bool Ftdi::write(ByteScanner &buf){
  int size=buf.freespace();//todo:1 MaxTracker
  size=ftdi_write_data(ftdi, &buf.peek(), size);
  if(size>=0){
    buf.skip(size);
    return  true;
  } else {
    isOk(size);
    return false;
  }
}

bool Ftdi::set_write_chunk_size(unsigned int chunksize){
  return isOk(ftdi_write_data_set_chunksize(ftdi, chunksize));
}

unsigned Ftdi::write_chunk_size(){
  unsigned chunk(0);
  if ( isOk( ftdi_write_data_get_chunksize(ftdi, &chunk) )) {
    return chunk;
  } else {
    return BadLength;
  }
}

bool Ftdi::setLatency(unsigned char latency){
  return isOk(ftdi_set_latency_timer(ftdi, latency));
}

unsigned Ftdi::latency(){
  unsigned char latency = 0;
  ftdi_get_latency_timer(ftdi, &latency);
  return latency;
}

bool Ftdi::setBitmode(unsigned char bitmask, enum ftdi_mpsse_mode mode){
  return isOk(ftdi_set_bitmode(ftdi, bitmask, mode));
}

bool Ftdi::bitbang_disable(){
  return ftdi_disable_bitbang(ftdi);
}

int Ftdi::read_pins(unsigned char *pins){
  return ftdi_read_pins(ftdi, pins);
}

const char* Ftdi::errorString(){
  return ftdi_get_error_string(ftdi);
}

bool Ftdi::getStrings(){
  char vendor[512], desc[512], serial[512];

  if(isOk(ftdi_usb_get_strings(ftdi, dev, vendor, 512, desc, 512, serial, 512))) {
    this->vendor = vendor;
    this->description = desc;
    this->serial = serial;
    return true;
  } else {
    return false;
  }
}

bool Ftdi::get_strings_and_reopen(){
  if ( dev == nullptr ) {
    dev = libusb_get_device(ftdi->usb_dev);
  }

  // Get device strings (closes device)
  opened = getStrings();  //temporary abuse of opened, but it is handy.
  if(isOpen()) {     //it isn't actually open, we need to reattach device
    opened = isOk(ftdi_usb_open_dev(ftdi, dev));
  }
  return opened;
}

void Ftdi::set_context(struct ftdi_context* context){
  ftdi_free(ftdi);
  ftdi = context;
}

unsigned Ftdi::findAll(ftdi_device_list **devlist, uint16_t vendor, uint16_t product){
  int length=ftdi_usb_find_all(ftdi,devlist, vendor, product);
  if(isOk(length)){
    return unsigned(length);
  } else {
    return BadLength;
  }
}

void Ftdi::linkDevice(struct libusb_device *dev){
  this->dev = dev;
}

struct ftdi_context* Ftdi::context(){
  return ftdi;
}

///////////////////

FtdiDeviceList::~FtdiDeviceList(){
  if (devlist) {
    ftdi_list_free(&devlist);
    devlist = 0;
  }
}

FtdiDeviceList::FtdiDeviceList(Ftdi & context, uint16_t vendor, uint16_t product) : devlist(0){
  unsigned length=context.findAll(&devlist, vendor, product);
  if(length!=BadLength){
    //presize using length
    allocate(length);//
    for (auto it=devlist; it!= 0; it= it->next) {
      Ftdi &c(*append(new Ftdi()));
      c.linkDevice(it->dev);//backlink
      c.getStrings();
    }
    if(length!=this->quantity()){
      dbg("returned length wasn't same as actual length");
    }
  }
}
