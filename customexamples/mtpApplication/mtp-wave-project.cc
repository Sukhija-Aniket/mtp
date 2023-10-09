#include "ns3/wave-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/core-module.h"
#include "custom-application.h"
#include "udp-client.h"
#include "udp-server.h"
#include "wave-setup.h"
#include "ns3/custom-display.h"
#include "ns3/trace-functions.h"
#include "ns3/core-module.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("CustomApplicationExample");

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

int main (int argc, char *argv[])
{

  int maxPackets = 20;
  int packetInterval = 0.001;
  int packetSize = 200;
  vector<vector<DisplayObject>*> objContainer = CreateObjContainer();


  CommandLine cmd;

  //Number of nodes
  uint32_t nNodes = 5;
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

  //LogComponentEnable ("CustomApplication", LOG_LEVEL_FUNCTION);
  /*
    You must setup Mobility. Any mobility will work. Use one suitable for your work
  */
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  mobility.Install(nodes);
  for (uint32_t i=0 ; i<nodes.GetN(); i++)
  {
    //set initial positions, and velocities
    NS_LOG_LOGIC ("Setting up mobility for node " << i);
    NS_LOG_ERROR ("An error happened :(");
    Ptr<ConstantVelocityMobilityModel> cvmm = DynamicCast<ConstantVelocityMobilityModel> (nodes.Get(i)->GetObject<MobilityModel>());
    cvmm->SetPosition ( Vector (20+i*5, 20+(i%2)*5, 0));
    cvmm->SetVelocity ( Vector (10+((i+1)%2)*5,0,0) );
  }

  WaveSetup wave;
  NetDeviceContainer devices = wave.ConfigureDevices(nodes);

  //Create Application in nodes

  //Method 1
  /*
  for (uint32_t i=0; i<nodes.GetN(); i++)
  {
    Ptr<CustomApplication> app_i = CreateObject<CustomApplication>();
    app_i->SetBroadcastInterval (Seconds(interval));
    app_i->SetStartTime (Seconds (0));
    app_i->SetStopTime (Seconds (simTime));
    nodes.Get(i)->AddApplication (app_i);
  }
  */


  //Method 2 using ObjcetFactor.
  for (uint32_t i=0 ; i<nodes.GetN() ; i++)
  {
    ObjectFactory fact;
    fact.SetTypeId ("ns3::CustomApplication");
    fact.Set ("Interval", TimeValue (Seconds(interval)));
    Ptr<UdpClient> udpClient = fact.Create <UdpClient> ();
    udpClient->SetStartTime(Seconds(0));
    udpClient->SetStopTime(Seconds(simTime));
    udpClient->GetSocket()->SetIpTos(255);
    udpClient->SetAttribute("MaxPackets", UintegerValue(maxPackets));
    udpClient->SetAttribute("Interval", TimeValue(Seconds(packetInterval)));
    udpClient->SetAttribute("PacketSize", UintegerValue(packetSize));
    nodes.Get(i)->AddApplication(udpClient);

    Ptr<UdpServer> udpServer = fact.Create<UdpServer> ();
    udpServer->SetStartTime(Seconds(1));
    udpServer->GetSocket()->SetIpTos(255);
    udpServer->SetStopTime(Seconds(simTime));
    nodes.Get(i)->AddApplication(udpServer);
  }

  Simulator::Schedule (Seconds(1.1), &CalculateThroughput);
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

  string fileName = getLogFileName (__FILE__);
  FILE* fp = freopen(fileName.c_str (), "w", stdout);
  getObjTrace(objContainer, UDPCLIENTTXNUM, fp);
  fileName = getOutputFileName(__FILE__);
  FILE* fp = freopen(fileName.c_str (), "w", stdout);
  getOutput(objContainer, fp, UDPCLIENTTXNUM, UDPSERVERRXNUM);


}
