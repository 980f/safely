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

#ifdef __linux
#ifndef _GNU_SOURCE // suppress warning, not sure who already set this.
#define _GNU_SOURCE /* for strsignal() and vasprintf() */
#endif
#endif

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <forward_list>
#include <map>
#include <netinet/in.h>
#include <unistd.h>

/** build options */
#ifndef NO_IPV6
#define HAVE_INET6
#endif

// gizmo to get the compiler to check our printf args:
#ifndef checkFargs
#ifdef __GNUC__
/* [->] borrowed from FreeBSD's src/sys/sys/cdefs.h,v 1.102.2.2.2.1 */
#define checkFargs(fmtarg, firstvararg) \
  __attribute__((__format__(__printf__, fmtarg, firstvararg)))
/* [<-] */
#else
#define checkFargs(fmtarg, firstvararg)
#endif
#endif


/** to help ensure timely frees.
 * Many people seem to not know that 'free' doesn't mind null pointers, it checks for them and does nothing so that we don't have to do that in a gazillion places.
 *
 * This frees what it has been given at constructor or by assignment when there is an assignment or a destruction. As such never assign to it from anything not malloc'd.
 */
struct AutoFree {
  //these are exposed because we are mutating C code into C++ and hiding them is tedious. We should hide at least the pointer member, but there are a few contexts (*printf) where the implicit conversion doesn't get invoked.
  char *pointer;
  size_t length = ~0; //~0 indicates unknown

  operator char *() const {
    return pointer;
  }

  operator bool() const {
    return pointer != nullptr;
  }

  bool operator!() const {
    return pointer == nullptr;
  }

  /**
   * @param str is this what the string ends with?
   * @param len length if known, else leave off or pass ~0
   * @returns whether the internal string ends with str .
   */
  bool endsWith(const char *str, unsigned len = ~0) const;

  /**
   *
   * @param str to append to end after allocating room for it
   * @param len length of string if known, ~0 for unknown (default)
   * @returns whether the append happened, won't if out of heap
   */
  bool cat(const char *str, size_t len = ~0);

  AutoFree(char *pointer = nullptr, unsigned length = ~0) :
      pointer(pointer), length(length) {
    // todo:M strlen if length = ~0
  }

  ~AutoFree() {
    free(pointer);
    // null fields in case someone tries to use this after it is deleted
    pointer = nullptr;
    length = 0;
  }

  void operator=(AutoFree &other) = delete;

  AutoFree &operator=(char *replacement) {
    free(pointer);
    pointer = replacement;
    length = replacement ? ~0 : 0;
    return *this;
  }

  AutoFree(AutoFree &&other) = delete;
};


class DarkHttpd {
  static DarkHttpd *forSignals; // trusting BSS zero to clear this.

  DarkHttpd() {
    forSignals = this;
  }
  class Exception : public std::exception {
    // todo: internal malloc string with delete
    const char *msg = nullptr;
    int returncode = 0; // what would have been returned to a shell on exit.
    ~Exception() override; // todo: free message
  public:
    Exception(int returncode, const char *msgf, ...) checkFargs(3,4);
  };

  /** replacing inline reproduction of map logic with an std map, after verifying who did the allocations.
   * this uses the default key compare function 'less<const char *>', which we might want to replace with strncmp(...longest_ext) */
  struct mime_mapping : std::map<const char *, const char *> {
    // size_t longest_ext = 0;
    /** arg returned by 'add to map' */
    using mime_ref = std::pair<std::_Rb_tree_iterator<std::pair<const char *const, const char *>>, bool>; // todo: indirect to a decltype on the function returning this.
    bool add(const char *extension, const char *mimetype);

    /** free contents, then forget them.*/
    void purge() {
      for (auto each: *this) {
        free((void *)each.first);
        free((void *)each.second);
      }
      clear();
    }
  } mime_map;

  /** todo: this seems to have become the same as mime_mapping except for idiot checks */
  struct forward_mapping : std::map<const char *, const char *> {
    // const char *host, *target_url; /* These point at argv. */
    void add(const char *const host, const char *const target_url) {
      insert_or_assign(host, target_url); // # allows breakpoint on these. We need to decide whether multiples are allowed for the same host, at present that has been excluded but the orignal might have allowed for that in which case we need a map of list of string.
    }

    /** free contents, then forget them.*/
    void purge() {
      for (auto each: *this) {
        free((void *)each.first);
        free((void *)each.second);
      }
      clear();
    }
  } forward_map;


public:
  class Fd { // a minimal one compared to safely/posix
    int fd = -1;

  public:
    // ReSharper disable once CppNonExplicitConversionOperator
    operator int() const {
      return fd;
    }

    int operator=(int newfd) { // NOLINT(*-unconventional-assign-operator)
      fd = newfd;
      return newfd;
    }

    bool operator==(int newfd) const {
      return fd == newfd;
    }

    // ReSharper disable once CppNonExplicitConversionOperator
    operator bool() const {
      return fd != -1;
    }

    /** @returns whether close() thinks it worked , but we discard our fd value regardless of that */
    bool close() {
      if (fd != -1) {
        int fi = ::close(fd);
        fd = -1;

        return fi != -1;
      }
      return true; // already closed is same as just now successfully closed.
    }

    /** lose track of the fd regardless of the associated file's state. Most uses seem like bugs, leaks of OS file handles.*/
    void forget() {
      fd = -1;
    }

    /** @returns whether the fd is real and not stdin, stdout, or stderr */
    bool isNotStd() const {
      return fd > 2;
    }

    /** close this oneif open, then make it attach to same file as fdother
     * @returns whether the OS was happy with doing this.
     */
    bool duplicate(int fdother) const {
      return dup2(fdother, fd) != -1; // makes fd point to same file as fdother
    }

    /** put this guy's file state into @param fdother . */
    bool copyinto(int fdother) const {
      return dup2(fd, fdother) != -1;
    }

    Fd() = default;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    Fd(int open) :
        fd(open) {
    }
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
    AutoFree request;

    /* request fields */
    AutoFree method; // as in GET, POST, ...
    AutoFree url;
    AutoFree referer;
    AutoFree user_agent;
    AutoFree authorization;

    off_t range_begin = 0;
    off_t range_end = 0;
    off_t range_begin_given = 0;
    off_t range_end_given = 0;

    AutoFree header;

    size_t header_sent = 0;
    bool header_dont_free = false;
    int header_only = 0;
    int http_code = 0;
    bool conn_closed = true;

    enum { REPLY_GENERATED,
      REPLY_FROMFILE,
      REPLY_NONE } reply_type = REPLY_NONE;

    AutoFree reply = nullptr;
    bool reply_dont_free = false;
    Fd reply_fd;
    off_t reply_start = 0;
    off_t reply_length = 0;
    off_t reply_sent = 0;
    off_t total_sent = 0;
    /* header + body = total, for logging */
  public:
    connection(DarkHttpd &parent) :
        service(parent) {
      memset(&client, 0, sizeof(client));
    }

    void clear();

    void recycle();

    void poll_check_timeout();

    const char *keep_alive() const;
    void default_reply(int errcode, const char *errname, const char *format, ...) checkFargs(4, 5);

    void redirect(const char *format, ...) checkFargs(2, 3);

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

  int parse_mimetype_line(const char *line);

  void parse_default_extension_map();

  void parse_extension_map_file(const char *filename);

  const char *url_content_type(const char *url);
  const char *get_address_text(const void *addr);

  void init_sockin();

  void usage(const char *argv0);

  void generate_dir_listing(connection &conn, const char *path, const char *decoded_url);

  // void process_get(connection &conn);

  void httpd_poll();

  bool daemonize_start();

  void daemonize_finish();

  void pidfile_remove();

  int pidfile_read();

  void freeall();

public:
  bool parse_commandline(int argc, char *argv[]);

  void accept_connection();

  void log_connection(const struct connection *conn);

  // void showUsageStats();

  void prepareToRun();

  void pidfile_create();

  void change_root();

#define DATE_LEN 30 /* strlen("Fri, 28 Feb 2003 00:02:08 GMT")+1 */

  const char *generated_on(const char date[DATE_LEN]) const;

  bool is_https_redirect(struct connection &conn) const;

  static void stop_running(int sig);

  void reportStats() const;

  int main(int argc, char **argv);

private:
  Fd fd_null;
  struct PipePair {
    Fd fds[2];
    int operator[](bool which) {
      return fds[which];
    }

    bool connect() {
      int punned[2] = {fds[0], fds[1]};
      return pipe(punned) != -1;
    }

  } lifeline;


  Fd sockin; /* socket to accept connections from */

  /** the entries will all be dynamically allocated */
  std::forward_list<connection *> entries;

  const char *forward_all_url = nullptr;

  bool forward_to_https = 0;

  /* If a connection is idle for timeout_secs or more, it gets closed and
   * removed from the connlist.
   */
  unsigned timeout_secs = 30;
  AutoFree keep_alive_field;

  /* Time is cached in the event loop to avoid making an excessive number of
   * gettimeofday() calls.
   */
  time_t now;

  /* To prevent a malformed request from eating up too much memory, die once the
   * request exceeds this many bytes:
   */
#define MAX_REQUEST_LENGTH 4000

  /* Defaults can be overridden on the command-line */
  const char *bindaddr = nullptr;//only assigned from cmdline
  uint16_t bindport = 8080; /* or 80 if running as root */
  int max_connections = -1; /* kern.ipc.somaxconn */
  const char *index_name = "index.html";
  bool no_listing = false;

#ifdef HAVE_INET6
  bool inet6 = false; /* whether the socket uses inet6 */
#endif
  const char *default_mimetype = nullptr;//always a pointer into a map which manages free'ing
  AutoFree wwwroot; /* a path name */

  char *logfile_name = nullptr; /* NULL = no logging */
  FILE *logfile = nullptr;

  /* [->] pidfile helpers, based on FreeBSD src/lib/libutil/pidfile.c,v 1.3
   * Original was copyright (c) 2005 Pawel Jakub Dawidek <pjd@FreeBSD.org>
   */
  Fd pidfile_fd;
  char *pidfile_name = nullptr; /* NULL = no pidfile */

  bool want_chroot = false;
  bool want_daemon = false;
  bool want_accf = false;
  bool want_keepalive = true;
  bool want_server_id = true;
  bool want_single_file = false;
  AutoFree server_hdr;
  AutoFree auth_key; /* NULL or "Basic base64_of_password" */
  AutoFree custom_hdrs;
  uint64_t num_requests = 0;
  uint64_t total_in = 0;
  uint64_t total_out = 0;
  bool accepting = true; /* set to 0 to stop accept()ing */
  bool syslog_enabled = false;
  volatile bool running = false; /* signal handler sets this to false */

#define INVALID_UID ((uid_t) ~0)
#define INVALID_GID ((gid_t) ~0)

  uid_t drop_uid = INVALID_UID;
  gid_t drop_gid = INVALID_GID;
};
