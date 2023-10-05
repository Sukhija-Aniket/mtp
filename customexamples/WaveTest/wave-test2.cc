/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006,2007 INRIA
 * Copyright (c) 2013 Dalian University of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 * Author: Junling Bu <linlinjavaer@gmail.com>
 *
 */
/**
 * This example shows basic construction of an 802.11p node.  Two nodes
 * are constructed with 802.11p devices, and by default, one node sends a single
 * packet to another node (the number of packets and interval between
 * them can be configured by command-line arguments).  The example shows
 * typical usage of the helper classes for this mode of WiFi (where "OCB" refers
 * to "Outside the Context of a BSS")."
 */

#include "ns3/vector.h"
#include "ns3/string.h"
#include "ns3/socket.h"
#include "ns3/double.h"
#include "ns3/config.h"
#include "ns3/log.h"
#include "ns3/command-line.h"
#include "ns3/mobility-model.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wave-module.h"
#include "ns3/position-allocator.h"
#include "ns3/mobility-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-interface-container.h"
#include <iostream>
#include "iomanip"
#include "ns3/ocb-wifi-mac.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/udp-echo-client.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/udp-echo-helper.h"
#include "ns3/wave-module.h"
#include "ns3/tag.h"
#include "ns3/custom-display.h"
#include "ns3/trace-functions.h"
#include "filesystem"
// #include "ns3/socket-priority-tag.h"

using namespace ns3;
using namespace std;

int numPacket = 0;

NS_LOG_COMPONENT_DEFINE ("WifiSimpleOcb");

/*
 * In WAVE module, there is no net device class named like "Wifi80211pNetDevice",
 * instead, we need to use Wifi80211pHelper to create an object of
 * WifiNetDevice class.
 *
 * usage:
 *  NodeContainer nodes;
 *  NetDeviceContainer devices;
 *  nodes.Create (2);
 *  YansWifiPhyHelper wifiPhy;
 *  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
 *  wifiPhy.SetChannel (wifiChannel.Create ());
 *  NqosWaveMacHelper wifi80211pMac = NqosWave80211pMacHelper::Default();
 *  Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default ();
 *  devices = wifi80211p.Install (wifiPhy, wifi80211pMac, nodes);
 *
 * The reason of not providing a 802.11p class is that most of modeling
 * 802.11p standard has been done in wifi module, so we only need a high
 * MAC class that enables OCB mode.
 */

/**
 * Receive a packet
 * \param socket Rx socket
 */
void
ReceivePacket (Ptr<Socket> socket)
{
  // while (socket->Recv ())
  //   {
  //     // NS_LOG_UNCOND ("Received one packet!");
  //     cout << "Packet: " << ++numPacket << endl;
  //   }
  Time now = Simulator::Now ();
  double t = now.GetSeconds ();
  cout << setprecision (15) << "Received at: " << t << endl;
}

void
SendPacket (Ptr<Socket> socket, uint32_t numBytes)
{
  Time now = Simulator::Now ();
  double t = now.GetSeconds ();
  cout << setprecision (15) << "Sent at: " << t << endl;
}

void
enqueueMac (std::string context, Ptr<const WifiMacQueueItem> item)
{
  Time now = Simulator::Now ();
  double t = now.GetSeconds ();
  cout << setprecision (15) << "Enqueue at: " << t << endl;
}

void
dequeueMac (std::string context, Ptr<const WifiMacQueueItem> item)
{
  Time now = Simulator::Now ();
  double t = now.GetSeconds ();
  cout << setprecision (15) << "Dequeue at: " << t << endl;
}

void beEnq(Ptr<const WifiMacQueueItem> item){
    Time now = Simulator::Now ();
  double t = now.GetSeconds ();
  Ptr<const Packet> pkt = item->GetPacket();
  cout << setprecision (15) << "BE Enqueue at: " << t << endl;
  cout << pkt->GetUid() << endl;
}

void beDeq(Ptr<const WifiMacQueueItem> item){
  Time now = Simulator::Now ();
  double t = now.GetSeconds ();
  Ptr<const Packet> pkt = item->GetPacket();
  cout << setprecision (15) << "BE Dequeue at: " << t << endl;
  cout << pkt->GetUid() << endl;
}

void voEnq(Ptr<const WifiMacQueueItem> item){
    Time now = Simulator::Now ();
  double t = now.GetSeconds ();
  Ptr<const Packet> pkt = item->GetPacket();
  cout << setprecision (15) << "VO Enqueue at: " << t << endl;
  cout << pkt->GetUid() << endl;
}

/**
 * Geerate traffic
 * \param socket Tx socket
 * \param pktSize packet size
 * \param pktCount number of packets
 * \param pktInterval interval between packet generation
 */

static void
GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize, uint32_t pktCount, Time pktInterval, HigherLayerTxVectorTag tag, TxInfo txInfo)
{
  if (pktCount > 0)
    {
        Ptr<Packet> pkt = Create<Packet> (pktSize);
        pkt->AddPacketTag (tag);
        SocketPriorityTag a;
        SocketPriorityTag prio;
        uint8_t p = (uint8_t)txInfo.priority;
        prio.SetPriority(p);
        pkt->ReplacePacketTag (prio);
        // std::filebuf fb;
        // fb.open("test.txt", std::ios::out);
        // Create an ostream object associated with the filebuf object.
        // std::ostream os(&fb);
        // pkt->PrintPacketTags(os);

        // TODO: make this change tomorrow!
        socket->SetIpTos(255);
        socket->Send (pkt);
        Simulator::Schedule (pktInterval, &GenerateTraffic, socket, pktSize, pktCount - 1,
                            pktInterval, tag, txInfo);
    }
  else
    {
      socket->Close ();
    }
}

void PhyTxEnd(Ptr<const Packet> pkt){
    cout << pkt->GetUid() << endl;
}

int
main (int argc, char *argv[])
{
  std::string phyMode ("OfdmRate6MbpsBW10MHz");
  uint32_t packetSize = 1000; // bytes
  uint32_t numPackets = 6;
  double interval = 1.0; // seconds
  bool verbose = false;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  cmd.AddValue ("packetSize", "size of application packet sent", packetSize);
  cmd.AddValue ("numPackets", "number of packets generated", numPackets);
  cmd.AddValue ("interval", "interval (seconds) between packets", interval);
  cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
  cmd.Parse (argc, argv);
  // Convert to time object
  Time interPacketInterval = Seconds (interval);

  // Create 2 nodes
  NodeContainer c;
  c.Create (2);

  // The below set of helpers will help us to put together the wifi NICs we want
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  YansWavePhyHelper wavePhy = YansWavePhyHelper::Default ();
  Ptr<YansWifiChannel> channel = wifiChannel.Create ();
  wavePhy.SetChannel (channel);

  // uint32_t channelID = channel->GetId ();

  // ns-3 supports generate a pcap trace
  wavePhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
  QosWaveMacHelper waveMac = QosWaveMacHelper::Default ();

  Wifi80211pHelper waveHelper = Wifi80211pHelper::Default ();
  waveHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
  						"DataMode", StringValue ("OfdmRate6MbpsBW10MHz"	),
  						"ControlMode",StringValue ("OfdmRate6MbpsBW10MHz"),
  						"NonUnicastMode", StringValue ("OfdmRate6MbpsBW10MHz"));
  NetDeviceContainer devices = waveHelper.Install (wavePhy, waveMac, c);

  // Tracing
  wavePhy.EnablePcap ("wave-simple-80211p", devices);

  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  positionAlloc->Add (Vector (5.0, 0.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (c);

  InternetStackHelper internet;
  internet.Install (c);

    TxInfo txInfo(CCH, 3, WifiMode ("OfdmRate12MbpsBW10MHz"), WIFI_PREAMBLE_LONG, 8);
    // txInfo.preamble = WIFI_PREAMBLE_LONG;
    // txInfo.channelNumber = CCH;
    // txInfo.dataRate = WifiMode ("OfdmRate12MbpsBW10MHz");
    // txInfo.priority = 7;
    // txInfo.txPowerLevel = 3;


    WifiTxVector txVector;
    txVector.SetChannelWidth (10);
    txVector.SetTxPowerLevel (txInfo.txPowerLevel);
    txVector.SetMode (txInfo.dataRate);
    txVector.SetPreambleType (txInfo.preamble);
    HigherLayerTxVectorTag tag = HigherLayerTxVectorTag (txVector, false);


    // Find Queue
    Ptr<WifiNetDevice> a = DynamicCast<WifiNetDevice> (devices.Get(1));
    Ptr<WifiMac> b = a->GetMac();

    Ptr<WifiMacQueue> be = b->GetTxopQueue(AcIndex::AC_BE);
    Ptr<WifiMacQueue> vo = b->GetTxopQueue(AcIndex::AC_VO);
    Ptr<WifiMacQueue> vi = b->GetTxopQueue(AcIndex::AC_VI);
    Ptr<WifiMacQueue> bk = b->GetTxopQueue(AcIndex::AC_BK);

    int contSize = 100;
    vector<vector<DisplayObject>*> objContainer(contSize);
    for(int i=0;i<contSize;i++) {
      objContainer[i] = new vector<DisplayObject>();
    }

    // be->TraceConnectWithoutContext("Enqueue", MakeCallback(&beEnq));
    // vo->TraceConnectWithoutContext("Enqueue", MakeCallback(&voEnq));
    be->TraceConnect("Enqueue", "be", MakeBoundCallback(MacEnqueueTrace, objContainer[MACENQUEUENUM]));
    vo->TraceConnect("Enqueue", "vo", MakeBoundCallback(MacEnqueueTrace, objContainer[MACENQUEUENUM]));
    vi->TraceConnect("Enqueue", "vi", MakeBoundCallback(MacEnqueueTrace, objContainer[MACENQUEUENUM]));
    bk->TraceConnect("Enqueue", "bk", MakeBoundCallback(MacEnqueueTrace, objContainer[MACENQUEUENUM]));

//   Ipv4AddressHelper ipv4;
//   NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink = Socket::CreateSocket (c.Get (0), tid);
  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);
  uint8_t prio = 3;
  recvSink->Bind (local);
  recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));
  recvSink->SetPriority(prio);

  Ptr<Socket> source = Socket::CreateSocket (c.Get (1), tid);
  InetSocketAddress remote = InetSocketAddress (Ipv4Address ("255.255.255.255"), 80);
  source->SetAllowBroadcast (true);
  source->Connect (remote);
  source->SetPriority(prio);
  // trace for send packets
  source->SetSendCallback (MakeCallback (&SendPacket));
  Simulator::ScheduleWithContext (source->GetNode ()->GetId (), Seconds (1.0), &GenerateTraffic,
                                  source, packetSize, numPackets, interPacketInterval, tag, txInfo);



  Simulator::Run ();
  Simulator::Destroy ();

  string fileName = getOutputFileName(__FILE__);
  cout<<fileName<<endl;
  FILE* fp = freopen(fileName.c_str(), "w", stdout);
  getObjTrace(objContainer, MACENQUEUENUM, fp);
  return 0;
}
