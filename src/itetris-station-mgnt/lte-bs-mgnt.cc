/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT, EU FP7 iTETRIS project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Sendoa Vaz <svaz@cbt.es> based on the code of Ramon Bauza <rbauza@umh.es>
 */

#include "ns3/log.h"
#include "lte-bs-mgnt.h"
#include "ns3/lte-net-device.h"
#include "ns3/addressing-support.h"
#include "ns3/ipv4.h"

NS_LOG_COMPONENT_DEFINE ("LteBsMgnt");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LteBsMgnt);

TypeId LteBsMgnt::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteBsMgnt")
    .SetParent<IpBaseStaMgnt> ()        
    .AddConstructor<LteBsMgnt> ()
    ;
  return tid;
}

LteBsMgnt::~LteBsMgnt ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

Ipv4Address* 
LteBsMgnt::GetIpAddress (uint32_t nodeId) const
{
  Ipv4Address address;
  TriggerVehiclesScanning (); 
  if (nodeId == ID_BROADCAST)
    {
      address = *GetIpBroadcastAddress ();
    }
  else
    {
	  for(VehicleList::const_iterator it = vehicleList.begin(); it != vehicleList.end(); ++it)
	  {
		  uint32_t id = (*it)->GetNode()->GetId();
		  if(nodeId == id)
			  {
			  	  address = (*it)->GetNode()->GetObject<Ipv4> ()->GetAddress(1,0).GetLocal();
			  	  return (&address);
			  }
	  }
    }
  return (&address);
}

void
LteBsMgnt::AddVehicle(Ptr<LteNetDevice> netDevice)
{
	vehicleList.push_back(netDevice);
}

void 
LteBsMgnt::TriggerVehiclesScanning (void) const
{  
//  DynamicCast<LteBaseStationManager>(DynamicCast<UMTSNetDevice>(m_netDevice)->GetManager())->TriggerNodeUEScanning();
}

uint32_t 
LteBsMgnt::GetNumberOfActiveConnections (void) const
{
  return (0);
}

uint32_t 
LteBsMgnt::GetNumberOfRegisteredUsers (void) const
{

	// check if correct and add the method
	 TriggerVehiclesScanning();
//	 uint32_t users = DynamicCast<LteBaseStationManager>(DynamicCast<LTENetDevice>(m_netDevice)->GetManager())->getRegistedUsers();
	return 0;//users;
}

double
LteBsMgnt::GetCoverageRange (void) const
{
//	return DynamicCast<LteBaseStationManager>(DynamicCast<LteNetDevice>(m_netDevice)->GetManager())->GetCoverageRange();
}

} // namespace ns3
