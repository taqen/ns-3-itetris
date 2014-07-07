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
#include "lte-vehicle-scan-mngr.h"
#include "ns3/lte-net-device.h"
#include "ns3/mobility-model.h"


NS_LOG_COMPONENT_DEFINE ("LteVehicleScanMngr");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LteVehicleScanMngr);

TypeId LteVehicleScanMngr::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteVehicleScanMngr")
    .SetParent<Object> ()            
    .AddConstructor<LteVehicleScanMngr> ()
    ;
  return tid;
}

LteVehicleScanMngr::~LteVehicleScanMngr ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

Ptr<IpBaseStation> 
LteVehicleScanMngr::GetBestServingBs (void)
{  
  //   TODO Call the corresponding NetDevice (m_netDevice) to obtain the best serving base station
//   In the current implementation the IpBaseStation is hardcoded
  Ptr<IpBaseStation> station = CreateObject<IpBaseStation> (1,Ipv4Address("192.168.1.134"),20,43);
  return (station);

}

} // namespace ns3
