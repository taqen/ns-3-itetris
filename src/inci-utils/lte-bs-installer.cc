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
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-static-routing.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/internet-stack-helper.h"

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

NetDeviceContainer
LteBsInstaller::DoInstall (NodeContainer container)
{
  NS_LOG_INFO ("*** LteBsInstaller ***");
  
//  InternetStackHelper internet;
//  internet.Install (container);

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
//  internet.Install (enbContainer);

  NetDeviceContainer edevices = lte->InstallEnbDevice(enbContainer);
//  m_ipAddressHelper.Assign (edevices);

  NetDeviceContainer devices = lte->InstallUeDevice(container);
//  m_ipAddressHelper.Assign(devices);

  enbNodes.Add(container);

  Ptr<Node> pgw = epcHelper->GetPgwNode ();
  Ptr<Node> remoteHost = container.Get(0);

  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
  Ipv4InterfaceContainer internetIpIfaces = m_ipAddressHelper.Assign(internetDevices);

  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);


  Ipv4InterfaceContainer ueIpIface = epcHelper->AssignUeIpv4Address(devices);
  for (uint32_t u = 0; u < container.GetN (); ++u)
  {
	  Ptr<Node> ueNode = container.Get (u);
	  // Set the default gateway for the UE
	  Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
         ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
  }

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
