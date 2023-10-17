// /* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
// /*
//  * Copyright (c) 2005,2006,2007 INRIA
//  * Copyright (c) 2013 Dalian University of Technology
//  *
//  * This program is free software; you can redistribute it and/or modify
//  * it under the terms of the GNU General Public License version 2 as
//  * published by the Free Software Foundation;
//  *
//  * This program is distributed in the hope that it will be useful,
//  * but WITHOUT ANY WARRANTY; without even the implied warranty of
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  * GNU General Public License for more details.
//  *
//  * You should have received a copy of the GNU General Public License
//  * along with this program; if not, write to the Free Software
//  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  *
//  * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
//  * Author: Junling Bu <linlinjavaer@gmail.com>
//  *
//  */
// /**
//  * This example shows basic construction of an 802.11p node.  Two nodes
//  * are constructed with 802.11p devices, and by default, one node sends a single
//  * packet to another node (the number of packets and interval between
//  * them can be configured by command-line arguments).  The example shows
//  * typical usage of the helper classes for this mode of WiFi (where "OCB" refers
//  * to "Outside the Context of a BSS")."
//  */

// #include "ns3/vector.h"
// #include "ns3/string.h"
// #include "ns3/socket.h"
// #include "ns3/double.h"
// #include "ns3/config.h"
// #include "ns3/log.h"
// #include "ns3/command-line.h"
// #include "ns3/mobility-model.h"
// #include "ns3/yans-wifi-helper.h"
// #include "ns3/position-allocator.h"
// #include "ns3/mobility-helper.h"
// #include "ns3/internet-stack-helper.h"
// #include "ns3/ipv4-address-helper.h"
// #include "ns3/ipv4-interface-container.h"
// #include <iostream>

// #include "ns3/custom-display.h"
// #include "ns3/custom-enums.h"
// #include "ns3/trace-functions.h"
// #include "unistd.h"
// #include <filesystem>
// #include "ns3/ocb-wifi-mac.h"
// #include "ns3/wifi-80211p-helper.h"
// #include "ns3/wave-mac-helper.h"

// using namespace ns3;

// NS_LOG_COMPONENT_DEFINE ("WifiSimpleOcb");

// /*
//  * In WAVE module, there is no net device class named like "Wifi80211pNetDevice",
//  * instead, we need to use Wifi80211pHelper to create an object of
//  * WifiNetDevice class.
//  *
//  * usage:
//  *  NodeContainer nodes;
//  *  NetDeviceContainer devices;
//  *  nodes.Create (2);
//  *  YansWifiPhyHelper wifiPhy;
//  *  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
//  *  wifiPhy.SetChannel (wifiChannel.Create ());
//  *  NqosWaveMacHelper wifi80211pMac = NqosWave80211pMacHelper::Default();
//  *  Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default ();
//  *  devices = wifi80211p.Install (wifiPhy, wifi80211pMac, nodes);
//  *
//  * The reason of not providing a 802.11p class is that most of modeling
//  * 802.11p standard has been done in wifi module, so we only need a high
//  * MAC class that enables OCB mode.
//  */

// /**
//  * Receive a packet
//  * \param socket Rx socket
//  */
// void ReceivePacket (Ptr<Socket> socket)
// {
//   while (socket->Recv ())
//     {
//       NS_LOG_UNCOND ("Received one packet!");
//     }
// }

// /**
//  * Geerate traffic
//  * \param socket Tx socket
//  * \param pktSize packet size
//  * \param pktCount number of packets
//  * \param pktInterval interval between packet generation
//  */
// static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize,
//                              uint32_t pktCount, Time pktInterval )
// {
//   if (pktCount > 0)
//     {
//       socket->SetIpTos(255);
//       socket->Send (Create<Packet> (pktSize));
//       Simulator::Schedule (pktInterval, &GenerateTraffic,
//                            socket, pktSize,pktCount - 1, pktInterval);
//     }
//   else
//     {
//       socket->Close ();
//     }
// }

// void getOutput(vector<vector<DisplayObject>*> objGrid, FILE* fp, int sender, int reciever) {
//   int numpacketsSent = objGrid[sender]->size();
//   int numpacketsRecieved = objGrid[reciever]->size();
//   int numpacketsDropped = numpacketsSent - numpacketsRecieved;
//   int phycnt = 0;
//   int ipv4cnt = 0;
//   int maccnt = 0;
//   int appcnt = 0;
//   int enqueDequecnt = 0;
//   double phyavg = 0;
//   double macavg = 0;
//   double ipv4avg = 0;
//   double appavg = 0;
//   double enqueDequeavg = 0;
//   // create a map
//   std::map<int,std::vector<double>> mp;
//   int n = objGrid.size();

//   for(auto &obj: *(objGrid[sender])) {
//     if (mp.find(obj.getUid()) == mp.end()) {
//       mp[obj.getUid()] = std::vector<double>(n);
//     }
//   }

//   for(int i=0;i<n;i++) {
//     for(auto &obj: *(objGrid[i])) {
//       if (mp.find(obj.getUid()) == mp.end()) continue;
//       mp[obj.getUid()][i] = obj.getTime();
//     }
//   }

//   for(auto x:mp) {
//     if (x.second[PHYTXBEGINENUM] > 0 && x.second[PHYRXENDNUM] > 0) {
//       phyavg += (x.second[PHYRXENDNUM] - x.second[PHYTXBEGINENUM]);
//       phycnt++;
//     }
//     if (x.second[IPV4L3PROTOCOLTXNUM] > 0 && x.second[IPV4L3PROTOCOLRXNUM] > 0) {
//       ipv4avg += (x.second[IPV4L3PROTOCOLRXNUM] - x.second[IPV4L3PROTOCOLTXNUM]);
//       ipv4cnt++;
//     }
//     if (x.second[MACTXNUM] > 0 && x.second[MACRXNUM] > 0) {
//       macavg += (x.second[MACRXNUM] - x.second[MACTXNUM]);
//       maccnt++;
//     }
//     if (x.second[sender] > 0 && x.second[reciever] > 0) {
//       appavg += (x.second[reciever] - x.second[sender]);
//       appcnt++;
//     }
//     if (x.second[MACENQUEUENUM] > 0 && x.second[MACDEQUEUENUM] > 0) {
//       enqueDequeavg += (x.second[MACDEQUEUENUM] - x.second[MACENQUEUENUM]);
//       enqueDequecnt++;
//     }
//   }

//   enqueDequeavg /= enqueDequecnt;
//   macavg /= maccnt;
//   phyavg /= phycnt;
//   ipv4avg /= ipv4cnt;
//   appavg /= appcnt;

//   cout<<"Total Transmitted packets: "<<numpacketsSent<<endl;
//   cout<<"Total Received packets: "<<numpacketsRecieved<<endl;
//   cout<<"Total Dropped packets: "<<numpacketsDropped<<endl;

//   cout<<"AppAvg: "<<appavg<<endl;
//   cout<<"Ipv4Avg: "<<ipv4avg<<endl;
//   cout<<"EnqueDequeAvg: "<<enqueDequeavg<<endl;
//   cout<<"MacAvg: "<<macavg<<endl;
//   cout<<"phyAvg : "<<phyavg<<endl;
// }


// int main (int argc, char *argv[])
// {
//   std::string phyMode ("OfdmRate1MbpsBW10MHz");
//   uint32_t packetSize = 1000; // bytes
//   uint32_t numPackets = 4;
//   double interval = 1.0; // seconds
//   bool verbose = false;

//   vector<vector<DisplayObject>*> objContainers = CreateObjContainer();

//   CommandLine cmd (__FILE__);

//   cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
//   cmd.AddValue ("packetSize", "size of application packet sent", packetSize);
//   cmd.AddValue ("numPackets", "number of packets generated", numPackets);
//   cmd.AddValue ("interval", "interval (seconds) between packets", interval);
//   cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
//   cmd.Parse (argc, argv);
//   // Convert to time object
//   Time interPacketInterval = Seconds (interval);


//   NodeContainer c;
//   c.Create (2);

//   // The below set of helpers will help us to put together the wifi NICs we want
//   YansWifiPhyHelper wifiPhy;
//   YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
//   Ptr<YansWifiChannel> channel = wifiChannel.Create ();
//   wifiPhy.SetChannel (channel);
//   // ns-3 supports generate a pcap trace
//   wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11);
//   QosWaveMacHelper wifi80211pMac = QosWaveMacHelper::Default ();
//   // TODO: changed the mac helper from Nqos to Qos
//   Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default ();
//   if (verbose)
//     {
//       wifi80211p.EnableLogComponents ();      // Turn on all Wifi 802.11p logging
//     }

//   wifi80211p.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
//                                       "DataMode",StringValue (phyMode),
//                                       "ControlMode",StringValue (phyMode));
//   NetDeviceContainer devices = wifi80211p.Install (wifiPhy, wifi80211pMac, c);

//   Ptr<WifiNetDevice> sender = DynamicCast<WifiNetDevice> (devices.Get(1));
//   Ptr<WifiNetDevice> receiver = DynamicCast<WifiNetDevice> (devices.Get(0));
//   Ptr<WifiMacQueue> senderMacQueue = sender->GetMac()->GetTxopQueue(AcIndex::AC_VO);
//   Ptr<WifiMacQueue> receiverMacQueue = receiver->GetMac()->GetTxopQueue(AcIndex::AC_VO);
//   // senderMacQueue->TraceConnect("Enqueue", "apEnqueue", MakeBoundCallback(&MacEnqueueTrace, objContainers[MACENQUEUENUM]));
//   receiverMacQueue->TraceConnect("Enqueue", "apEnqueue", MakeBoundCallback(&MacEnqueueTrace, objContainers[MACENQUEUENUM]));
//   // senderMacQueue->TraceConnect("Dequeue", "apDequeue", MakeBoundCallback(&MacDequeueTrace, objContainers[MACDEQUEUENUM]));
//   receiverMacQueue->TraceConnect("Dequeue", "apDequeue", MakeBoundCallback(&MacDequeueTrace, objContainers[MACDEQUEUENUM]));

//   // Tracing
//   std::string fileName = getFileName(__FILE__);
//   wifiPhy.EnablePcap (fileName, devices);
//   wifiPhy.EnableAscii (fileName, devices);

//   MobilityHelper mobility;
//   Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
//   positionAlloc->Add (Vector (0.0, 0.0, 0.0));
//   positionAlloc->Add (Vector (5.0, 0.0, 0.0));
//   mobility.SetPositionAllocator (positionAlloc);
//   mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
//   mobility.Install (c);

//   InternetStackHelper internet;
//   internet.Install (c);

//   Ipv4AddressHelper ipv4;
//   NS_LOG_INFO ("Assign IP Addresses.");
//   ipv4.SetBase ("10.1.1.0", "255.255.255.0");
//   Ipv4InterfaceContainer i = ipv4.Assign (devices);

//   TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
//   Ptr<Socket> recvSink = Socket::CreateSocket (c.Get (0), tid);
//   InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);
  // recvSink->Bind (local);
//   recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));

//   Ptr<Socket> source = Socket::CreateSocket (c.Get (1), tid);
//   InetSocketAddress remote = InetSocketAddress (Ipv4Address ("255.255.255.255"), 80);
//   source->SetAllowBroadcast (true);
//   source->Connect (remote);

//   Simulator::ScheduleWithContext (source->GetNode ()->GetId (),
//                                   Seconds (1.0), &GenerateTraffic,
//                                   source, packetSize, numPackets, interPacketInterval);


//   // Config::Connect("NodeList/*/ApplicationList/*/$ns3::UdpEchoClient/TxWithAddresses", MakeBoundCallback (&UdpEchoClientTxWithAddressesTrace, objContainers[UDPECHOCLIENTTXNUM]));
//   Config::Connect("NodeList/*/$ns3::Ipv4L3Protocol/Tx", MakeBoundCallback(&Ipv4L3ProtocolTxTrace, objContainers[IPV4L3PROTOCOLTXNUM]));
//   Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTx", MakeBoundCallback(&MacTxTrace, objContainers[MACTXNUM]));
//   Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxBegin", MakeBoundCallback(&PhyTxBeginTrace, objContainers[PHYTXBEGINENUM]));
//   Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxEnd", MakeBoundCallback(&PhyTxEndTrace,objContainers[PHYTXENDENUM]));
//   Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeBoundCallback(&MacRxTrace, objContainers[MACRXNUM]));
//   // Config::Connect("NodeList/*/ApplicationList/*/$ns3::UdpEchoServer/RxWithAddresses", MakeBoundCallback(&UdpEchoServerRxWithAddressesTrace, objContainers[UDPECHOSERVERTXNUM]));
//   Config::Connect("NodeList/*/$ns3::Ipv4L3Protocol/Rx", MakeBoundCallback(&Ipv4L3ProtocolRxTrace, objContainers[IPV4L3PROTOCOLRXNUM]));
//   Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxBegin", MakeBoundCallback(&PhyRxBeginTrace,objContainers[PHYRXBEGINENUM]));
//   Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxEnd", MakeBoundCallback(&PhyRxEndTrace,objContainers[PHYRXENDNUM]));

//   Simulator::Run ();
//   Simulator::Destroy ();

//   FILE*fp = freopen((getLogFileName(__FILE__)).c_str(), "w", stdout);
//   getObjTrace(objContainers, MACTXNUM, fp);
//   fp = freopen((getOutputFileName(__FILE__)).c_str(), "w", stdout);
//   getOutput(objContainers, fp, MACTXNUM, MACRXNUM);

//   return 0;
// }
