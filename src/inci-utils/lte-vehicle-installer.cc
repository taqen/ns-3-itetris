/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT EU FP7 iTETRIS project
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
 * Author: Taqi Ad-Din <takai.kenn@gmail.com>
 */

#include "lte-vehicle-installer.h"


 
NS_LOG_COMPONENT_DEFINE ("LteVehicleInstaller");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (LteVehicleInstaller);

TypeId LteVehicleInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteVehicleInstaller")
    .SetParent<Object> ()     
    .AddConstructor<LteVehicleInstaller>()
    ;
  return tid;
}
    
NetDeviceContainer
LteVehicleInstaller::DoInstall (NodeContainer container)
{
  NS_LOG_INFO ("*** LteVehicleInstaller ***");
  
  NetDeviceContainer devices = lte->InstallUeDevice(container);
  m_ipAddressHelper.Assign (devices);
  ueNodes.Add(container);
  ueDevices.Add(devices);

   uint32_t index = 0;

  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); i++)
    {
      if (!(*i)->IsMobileNode ())
        {
          NS_LOG_INFO ("Node LTE defined as MobileNode");
	  (*i)->SetMobileNode (true);
        }

      Ptr<NetDevice> device = devices.Get(index);

      // Check if the NetDevice has the object LteScanMngr already installed
      Ptr<VehicleScanMngr> vehicleScanMngr = device->GetObject <LteVehicleScanMngr> ();
      if (vehicleScanMngr == NULL)
	{
          vehicleScanMngr = CreateObject <LteVehicleScanMngr> ();
          vehicleScanMngr->SetNetDevice (device);
	  vehicleScanMngr->SetNode (*i);	//Added by Sendoa
	  (*i)->AggregateObject (vehicleScanMngr);
          NS_LOG_INFO ("The object LteScanMngr has been attached to the NetDevice");
        }

      // Check if the vehicle has the object VehicleStaMgnt already installed
      Ptr<VehicleStaMgnt> vehicleStaMgnt = (*i)->GetObject <VehicleStaMgnt> ();
      if (vehicleStaMgnt == NULL)
	{
          vehicleStaMgnt = CreateObject <VehicleStaMgnt> ();
          vehicleStaMgnt->SetNode (*i);
          (*i)->AggregateObject (vehicleStaMgnt);
          NS_LOG_INFO ("The object VehicleStaMgnt has been installed in the vehicle");
        }
      vehicleStaMgnt->AddIpTechnology ("Lte",device, vehicleScanMngr);
      
//      DynamicCast<LteNetDevice>(devices.Get(index))->SetIpAddress(); //TODO support
      
       // Check if the vehicle has the Facilties already installed
      Ptr<iTETRISns3Facilities> facilities = (*i)->GetObject <iTETRISns3Facilities> ();
      if (facilities == NULL)
	{
          C2CFacilitiesHelper facilitiesHelper;
          facilitiesHelper.AddDefaultServices (m_servListHelper);
          facilitiesHelper.Install (*i);
          NS_LOG_INFO ("The object iTETRISns3Facilities has been installed in the vehicle");
        }
      else
        {
          C2CFacilitiesHelper facilitiesHelper;
          facilitiesHelper.SetServiceListHelper (m_servListHelper);
          facilitiesHelper.AddServices (facilities, *i);
          NS_LOG_INFO ("New services have been installed in the vehicle");
        }


      index ++;
    }
  
  lte->Attach(devices);
  AddVehicles(enbNodes, devices);

  return devices;
}

} // namespace ns3
