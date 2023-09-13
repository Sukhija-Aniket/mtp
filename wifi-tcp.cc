/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015, IMDEA Networks Institute
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
 * Author: Hany Assasa <hany.assasa@gmail.com>
.*
 * This is a simple example to test TCP over 802.11n (with MPDU aggregation enabled).
 *
 * Network topology:
 *
 *   Ap    STA
 *   *      *
 *   |      |
 *   n1     n2
 *
 * In this example, an HT station sends TCP packets to the access point.
 * We report the total throughput received during a window of 100ms.
 * The user can specify the application data rate and choose the variant
 * of TCP i.e. congestion control algorithm to use.
 */

#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h"
#include "ns3/packet-sink.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/tcp-westwood.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/wifi-net-device.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/wifi-mac-header.h"
#include "ns3/tcp-socket-base.h"
#include "ns3/traffic-control-helper.h"
#include "ns3/queue.h"
#include "mutex"
#include "ns3/custom-display.h"
#include "ns3/trace-functions.h"
#include "iomanip"
#include "ns3/wifi-mac.h"
#include "ns3/wifi-mac-queue.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-mac-queue-item.h"

NS_LOG_COMPONENT_DEFINE ("wifi-tcp");

using namespace ns3;
using namespace std;

Ptr<PacketSink> sink;                         /* Pointer to the packet sink application */
Ptr<UdpEchoServer> udpServerSink;
uint64_t lastTotalRx = 0;                     /* The value of the last total received bytes */
uint64_t lastTotalTx = 0;                     /* The value of the last total trasmitted bytes */

mutex mtx;

map<uint64_t,vector<DisplayObject>> mp;       /* Map to sore all required information */
uint64_t totalNumPackets = 0;
uint64_t successfulNumPacketsTransmitted = 0;
uint64_t totalTransmissions = 0;
uint64_t Re_Phy = 0;
uint64_t Re_Mac = 0;
uint64_t numPacketsRecieved = 0;
uint64_t dataPacketDrops = 0;
uint64_t totalPacketDrops = 0;
double tm_phy = 0;
double tm_mc = 0;
double tm_ap = 0;
map<uint64_t, uint64_t> phy_freq;
map<uint64_t, uint64_t> mc_freq;

double totalTime = 0;

void
CalculateThroughput ()
{
  Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
  // double cur = (sink->GetTotalRx () - lastTotalRx) * (double) 8 / 1e5;     /* Convert Application RX Packets to MBits. */
  // std::cout << now.GetSeconds () << "s: \t" << cur << " Mbit/s" << std::endl;
  // lastTotalRx = sink->GetTotalRx ();
  Simulator::Schedule (MilliSeconds (100), &CalculateThroughput);
}

void calculateThroughputPhysicalLayer (Ptr<WifiNetDevice> device) {
  Ptr<WifiPhy> phy = device->GetPhy ();
  Ptr<Channel> channel = phy->GetChannel();
}

// Extra Traces I wish to have a look at //
void QosTxopTrace(string context, Time t1, Time t2) {
  cout<<context<<" Time Range: "<<t1<<":"<<t2<<endl;
  return;
}

void MonitorSnifferTxTrace(string context,Ptr<const Packet> pkt, uint16_t channel , WifiTxVector tx, MpduInfo mpdu, SignalNoiseDbm snr, uint16_t abc) {
  cout<<context<<endl;
  cout<<"\tChannel: "<<channel<<" Tx: "<<tx.GetMode()<<"\t Signal="<<snr.signal<<" \tNoise=" << snr.noise <<" abc:" <<abc << endl;
  cout<<"\t Time: "<<Simulator::Now()<<endl;

  WifiMacHeader hdr;
  if (pkt->PeekHeader(hdr)) {
    cout<<" \tAddr1: "<<hdr.GetAddr1()<<" \tAddr2: "<<hdr.GetAddr2()<<endl;
    cout<<" \tSequenceNumber: "<< hdr.GetSequenceNumber() << (hdr.IsData() ? " Data" : " Not Data") <<endl;

  }
  cout<<RESET<<endl;
}

void displayDetails() {
  FILE* fp = freopen("outputDetails.txt", "w", stdout);
  if (fp != NULL) {
    cout<<"\n\n\n\n\n\n"<<GREEN;
    for(auto x:mp) {
      cout<<" packet Uid: "<<x.first<<"\n";
      for(auto y:x.second) {
        cout<<y<<"\n";
      }
    }
    cout<<RESET<<endl;
  }
  fclose(fp);
}

void displayOutput(double simulationTime)  {
  totalNumPackets = mc_freq.size();
  successfulNumPacketsTransmitted = phy_freq.size();
  for(auto x:phy_freq) {
    totalTransmissions += x.second;
    Re_Phy += (x.second -1);
  }
  for(auto x:mc_freq) {
    Re_Mac += (x.second-1);
  }
  dataPacketDrops = totalTransmissions - numPacketsRecieved;
  double averageThroughput = ((sink->GetTotalRx () * 8) / (1e6 * simulationTime));
  double finalLossPnt = ((dataPacketDrops * 1.0)/successfulNumPacketsTransmitted) * 100;
  double actualLossPnt = ((dataPacketDrops * 1.0)/totalTransmissions) * 100;
  tm_phy/= numPacketsRecieved;
  tm_mc/= numPacketsRecieved;


  FILE* fp = freopen("output.txt","w", stdout);
  if (fp != NULL) {
    cout<<"Number of Packets in Mac Layer: "<<totalNumPackets<<endl;
    cout<<"Successfully Transmitted Packets:"<<successfulNumPacketsTransmitted<<endl;
    cout<<"Number of Transmitted Packets: "<<totalTransmissions<<endl;
    cout<<"ReTrasmission from Phy Layer: "<<Re_Phy<<endl;
    cout<<"ReTrasmission from Mac Layer: "<<Re_Mac<<endl;
    cout<<"Number of Packets Recieved:"<<numPacketsRecieved<<endl;
    cout<<"Data Packet Drops: "<<dataPacketDrops<<endl;
    cout<<"Total Packets Dropped: "<<totalPacketDrops<<endl;
    cout<<"Loss %: "<<finalLossPnt<<"\nactual Loss %: "<<actualLossPnt<<endl;
    if (averageThroughput < 50) {
      NS_LOG_ERROR ("Obtained throughput is not in the expected boundaries!");
      exit (1);
    }
    cout<<"\n\nConsidering the packets that were recieved during Calculations\n\n";
    cout<<setprecision(10)<<"Average Time Taken in Physical Layer: "<<tm_phy<<endl;
    cout<<setprecision(10)<<"Average Time Taken from Mac to Mac Layer Transmission: "<<tm_mc<<endl;
    cout<<setprecision(10)<<"Average Throughput: "<<averageThroughput<<" Mbit/s"<<endl;
  }
  fclose(fp);
}

void TraceTcpTx(Ptr<const Packet> pkt,const TcpHeader &tcpHeader, Ptr<const TcpSocketBase> tcpSocketBase) {
  cout<<"Let's see if this works"<<endl;
  Trace("TraceTcpTx", pkt, YELLOW);
}

int
main (int argc, char *argv[])
{

  uint32_t payloadSize = 1472;                       /* Transport layer payload size in bytes. */
  std::string dataRate = "100Mbps";                  /* Application layer datarate. */
  std::string tcpVariant = "TcpNewReno";             /* TCP variant type. */
  std::string phyRate = "HtMcs0";                    /* Physical layer bitrate. */
  double simulationTime = 3;                        /* Simulation time in seconds. */
  bool pcapTracing = true;                          /* PCAP Tracing is enabled or not. */

  // output Objects
  int contSize = 20;
  vector<vector<DisplayObject>*> objContainers(contSize);
  for(int i=0;i<contSize;i++) {
    objContainers[i] = new vector<DisplayObject>();
  }

  /* Command line argument parser setup. */
  CommandLine cmd (__FILE__);
  cmd.AddValue ("payloadSize", "Payload size in bytes", payloadSize);
  cmd.AddValue ("dataRate", "Application data rate", dataRate);
  cmd.AddValue ("tcpVariant", "Transport protocol to use: TcpNewReno, "
                "TcpHybla, TcpHighSpeed, TcpHtcp, TcpVegas, TcpScalable, TcpVeno, "
                "TcpBic, TcpYeah, TcpIllinois, TcpWestwood, TcpWestwoodPlus, TcpLedbat ", tcpVariant);
  cmd.AddValue ("phyRate", "Physical layer bitrate", phyRate);
  cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
  cmd.AddValue ("pcap", "Enable/disable PCAP Tracing", pcapTracing);
  cmd.Parse (argc, argv);

  // tcpVariant = std::string ("ns3::") + tcpVariant;
  // // Select TCP variant
  // if (tcpVariant.compare ("ns3::TcpWestwoodPlus") == 0)
  //   {
  //     // Ipv4L3Protocol a;
  //     // TcpWestwoodPlus is not an actual TypeId name; we need TcpWestwood here
  //     Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
  //     // the default protocol type in ns3::TcpWestwood is WESTWOOD
  //     Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
  //   }
  // else
  //   {
  //     TypeId tcpTid;
  //     NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (tcpVariant, &tcpTid), "TypeId " << tcpVariant << " not found");
  //     Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (tcpVariant)));
  //   }

  /* Configure TCP Options */
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));

  WifiMacHelper wifiMac;
  WifiHelper wifiHelper;
  wifiHelper.SetStandard (WIFI_STANDARD_80211n);

  /* Set up Legacy Channel */
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));

  /* Setup Physical Layer */
  YansWifiPhyHelper wifiPhy;
  wifiPhy.SetChannel (wifiChannel.Create ());
  wifiPhy.SetErrorRateModel ("ns3::YansErrorRateModel");
  wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode", StringValue (phyRate),
                                      "ControlMode", StringValue ("OfdmRate24Mbps"));


  NodeContainer networkNodes;
  networkNodes.Create (2);
  Ptr<Node> apWifiNode = networkNodes.Get (0);
  Ptr<Node> staWifiNode = networkNodes.Get (1);

  /* Configure AP */
  Ssid ssid = Ssid ("network");
  wifiMac.SetType ("ns3::ApWifiMac",
                   "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifiHelper.Install (wifiPhy, wifiMac, apWifiNode);
  // Ptr<WifiNetDevice> apDevice = DynamicCast<WifiNetDevice>(apDevices.Get(0));

  // Ptr<WifiMac> apMac = apDevice->GetMac();
  // Ptr<WifiMacQueue> apQueue = apMac->GetTxopQueue(AcIndex::AC_BE);
  // apQueue->SetAttribute("MaxSize", QueueSizeValue(QueueSize(QueueSizeUnit::PACKETS, 500)));
  // apQueue->TraceConnect("Enqueue", "apEnqueue", MakeBoundCallback(&MacEnqueueTrace, objContainers[8]));
  // apQueue->TraceConnect("Dequeue", "apDequeue", MakeBoundCallback(&MacDequeueTrace, objContainers[9]));

  cout<<"or here"<<endl;
  /* Configure STA */
  wifiMac.SetType ("ns3::StaWifiMac",
                   "Ssid", SsidValue (ssid));

  NetDeviceContainer staDevices;
  staDevices = wifiHelper.Install (wifiPhy, wifiMac, staWifiNode);
  Ptr<WifiNetDevice> staDevice = DynamicCast<WifiNetDevice>(staDevices.Get(0));

  Ptr<WifiMac> staMac = staDevice->GetMac();
  staMac->SetAttribute("BE_MaxAmpduSize", UintegerValue(0));
  Ptr<WifiMacQueue> staQueueBE = staMac->GetTxopQueue(AcIndex::AC_BE);
  staQueueBE->SetAttribute("MaxSize", QueueSizeValue(QueueSize(QueueSizeUnit::PACKETS, 500)));
  staQueueBE->TraceConnect("Enqueue", "staEnqueueBE", MakeBoundCallback(&MacEnqueueTrace, objContainers[10]));
  staQueueBE->TraceConnect("Dequeue", "staDequeueBE", MakeBoundCallback(&MacDequeueTrace, objContainers[11]));

  // staMac->SetAttribute("VO_MaxAmpduSize",UintegerValue(0));
  // Ptr<WifiMacQueue> staQueueVO = staMac->GetTxopQueue(AcIndex::AC_VO);
  // staQueueVO->SetAttribute("MaxSize", QueueSizeValue(QueueSize(QueueSizeUnit::PACKETS, 500)));
  // staQueueVO->TraceConnect("Enqueue", "staEnqueueVO", MakeBoundCallback(&MacEnqueueTrace, objContainers[12]));
  // staQueueVO->TraceConnect("Dequeue", "staDequeueVO", MakeBoundCallback(&MacDequeueTrace, objContainers[13]));

  /* Mobility model */
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  positionAlloc->Add (Vector (1.0, 1.0, 0.0));

  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (apWifiNode);
  mobility.Install (staWifiNode);

  /* Internet stack */
  InternetStackHelper stack;
  stack.Install (networkNodes);

  cout<<"or or here"<<endl;
  // TrafficControlHelper tch;
  // tch.SetRootQueueDisc("ns3::CoDelQueueDisc", "MaxSize", StringValue("1000p"));
  // QueueDiscContainer queueContainer = tch.Install(apDevice);
  // tch.Install(staDevice);
  // Ptr<QueueDisc> queue1 =  queueContainer.Get(0);


  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterface;
  apInterface = address.Assign (apDevices);
  Ipv4InterfaceContainer staInterface;
  staInterface = address.Assign (staDevices);

  /* Populate routing table */
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  /* Install TCP Receiver on the access point */
  UdpEchoServerHelper echoServer (9);
  ApplicationContainer serverApp = echoServer.Install (apWifiNode);
  udpServerSink = StaticCast<UdpEchoServer>(serverApp.Get(0));
  cout<<"can be here"<<endl;
  // PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 9));
  // ApplicationContainer sinkApp = sinkHelper.Install (apWifiNode);
  // sink = StaticCast<PacketSink> (sinkApp.Get (0));
  /* Install TCP/UDP Transmitter on the station */

  UdpEchoClientHelper echoClient (apInterface.GetAddress (0), 9);
  echoClient.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  echoClient.SetAttribute("MaxPackets", UintegerValue(10));
  echoClient.SetAttribute ("Interval", TimeValue (Time ("0.00001")));
  ApplicationContainer echoClientApp = echoClient.Install (staWifiNode);

  cout<<"really here"<<endl;
  // OnOffHelper server ("ns3::TcpSocketFactory", (InetSocketAddress (apInterface.GetAddress (0), 9)));
  // echoClient.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  // echoClient.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  // echoClient.SetAttribute ("DataRate", DataRateValue (DataRate (dataRate)));
  // ApplicationContainer serverApp = server.install(apInterface.GetAddress(1), 9);

  // Ptr<Socket> genericSocket = serverApp.Get(0)->GetObject<OnOffApplication>()->GetSocket();
  // cout<<genericSocket<<endl;
  // Ptr<TcpSocket> tcpSocket = DynamicCast<TcpSocket>(genericSocket);
  // tcpSocket->TraceDisconnectWithoutContext("Tx",MakeCallback(&TraceTcpTx));
  /* Start Applications */
  serverApp.Start (Seconds (0.0));
  echoClientApp.Start (Seconds (1.0));
  Simulator::Schedule (Seconds (1.1), &CalculateThroughput);

    cout<<"simulator"<<endl;
  Config::Connect("NodeList/1/ApplicationList/*/$ns3::UdpEchoClient/TxWithAddresses", MakeBoundCallback (&UdpEchoClientTxWithAddressesTrace, objContainers[0]));
  Config::Connect("NodeList/1/$ns3::Ipv4L3Protocol/Tx", MakeBoundCallback(&Ipv4L3ProtocolTxTrace, objContainers[1]));
  Config::Connect("NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTx", MakeBoundCallback(&MacTxTrace, objContainers[2]));
  Config::Connect("NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxBegin", MakeBoundCallback(&PhyTxBeginTrace, objContainers[3]));
  Config::Connect("NodeList/1/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxEnd", MakeBoundCallback(&PhyTxEndTrace,objContainers[4]));
  Config::Connect("NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeBoundCallback(&MacRxTrace, objContainers[5]));
  Config::Connect("NodeList/*/ApplicationList/*/$ns3::UdpEchoServer/RxWithAddresses", MakeBoundCallback(&UdpEchoServerRxWithAddressesTrace, objContainers[16]));
  Config::Connect("NodeList/0/$ns3::Ipv4L3Protocol/Rx", MakeBoundCallback(&Ipv4L3ProtocolRxTrace, objContainers[7]));
  // Config::Connect("NodeList/0/ApplicationList/*/$ns3::PacketSink/TxWithAddresses", MakeBoundCallback(&PacketSinkApplicationTxWithAddressesTrace, objContainers[12]));
  // Config::Connect("NodeList/0/ApplicationList/*/$ns3::UdpEchoServer/RxWithAddresses", MakeBoundCallback (&UdpEchoServerRxWithAddressesTrace, objContainers[14]));
  Config::Connect("NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxBegin", MakeBoundCallback(&PhyRxBeginTrace,objContainers[15]));
  Config::Connect("NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxEnd", MakeBoundCallback(&PhyRxEndTrace,objContainers[16]));


  if(pcapTracing) {
    wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
    wifiPhy.EnablePcap("apDevice",apDevices);
    wifiPhy.EnablePcap("staDevice",staDevices);
  }

  /* Start Simulation */
  Simulator::Stop (Seconds (simulationTime + 1));
  Simulator::Run ();
  // cout<<objContainers[6]->size()<<" onOff: "<<objContainers[0]->size()<<endl;
  Simulator::Destroy ();
  FILE* fp = freopen("abc.txt", "w", stdout);
  prettyPrint (*(objContainers[6]));
  prettyPrint (*(objContainers[0]));
  getTimeTrace(objContainers,fp);
  // displayDetails();
  // displayOutput(simulationTime);
  fclose (fp);

  return 0;
}


/*
Trace Sources to use:
PhysicalLayerTimings: --> phyRxEnd - phyTxBegin
drops: phyTxDrop
TcpSocket:GetLastRxTrace, when transport layer recieves the packet.
Ipv4L3Protocol::GetRxTrace(), when network layer recieves the packet.
MacTx: when packet has been transmitted.
MacRx: when packet has been received.
MacTxDrop: when packet has been dropped in mac layer.
*/
