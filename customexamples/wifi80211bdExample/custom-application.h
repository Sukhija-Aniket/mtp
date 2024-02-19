#ifndef CUSTOM_APPLICATION_H
#define CUSTOM_APPLICATION_H

#include "ns3/application.h"
#include "ns3/address.h"
#include "ns3/wave-net-device.h"
#include "ns3/wifi-phy.h"
#include "ns3/mobility-model.h"
#include "custom-data-tag.h"
#include "ns3/random-variable-stream.h"
#include <vector>

namespace ns3 {

    class Address;

    /** 
     * \brief A struct to represent information about this node's neighbors. I chose MAC address and the time last message was received form that node
     * The time 'last_beacon' is used to determine whether we should remove the neighbor from the list.
     */

    typedef struct {
        Mac48Address neighbor_mac;
        Time last_beacon;
    } NeighborInformation;

class CustomApplication : public Application
{
public:

    static TypeId GetTypeId (void);
    virtual TypeId GetInstanceTypeId (void) const;

    CustomApplication();
    virtual ~CustomApplication();

    /** \brief Broadcast some information
     */
    void BroadcastInformation();
    void BroadcastInformationWithParameters(std::vector<uint32_t> &data, uint32_t index);

    /** \brief This function is called when a net device receives a packet.
     * I connect to the callback in StartApplication. This matches the signiture of NetDevice receive.
     */
    bool ReceivePacket (Ptr<NetDevice> device,Ptr<const Packet> packet,uint16_t protocol, const Address &sender);

    void PromiscRx (Ptr<const Packet> packet, uint16_t channelFreq, WifiTxVector tx, MpduInfo mpdu, SignalNoiseDbm sn, uint16_t sta_id);

    void SetBroadcastInterval (Time interval);

    void SetPacketSize (uint32_t packetSize);

    /** \brief Update a neighbor's last contact time, or add a new neighbor
     */
    void UpdateNeighbor (Mac48Address addr);
    /** \brief Print a list of neighbors
     */
    void PrintNeighbors (void);

    /** \brief Change the data rate used for broadcasts.
     */
    void SetWifiMode (WifiMode mode);

    /** \brief Remove neighbors you haven't heard from after some time.
     */
    void RemoveOldNeighbors (void);

    /** \brief Enables data to be used.
     */
    void SetData (std::vector<uint32_t> &data);

    // (Aniket Sukhija)
    /** \brief Get the retransmission probability for 802.11bd
     */
    double GetRetransmissionProb80211bd();
    
    /** \brief Set the retransmission probability for 802.11bd
     */
    void SetRetransmissionProb80211bd(double p);
    
    uint32_t GetMaxRetransmissionLimit();

    void SetMaxRetransmissionLimit(uint32_t maxLimit);

    void DoRetransmissionbd(uint32_t &totalPktSize, uint32_t maxLimit);

    void Retransmissionbd(uint32_t &totalPktSize);

    // (Aniket Sukhija)

    //You can create more functions like getters, setters, and others

    private:
        /** \brief This is an inherited function. Code that executes once the application starts
         */
        void Configure(void);
        void StartApplication(void);
        void StartApplicationWithData(void);

        bool m_enabled; /* Checks whether to use m_data or not */
        std::vector<uint32_t> m_data; /** Stores data of various priorities to be used.*/
        Time m_broadcast_time; /**< How often do you broadcast messages */
        uint32_t m_packetSize; /**< Packet size in bytes */
        Ptr<WaveNetDevice> m_waveDevice; /**< A WaveNetDevice that is attached to this device */

        std::vector <NeighborInformation> m_neighbors; /**< A table representing neighbors of this node */

        Time m_time_limit; /**< Time limit to keep neighbors in a list */

        WifiMode m_mode; /**< data rate used for broadcasts */

        //You can define more stuff to record statistics, etc.
        double m_retransmissionProb80211bd; // (Aniket Sukhija)
        int m_maxRetranssionLimit; // (Aniket Sukhija)
        Ptr<UniformRandomVariable> m_random; //!< Provides uniform random variables. (Aniket Sukhija)
};

} // namespace ns3

#endif /* CUSTOM_APPLICATION_H */
