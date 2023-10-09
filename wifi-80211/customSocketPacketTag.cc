// #include "ns3/log.h"
// #include "ns3/packet.h"
// #include "node.h"
// #include "ns3/socket.h"
// #include "ns3/socket-factory.h"
// #include "customSocketPacketTag.h"
// #include <limits>


// namespace ns3 {

// CustomSocketPriorityTag::CustomSocketPriorityTag ()
// {
// }

// Ptr<SocketPriorityTag>
// CustomSocketPriorityTag::GetCustomPriorityTag (Ptr<const Packet> pkt) const
// {
//   PacketTagIterator i = pkt->GetPacketTagIterator ();
//   while (i.HasNext ()) {
//     PacketTagIterator::Item item = i.Next ();
//     std::string name  = item.GetTypeId ().GetName();

//     if (name == "ns3::CustomSocketPriorityTag") {
//       NS_ASSERT (item.GetTypeId ().HasConstructor ());
//       Callback<ObjectBase *> constructor = item.GetTypeId ().GetConstructor();
//       NS_ASSERT (!constructor.IsNull ());
//       ObjectBase *instance = constructor ();
//       SocketPriorityTag* tag = dynamic_cast<SocketPriorityTag *> (instance);
//       Ptr<SocketPriorityTag> tg = CreateObject<SocketPriorityTag>();
//       tg->SetPriority (tag->GetPriority ());
//       return tg;
//     }
//   }
//   return NULL;
// }

// void
// CustomSocketPriorityTag::SetPriority (uint8_t priority)
// {
//   m_priority = priority;
// }

// uint8_t
// CustomSocketPriorityTag::GetPriority (void) const
// {
//   return m_priority;
// }

// TypeId
// CustomSocketPriorityTag::GetTypeId (void)
// {
//   static TypeId tid = TypeId ("ns3::CustomSocketPriorityTag")
//     .SetParent<SocketPriorityTag> ()
//     .SetGroupName("Network")
//     .AddConstructor<CustomSocketPriorityTag> ()
//     ;
//   return tid;
// }

// TypeId
// CustomSocketPriorityTag::GetInstanceTypeId (void) const
// {
//   return GetTypeId ();
// }

// uint32_t
// CustomSocketPriorityTag::GetSerializedSize (void) const
// {
//   return sizeof (uint8_t);
// }

// void
// CustomSocketPriorityTag::Serialize (TagBuffer i) const
// {
//   i.WriteU8 (m_priority);
// }

// void
// CustomSocketPriorityTag::Deserialize (TagBuffer i)
// {
//   m_priority = i.ReadU8();
// }

// void
// CustomSocketPriorityTag::Print (std::ostream &os) const
// {
//   os << "SO_PRIORITY = " << m_priority;
// }

// }
