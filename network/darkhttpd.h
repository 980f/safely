/**
// Created by andyh on 12/14/24.
// Copyright (c) 2024 Andy Heilveil, (github/980f). All rights reserved.
*/

/* This module was started using C source whose license was:
 *
 * darkhttpd - a simple, single-threaded, static content webserver.
 * https://unix4lyfe.org/darkhttpd/
 * Copyright (c) 2003-2024 Emil Mikulic <emikulic@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * It has been heavily modified to make it a module that can be included in other programs.
 */

#pragma once
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <forward_list>
#include <map>
#include <unistd.h>
#include <netinet/in.h>

/** build options */
#ifndef NO_IPV6
# define HAVE_INET6
#endif

//gizmo to get the compiler to check our printf args:
#ifndef __printflike
# ifdef __GNUC__
/* [->] borrowed from FreeBSD's src/sys/sys/cdefs.h,v 1.102.2.2.2.1 */
#  define __printflike(fmtarg, firstvararg) \
__attribute__((__format__(__printf__, fmtarg, firstvararg)))
/* [<-] */
# else
#  define __printflike(fmtarg, firstvararg)
# endif
#endif


class DarkHttpd {

  class Exception : public std::exception {
    //todo: internal malloc string with delete
    const char *msg = nullptr;
    int returncode = 0; //what would have been returned to a shell on exit.
    ~Exception() override; //todo: free message
  public:
    Exception(int returncode, const char *msgf, ...);
  };

  /** replacing inline reproduction of map logic with an std map, after verifying who did the allocations.
   * this uses the default key compare function 'less<const char *>', which we might want to replace with strncmp(...longest_ext) */
  struct mime_mapping : std::map<const char *, const char *> {
    size_t longest_ext = 0;
    /** arg returned by 'add to map' */
    using mime_ref = std::pair<std::_Rb_tree_iterator<std::pair<const char * const, const char *> >, bool>; //todo: indirect to a decltype on the function returning this.
    mime_ref add(const char *extension, const char *mimetype);

    /** free contents, then forget them.*/
    void purge() {

    }
  } mime_map;

  struct forward_mapping : std::map<const char *, const char *> {
    // const char *host, *target_url; /* These point at argv. */
    void add(const char *const host, const char *const target_url) {
      insert_or_assign(host, target_url); //# allows breakpoint on these. We need to decide whether multiples are allowed for the same host, at present that has been excluded but the orignal might have allowed for that in which case we need a map of list of string.
    }
  } forward_map;

public:
  class Fd { // a minimal one compared to safely/posix
    int fd = -1;

  public:
    operator int() const {
      return fd;
    }

    int operator =(int newfd) {
      fd = newfd;
      return newfd;
    }

    bool operator ==(int newfd) const {
      return fd == newfd;
    }

    operator bool() const {
      return fd != -1;
    }

    /** @returns whether close() thinks it worked , but we discard our fd value regardless of that */
    bool close() {
      if (fd!=-1) {
        int fi = ::close(fd);
        fd = -1;

        return fi != -1;
      }
      return true;//already closed is same as just now successfully closed.
    }

    /** lose track of the fd regardless of the associated file's state. Most uses seem like bugs, leaks of OS file handles.*/
    void forget() {
      fd=-1;
    }

    /** @returns whether the fd is real and not stdin, stdout, or stderr */
    bool isNotStd() const {
      return fd > 2;
    }

    /** close this oneif open, then make it attach to same file as fdother
     * @returns whether the OS was happy with doing this.
     */
    bool duplicate(int fdother) const {
      return dup2(fdother,fd)!=-1;//makes fd point to same file as fdother
    }

    /** put this guy's file state into @param fdother. */
    bool copyinto(int fdother) const {
      return dup2(fd,fdother)!=-1;
    }

    Fd() = default;

    Fd(int open):fd(open){}
  };



  struct connection {
    //    LIST_ENTRY(connection) entries;

    Fd socket;
    DarkHttpd &service;
#ifdef HAVE_INET6
    struct in6_addr client;
#else
    in_addr_t client;
#endif
    time_t last_active = 0;

    enum {
      RECV_REQUEST, /* receiving request */
      SEND_HEADER, /* sending generated header */
      SEND_REPLY, /* sending reply */
      DONE /* connection closed, need to remove from queue */
    } state = DONE; // DONE makes it harmless so it gets garbage-collected if it should, for some reason, fail to be correctly filled out.

    /* char request[request_length+1] is null-terminated */
    char *request = nullptr;
    size_t request_length = 0;

    /* request fields */
    char *method = nullptr; //as in GET, POST, ...
    char *url = nullptr;
    char *referer = nullptr;
    char *user_agent = nullptr;
    char *authorization = nullptr;

    off_t range_begin = 0;
    off_t range_end = 0;
    off_t range_begin_given = 0;
    off_t range_end_given = 0;

    char *header = nullptr;
    size_t header_length = 0;
    size_t header_sent = 0;
    bool header_dont_free = false;
    int header_only = 0;
    int http_code = 0;
    bool conn_closed = true;

    enum { REPLY_GENERATED, REPLY_FROMFILE, REPLY_NONE } reply_type=REPLY_NONE;

    char *reply = nullptr;
    bool reply_dont_free = false;
    Fd reply_fd;
    off_t reply_start = 0;
    off_t reply_length = 0;
    off_t reply_sent = 0;
    off_t total_sent = 0;
    /* header + body = total, for logging */
  public:
    connection(DarkHttpd &parent):service(parent) {
      memset(&client, 0, sizeof(client));
    }

    void free();

    void recycle_connection();

    void poll_check_timeout();

    const char *keep_alive() const;

    void default_reply(int errcode, const char *errname, const char *format, ...);

    void redirect(const char *format, ...);

    char *parse_field(const char *field);

    void redirect_https();

    void parse_range_field();

    int parse_request();

    void process_get();

    void process_request();

    void poll_recv_request();

    void poll_send_header();

    void poll_send_reply();
  };


  static void xclose(Fd fd);

  void parse_mimetype_line(const char *line);

  void parse_default_extension_map();

  void parse_extension_map_file(const char *filename);

  const char *url_content_type(const char *url);

  void init_sockin();

  void usage(const char *argv0);

  void generate_dir_listing(struct connection &conn, const char *path, const char *decoded_url);

  void process_get(struct connection &conn);

  void httpd_poll();

  bool daemonize_start();

  void daemonize_finish();

  void pidfile_remove();

  int pidfile_read();

  void freeall();

public:
  void parse_commandline(int argc, char *argv[]);

  void accept_connection();

  void log_connection(const struct connection *conn);

  void showUsageStats();

  void prepareToRun();

  void pidfile_create();

  void change_root();

  #define DATE_LEN 30 /* strlen("Fri, 28 Feb 2003 00:02:08 GMT")+1 */

  const char *generated_on(const char date[DATE_LEN]) const;

  bool is_https_redirect(struct connection &conn) const;

  void stop_running(int sig);

  void reportStats();

  int main(int argc, char **argv);

  /** free with nulling of pointer to make use after free uniformly a sigsegv instead of random crash or corruption
   * @see safely/system library.
   */
   template<typename Something> static  void Free(Something **malloced) {
    if (malloced) {
      ::free(*malloced);
      *malloced = nullptr;
    }
  }


private:
  Fd fd_null;
  struct PipePair {
    Fd fds[2];
    int operator[](bool which) {
      return fds[which];
    }

    bool connect() {
      int punned[2]={fds[0],fds[1]};
      return pipe(punned)!=-1;
    }

  } lifeline ;

  /* [->] pidfile helpers, based on FreeBSD src/lib/libutil/pidfile.c,v 1.3
 * Original was copyright (c) 2005 Pawel Jakub Dawidek <pjd@FreeBSD.org>
 */
  Fd pidfile_fd;

  Fd sockin; /* socket to accept connections from */

  /** the entries will all be dynamically allocated */
  std::forward_list<connection *> entries;

  const char *forward_all_url = nullptr;

  int forward_to_https = 0;


  /* If a connection is idle for timeout_secs or more, it gets closed and
   * removed from the connlist.
   */
  int timeout_secs = 30;
  char *keep_alive_field = nullptr;

  /* Time is cached in the event loop to avoid making an excessive number of
   * gettimeofday() calls.
   */
  static time_t now;

  /* To prevent a malformed request from eating up too much memory, die once the
   * request exceeds this many bytes:
   */
#define MAX_REQUEST_LENGTH 4000

  /* Defaults can be overridden on the command-line */
  const char *bindaddr = nullptr;
  uint16_t bindport = 8080; /* or 80 if running as root */
  int max_connections = -1; /* kern.ipc.somaxconn */
  const char *index_name = "index.html";
  int no_listing = 0;

#ifdef HAVE_INET6
  bool inet6 = false;               /* whether the socket uses inet6 */
#endif
  char *wwwroot = nullptr; /* a path name */
  char *logfile_name = nullptr; /* NULL = no logging */
  FILE *logfile = nullptr;
  char *pidfile_name = nullptr; /* NULL = no pidfile */
  bool want_chroot = false;
  bool want_daemon = false;
  bool want_accf = false;
  bool want_keepalive = true;
  bool want_server_id = true;
  bool want_single_file = false;
  char *server_hdr = nullptr;
  char *auth_key = nullptr; /* NULL or "Basic base64_of_password" */
  char *custom_hdrs = nullptr;
  uint64_t num_requests = 0;
  uint64_t total_in = 0;
  uint64_t total_out = 0;
  bool accepting = true; /* set to 0 to stop accept()ing */
  bool syslog_enabled = false;
  volatile bool running = false; /* signal handler sets this to false */

#define INVALID_UID ((uid_t) -1)
#define INVALID_GID ((gid_t) -1)

  uid_t drop_uid = INVALID_UID;
  gid_t drop_gid = INVALID_GID;
};
