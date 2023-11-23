#ifndef CUSTOM_ENUMS_H
#define CUSTOM_ENUMS_H

#include "ns3/core-module.h"
#include "ns3/custom-display.h"
#include "mutex"
#include "ns3/phy-entity.h"
#include "ns3/wifi-mac.h"
#include "ns3/wifi-mac-queue.h"
#include "ns3/tcp-header.h"
#include "ns3/tcp-socket-base.h"
#include "filesystem"

#define PHYTXBEGIN BLUE
#define PHYTXEND GREEN
#define PHYRXBEGIN CYAN
#define PHYRXEND YELLOW
#define MACTX RED
#define MACRX MAGENTA
#define PHYTXDROP BRIGHT_BLUE
#define PHYRXDROP BRIGHT_GREEN
#define MACTXDROP BRIGHT_RED
#define MACDROP BRIGHT_BLUE
#define MACRXDROP BRIGHT_MAGENTA
#define MACEXPIRED BRIGHT_BLUE
#define IPV4L3PROTOCOLTX BRIGHT_YELLOW
#define IPV4L3PROTOCOLUNICAST BRIGHT_GREEN
#define IPV4L3PROTOCOLMULTICAST BRIGHT_BLUE
#define IPV4L3PROTOCOLRX BRIGHT_CYAN
#define TCPSOCKETBASETX CYAN
#define TCPSOCKETBASERX YELLOW

#define UDPSERVERTX BLUE
#define UDPSERVERRX GREEN
#define UDPECHOSERVERTX CYAN
#define UDPECHOSERVERRX YELLOW
#define PACKETSINKRX RED

#define UDPCLIENTTX BLUE
#define UDPCLIENTRX GREEN
#define UDPECHOCLIENTTX CYAN
#define UDPECHOCLIENTRX YELLOW
#define BULKSENDTX RED
#define ONOFFTX MAGENTA

#define MACENQUEUE BLUE
#define MACDEQUEUE GREEN

#define ENQUEUE BRIGHT_BLUE
#define DEQUEUE BRIGHT_GREEN


enum TraceNum {
  UDPCLIENTTXNUM,         // Application
  UDPECHOCLIENTTXNUM,
  ONOFFTXNUM,
  BULKSENDTXNUM,
  TCPSOCKETBASETXNUM,     // Transport
  IPV4L3PROTOCOLTXNUM,    // Network
  IPV4L3PROTOCOLUNICASTNUM,
  IPV4L3PROTOCOLMULTICASTNUM,
  MACTXNUM,               // MAC
  ENQUEUENUM,
  MACENQUEUENUM,
  MACDROPNUM,
  MACTXDROPNUM,
  MACEXPIREDNUM,
  PHYTXBEGINNUM,          // PHY TX
  PHYTXENDNUM,
  PHYTXDROPNUM,
  PHYRXBEGINNUM,          // PHY RX
  PHYRXENDNUM,
  PHYRXDROPNUM,
  MACRXNUM,               // MAC
  DEQUEUENUM,
  MACDEQUEUENUM,
  MACRXDROPNUM,
  MACTXBACKOFFNUM,
  IPV4L3PROTOCOLRXNUM,    // Network
  TCPSOCKETBASERXNUM,     // Transport
  UDPSERVERRXNUM,         // Application
  UDPECHOSERVERRXNUM,
  PACKETSINKRXNUM,
};

enum FeatureNum {
  PHYAVERAGE = 80,
  MACAVERAGE,
  APPLICATIONAVERAGE,
  IPV4L3AVERAGE,
  PHYTXDROPCOUNT,
  PHYRXDROPCOUNT,
  PHYDROPCOUNT,
  MACDROPCOUNT,
  TOTALTXCOUNT,
  TOTALRXCOUNT,
  TOTALDROPCOUNT,
};

enum NS3_TOS_VAL {
  TOS_BE = 6,
  TOS_BK = 56,
  TOS_VI = 175,
  TOS_VO = 255
};

#endif
