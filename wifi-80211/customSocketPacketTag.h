#include "ns3/callback.h"
#include "ns3/ptr.h"
#include "ns3/tag.h"
#include "ns3/object.h"
#include "ns3/net-device.h"
#include "address.h"
#include <stdint.h>
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"

#ifndef CUSTOM_SOCKET_PRIORITY_TAG_H
#define 

namespace ns3 {

class SocketPriorityTag : public Tag
{
public:
  SocketPriorityTag ();

  /**
   * \brief Set the tag's priority
   *
   * \param priority the priority
   */
  void SetPriority (uint8_t priority);

  /**
   * \brief Get the tag's priority
   *
   * \returns the priority
   */
  uint8_t GetPriority (void) const;

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  // inherited function, no need to doc.
  virtual TypeId GetInstanceTypeId (void) const;

  // inherited function, no need to doc.
  virtual uint32_t GetSerializedSize (void) const;

  // inherited function, no need to doc.
  virtual void Serialize (TagBuffer i) const;

  // inherited function, no need to doc.
  virtual void Deserialize (TagBuffer i);

  // inherited function, no need to doc.
  virtual void Print (std::ostream &os) const;
private:
  uint8_t m_priority;  //!< the priority carried by the tag
};

}
