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
 * Author: Sendoa Vaz <svaz@cbt.es>
 */

#ifndef LTE_INSTALLER_H
#define LTE_INSTALLER_H

#include "ns3/simulator.h"
#include "ns3/node-container.h"
#include "ns3/lte-helper.h"
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include "ns3/ipv4-address-helper.h"
#include "comm-module-installer.h"
#include "ns3/C2C-IP-helper.h"
#include "ns3/Lte-App-helper.h"
#include "ns3/CAMmanage-helper.h"
#include "ns3/service-list-helper.h"
#include "ns3/c2c-interface-helper.h"
#include "ns3/point-to-point-epc-helper.h"

namespace ns3
{

class LteInstaller : public CommModuleInstaller
{
  public:
    static TypeId GetTypeId (void);
    LteInstaller(void);
    void Install (NodeContainer container); 
    void Configure (std::string filename);
    void AssignIpAddress(NetDeviceContainer devices);
    void ProcessApplicationInstall (xmlTextReaderPtr reader);
    ~LteInstaller();
    
    virtual NetDeviceContainer DoInstall (NodeContainer container) = 0;

    void AddVehicles(NodeContainer container,NetDeviceContainer netDevices);

  protected:
    void AddInterfacesToIpInterfaceList (NodeContainer container);

    static Ptr<LteHelper> lte;
    static Ptr<PointToPointEpcHelper> epcHelper;
  
    static Ipv4AddressHelper m_ipAddressHelper;

    static NodeContainer enbNodes;
    static NodeContainer ueNodes;
    static NetDeviceContainer ueDevices;
    static NetDeviceContainer enbDevices;

    C2CIPHelper* m_c2cIpHelper;
    LTEAppHelper* m_lteAppHelper;
    CAMmanageHelper* m_camHelper;
    ServiceListHelper* m_servListHelper;
    c2cInterfaceHelper inf;
 
};

}

#endif





