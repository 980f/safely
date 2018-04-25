#ifndef TCPSOCKET_H
#define TCPSOCKET_H

/******* DEPRECATED  ***************
 * This is missing pieces, it was disconnected from glib IO system but hasn't yet been connected to AIO system.
 */


#include "sigcuser.h"
#include "charscanner.h"
#include "fildes.h"

/** common part of client and server sockets. server socket itself is declared elsewhere */
class TcpSocketBase {
public:
  /** diagnostic counters */
  struct Stats {
    /** successful connection */
    u32 connects;
    /** failed attempt or lost connection after succeeded */
    u32 disconnects;
    /** read events */
    u32 reads;
    /** write events */
    u32 writes;
    Stats();
    void clear();
  } socketStats;

  /** host ordered connection parameters */
  struct ConnectArgs {
    u32 ipv4;
    u16 port;
    ConnectArgs(int ipv4 = 0,int port = 0);//defaults are the same values as set by @see erase

    /** sets to blatantly ridiculous values, @see isPossible()*/
    void erase();
    /** neither parameter is 0 */
    bool isPossible();
  } connectArgs;

protected:
  Fildes fd;
  /** create from an fd of an open (connected) socket or use the default arg and call connect() */
  TcpSocketBase(int fd = ~0,u32 remoteAddress = 0,int port = 0);

public:
  virtual ~TcpSocketBase();
  /** @returns address of other end of link */
  u32 remoteIpv4();
  /** @returns whether socket seems to be open */
  bool isConnected() const;
  /** call this to disconnect the socket.
   *  @returns false so that it easily drops into a TcpSocket::Slot*/
  virtual bool disconnect();
}; // class TcpSocketBase

//////////////////////////////////////////////

/** client socket. Uses aio (via incrementalfiletransfer.cpp) to move bytes in background.
 */
class TcpSocket : public TcpSocketBase {

protected:
  /** create from an fd of an open (connected) socket or use the default arg and call connect() */
  TcpSocket(int fd = BadIndex,u32 remoteAddress = 0,int port = 0);

  /** enables attempts to reconnect */
  bool autoConnect;
  /** pointer into actual sending buffer, which is NOT part of this object. */
  ByteScanner sendbuf;
  /** called by glib when there is something to read (after startReception has once been called) */
  bool readable();
  /** called by glib when remote disconnects (after startReception has once been called) */
  bool hangup();

  /** called by glib when data can be written (after writeInterest has recently been called) */
  bool writeable();
public:
  virtual ~TcpSocket();
  /** @returns isConnected() */
  bool connect(unsigned ipv4, unsigned port);
  /** disconnect and release socket, if @param andNotify then call 'onConnectionChange' actions*/
  void disconnect(bool andNotify);
  /** throw away all pending input, best effort-some bytes might sneak in right after it returns.*/
  void flush();
protected:
  BooleanSignal notifyConnected;
  /** called when some data has arrived. You MUST copy the data, the underlying pointer of @param raw is to a piece of the stack. */
  virtual void reader(ByteScanner&raw) = 0;
  /** called when can write, should set ByteScanner to point to data, and return true if should be sent.
   *  The data YOU point to by modifying @param raw must stay allocated until the next call to writer(). You could poll the TcpSocket to see if it is done with the write, we should probably add a callback for 'transmit buffer empty'.*/
  virtual bool writer(ByteScanner&raw) = 0;

public:
  sigc::connection whenConnectionChanges(const BooleanSlot &nowConnected, bool kickme = false);
  bool reconnect();
  void startReception();
  bool setNodelay();
}; // class TcpSocket

#include <netinet/in.h>
/** address etc will be maintained in network order, with accessors for host order.*/
struct SocketAddress {
  sockaddr_in sin;
  /** for one that is to be filled by a posix call*/
  SocketAddress();
  /** for one that is to be sent to a posix call */
  SocketAddress(TcpSocket::ConnectArgs &cargs);
  u16 hostPort() const;
  u32 hostAddress() const;
  /** address for posix functions */
  const sockaddr *addr() const;
  sockaddr *addr();

  /** struct size for posix functions*/
  int len() const;
  /** @returns whether an ERROR occured, @param fd is value returned from your call to socket() */
  bool connect(int fd);
};

/** Class to provide "blocking" effect in connect. */
class BlockingConnectSocket : public TcpSocketBase {
public:
protected:
  /** create from an fd of an open (connected) socket or use the default arg and call connect() */
  BlockingConnectSocket(int fd = ~0,u32 remoteAddress = 0,int port = 0);

  /** enables attempts to reconnect */
  bool autoConnect;

  /** pointer into actual sending buffer, which is NOT part of this object. */
  ByteScanner sendbuf;

  /** called by glib when there is something to read (after startReception has once been called) */
  bool readable();

  /** called by glib when remote disconnects (after startReception has once been called) */
  bool hangup();

  /** called by glib when data can be written (after writeInterest has recently been called) */
  bool writeable();

public:
  virtual ~BlockingConnectSocket();

  /** @returns isConnected()*/
  bool connect(unsigned ipv4, unsigned port);

  /** disconnect and release socket, if @param andNotify then call 'onConnectionChange' actions */
  void disconnect(bool andNotify);

  /** throw away all pending input, best effort-some bytes might sneak in right after it returns.
   * this is BLOCKING, but due to the way Glib works it will leave in a finite amount of time.
   * added to dump the qchardware incoming queue when the logic is way behind */
  void flush();

protected:
  BooleanSignal notifyConnected;

  /** called when some data has arrived. You MUST copy the data, the underlying pointer of @param raw is to a piece of the stack. */
  virtual void reader(ByteScanner&raw) = 0;

  /** called when can write, should set ByteScanner to point to data, and return true if should be sent.
   *  The data YOU point to by modifying @param raw must stay allocated until the next call to writer(). You could poll the TcpSocket to see if it is done with the write,
   *  we should probably add a callback for 'transmit buffer empty'. */
  virtual bool writer(ByteScanner&raw) = 0;

public:
  /** register for connection change signals */
  sigc::connection whenConnectionChanges(const BooleanSlot &nowConnected, bool kickme = false);

  bool reconnect();
  void startReception();
}; // class BlockingConnectSocket


#endif // TCPSOCKET_H
