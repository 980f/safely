#ifndef ART_INSTRUMENT
#define ART_INSTRUMENT

#include "eztypes.h"  //for const'd globals

#define HostBaudStuff 460800, 'N', 8
#define useCTS false

//qcu-pcu uses the same as the host for mental convenience

//on the QCU the hostport is connected to the linux system, on the PCU it connects to the QCU.
#if P103 == 1
#define HostUart 2
#else
#define HostUart 1
#endif

//havta manually resolve the irq number rather than wrestling with the preprocessor.
#if HostUart == 1
#define Host_Data_irq 37
#elif HostUart == 2
#define Host_Data_irq 38
#elif HostUart == 3
#define Host_Data_irq 39
#else
#warning HostUart not valid or not defined in art.h
#endif
///

//allow device to have a non-standard ticker rate, or live with this default:
#ifndef TickerHertz
#define TickerHertz 4000
#endif

//this determines polling and the like operation. Value in Hz
#define ArtHeartRate 100

///end arthead

//buffer space need in firmware for normal stuff, excludes spectra
#define MaximumCommand (15*9+10+3)

#if isQCU
#include "qcu-art.h"
#elif isPCU
#include "pcu-art.h"
#else
#include "sigcuser.h"
#define MaximumResponse 20000
#endif  //PCU

//the number below can be tweaked per system in the above if/elif
//args quantity here limits number of command parameters
#define ArgsPerMessage 15

#endif /* ifndef ART_INSTRUMENT */
