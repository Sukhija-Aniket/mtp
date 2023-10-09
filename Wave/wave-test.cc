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
#include "ns3/yans-wifi-helper.h"
#include "ns3/wave-module.h"
#include "ns3/position-allocator.h"
#include "ns3/yans-wifi-channel.h"
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
#include "ns3/custom-enums.h"
// #include "ns3/socket-priority-tag.h"

using namespace ns3;
using namespace std;

int numPacket = 0;
double CSthresh = 5.9425e-12;
int channelFrequency = 5900;

uint16_t channelBandwidth = 10;
string channelBand = "BAND_5GHZ";

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

void viEnq(Ptr<const WifiMacQueueItem> item){
    Time now = Simulator::Now ();
  double t = now.GetSeconds ();
  Ptr<const Packet> pkt = item->GetPacket();
  cout << setprecision (15) << "VI Enqueue at: " << t << endl;
  cout << pkt->GetUid() << endl;
}

void bkEnq(Ptr<const WifiMacQueueItem> item){
    Time now = Simulator::Now ();
  double t = now.GetSeconds ();
  Ptr<const Packet> pkt = item->GetPacket();
  cout << setprecision (15) << "BK Enqueue at: " << t << endl;
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
GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize, uint32_t pktCount, Time pktInterval)
{
  if (pktCount > 0)
    {   
        Ptr<Packet> pkt = Create<Packet> (pktSize);
        // pkt->AddPacketTag (tag);
        // SocketPriorityTag prio;
        // uint8_t p = (uint8_t)txInfo.priority;
        // prio.SetPriority(p);
        // pkt->ReplacePacketTag (prio);
        // std::filebuf fb;
        // fb.open("test.txt", std::ios::out);
        // Create an ostream object associated with the filebuf object.
        // std::ostream os(&fb);
        // pkt->PrintPacketTags(os);
        socket->Send (pkt);
        Simulator::Schedule (pktInterval, &GenerateTraffic, socket, pktSize, pktCount - 1,
                            pktInterval);
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
  uint32_t numPackets = 1;
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
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper();
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(500));
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue(5.9e9));

  YansWavePhyHelper wavePhy = YansWavePhyHelper::Default ();
  Ptr<YansWifiChannel> channel = wifiChannel.Create ();
  wavePhy.SetChannel (channel);
  uint32_t channelID = channel->GetId ();

  vector<double> sensi = {-101.0, -50.0, -20.0, -10.0, 0.0, 10.0, 20.0, };

  for(int iter = 0; iter<sensi.size(); iter++){

  }
  // Physical Layer parameters settings
  wavePhy.Set("Frequency", UintegerValue(channelFrequency));
  string channelTuple = "{" + to_string(channelID) + ", " + to_string(channelBandwidth) + ", " + channelBand + ", 0}";
  wavePhy.Set("ChannelSettings", StringValue(channelTuple));

  // ns-3 supports generate a pcap trace
  wavePhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
  QosWaveMacHelper waveMac = QosWaveMacHelper::Default ();

  Wifi80211pHelper waveHelper = Wifi80211pHelper::Default ();

  waveHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
  						"DataMode", StringValue ("OfdmRate6MbpsBW10MHz"	),
  						"ControlMode",StringValue ("OfdmRate6MbpsBW10MHz"),
  						"NonUnicastMode", StringValue ("OfdmRate6MbpsBW10MHz"));
  NetDeviceContainer devices = waveHelper.Install (wavePhy, waveMac, c);
  Ptr<WifiNetDevice> node1 = DynamicCast<WifiNetDevice> (devices.Get(0));
  Ptr<WifiPhy> node1Phy = node1->GetPhy();
  node1Phy->SetTxPowerStart(30.0);
  node1Phy->SetTxPowerEnd(30.0);
  Ptr<WifiNetDevice> node2 = DynamicCast<WifiNetDevice> (devices.Get(1));
  Ptr<WifiPhy> node2Phy = node2->GetPhy();
  node2Phy->SetTxPowerStart(30.0);
  node2Phy->SetTxPowerEnd(30.0);

  // Tracing
  wavePhy.EnablePcap ("wave-simple-80211p", devices);

  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  positionAlloc->Add (Vector (500.0, 0.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (c);

  InternetStackHelper internet;
  internet.Install (c);
    
    // Find Queue
    Ptr<WifiNetDevice> a = DynamicCast<WifiNetDevice> (devices.Get(1));
    Ptr<WifiMac> b = a->GetMac();

    Ptr<WifiMacQueue> be = b->GetTxopQueue(AcIndex::AC_BE);
    Ptr<WifiMacQueue> vo = b->GetTxopQueue(AcIndex::AC_VO);
    Ptr<WifiMacQueue> bk = b->GetTxopQueue(AcIndex::AC_BK);
    Ptr<WifiMacQueue> vi = b->GetTxopQueue(AcIndex::AC_VI);

    be->TraceConnectWithoutContext("Enqueue", MakeCallback(&beEnq));
    // be->TraceConnectWithoutContext("Dequeue", MakeCallback(&beDeq));
    vo->TraceConnectWithoutContext("Enqueue", MakeCallback(&voEnq));
    vi->TraceConnectWithoutContext("Enqueue", MakeCallback(&viEnq));
    bk->TraceConnectWithoutContext("Enqueue", MakeCallback(&bkEnq));

//   Ipv4AddressHelper ipv4;
//   NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> recvSink = Socket::CreateSocket (c.Get (0), tid);
  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);
  recvSink->Bind (local);
  recvSink->SetIpTos(NS3_TOS_VAL::TOS_VO);
  recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));
  
  Ptr<Socket> source = Socket::CreateSocket (c.Get (1), tid);
  InetSocketAddress remote = InetSocketAddress (Ipv4Address ("255.255.255.255"), 80);
  source->SetAllowBroadcast (true);
  source->Connect (remote);
  source->SetIpTos(NS3_TOS_VAL::TOS_VO);
  // trace for send packets
  source->SetSendCallback (MakeCallback (&SendPacket));

  Simulator::ScheduleWithContext (source->GetNode ()->GetId (), Seconds (1.0), &GenerateTraffic,
                                  source, packetSize, numPackets, interPacketInterval);
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}

