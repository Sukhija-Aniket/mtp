#include "ns3/wave-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/core-module.h"
#include "custom-application.h"
#include "custom-mobility-model.h"
#include "ns3/trace-functions.h"
#include "ns3/functions.h"
#include <sys/stat.h>
#include <sys/types.h>

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("CustomApplicationExamplebd");


map<double, string> OfdmDataRate10MHzMap = {
    {1.0, "OfdmRate1MbpsBW10MHz"},
    {3.0, "OfdmRate3MbpsBW10MHz"},
    {4.5, "OfdmRate4_5MbpsBW10MHz"},
    {6.0, "OfdmRate6MbpsBW10MHz"},
    {9.0, "OfdmRate9MbpsBW10MHz"},
    {12.0, "OfdmRate12MbpsBW10MHz"},
    {18.0, "OfdmRate18MbpsBW10MHz"},
    {24.0, "OfdmRate24MbpsBW10MHz"},
    {27.0, "OfdmRate27MbpsBW10MHz"}
};

NetDeviceContainer ConfigureDevices(NodeContainer &nodes, vector<vector<DisplayObject>*> objContainers, bool enablePcap, uint32_t packetSize, double dataRate,  WifiStandard wifiStandard, string file) {
  const uint32_t mac_header_size = 26;
  /*
    Setting up WAVE devices. With PHY & MAC using default settings.
  */
  YansWifiChannelHelper waveChannel = YansWifiChannelHelper();
  waveChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  waveChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue(5.9e9));
  YansWavePhyHelper wavePhy =  YansWavePhyHelper::Default ();
  Ptr<YansWifiChannel> channel = waveChannel.Create(); // A channel with propagation delay and loss model set
  wavePhy.SetChannel (channel);

  wavePhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
  //Setup up MAC
  QosWaveMacHelper waveMac = QosWaveMacHelper::Default ();
  WaveHelper waveHelper = WaveHelper::Default ();

  waveHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
  						"DataMode", StringValue (OfdmDataRate10MHzMap[dataRate]), // Set rate as 27 Mbps
  						"ControlMode",StringValue ("OfdmRate1MbpsBW10MHz"), // Not being used
  						"NonUnicastMode", StringValue (OfdmDataRate10MHzMap[dataRate]), // Set Broadcast rate as 27Mbps
              "MaxSlrc", UintegerValue(2),
              "MaxSsrc", UintegerValue(2),
              "FragmentationThreshold", UintegerValue(packetSize + mac_header_size + WIFI_MAC_FCS_LENGTH)); // (Aniket Sukhija) Added to decrease limit on fragmentation

  NetDeviceContainer devices = waveHelper.Install (wavePhy, waveMac, nodes, wifiStandard);

  for(uint32_t iNode=0;iNode<devices.GetN();iNode++){
    Ptr<WaveNetDevice> node = DynamicCast<WaveNetDevice> (devices.Get(iNode));
    // Note: Change AIFs[1] = 3; AIFs[0] = 2; cw_min = 15; cw_max = 31
    Ptr<OcbWifiMac> nodeMac = node->GetMac(178);
    Ptr<QosTxop> viTxop = nodeMac->GetQosTxop(AC_VI);
    Ptr<QosTxop> voTxop = nodeMac->GetQosTxop(AC_VO);

    // viTxop->TraceConnect("BackoffTrace", "VIBackOffTrace/" + to_string(iNode), ns3::MakeBoundCallback(&MacTxBackOffTrace, objContainers[VIMACTXBACKOFFNUM]));
    // voTxop->TraceConnect("BackoffTrace", "VOBackOffTrace/" + to_string(iNode), ns3::MakeBoundCallback(&MacTxBackOffTrace, objContainers[VOMACTXBACKOFFNUM]));
    // viTxop->TraceConnect("CwTrace", "VICwTrace/" +to_string(iNode), ns3::MakeBoundCallback(&MacTxCwTrace, objContainers[VIMACTXCWNUM]));
    // voTxop->TraceConnect("CwTrace", "VOCwTrace/" +to_string(iNode), ns3::MakeBoundCallback(&MacTxCwTrace, objContainers[VOMACTXCWNUM]));

    viTxop->SetMinCw(15);
    viTxop->SetMaxCw(31);

    voTxop->SetMinCw(7);
    voTxop->SetMaxCw(15);

    Ptr<WifiPhy> nodePhy = node->GetPhy(0);
    // (Aniket Sukhija)
    // WifiPhyOperatingChannel opChannel = nodePhy->GetOperatingChannel();

    // uint16_t number = opChannel.GetNumber();
    // uint16_t channel = opChannel.GetFrequency();
    // uint16_t cw = opChannel.GetWidth();
    // uint16_t band = nodePhy->GetPhyBand();
    // NS_LOG_UNCOND("number: "<<number<<", channel: "<<channel<<", cw: "<<cw<<", band: "<<band);
    // (Aniket Sukhija)

    // Set Tx Power to 500m
    nodePhy->SetTxPowerStart(19.9);
    nodePhy->SetTxPowerEnd(19.9);
    // Set CSR to 700m
    nodePhy->SetCcaEdThreshold(-84.87);
    // Set RX Sensitivity
    nodePhy->SetRxSensitivity(-84.87);
  }

  if(enablePcap) {
    uint32_t numNodes = devices.GetN();
    string name = "pcaps/" + file + "-n" + std::to_string(numNodes);
    string fileName = getCustomFileName(__FILE__, name);
    wavePhy.EnablePcap(fileName, devices);
  }
  return devices;
}

void ConfigureQueue(Ptr<WifiMacQueue> q, string context, vector<vector<DisplayObject>*> &objCont) {
    q->TraceConnect("Enqueue", (context + "/Enqueue"), MakeBoundCallback(&MacEnqueueTrace, objCont[MACENQUEUENUM]));
    q->TraceConnect("Dequeue", (context + "/Dequeue"), MakeBoundCallback(&MacDequeueTrace, objCont[MACDEQUEUENUM]));
    // q->TraceConnect("Expired", (context + "/Expired"), MakeBoundCallback(&MacExpiredTrace, objCont[MACEXPIREDNUM]));
    // q->TraceConnect("Drop", (context + "/Drop"), MakeBoundCallback(&MacDropTrace, objCont[MACDROPNUM]));
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



int main (int argc, char *argv[])
{
  CommandLine cmd;
  vector<vector<DisplayObject>*> objContainers = CreateObjContainer();

  uint32_t nNodes = 10;
  double simTime = 1;
  int distance = 100;
  uint32_t packetSize = 500;
  uint32_t prioPacketGenRate = 30, packetGenRate = 30;

  double datarate = 27;
  string folder = "";
  string baseName = getBaseName(__FILE__);

  bool enablePcap = false;
  WifiStandard wifiStandard = WifiStandard::WIFI_STANDARD_80211p;

  cmd.AddValue ("time","Simulation Time", simTime);
  cmd.AddValue ("num_nodes", "Number of nodes", nNodes);
  cmd.AddValue("distance", "total Distance", distance);
  cmd.AddValue("packet_size", "packet Size", packetSize);
  cmd.AddValue("pcap", "Enable Pcap", enablePcap);
  cmd.AddValue("data_rate", "Data Rate", datarate);
  cmd.AddValue("critical_rate", "critical packet generation rate", prioPacketGenRate);
  cmd.AddValue("general_rate", "general packet generation rate", packetGenRate);
  cmd.AddValue("folder", "Sub Folder", folder);
  cmd.AddValue("file", "file", baseName);
  cmd.Parse (argc, argv);

  NodeContainer nodes;
  nodes.Create(nNodes);

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::CustomMobilityModel");
  mobility.Install(nodes);

  // Calculate topologies from the input file (generated by python code)
  vector<Vector3D> positions = getPV(nodes.GetN(), __FILE__, "inputs/positions-" + to_string(nNodes) + '-' + to_string(distance) + ".txt");
  vector<Vector3D> velocities = getPV(nodes.GetN(), __FILE__, "inputs/velocities-" + to_string(nNodes) + '-' + to_string(distance) + ".txt");
  vector<double> startTimes = getStartTimes(nodes.GetN(), __FILE__, "inputs/startTimes-" + to_string(nNodes) + '-' + to_string(distance) + ".txt");
  vector<uint32_t> packetGenRates = vector<uint32_t>(nodes.GetN(),packetGenRate);
  vector<uint32_t> prioPacketGenRates =  vector<uint32_t>(nodes.GetN(),prioPacketGenRate);
  if (folder == "") {
    packetGenRates = getGenRates(nodes.GetN(), __FILE__, "inputs/packetGenRates-" + to_string(nNodes) + '-' + to_string(distance) + ".txt");
    prioPacketGenRates = getGenRates(nodes.GetN(), __FILE__, "inputs/prioPacketGenRates-" + to_string(nNodes) + '-' + to_string(distance) + ".txt");
  }


  // Set dynamics of the node
  for (uint32_t i=0 ; i<nodes.GetN(); i++)
  {
    //set initial positions, and velocities
    Ptr<CustomMobilityModel> cmm = DynamicCast<CustomMobilityModel> (nodes.Get(i)->GetObject<MobilityModel>());
    cmm->SetPosition (positions[i]);
    cmm->SetVelocityAndAcceleration (Vector3D(0, 0, 0), Vector3D(0, 0, 0)); // setting velocity as 0
  }


  // Wifi Phy and Mac Layer
  NetDeviceContainer devices = ConfigureDevices(nodes, objContainers, enablePcap, packetSize, datarate, wifiStandard, baseName);

  //Create Application in nodes
  for (uint32_t i=0; i<nodes.GetN(); i++)
  {
    Ptr<CustomApplication> app_i = CreateObject<CustomApplication>();
    double interval = 1.0/(packetGenRates[i] + prioPacketGenRates[i]);
    app_i->SetBroadcastInterval (Seconds(interval));
    app_i->SetStartTime (Seconds (startTimes[i]));
    app_i->SetStopTime (Seconds (simTime));
    app_i->SetPacketSize(packetSize);
    app_i->SetWifiMode (OfdmDataRate10MHzMap[datarate]);
    app_i->SetRetransmissionProb80211bd(0.72);
    app_i->SetMaxRetransmissionLimit(3);
    vector<uint32_t> data = generateData2(prioPacketGenRates[i], packetGenRates[i]);
    app_i->SetData(data);
    nodes.Get(i)->AddApplication (app_i);
  }

  ConnectTraceMACQueues(nodes, objContainers);

  // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/MacEntities/*/MacTx", ns3::MakeBoundCallback(&MacTxTrace, objContainers[MACTXNUM]));
  // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/MacEntities/*/MacTxDrop", ns3::MakeBoundCallback(&MacTxDropTrace, objContainers[MACTXDROPNUM]));
  // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyTxBegin", MakeBoundCallback(&PhyTxBeginTrace, objContainers[PHYTXBEGINNUM]));
  // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyTxEnd", MakeBoundCallback(&PhyTxEndTrace,objContainers[PHYTXENDNUM]));
  // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyTxDrop", MakeBoundCallback(&PhyTxDropTrace, objContainers[PHYTXDROPNUM]));
  // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyRxBegin", MakeBoundCallback(&PhyRxBeginTrace,objContainers[PHYRXBEGINNUM]));
  // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyRxEnd", MakeBoundCallback(&PhyRxEndTrace,objContainers[PHYRXENDNUM]));
  // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/PhyEntities/*/PhyRxDrop", MakeBoundCallback(&PhyRxDropTrace, objContainers[PHYRXDROPNUM]));
  // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/MacEntities/*/MacRx", MakeBoundCallback(&MacRxTrace, objContainers[MACRXNUM]));
  // Config::Connect("NodeList/*/DeviceList/*/$ns3::WaveNetDevice/MacEntities/*/MacRxDrop", MakeBoundCallback(&MacRxDropTrace, objContainers[MACRXDROPNUM]));

  string fileName = "outputs/";
  if (folder=="") {
    fileName = fileName + baseName + "-n" + to_string(nNodes) + "-d" + to_string(distance);
  } else {
    // string folderName = getCustomFileName (__FILE__, fileN+folder);
    // mkdir(folderName.c_str(), 0777); # Shifted to python
    fileName = fileName + folder + "/" + baseName;
  }

  Simulator::Stop(Seconds(simTime+1));
  Simulator::Run();
  Simulator::Destroy();

  string logFileName = getCustomFileName (__FILE__, fileName + ".log");
  FILE* fp = freopen(logFileName.c_str (), "w", stdout);
  getObjTrace(objContainers, MACENQUEUENUM, fp);

  fclose(fp);
}
