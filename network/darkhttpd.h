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
#include <cstdlib>
#include <cstring>
#include <map>


class DarkHttpd {

  /* [->] LIST_* macros taken from FreeBSD's src/sys/sys/queue.h,v 1.56
   * Copyright (c) 1991, 1993
   *      The Regents of the University of California.  All rights reserved.
   *
   * Under a BSD license.
   */
  /** doubly linked list, with all content outside the class */
  template <typename Type> class ListElement {
    //#define LIST_ENTRY(type)
    Type *le_next=0;   /* next element content */
    Type **le_prev=0;  /* address of previous next element */
  };

  class Exception : public std::exception {
    //todo: internal malloc string with delete
    const char *msg=nullptr;
    int returncode=0;//what would have been returned to a shell on exit.
    ~Exception() override;//todo: free message
  public:
    Exception(int returncode,const char *msgf,...);
  };

#define LIST_ENTRY(Type) struct Type: public ListElement<Type> {};
#if 0
#define LIST_HEAD(name, type)
struct name {
        struct type *lh_first;  /* first element */
}

#define LIST_HEAD_INITIALIZER(head)                                     \
        { NULL }

#define LIST_FIRST(head)        ((head)->lh_first)

#define LIST_FOREACH_SAFE(var, head, field, tvar)                       \
    for ((var) = LIST_FIRST((head));                                    \
        (var) && ((tvar) = LIST_NEXT((var), field), 1);                 \
        (var) = (tvar))

#define LIST_INSERT_HEAD(head, elm, field) do {                         \
        if ((LIST_NEXT((elm), field) = LIST_FIRST((head))) != NULL)     \
                LIST_FIRST((head))->field.le_prev = &LIST_NEXT((elm), field);\
        LIST_FIRST((head)) = (elm);                                     \
        (elm)->field.le_prev = &LIST_FIRST((head));                     \
} while (0)

#define LIST_NEXT(elm, field)   ((elm)->field.le_next)

#define LIST_REMOVE(elm, field) do {                                    \
        if (LIST_NEXT((elm), field) != NULL)                            \
                LIST_NEXT((elm), field)->field.le_prev =                \
                    (elm)->field.le_prev;                               \
        *(elm)->field.le_prev = LIST_NEXT((elm), field);                \
} while (0)
/* [<-] */

static LIST_HEAD(conn_list_head, connection) connlist =
    LIST_HEAD_INITIALIZER(conn_list_head);

  };
#endif


/** replacing inline reproduction of map logic with an std map, after verifying who did the allocations.
 * this uses the default key compare function 'less<const char *>', which we might want to replace with strncmp(...longest_ext) */
struct mime_mapping: std::map<const char *,const char *> {
  size_t longest_ext = 0;
  // static size_t mime_map_size = 0;
  /** arg checking 'add to map' */
  using mime_ref=std::pair<std::_Rb_tree_iterator<std::pair<const char * const, const char *>>, bool>;//todo: indirect to a decltype on the function returning this.
  mime_ref add(const char *extension, const char *mimetype);
} mime_map;

struct forward_mapping: std::map<const char *,const char *> {
  // const char *host, *target_url; /* These point at argv. */
  void add(const char * const host,const char * const target_url){
    insert_or_assign(host,target_url);//# allows breakpoint on these. We need to decide whether multiples are allowed for the same host, at present that has been excluded but the orignal might have allowed for that in which case we need a map of list of string.
  }
} forward_map;

  void parse_mimetype_line(const char *line);

  void parse_default_extension_map();

  const char *url_content_type(const char *url);

  void showUsageStats();

  void prepareToRun();

  void freeall();

  int main(int argc, char **argv);

/** free with nulling of pointer to make use after free uniformly a sigsegv instead of random crash or corruption
 * @see safely/system library.
 */
static void free(void **malloced){
  if (malloced) {
    ::free(*malloced);
    malloced = nullptr;
  }
}
};