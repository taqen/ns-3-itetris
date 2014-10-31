/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010,CBT EU FP7 iTETRIS project
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
 * Author: Taqi Ad-Din Kennouche <takai.kenn@gmail.com>
 */

#include "lte-installer.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/lte-helper.h"
// Added for reading xml files 
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
//#include "ns3/lte-vehicle-scan-mngr.h"
#include "ns3/vehicle-scan-mngr.h"
//#include "ns3/lte-bs-mgnt.h"
#include "ns3/C2C-IP-helper.h"
#include "ip-interface-list.h"
#include "ns3/service-list-helper.h"
#include "ns3/itetris-technologies.h"
#include "ns3/point-to-point-epc-helper.h"
#include "ns3/lte-net-device.h"
#include "ns3/lte-bs-mgnt.h"
#include "ns3/internet-stack-helper.h"

NS_LOG_COMPONENT_DEFINE ("LteInstaller");

namespace ns3
{

Ipv4AddressHelper LteInstaller::m_ipAddressHelper;
Ptr<LteHelper> LteInstaller::lte;
Ptr<PointToPointEpcHelper> LteInstaller::epcHelper;
NodeContainer LteInstaller::ueNodes;
NodeContainer LteInstaller::enbNodes;
NetDeviceContainer LteInstaller::ueDevices;
NetDeviceContainer LteInstaller::enbDevices;

NS_OBJECT_ENSURE_REGISTERED (LteInstaller);

TypeId LteInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteInstaller")
    .SetParent<Object> ()     
                 
    ;
  return tid;
}
    
LteInstaller::LteInstaller () {

   m_servListHelper = new ServiceListHelper ();
   m_lteAppHelper=NULL;
   m_ipAddressHelper.SetBase ("10.3.0.0", "255.255.0.0");
   if(epcHelper==NULL)
     {
       epcHelper = CreateObject<PointToPointEpcHelper> ();
       lte = CreateObject<LteHelper> ();
       lte->SetEpcHelper(epcHelper);
     }
}

LteInstaller::~LteInstaller()
{ 
  delete m_servListHelper;  
  delete m_lteAppHelper;
  m_servListHelper = NULL; 
  m_lteAppHelper=NULL;
}

void
LteInstaller::Install (NodeContainer container)
{
  NetDeviceContainer netDevices = DoInstall(container);
  AddInterfacesToIpInterfaceList(container);
//  m_ipAddressHelper.Assign(netDevices);

}

void
LteInstaller::Configure (std::string filename)
{
  xmlTextReaderPtr reader = xmlNewTextReaderFilename(filename.c_str ());
  if (reader == NULL)
    {
      NS_FATAL_ERROR ("Error at xmlReaderForFile");
    }

  NS_LOG_DEBUG ("Reading config file for LTE");

  int rc;
  rc = xmlTextReaderRead (reader);
  while (rc > 0)
    {
      const xmlChar *tag = xmlTextReaderConstName(reader);
      if (tag == 0)
        {
          NS_FATAL_ERROR ("Invalid value");
        }

      // LTEPhy type setting
      if (std::string ((char*)tag) == "LtePhy")
        {
          xmlChar *nodeType = xmlTextReaderGetAttribute (reader, BAD_CAST "name");

//          m_nodeType=(char *)nodeType;

          xmlFree (nodeType);
        }

//TODO Implement the configurable attributes
//        if (std::string ((char*)tag) == "phy")
//        {
//          xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
//          xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
//
//          if(m_nodeType=="NodeUE")
//          {
//            NS_LOG_DEBUG ("LTEPhyUE attribute=" << attribute <<" value=" << value);
//            umtsPhyUE.Set((char *)attribute,StringValue((char *)value));
//          }
//          else
//          {
//             NS_LOG_DEBUG ("LTEPhyBS attribute=" << attribute <<" value=" << value);
//            umtsPhyBS.Set((char *)attribute,StringValue((char *)value));
//          }
//
//
//          xmlFree (attribute);
//          xmlFree (value);
//        }

         // ipConfiguration
      if (std::string ((char*)tag) == "ip")
        {
          xmlChar *address = xmlTextReaderGetAttribute (reader, BAD_CAST "address");
          if (address != 0)
            {
              xmlChar *mask = xmlTextReaderGetAttribute (reader, BAD_CAST "mask");
              if (mask != 0)
                {
//                m_ipAddressHelper.SetBase ((char*)address, (char*)mask);

                  NS_LOG_DEBUG ("ip address="<<(char*)address<<" mask=" << (char*)mask);
                }
              xmlFree (mask);
            }
          xmlFree (address);
        }

        // Applications
      if (std::string ((char*)tag) == "application")
        {
          ProcessApplicationInstall (reader);
        }

      rc = xmlTextReaderRead (reader);
    }
  xmlFreeTextReader (reader);
} 

void 
LteInstaller::AddInterfacesToIpInterfaceList (NodeContainer container)
{
  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); ++i)
    {
    // Check if the node has the object IpInterfaceList installed
    Ptr<IpInterfaceList> interfaceList = (*i)->GetObject <IpInterfaceList> ();
    if (interfaceList == NULL)
      {
        interfaceList = CreateObject <IpInterfaceList> ();
        (*i)->AggregateObject (interfaceList);
        NS_LOG_INFO ("The object IpInterfaceList has been attached to the node");
      }
      Ptr<Ipv4> ipStack = (*i)->GetObject <Ipv4> (); 
      uint32_t index = ipStack->GetNInterfaces ();
      bool res = interfaceList->AddIpInterface("Lte", ipStack->GetAddress (index-1,0));
      NS_ASSERT_MSG (res, "LteInstaller::AddInterfacesToIpInterfaceList - The IP interface cannot be added to the IpInterfaceList");
      NS_LOG_INFO ("IP address " << ipStack->GetAddress (index-1,0));
      
      
    }
}

void
LteInstaller::AddVehicles(NodeContainer container,NetDeviceContainer netDevices)
{
	for (NodeContainer::Iterator i = container.Begin (); i != container.End (); ++i)
	{
		for(NetDeviceContainer::Iterator iterator=netDevices.Begin();iterator!=netDevices.End();++iterator)
		{
			Ptr<LteBsMgnt> lteBsMgnt = (*i)->GetObject <LteBsMgnt> ();
			if(lteBsMgnt)
				lteBsMgnt->AddVehicle(DynamicCast<LteNetDevice>(*iterator));
			else
				NS_FATAL_ERROR("No LteBsMgnt Found");
		}

	}
}

//TODO Support for applications
void
LteInstaller::ProcessApplicationInstall (xmlTextReaderPtr reader)
{
  int rc;
  std::string appType, appName;

  rc = xmlTextReaderRead (reader);
  while (rc > 0)
    {
      const xmlChar *tag = xmlTextReaderConstName(reader);
      if (tag == 0)
	{
	  NS_FATAL_ERROR ("Invalid value");
	}

      NS_LOG_DEBUG ("Tag read in ConfigurationFile=" << tag);

       if (std::string ((char*)tag) == "LteApp")
	{
          appType = "LteApp";
	  xmlChar *name = xmlTextReaderGetAttribute (reader, BAD_CAST "itetrisName");

	  if (name != 0)
	    {
	      appName = std::string ((char*)name);
	      m_lteAppHelper = new LTEAppHelper();
	      NS_LOG_DEBUG ("LTE Application itetrisName = "<<std::string ((char*)name));
	    }
	  xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
	  if (attribute != 0)
	    {
	      xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
	      if (value != 0)
		{
		  if (m_lteAppHelper)
		    {
		      m_lteAppHelper->SetAttribute((char*)attribute,StringValue((char*)value));
		      NS_LOG_DEBUG ("LTEApp attribute=" << attribute <<" value=" << value);
		    }

		    if(std::string((char*)value)=="BROADCAST"||std::string((char*)value)=="MULTICAST")
		    {
		      xmlChar *ip = xmlTextReaderGetAttribute (reader, BAD_CAST "ip");
		      if (ip != 0)
			{
			  if (m_lteAppHelper)
			    {
			      m_lteAppHelper->SetAttribute((char*)attribute,StringValue((char*)ip));
			      NS_LOG_DEBUG ("LTEApp attribute=" << attribute <<" value=" << ip);
			    }


			}
			xmlFree (ip);
		    }

		}


	      xmlFree (value);
	    }
	  xmlFree (name);
	  xmlFree (attribute);
	}
      else if (std::string ((char*)tag) == "application")
	{
	      m_servListHelper->Add (m_lteAppHelper, appName);
	      NS_LOG_DEBUG ("LteApp application with itetrisName="<<appName<<" has been added to the ServiceListHelper");
	      m_lteAppHelper = NULL;
	  return;
	}
      rc = xmlTextReaderRead (reader);
    }
}


} // namespace ns3
