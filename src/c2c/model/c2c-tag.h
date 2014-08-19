/*
 * c2c-tag.h
 *
 *  Created on: Jul 8, 2014
 *      Author: kentux
 */

#ifndef C2C_TAG_H_
#define C2C_TAG_H_

#include "ns3/packet.h"

namespace ns3 {

  class Tag;

  class c2cTag : public Tag
  {
  public:
    static TypeId GetTypeId (void);
    virtual TypeId GetInstanceTypeId (void) const;

    c2cTag ();
    virtual void Serialize (TagBuffer i) const;
    virtual void Deserialize (TagBuffer i);
    virtual uint32_t GetSerializedSize () const;
    virtual void Print (std::ostream &os) const;

  private:
    bool isC2C;
  };

}



#endif /* C2C_TAG_H_ */
