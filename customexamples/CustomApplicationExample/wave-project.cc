#include "ns3/wave-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/core-module.h"
#include "custom-application.h"
#include "wave-setup.h"
#include "ns3/custom-enums.h"
#include "ns3/custom-display.h"
#include "ns3/trace-functions.h"
#include "custom-mobility-model.h"
#include "iomanip"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("CustomApplicationExample");

// void SomeEvent ()
// {
//   for (uint32_t i=0 ; i<NodeList::GetNNodes(); i++)
//   {
//     Ptr<Node> n = NodeList::GetNode(i);
//     Ptr<CustomApplication> c_app = DynamicCast <CustomApplication> (n->GetApplication(0));
//     c_app->SetWifiMode (WifiMode("OfdmRate3MbpsBW10MHz"));
//   }
//     std::cout << "******************" <<std::endl;
// }

void MacEnqueueTrace1(std::string context, Ptr<const WifiMacQueueItem> item) {
  Ptr<const Packet> pkt = item->GetPacket();
  uint32_t sz = pkt->GetSize();
  if(sz >= 200){
    Time now = Simulator::Now();
    double t = now.GetNanoSeconds();
    cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
  }
}


void MacDequeueTrace1(std::string context, Ptr<const WifiMacQueueItem> item) {
  Ptr<const Packet> pkt = item->GetPacket();
  uint32_t sz = pkt->GetSize();
  if(sz >= 200){
    Time now = Simulator::Now();
    double t = now.GetNanoSeconds();
    cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
  }
}

void EnqueueTrace1(std::string context, Ptr<const Packet> pkt) {
  uint32_t sz = pkt->GetSize();
  if(sz >= 200){
    Time now = Simulator::Now();
    double t = now.GetNanoSeconds();
    cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
  }
}

void DequeueTrace1(std::string context, Ptr<const Packet> pkt) {
  uint32_t sz = pkt->GetSize();
  if(sz >= 200){
    Time now = Simulator::Now();
    double t = now.GetNanoSeconds();
    cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
  }
}

void PhyTxBeginTrace1(std::string context, Ptr<const Packet> pkt, double pow) {
  uint32_t sz = pkt->GetSize();
  if(sz >= 200){
    Time now = Simulator::Now();
    double t = now.GetNanoSeconds();
    cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
  }
}

void PhyTxEndTrace1(std::string context, Ptr<const Packet> pkt) {
  Time now = Simulator::Now();
  double t = now.GetNanoSeconds();
  cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
}

void PhyRxBeginTrace1 (std::string context, Ptr<const Packet> pkt, RxPowerWattPerChannelBand rpw) {
  uint32_t sz = pkt->GetSize();
  if(sz >= 200){
    Time now = Simulator::Now();
    double t = now.GetNanoSeconds();
    cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
  }
}

void PhyRxEndTrace1 (std::string context, Ptr<const Packet> pkt) {
  uint32_t sz = pkt->GetSize();
  if(sz >= 200){
    Time now = Simulator::Now();
    double t = now.GetNanoSeconds();
    cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
  }
}

void MacTxTrace1(std::string context, Ptr<const Packet> pkt) {
  uint32_t sz = pkt->GetSize();
  if(sz >= 200){
    Time now = Simulator::Now();
    double t = now.GetNanoSeconds();
    cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
  }
}

void MacRxTrace1(std::string context, Ptr<const Packet> pkt) {
 uint32_t sz = pkt->GetSize();
  if(sz >= 200){
    Time now = Simulator::Now();
    double t = now.GetNanoSeconds();
    cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
  }
}


void PhyTxDropTrace1(std::string context, Ptr<const Packet> pkt) {
uint32_t sz = pkt->GetSize();
  if(sz >= 200){
    Time now = Simulator::Now();
    double t = now.GetNanoSeconds();
    cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
  }
}

void PhyRxDropTrace1(std::string context, Ptr<const Packet> pkt, WifiPhyRxfailureReason wfr) {
  uint32_t sz = pkt->GetSize();
  if(sz >= 200){
    Time now = Simulator::Now();
    double t = now.GetNanoSeconds();
    cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
  }
}

void MacExpiredTrace1(std::string context,  Ptr<const WifiMacQueueItem> item) {
  Ptr<const Packet> pkt = item->GetPacket();
  uint32_t sz = pkt->GetSize();
  if(sz >= 200){
    Time now = Simulator::Now();
    double t = now.GetNanoSeconds();
    cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
  }
}

void MacDropTrace1(std::string context, Ptr<const WifiMacQueueItem> item) {
  Ptr<const Packet> pkt = item->GetPacket();
   uint32_t sz = pkt->GetSize();
  if(sz >= 200){
    Time now = Simulator::Now();
    double t = now.GetNanoSeconds();
    cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
  }
}

void MacTxDropTrace1(std::string context, Ptr<const Packet> pkt) {
   uint32_t sz = pkt->GetSize();
  if(sz >= 200){
    Time now = Simulator::Now();
    double t = now.GetNanoSeconds();
    cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
  }
}

void MacRxDropTrace1(std::string context, Ptr<const Packet> pkt) {
 uint32_t sz = pkt->GetSize();
  if(sz >= 200){
    Time now = Simulator::Now();
    double t = now.GetNanoSeconds();
    cout << setprecision(15) << pkt->GetUid() << " " << context << " " << t << endl;
  }
}

vector<Vector3D> getPV(int n, string name)  {
  string fileName = getCustomFileName(__FILE__, name);
  FILE* fp = freopen(fileName.c_str(), "r", stdin);
  vector<Vector3D> pv(n);
  for(int i(0);i<n;i++) {
    cin>>pv[i].x>>pv[i].y>>pv[i].z;
  }
  fclose(fp);
  return pv;
}

vector<double> getStartTimes(int n, string name){
  string fileName = getCustomFileName(__FILE__, name);
  FILE* fp = freopen(fileName.c_str(), "r", stdin);
  vector<double> pv(n);
  for(int i(0);i<n;i++) {
    cin>>pv[i];
  }
  fclose(fp);
  return pv;
}

void ConnectTraceMACQueues(NodeContainer &nodes) {
  for(uint32_t i=0; i<nodes.GetN(); i++){
    Ptr<Node> node = nodes.Get(i);
    Ptr<WaveNetDevice> waveNetDevice = DynamicCast<WaveNetDevice> (node->GetDevice(0));
    Ptr<OcbWifiMac> waveMac = DynamicCast<OcbWifiMac> (waveNetDevice->GetMac(178));
    string context = "/NodeList/" + to_string(i) + "/DeviceList/0/$ns3::WifiNetDevice/Mac/Txop/Queue";
    Ptr<WifiMacQueue> vo = waveMac->GetTxopQueue(AcIndex::AC_VO);
    vo->SetMaxSize(QueueSize("1000000000p"));
    vo->SetMaxDelay(Time(MilliSeconds(500000)));
    vo->TraceConnect("Enqueue", (context + "/Enqueue"), MakeCallback(&MacEnqueueTrace1));
    vo->TraceConnect("Dequeue", (context + "/Dequeue"), MakeCallback(&MacDequeueTrace1));
    vo->TraceConnect("Expired", (context + "/Expired"), MakeCallback(&MacExpiredTrace1));
    vo->TraceConnect("Drop", (context + "/Drop"), MakeCallback(&MacDropTrace1));
  }
}

int main (int argc, char *argv[])
{
  CommandLine cmd;

  //Number of nodes
  uint32_t nNodes = 1;
  double simTime = 5; //4 seconds
  double interval = 0.002;
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
  mobility.SetMobilityModel ("ns3::CustomMobilityModel");
  mobility.Install(nodes);

  vector<Vector3D> positions = getPV(nodes.GetN(), "inputs/positions-" + to_string(nNodes) + ".txt");
  // vector<Vector3D> velocities = getPV(nodes.GetN(), "inputs/velocities.txt");
  vector<double> startTimes = getStartTimes(nodes.GetN(), "inputs/startTimes-" + to_string(nNodes) + ".txt");

  for (uint32_t i=0 ; i<nodes.GetN(); i++)
  {
    //set initial positions, and velocities
    NS_LOG_LOGIC ("Setting up mobility for node " << i);
    NS_LOG_ERROR ("An error happened :(");
    Ptr<CustomMobilityModel> cmm = DynamicCast<CustomMobilityModel> (nodes.Get(i)->GetObject<MobilityModel>());
    cmm->SetPosition (positions[i]);
    // set velocity as 0
    cmm->SetVelocityAndAcceleration (Vector3D(0, 0, 0), Vector3D(0, 0, 0));
  }

  WaveSetup wave;
  NetDeviceContainer devices = wave.ConfigureDevices(nodes, false);

  //Create Application in nodes

  //Method 1
  /*
  */
  for (uint32_t i=0; i<nodes.GetN(); i++)
  {
    Ptr<CustomApplication> app_i = CreateObject<CustomApplication>();
    app_i->SetBroadcastInterval (Seconds(interval));
    app_i->SetStartTime (Seconds (startTimes[i]));
    app_i->SetStopTime (Seconds (simTime));
    nodes.Get(i)->AddApplication (app_i);
  }

  string fileN = "outputs/wave-project-n" + to_string(nNodes);

  string fileName = getCustomFileName (__FILE__, fileN + ".log");
  FILE* fp = freopen(fileName.c_str (), "w", stdout);

  //Method 2 using ObjcetFactor.
  // for (uint32_t i=0 ; i<nodes.GetN() ; i++)
  // {
  //   ObjectFactory fact;
  //   fact.SetTypeId ("ns3::CustomApplication");
  //   fact.Set ("Interval", TimeValue (Seconds(interval)));
  //   Ptr<CustomApplication> appI = fact.Create <CustomApplication> ();
  //   appI->SetBroadcastInterval(MilliSeconds(2));
  //   appI->SetStartTime(Seconds(0));
  //   appI->SetStopTime (Seconds (simTime));
  //   nodes.Get(i)->AddApplication(appI);
  // }
  ConnectTraceMACQueues(nodes);
  // // Simulator::Schedule (Seconds (30), &SomeEvent);
  // // Config::Connect("/NodeList/*/DeviceList/*/$ns3::WaveNetDevice", MakeCallback(&MacDequeueTrace));
  // Config::Connect("/NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyTxBegin", MakeCallback(&PhyTxBeginTrace1));
  // Config::Connect("/NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyTxEnd", MakeCallback(&PhyTxEndTrace1));
  // Config::Connect("/NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyRxBegin", MakeCallback(&PhyRxBeginTrace1));
  // Config::Connect("/NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyRxEnd", MakeCallback(&PhyRxEndTrace1));
  // Config::Connect("/NodeList/*/DeviceList/*/$ns3::WaveNetDevice/MacEntities/*/MacTx", MakeCallback(&MacTxTrace1));
  // Config::Connect("/NodeList/*/DeviceList/*/$ns3::WaveNetDevice/MacEntities/*/MacRx", MakeCallback(&MacRxTrace1));
  // Config::Connect("/NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/0/PhyRxDrop", MakeCallback(&PhyRxDropTrace));

  Simulator::Stop(Seconds(simTime+1));
  Simulator::Run();
  // std::cout << "Post Simulation: " << std::endl;

  // for (uint32_t i=0 ; i<nodes.GetN(); i++)
  // {
  //   Ptr<CustomApplication> appI = DynamicCast<CustomApplication> (nodes.Get(i)->GetApplication(0));
  //   appI->PrintNeighbors ();
  // }
  Simulator::Destroy();
  fclose(fp);
}
