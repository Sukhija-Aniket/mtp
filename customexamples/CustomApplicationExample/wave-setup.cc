#include "wave-setup.h"

namespace ns3
{

WaveSetup::WaveSetup(){}
WaveSetup::~WaveSetup () {}

NetDeviceContainer WaveSetup::ConfigureDevices (NodeContainer& nodes, bool enablePcap)
{
  /*
    Setting up WAVE devices. With PHY & MAC using default settings. 
  */
  YansWifiChannelHelper waveChannel = YansWifiChannelHelper();
  waveChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  waveChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue(5.9e9));
  YansWavePhyHelper wavePhy =  YansWavePhyHelper::Default ();
  Ptr<YansWifiChannel> channel = waveChannel.Create();
  wavePhy.SetChannel (channel);
  uint32_t channelID = channel->GetId();
    wavePhy.Set("Frequency", UintegerValue(5900));
  std::string channelTuple = "{" + std::to_string(channelID) + ", " + std::to_string(10) + ", BAND_5GHZ, 0}";
  wavePhy.Set("ChannelSettings", StringValue(channelTuple));

  wavePhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
  //Setup up MAC
  QosWaveMacHelper waveMac = QosWaveMacHelper::Default ();
  WaveHelper waveHelper = WaveHelper::Default ();

  waveHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
  						"DataMode", StringValue ("OfdmRate3MbpsBW10MHz"	),
  						"ControlMode",StringValue ("OfdmRate3MbpsBW10MHz"),
  						"NonUnicastMode", StringValue ("OfdmRate6MbpsBW10MHz"));

  NetDeviceContainer devices = waveHelper.Install (wavePhy, waveMac, nodes);

  for(uint32_t iNode=0;iNode<devices.GetN();iNode++){
    Ptr<WaveNetDevice> node = DynamicCast<WaveNetDevice> (devices.Get(iNode));
    // Ptr<OcbWifiMac> nodeMac = node->GetMac(channelID);
    Ptr<WifiPhy> nodePhy = node->GetPhy(0);
    // Set Tx Power to 500m
    nodePhy->SetTxPowerStart(19.9);
    nodePhy->SetTxPowerEnd(19.9);
    // Set CSR to 700m
    nodePhy->SetCcaEdThreshold(-84.87);
    // Set RX Sensitivity
    nodePhy->SetRxSensitivity(-84.87);
  }

  if(enablePcap) {
    int numNodes = devices.GetN();
    wavePhy.EnablePcap("pcaps/static-node-delay-calc-n" + std::to_string(numNodes), devices);
  }
  //if you want PCAP trace, uncomment this!
  //wavePhy.EnablePcap ("custom-application", devices); //This generates *.pcap files

  return devices;
}

}