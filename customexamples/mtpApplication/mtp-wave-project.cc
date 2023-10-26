#include "ns3/wave-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/core-module.h"
#include "custom-application.h"
#include "custom-mobility-model.h"
#include "wave-setup.h"
#include "ns3/udp-client.h"
#include "ns3/udp-echo-client.h"
#include "ns3/udp-server.h"
#include "ns3/udp-echo-server.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/custom-enums.h"
#include "ns3/custom-display.h"
#include "ns3/trace-functions.h"
#include "ns3/node-list.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("mtpApplication");

void SomeEvent ()
{
  for (uint32_t i=0 ; i<NodeList::GetNNodes(); i++)
  {
    Ptr<Node> n = NodeList::GetNode(i);
    Ptr<CustomApplication> c_app = DynamicCast <CustomApplication> (n->GetApplication(0));
    c_app->SetWifiMode (WifiMode("OfdmRate3MbpsBW10MHz"));
  }
    std::cout << "******************" <<std::endl;
}

void
CalculateThroughput ()
{
  Time now = Simulator::Now ();                                         /* Return the simulator's virtual time. */
  // double cur = (sink->GetTotalRx () - lastTotalRx) * (double) 8 / 1e5;     /* Convert Application RX Packets to MBits. */
  // std::cout << now.GetSeconds () << "s: \t" << cur << " Mbit/s" << std::endl;
  // lastTotalRx = sink->GetTotalRx ();
  Simulator::Schedule (MilliSeconds (100), &CalculateThroughput);
}

void getOutput(vector<vector<DisplayObject>*> objGrid, FILE* fp, int sender, int reciever) {
  int numpacketsSent = objGrid[sender]->size();
  int numpacketsRecieved = objGrid[reciever]->size();
  int numpacketsDropped = numpacketsSent - numpacketsRecieved;
  int phycnt = 0;
  int ipv4cnt = 0;
  int maccnt = 0;
  int appcnt = 0;
  int enqueDequecnt = 0;
  double phyavg = 0;
  double macavg = 0;
  double ipv4avg = 0;
  double appavg = 0;
  double enqueDequeavg = 0;
  // create a map
  std::map<int,std::vector<double>> mp;
  int n = objGrid.size();

  for(auto &obj: *(objGrid[sender])) {
    if (mp.find(obj.getUid()) == mp.end()) {
      mp[obj.getUid()] = std::vector<double>(n);
    }
  }

  for(int i=0;i<n;i++) {
    for(auto &obj: *(objGrid[i])) {
      if (mp.find(obj.getUid()) == mp.end()) continue;
      mp[obj.getUid()][i] = obj.getTime();
    }
  }

  for(auto x:mp) {
    if (x.second[PHYTXBEGINNUM] > 0 && x.second[PHYRXENDNUM] > 0) {
      phyavg += (x.second[PHYRXENDNUM] - x.second[PHYTXBEGINNUM]);
      phycnt++;
    }
    if (x.second[IPV4L3PROTOCOLTXNUM] > 0 && x.second[IPV4L3PROTOCOLRXNUM] > 0) {
      ipv4avg += (x.second[IPV4L3PROTOCOLRXNUM] - x.second[IPV4L3PROTOCOLTXNUM]);
      ipv4cnt++;
    }
    if (x.second[MACTXNUM] > 0 && x.second[MACRXNUM] > 0) {
      macavg += (x.second[MACRXNUM] - x.second[MACTXNUM]);
      maccnt++;
    }
    if (x.second[sender] > 0 && x.second[reciever] > 0) {
      appavg += (x.second[reciever] - x.second[sender]);
      appcnt++;
    }
    if (x.second[MACENQUEUENUM] > 0 && x.second[MACDEQUEUENUM] > 0) {
      enqueDequeavg += (x.second[MACDEQUEUENUM] - x.second[MACENQUEUENUM]);
      enqueDequecnt++;
    }
  }

  enqueDequeavg /= enqueDequecnt;
  macavg /= maccnt;
  phyavg /= phycnt;
  ipv4avg /= ipv4cnt;
  appavg /= appcnt;

  cout<<"Total Transmitted packets: "<<numpacketsSent<<endl;
  cout<<"Total Received packets: "<<numpacketsRecieved<<endl;
  cout<<"Total Dropped packets: "<<numpacketsDropped<<endl;

  cout<<"AppAvg: "<<appavg<<endl;
  cout<<"Ipv4Avg: "<<ipv4avg<<endl;
  cout<<"EnqueDequeAvg: "<<enqueDequeavg<<endl;
  cout<<"MacAvg: "<<macavg<<endl;
  cout<<"phyAvg : "<<phyavg<<endl;
}

vector<Vector3D> getPV(int n, string name)  {
  string fileName = getCustomFileName(__FILE__, name);
  cout<<fileName<<endl;
  FILE* fp = freopen(fileName.c_str(), "r", stdin);
  vector<Vector3D> pv(n);
  for(int i(0);i<n;i++) {
    cin>>pv[i].x>>pv[i].y>>pv[i].z;
  }
  fclose(fp);
  return pv;
}

int main (int argc, char *argv[])
{

  int maxPackets = 20;
  double packetInterval = 0.001;
  int packetSize = 200;
  vector<vector<DisplayObject>*> objContainers = CreateObjContainer();
  string fileName;
  CommandLine cmd;

  //Number of nodes
  uint32_t nNodes = 10;
  double simTime = 60; //4 seconds
  double interval = 0.5;
  bool enablePcap = false;
  cmd.AddValue ("t","Simulation Time", simTime);
  cmd.AddValue ("i", "Broadcast interval in seconds", interval);
  cmd.AddValue ("n", "Number of nodes", nNodes);
  cmd.AddValue ("pcap", "Enable PCAP", enablePcap);
  cmd.Parse (argc, argv);

  NodeContainer nodes;
  nodes.Create(nNodes);

  // LogComponentEnable ("CustomApplication", LOG_LEVEL_FUNCTION);
  /*
    You must setup Mobility. Any mobility will work. Use one suitable for your work
  */
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::CustomMobilityModel");
  mobility.Install(nodes);
  vector<Vector3D> positions = getPV(nodes.GetN(), "positions.txt");
  vector<Vector3D> velocities = getPV(nodes.GetN(), "velocities.txt");
  for (uint32_t i=0 ; i<nodes.GetN(); i++)
  {
    //set initial positions, and velocities
    NS_LOG_LOGIC ("Setting up mobility for node " << i);
    NS_LOG_ERROR ("An error happened :(");
    Ptr<CustomMobilityModel> cmm = DynamicCast<CustomMobilityModel> (nodes.Get(i)->GetObject<MobilityModel>());
    cmm->SetPosition (positions[i]);
    cmm->SetVelocityAndAcceleration (velocities[i], Vector3D(0,0,0));
  }

  // Wifi Phy and Mac Layer
  WaveSetup wave;
  NetDeviceContainer devices = wave.ConfigureDevices(nodes);

  // Network Layer
  InternetStackHelper internet;
  internet.Install(nodes);
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  // Application Layer
  ObjectFactory client;
  client.SetTypeId("ns3::UdpEchoClient");
  uint16_t serverPort = 12345;
  ObjectFactory server;
  server.SetTypeId ("ns3::UdpEchoServer");
  InetSocketAddress sockAddress = InetSocketAddress(Ipv4Address::GetBroadcast(), serverPort);
  sockAddress.SetTos(255);
  for (uint32_t i=0 ; i<nodes.GetN() ; i++)
  {
    Ptr<UdpEchoServer> udpEchoServer = server.Create<UdpEchoServer> ();
    udpEchoServer->SetAttribute("Port", UintegerValue(sockAddress.GetPort()));
    udpEchoServer->SetStartTime(Seconds(0));
    udpEchoServer->SetStopTime(Seconds(simTime));
    nodes.Get(i)->AddApplication(udpEchoServer);

    Ptr<UdpEchoClient> udpEchoClient = client.Create <UdpEchoClient> ();
    udpEchoClient->SetAttribute("MaxPackets", UintegerValue(maxPackets));
    udpEchoClient->SetAttribute("Interval", TimeValue(Seconds(packetInterval)));
    udpEchoClient->SetAttribute("PacketSize", UintegerValue(packetSize));
    udpEchoClient->SetAttribute("RemoteAddress", AddressValue(sockAddress));
    udpEchoClient->SetStartTime(Seconds(1));
    udpEchoClient->SetStopTime(Seconds(simTime));
    nodes.Get(i)->AddApplication(udpEchoClient);

  }

  Config::Connect("NodeList/*/ApplicationList/*/$ns3::UdpEchoClient/Tx", MakeBoundCallback (&UdpEchoClientTxTrace, objContainers[UDPECHOCLIENTTXNUM]));
  Config::Connect("NodeList/*/$ns3::Ipv4L3Protocol/Tx", MakeBoundCallback(&Ipv4L3ProtocolTxTrace, objContainers[IPV4L3PROTOCOLTXNUM]));
  Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTx", MakeBoundCallback(&MacTxTrace, objContainers[MACTXNUM]));
  Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTxDrop", MakeBoundCallback(&MacTxDropTrace, objContainers[MACTXDROPNUM]));
  Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxBegin", MakeBoundCallback(&PhyTxBeginTrace, objContainers[PHYTXBEGINNUM]));
  Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxEnd", MakeBoundCallback(&PhyTxEndTrace,objContainers[PHYTXENDNUM]));
  Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxDrop", MakeBoundCallback(&PhyTxDropTrace, objContainers[PHYTXDROPNUM]));
  Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxBegin", MakeBoundCallback(&PhyRxBeginTrace,objContainers[PHYRXBEGINNUM]));
  Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxEnd", MakeBoundCallback(&PhyRxEndTrace,objContainers[PHYRXENDNUM]));
  Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxDrop", MakeBoundCallback(&PhyRxDropTrace, objContainers[PHYRXDROPNUM]));
  Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRx", MakeBoundCallback(&MacRxTrace, objContainers[MACRXNUM]));
  Config::Connect("NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacRxDrop", MakeBoundCallback(&MacRxDropTrace, objContainers[MACRXDROPNUM]));
  Config::Connect("NodeList/*/$ns3::Ipv4L3Protocol/Rx", MakeBoundCallback(&Ipv4L3ProtocolRxTrace, objContainers[IPV4L3PROTOCOLRXNUM]));
  Config::Connect("NodeList/*/ApplicationList/*/$ns3::UdpEchoServer/Rx", MakeBoundCallback(&UdpEchoServerRxTrace, objContainers[UDPECHOSERVERRXNUM]));

  // Simulator::Schedule (Seconds(1.1), &CalculateThroughput);
  // Simulator::Schedule (Seconds (30), &SomeEvent);
  Simulator::Stop(Seconds(simTime));
  Simulator::Run();
  Simulator::Destroy();

  // std::cout << "Post Simulation: " << std::endl;

  // for (uint32_t i=0 ; i<nodes.GetN(); i++)
  // {
  //   Ptr<CustomApplication> appI = DynamicCast<CustomApplication> (nodes.Get(i)->GetApplication(0));
  //   appI->PrintNeighbors ();
  // }

  fileName = getCustomFileName (__FILE__, "output.log");
  FILE* fp = freopen(fileName.c_str (), "w", stdout);
  getObjTrace(objContainers, UDPECHOCLIENTTXNUM, fp);
  fileName = getCustomFileName(__FILE__, "output.txt");
  FILE* fp2 = freopen(fileName.c_str (), "w", stdout);
  getOutput(objContainers, fp2, UDPECHOCLIENTTXNUM, UDPECHOSERVERRXNUM);

}
