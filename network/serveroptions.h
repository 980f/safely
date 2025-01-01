#ifndef SERVEROPTIONS_H
#define SERVEROPTIONS_H

#include <stored.h>
#include <storedlabel.h>

#include "storednumeric.h"

struct ServerOptions:public Stored {
  StoredBoolean enabled;
  /** service port */
  StoredCount port;
  /** tcp server parameter */
  StoredCount backlog;
  /** number of allowed connections */
  StoredCount allowed;
  /** interface to serve on, name or dotted decimal */
  StoredLabel interface;
  ServerOptions(Storable &node);
};

#endif // SERVEROPTIONS_H
