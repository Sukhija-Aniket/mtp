#ifndef WAVE_SETUP_H
#define WAVE_SETUP_H
#include "ns3/core-module.h"
#include "ns3/vector.h"
#include "ns3/string.h"
#include "ns3/double.h"
#include "ns3/config.h"
#include "ns3/log.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wave-module.h"
#include "ns3/yans-wifi-channel.h"
#include <iostream>
#include "iomanip"
#include "ns3/ocb-wifi-mac.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/wave-module.h"

namespace ns3
{
/** \brief This is a "utility class". It does not an extension to ns3. 
 */
  class WaveSetup
  {
    private:
      int numPacket = 1;
      int channelFrequency = 5900;
      uint16_t channelBandwidth = 10;
      std::string channelBand = "BAND_5GHZ";

    public:
      WaveSetup ();
      virtual ~WaveSetup ();

      NetDeviceContainer ConfigureDevices (NodeContainer &n);
  };
}

#endif 