#ifndef TRACE_FUNCTIONS_H
#define TRACE_FUNCTIONS_H

#include "ns3/core-module.h"
#include "ns3/phy-entity.h"
#include "ns3/wifi-mac.h"
#include "ns3/wifi-mac-queue.h"
#include "tcp-header.h"
#include "tcp-socket-base.h"
#include "filesystem"
#include "ns3/ipv4.h"
#include "ns3/custom-display.h"
#include "ns3/custom-enums.h"

#define CONT_SIZE 100

const char* traceMap[] = {
  "UDPCLIENTTXNUM",         // Application
  "UDPECHOCLIENTTXNUM",
  "ONOFFTXNUM",
  "BULKSENDTXNUM",
  "TCPSOCKETBASETXNUM",     // Transport
  "IPV4L3PROTOCOLTXNUM",    // Network
  "IPV4L3PROTOCOLUNICASTNUM",
  "IPV4L3PROTOCOLMULTICASTNUM",
  "MACTXNUM",               // MAC
  "ENQUEUENUM",
  "MACENQUEUENUM",
  "MACDROPNUM",
  "MAXTXDROPNUM",
  "MACEXPIREDNUM",
  "PHYTXBEGINNUM",          // PHY TX
  "PHYTXENDNUM",
  "PHYTXDROPNUM",
  "PHYRXBEGINNUM",          // PHY RX
  "PHYRXENDNUM",
  "PHYRXDROPNUM",
  "MACRXNUM",               // MAC
  "DEQUEUENUM",
  "MACDEQUEUENUM",
  "MACRXDROPNUM",
  "VIMACTXBACKOFFNUM",
  "VOMACTXBACKOFFNUM",
  "VIMACTXCWNUM",
  "VOMACTXCWNUM",
  "IPV4L3PROTOCOLRXNUM",    // Network
  "TCPSOCKETBASERXNUM",     // Transport
  "UDPSERVERRXNUM",         // Application
  "UDPECHOSERVERRXNUM",
  "PACKETSINKRXNUM",
};

using namespace std;

namespace ns3 {

DisplayObject Trace(std::string context, Ptr<const Packet> pkt, std::string color) {
  Time now = Simulator::Now ();
  double t = now.GetNanoSeconds();
  const uint64_t sz = pkt->GetSize();
  const uint64_t uid = pkt->GetUid();
  // cout<<"Trace functions uid: "<<uid<<" sz: "<<sz<<endl;
  DisplayObject obj = DisplayObject(context, t, sz, RESET, uid);
  return obj;
}

void PhyTxBeginTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, double pow) {
  DisplayObject obj = Trace(context, pkt, BLUE);
  (*objs).push_back(obj);
}

void PhyTxEndTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt) {
  DisplayObject obj = Trace(context, pkt, GREEN);
  (*objs).push_back(obj);
}

void PhyRxBeginTrace (std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, RxPowerWattPerChannelBand rpw) {
  DisplayObject obj = Trace(context, pkt, CYAN);
  (*objs).push_back(obj);
}

void PhyRxEndTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt) {
  DisplayObject obj = Trace(context, pkt, YELLOW);
  (*objs).push_back(obj);
}

void MacTxTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt) {
  DisplayObject obj = Trace(context, pkt, RED);
  (*objs).push_back(obj);
}

void MacRxTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt) {
  DisplayObject obj = Trace(context, pkt, MAGENTA);
  (*objs).push_back(obj);
}

void PhyTxDropTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt) {
  DisplayObject obj = Trace(context, pkt, BRIGHT_BLUE);
  (*objs).push_back(obj);
}

void PhyRxDropTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, WifiPhyRxfailureReason wfr) {
  DisplayObject obj = Trace(context, pkt, PHYRXDROP);
  (*objs).push_back(obj);
}

void MacExpiredTrace(std::vector<DisplayObject> *objs, std::string context,  Ptr<const WifiMacQueueItem> item) {
  Ptr<const Packet> pkt = item->GetPacket();
  DisplayObject obj = Trace(context, pkt, MACEXPIRED);
  (*objs).push_back(obj);
}

void MacDropTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const WifiMacQueueItem> item) {
  Ptr<const Packet> pkt = item->GetPacket();
  DisplayObject obj = Trace(context, pkt, MACDROP);
  (*objs).push_back(obj);
}

void MacTxDropTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt) {
  DisplayObject obj = Trace(context, pkt, MACRXDROP);
  (*objs).push_back(obj);
}

void MacRxDropTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt) {
  DisplayObject obj = Trace(context, pkt, MACRXDROP);
  (*objs).push_back(obj);
}

void MacTxBackOffTrace(std::vector<DisplayObject> *objs, std::string context, uint32_t backoffNum) {
  Time now = Simulator::Now ();
  double t = now.GetNanoSeconds();
  cout<<context<<": "<<backoffNum<<endl;
  DisplayObject obj = DisplayObject(context, t, backoffNum, 1000);
  (*objs).push_back(obj);
}

void MacTxCwTrace(std::vector<DisplayObject> *objs, std::string context, uint32_t oldCw, uint32_t newCw) {
  Time now = Simulator::Now ();
  double t = now.GetNanoSeconds();
  cout<<context<<" old: "<<oldCw<<" new: "<<newCw<<endl;
  DisplayObject obj = DisplayObject(context, t, oldCw, 1000);
  (*objs).push_back(obj);
 
}

void Ipv4L3ProtocolTxTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, Ptr<Ipv4> ipv4, uint32_t n) {
  DisplayObject obj = Trace(context, pkt, IPV4L3PROTOCOLTX);
  (*objs).push_back(obj);
}

void Ipv4L3ProtocolUnicastTrace(std::vector<DisplayObject> *objs, std::string context, const Ipv4Header& header, Ptr<const Packet> pkt, uint32_t n) {
  DisplayObject obj = Trace(context, pkt, IPV4L3PROTOCOLUNICAST);
  (*objs).push_back(obj);
}

void Ipv4L3ProtocolMulticastTrace(std::vector<DisplayObject> *objs, std::string context, const Ipv4Header& header, Ptr<const Packet> pkt, uint32_t n) {
  DisplayObject obj = Trace(context, pkt, IPV4L3PROTOCOLMULTICAST);
  (*objs).push_back(obj);
}

void Ipv4L3ProtocolRxTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, Ptr<Ipv4> ipv4, uint32_t n) {
  DisplayObject obj = Trace(context, pkt, IPV4L3PROTOCOLRX);
  (*objs).push_back(obj);
}

void TcpSocketBaseTxTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, const TcpHeader &tcpHeader, Ptr<const TcpSocketBase> tcpSocketBase) {
  DisplayObject obj = Trace(context, pkt, TCPSOCKETBASETX);
  (*objs).push_back(obj);
}

void TcpSocketBaseRxTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, const TcpHeader &tcpHeader, Ptr<const TcpSocketBase> tcpSocketBase) {
  DisplayObject obj = Trace(context, pkt, TCPSOCKETBASERX);
  (*objs).push_back(obj);
}

void UdpClientTxWithAddressesTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, const Address& addr1, const Address& addr2) {
  DisplayObject obj = Trace(context, pkt, UDPCLIENTTX);
  (*objs).push_back(obj);
}

void UdpClientRxWithAddressesTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, const Address& addr1, const Address& addr2) {
  DisplayObject obj = Trace(context, pkt, UDPCLIENTRX);
  (*objs).push_back(obj);
}

void UdpEchoClientTxWithAddressesTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, const Address& addr1, const Address& addr2) {
  DisplayObject obj = Trace(context, pkt, UDPECHOCLIENTTX);
  (*objs).push_back(obj);
}

void UdpEchoClientTxTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt) {
  DisplayObject obj = Trace(context, pkt, UDPECHOCLIENTTX);
  (*objs).push_back(obj);
}

void UdpEchoClientRxWithAddressesTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, const Address& addr1, const Address& addr2) {
  DisplayObject obj = Trace(context, pkt, UDPECHOCLIENTRX);
  (*objs).push_back(obj);
}

void UdpEchoClientRxTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt) {
  DisplayObject obj = Trace(context, pkt, UDPECHOCLIENTRX);
  (*objs).push_back(obj);
}

void UdpServerRxWithAddressesTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, const Address& addr1, const Address& addr2) {
  DisplayObject obj = Trace(context, pkt, UDPSERVERRX);
  (*objs).push_back(obj);
}

void UdpServerRxTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt) {
  DisplayObject obj = Trace(context, pkt, UDPSERVERRX);
  (*objs).push_back(obj);
}

void UdpEchoServerRxWithAddressesTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, const Address& addr1, const Address& addr2) {
  DisplayObject obj = Trace(context, pkt, UDPECHOSERVERRX);
  (*objs).push_back(obj);
}

void UdpEchoServerRxTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt) {
  DisplayObject obj = Trace(context, pkt, UDPECHOSERVERRX);
  (*objs).push_back(obj);
}

void OnOffApplicationTxWithAddressesTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, const Address& addr1, const Address& addr2) {
  DisplayObject obj = Trace(context, pkt, ONOFFTX);
  (*objs).push_back(obj);
}

void BulkSendApplicationTxWithAddressesTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, const Address& addr1, const Address& addr2) {
  DisplayObject obj = Trace(context, pkt, BULKSENDTX);
  (*objs).push_back(obj);
}

void PacketSinkApplicationTxWithAddressesTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, const Address& addr1, const Address& addr2) {
  DisplayObject obj = Trace(context, pkt, PACKETSINKRX);
  (*objs).push_back(obj);
}

void PacketSinkApplicationRxWithAddressesTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt, const Address& addr1, const Address& addr2) {
  DisplayObject obj = Trace(context, pkt, PACKETSINKRX);
  (*objs).push_back(obj);
}

void MacEnqueueTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const WifiMacQueueItem> item) {
  Ptr<const Packet> pkt = item->GetPacket();
  DisplayObject obj = Trace(context, pkt, MACENQUEUE);
  (*objs).push_back(obj);
}

void MacDequeueTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const WifiMacQueueItem> item) {
  Ptr<const Packet> pkt = item->GetPacket();
  DisplayObject obj = Trace(context, pkt, MACDEQUEUE);
  (*objs).push_back(obj);
}

void EnqueueTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt) {
  DisplayObject obj = Trace(context, pkt, ENQUEUE);
  (*objs).push_back(obj);
}


void DequeueTrace(std::vector<DisplayObject> *objs, std::string context, Ptr<const Packet> pkt) {
  DisplayObject obj = Trace(context, pkt, DEQUEUE);
  (*objs).push_back(obj);
}

bool cmp(std::pair<double,int>&a, std::pair<double,int> &b) {
  return a.first < b.first;
}

bool objcmp(std::pair<DisplayObject,int>&a, std::pair<DisplayObject,int> &b) {
  return a.first.getTime() < b.first.getTime();
}

void getTimeTrace(std::vector<std::vector<DisplayObject>*> objGrid, int clr, FILE* fp=NULL) {
  std::map<int, std::vector<std::pair<std::string,double>>> mp;
  int n = objGrid.size();

  for(auto &obj: *(objGrid[clr])) {
    if (mp.find(obj.getUid()) == mp.end()) {
      mp[obj.getUid()] = std::vector<std::pair<std::string,double>>(n,std::make_pair("",-1));
    }
  }
  for(int i=0;i<n;i++) {
    for(auto &obj: *(objGrid[i])) {
      if (mp.find(obj.getUid()) == mp.end()) continue;
      mp[obj.getUid()][i] = std::make_pair(obj.getName(), obj.getTime());
    }
  }
  for(auto x:mp) {
    std::cout<<std::setprecision(15)<<"Uid: "<<x.first<<std::endl;
    std::vector<std::pair<double, int>> temp(n);
    for(int i=0;i<n;i++) temp[i] = std::make_pair(x.second[i].second, i);
    sort(temp.begin(),temp.end(),cmp);
    for(int i=0;i<n;i++) {
      if (temp[i].first == -1) continue;
      std::cout<<std::setprecision(15)<<traceMap[temp[i].second]<<": "<<temp[i].first<<std::endl;
      // std::cout<<std::setprecision(15)<<traceMap[temp[i].second]<<": "<<temp[i].first<<": "<<x.second[temp[i].second].first<<std::endl;
    }
  }

  fclose(fp);
}

void getObjTrace(std::vector<std::vector<DisplayObject>*> objGrid, int clr, FILE* fp=NULL) {
  std::map<int, std::vector<std::vector<std::pair<std::string, DisplayObject>>>> mp;
  int n = objGrid.size();
  for(auto &obj: *(objGrid[clr])) {
    if (mp.find(obj.getUid()) == mp.end()) {
      mp[obj.getUid()] = std::vector<std::vector<std::pair<std::string,DisplayObject>>>(n, std::vector<std::pair<std::string, DisplayObject>>());
    }
  }

  for(int i=0;i<n;i++) {
    for(auto &obj: *(objGrid[i])) {
      if (mp.find(obj.getUid()) == mp.end()) continue;
      mp[obj.getUid()][i].push_back(std::make_pair(obj.getName(), obj));
    }
  }

  for(auto x:mp) {
    // std::cout<<std::setprecision(15)<<"Uid: "<<x.first<<std::endl;
    std::vector<std::pair<DisplayObject, int>> temp;
    for(int i=0;i<n;i++) {
      for(auto y: x.second[i]) {
        temp.push_back(std::make_pair(y.second, i));
      }
    }
    sort(temp.begin(),temp.end(),objcmp);
    for(uint32_t i=0;i<temp.size();i++) {
      if (temp[i].first.getTime() == -1) continue;
      std::cout<<temp[i].first;
    }
  }

  fclose(fp);
}

std::vector<std::vector<DisplayObject>*> CreateObjContainer() {
  vector<vector<DisplayObject>*> objContainer(CONT_SIZE);
  for(int i=0;i<CONT_SIZE;i++) {
    objContainer[i] = new vector<DisplayObject>();
  }
  return objContainer;
}

}

#endif
