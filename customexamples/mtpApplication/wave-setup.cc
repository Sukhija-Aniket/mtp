#include "wave-setup.h"

namespace ns3
{

WaveSetup::WaveSetup(){}
WaveSetup::~WaveSetup () {}

NetDeviceContainer WaveSetup::ConfigureDevices (NodeContainer& nodes)
{
  /*
    Setting up WAVE devices. With PHY & MAC using default settings.
  */



 // The below set of helpers will help us to put together the wifi NICs we want
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper();
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  // wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(500));
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue(5.9e9));

  YansWavePhyHelper wavePhy = YansWavePhyHelper::Default ();
  Ptr<YansWifiChannel> channel = wifiChannel.Create ();
  wavePhy.SetChannel (channel);
  uint32_t channelID = channel->GetId ();

  // Physical Layer parameters settings
  wavePhy.Set("Frequency", UintegerValue(channelFrequency));
  std::string channelTuple = "{" + std::to_string(channelID) + ", " + std::to_string(channelBandwidth) + ", " + channelBand + ", 0}";
  wavePhy.Set("ChannelSettings", StringValue(channelTuple));

  // ns-3 supports generate a pcap trace
  wavePhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
  QosWaveMacHelper waveMac = QosWaveMacHelper::Default ();

  Wifi80211pHelper waveHelper = Wifi80211pHelper::Default ();

  waveHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
  						"DataMode", StringValue ("OfdmRate3MbpsBW10MHz"	),
  						"ControlMode",StringValue ("OfdmRate1MbpsBW10MHz"), // Change to 1 Mbps
  						"NonUnicastMode", StringValue ("OfdmRate6MbpsBW10MHz"));
  NetDeviceContainer devices = waveHelper.Install (wavePhy, waveMac, nodes);

  for(uint32_t iNode=0;iNode<devices.GetN();iNode++){
    Ptr<WifiNetDevice> node = DynamicCast<WifiNetDevice> (devices.Get(iNode));
    Ptr<WifiPhy> nodePhy = node->GetPhy();
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

}
