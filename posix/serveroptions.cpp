#include "serveroptions.h"


ServerOptions::ServerOptions(Storable &node) : Stored(node),
  ConnectChild(enabled,true),
  ConnectChild(port,0x980f),
  ConnectChild(backlog,5),
  ConnectChild(allowed,2),
  ConnectChild(interface,"localhost"){//"localhost" is safest security wise, "0.0.0.0" is most promiscuous.
  //#nada
}
