#include "c2c-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

TypeId
c2cTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::c2cTag")
    .SetParent<Tag> ()
    .AddConstructor<c2cTag> ()
    ;
  return tid;
}

TypeId
c2cTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

c2cTag::c2cTag()
{
  isC2C = true;
}

uint32_t
c2cTag::GetSerializedSize (void) const
{
  return 2;
}

void
c2cTag::Serialize (TagBuffer i) const
{
  i.WriteU16 (isC2C);
}

void
c2cTag::Deserialize (TagBuffer i)
{
  isC2C = i.ReadU16 ();
}

void
c2cTag::Print (std::ostream &os) const
{
  os << "isC2C=" << isC2C;
}

} //namespace ns3
