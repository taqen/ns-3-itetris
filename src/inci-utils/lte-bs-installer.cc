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
 * Author: Sendoa Vaz <svaz@cbt.es>
 */

#include "lte-bs-installer.h"
#include "ns3/itetris-mobility-model.h"

NS_LOG_COMPONENT_DEFINE ("LteBsInstaller");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (LteBsInstaller);

TypeId LteBsInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteBsInstaller")
    .SetParent<Object> ()     
    .AddConstructor<LteBsInstaller>()
    ;
  return tid;
}

LteBsInstaller::LteBsInstaller ()
{

}

//void
//LteBsInstaller::AddVehicles(NodeContainer container,NetDeviceContainer netDevices)
//{
//	for (NodeContainer::Iterator i = container.Begin (); i != container.End (); ++i)
//	{
//		for(NetDeviceContainer::Iterator iterator=netDevices.Begin();iterator!=netDevices.End();++iterator)
//		{
//			Ptr<LteBsMgnt> lteBsMgnt = (*i)->GetObject <LteBsMgnt> ();
//			if(lteBsMgnt)
//				lteBsMgnt->AddVehicle(DynamicCast<LteNetDevice>(*iterator));
//			else
//				NS_FATAL_ERROR("No LteBsMgnt Found");
//		}
//
//	}
//}

NetDeviceContainer
LteBsInstaller::DoInstall (NodeContainer container)
{
  NS_LOG_INFO ("*** LteBsInstaller ***");
  
  NodeContainer::Iterator it = container.Begin();
  NodeContainer enbContainer;
  enbContainer.Create(container.GetN());
  NodeContainer::Iterator enbit = enbContainer.Begin();

  while(it!=container.End() && enbit!=enbContainer.End())
  {
	  Ptr<ItetrisMobilityModel> model = (*it)->GetObject<ItetrisMobilityModel>();
	  (*enbit)->AggregateObject(CopyObject(model));
	  ++it;
	  ++enbit;
  }
  NetDeviceContainer edevices = lte->InstallEnbDevice(enbContainer);
  m_ipAddressHelper.Assign (edevices);

  NetDeviceContainer devices = lte->InstallUeDevice(container);
  m_ipAddressHelper.Assign(devices);

  enbNodes.Add(container);

  uint32_t index = 0;

  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); i++)
    {
      // Check if the base station has the object LteBsMgnt already installed
//TODO implement
      Ptr<LteBsMgnt> lteBsMgnt = (*i)->GetObject <LteBsMgnt> ();
      if (lteBsMgnt  == NULL)
      {
    	  Ptr<NetDevice> device = devices.Get(index);
    	  lteBsMgnt = CreateObject <LteBsMgnt> ();
    	  lteBsMgnt->SetNode (*i);
    	  lteBsMgnt->SetNetDevice (device);
    	  (*i)->AggregateObject (lteBsMgnt);
    	  NS_LOG_INFO ("The object LteBsMgnt has been installed in the base station");
      }
        
//        DynamicCast<LTENetDevice>(devices.Get(index))->SetIpAddress();
        
      // Check if the vehicle has the Facilties already installed
//     Ptr<iTETRISns3Facilities> facilities = (*i)->GetObject <iTETRISns3Facilities> ();
      Ptr<IPCIUFacilities> facilities = (*i)->GetObject <IPCIUFacilities> ();
      if (facilities == NULL)
      {
    	  IPCIUFacilitiesHelper facilitiesHelper;
    	  facilitiesHelper.SetServiceListHelper (m_servListHelper);
    	  facilitiesHelper.Install (*i);
    	  NS_LOG_INFO ("The object IPCIUFacilities has been installed in the vehicle");
      }

      index ++;
    }
    
  lte->Attach(devices);
  AddVehicles(container, ueDevices);
  enbDevices.Add(devices);
  return devices;
}

} // namespace ns3
