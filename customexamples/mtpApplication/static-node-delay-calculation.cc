#include "ns3/wave-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/core-module.h"
#include "custom-application.h"
#include "custom-mobility-model.h"
#include "iomanip"
#include "ns3/trace-functions.h"
// #include "wave-setup.h"
#include <random>

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("CustomApplicationExample");

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
  // FILE* fp = freopen(fileName.c_str(), "r", stdin);
  vector<double> startTimes(n);
  if(freopen(fileName.c_str(), "r", stdin)){
    for(int i(0);i<n;i++) {
      string x;
      cin>>x;
      cout << x << " " << i << endl;
    }
  }
  // fclose(fp);
  return startTimes;
}

NetDeviceContainer ConfigureDevices(NodeContainer &nodes) {
  /*
    Setting up WAVE devices. With PHY & MAC using default settings. 
  */
  YansWifiChannelHelper waveChannel = YansWifiChannelHelper();
  waveChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  waveChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue(5.9e9));
  Ptr<YansWifiChannel> channel = waveChannel.Create();
  YansWavePhyHelper wavePhy =  YansWavePhyHelper::Default ();
  wavePhy.SetChannel (channel);
  wavePhy.Set("Frequency", UintegerValue(5900));

  wavePhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
  //Setup up MAC
  QosWaveMacHelper waveMac = QosWaveMacHelper::Default ();
  WaveHelper waveHelper = WaveHelper::Default ();

  waveHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
  						"DataMode", StringValue ("OfdmRate3MbpsBW10MHz"	),
  						"ControlMode",StringValue ("OfdmRate1MbpsBW10MHz"),
  						"NonUnicastMode", StringValue ("OfdmRate6MbpsBW10MHz"),
              "MaxSlrc", UintegerValue(2),
              "MaxSsrc", UintegerValue(2));
  // );

  NetDeviceContainer devices = waveHelper.Install (wavePhy, waveMac, nodes);

  for(uint32_t iNode=0;iNode<devices.GetN();iNode++){
    Ptr<WaveNetDevice> node = DynamicCast<WaveNetDevice> (devices.Get(iNode));
    // Change AIFs[1] = 3; AIFs[0] = 2; cw_min = 15; cw_max = 31
    Ptr<OcbWifiMac> nodeMac = node->GetMac(178);
    Ptr<QosTxop> viTxop = nodeMac->GetQosTxop(AC_VI);
    Ptr<QosTxop> voTxop = nodeMac->GetQosTxop(AC_VO);
    // viTxop->TraceConnect("BackoffTrace", "BackOffTrace", ns3::MakeBoundCallback(&MacTxBackOffTrace, objContainers[MACTXBACKOFFNUM]));
    // voTxop->TraceConnect("BackoffTrace", "BackOffTrace", MakeBoundCallback(&MacTxBackOffTrace, objContainers[MACTXBACKOFFNUM]));
    viTxop->SetMinCw(15);
    viTxop->SetMaxCw(31);
    voTxop->SetMinCw(7);
    voTxop->SetMinCw(15);
    viTxop->SetAifsn(3);
    voTxop->SetAifsn(2);

    Ptr<WifiPhy> nodePhy = node->GetPhy(0);
    // Time sifs = Time::FromInteger(32, Time::US);
    // nodePhy->SetSifs(sifs);
    // Time slot = Time::FromInteger(13, Time::US);
    // nodePhy->SetSlot(slot);

    // Set Tx Power to 500m
    nodePhy->SetTxPowerStart(19.9);
    nodePhy->SetTxPowerEnd(19.9);
    // Set CSR to 700m
    nodePhy->SetCcaEdThreshold(-84.87);
    // Set RX Sensitivity
    nodePhy->SetRxSensitivity(-84.87);
  }

  return devices;
}


vector<uint32_t> getGenRates(int n, string name) {
  string fileName = getCustomFileName(__FILE__, name);
  FILE* fp = freopen(fileName.c_str(), "r", stdin);
  vector<uint32_t> genRates(n);
  for(int i(0);i<n;i++) {
    cin>>genRates[i];
  }

  fclose(fp);
  return genRates;
}

void ConfigureQueue(Ptr<WifiMacQueue> q, string context, vector<vector<DisplayObject>*> &objCont) {
    q->SetMaxSize(QueueSize("1000000000p"));
    q->SetMaxDelay(Time(MilliSeconds(500000)));
    q->TraceConnect("Enqueue", (context + "/Enqueue"), MakeBoundCallback(&MacEnqueueTrace, objCont[MACENQUEUENUM]));
    q->TraceConnect("Dequeue", (context + "/Dequeue"), MakeBoundCallback(&MacDequeueTrace, objCont[MACDEQUEUENUM]));
    q->TraceConnect("Expired", (context + "/Expired"), MakeBoundCallback(&MacExpiredTrace, objCont[MACEXPIREDNUM]));
    q->TraceConnect("Drop", (context + "/Drop"), MakeBoundCallback(&MacDropTrace, objCont[MACDROPNUM]));
}

void ConnectTraceMACQueues(NodeContainer &nodes, vector<vector<DisplayObject>*> &objCont) {
  for(uint32_t i=0; i<nodes.GetN(); i++){
    Ptr<Node> node = nodes.Get(i);
    Ptr<WaveNetDevice> waveNetDevice = DynamicCast<WaveNetDevice> (node->GetDevice(0));
    Ptr<OcbWifiMac> waveMac = DynamicCast<OcbWifiMac> (waveNetDevice->GetMac(178));

    string context = "/NodeList/" + to_string(i) + "/DeviceList/0/$ns3::WifiNetDevice/Mac/Txop/VOQueue";
    Ptr<WifiMacQueue> q = waveMac->GetTxopQueue(AcIndex::AC_VO);
    ConfigureQueue(q, context, objCont);
    context = "/NodeList/" + to_string(i) + "/DeviceList/0/$ns3::WifiNetDevice/Mac/Txop/VIQueue";
    q = waveMac->GetTxopQueue(AcIndex::AC_VI);
    ConfigureQueue(q, context, objCont);
  }
}

/* Method 1*/
vector<uint32_t> generateData(uint32_t prioRate, uint32_t genRate)
{
  vector<uint32_t> data(genRate, 5);
  vector<bool> marked(genRate, false);
  default_random_engine generator;
  uniform_int_distribution<int> distribution(0, genRate-1);
  uint32_t cnt = 0;
  while(cnt < prioRate) {
    int num = distribution(generator);
    if (marked[num]) continue;
    data[num] = 7;
    marked[num] = true;
    cnt++;
  }
  return data;
}

/* Method 2 */
vector<uint32_t> generateData2(uint32_t prioRate, uint32_t genRate)
{
  int genv = 5;
  int priov = 7;
  if (genRate < prioRate) {
    swap(genRate, prioRate);
    swap(genv, priov);
  }
  vector<bool> marked(genRate, false);
  vector<uint32_t> prioNums;
  default_random_engine generator;
  uniform_int_distribution<uint32_t> distribution(0, genRate-1);
  uint32_t cnt = 0;
  while(cnt < prioRate)
  {
    uint32_t num = distribution(generator);
    if (marked[num]) continue;
    prioNums.push_back(num);
    cnt++;
  }
  sort(prioNums.begin(), prioNums.end());
  vector<uint32_t> data(genRate + prioRate);
  uint32_t idx = 0;
  while(cnt < genRate)
  {
    if ((idx < prioRate) && (prioNums[idx] <= cnt))
    {
      data[idx+cnt] = priov;
      idx++;
      data[idx+cnt] = genv;
      cnt++;
    }
    else
    {
      data[idx+cnt] = genv;
      cnt++;
    }
  }
  return data;
}

int main (int argc, char *argv[])
{
  CommandLine cmd;
  //Number of nodes
  uint32_t nNodes = 2;
  double simTime = 1;
  int distance = 2000;
  cmd.AddValue ("t","Simulation Time", simTime);
  cmd.AddValue ("n", "Number of nodes", nNodes);
  cmd.AddValue("d", "total Distance", distance);
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
  vector<Vector3D> positions = getPV(nodes.GetN(), "inputs/positions-" + to_string(nNodes) + '-' + to_string(distance) + ".txt");
  vector<Vector3D> velocities = getPV(nodes.GetN(), "inputs/velocities-" + to_string(nNodes) + '-' + to_string(distance) + ".txt");
  vector<double> startTimes = {0.02, 0.09};
  vector<uint32_t> packetGenRates = {30, 30};
  vector<uint32_t> prioPacketGenRates = {60, 60};

  for (uint32_t i=0 ; i<nodes.GetN(); i++)
  {
    cout << setprecision(15) << positions[i].x << " " << velocities[i].x << " " << startTimes[i] << " " << packetGenRates[i] << " " << prioPacketGenRates[i] << endl;
    //set initial positions, and velocities
    NS_LOG_LOGIC ("Setting up mobility for node " << i);
    NS_LOG_ERROR ("An error happened :(");
    Ptr<CustomMobilityModel> cmm = DynamicCast<CustomMobilityModel> (nodes.Get(i)->GetObject<MobilityModel>());
    cmm->SetPosition (positions[i]);
    // set velocity as 0
    cmm->SetVelocityAndAcceleration (Vector3D(0, 0, 0), Vector3D(0, 0, 0));
  }

  // Wifi Phy and Mac Layer
  // WaveSetup wave;

  NetDeviceContainer devices = ConfigureDevices(nodes);

  //Create Application in nodes

  //Method 1
  /*
  */
  for (uint32_t i=0; i<nodes.GetN(); i++)
  {
    Ptr<CustomApplication> app_i = CreateObject<CustomApplication>();
    double interval = 1.0/(prioPacketGenRates[i] + packetGenRates[i]);
    app_i->SetBroadcastInterval (Seconds(interval));
    app_i->SetStartTime (Seconds (startTimes[i]));
    app_i->SetStopTime (Seconds (simTime));
    vector<uint32_t> data = generateData2(prioPacketGenRates[i], packetGenRates[i]);
    app_i->SetData(data);
    nodes.Get(i)->AddApplication (app_i);
  }

  // ConnectTraceMACQueues(nodes, objContainers);
  
  // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/MacEntities/*/MacTx", MakeBoundCallback(&MacTxTrace, objContainers[MACTXNUM]));
  // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/MacEntities/*/MacTxDrop", MakeBoundCallback(&MacTxDropTrace, objContainers[MACTXDROPNUM]));
  // // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyTxBegin", MakeBoundCallback(&PhyTxBeginTrace, objContainers[PHYTXBEGINNUM]));
  // // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyTxEnd", MakeBoundCallback(&PhyTxEndTrace,objContainers[PHYTXENDNUM]));
  // // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyTxDrop", MakeBoundCallback(&PhyTxDropTrace, objContainers[PHYTXDROPNUM]));
  // // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyRxBegin", MakeBoundCallback(&PhyRxBeginTrace,objContainers[PHYRXBEGINNUM]));
  // // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyRxEnd", MakeBoundCallback(&PhyRxEndTrace,objContainers[PHYRXENDNUM]));
  // // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyRxDrop", MakeBoundCallback(&PhyRxDropTrace, objContainers[PHYRXDROPNUM]));
  // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/MacEntities/*/MacRx", MakeBoundCallback(&MacRxTrace, objContainers[MACRXNUM]));
  // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/MacEntities/*/MacRxDrop", MakeBoundCallback(&MacRxDropTrace, objContainers[MACRXDROPNUM]));
  // string fileN = "outputs/wave-project-n" + to_string(nNodes) + "-d" + to_string(distance);

  // Simulator::Stop(Seconds(simTime+1));
  // Simulator::Run();
  // Simulator::Destroy();

  // int sz = objContainers[MACTXBACKOFFNUM]->size();
  // int sz2 = objContainers[MACENQUEUENUM]->size();
  // cout<<"this is size: "<<sz <<" "<<sz2<<endl;
  // string fileName = getCustomFileName (__FILE__, fileN + ".log");
  // FILE* fp = freopen(fileName.c_str (), "w", stdout);
  // getObjTrace(objContainers, MACENQUEUENUM, fp);

  // fclose(fp);
}
