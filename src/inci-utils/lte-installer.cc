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
//#include "ns3/Lte-App-helper.h"
#include "ip-interface-list.h"
#include "ns3/service-list-helper.h"
#include "ns3/itetris-technologies.h"
#include "ns3/point-to-point-epc-helper.h"

NS_LOG_COMPONENT_DEFINE ("LteInstaller");

namespace ns3
{

Ipv4AddressHelper LteInstaller::m_ipAddressHelper;
Ptr<LteHelper> LteInstaller::lte;
Ptr<PointToPointEpcHelper> LteInstaller::epcHelper;
//NodeContainer LteInstaller::vehicleContainer;
//NodeContainer LteInstaller::baseStationContainer;
//
//NetDeviceContainer LteInstaller::baseStationDeviceContainer;
//NetDeviceContainer LteInstaller::vehicleDeviceContainer;

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
   m_c2cIpHelper = NULL;
//   m_lteAppHelper=NULL;
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
  delete m_c2cIpHelper; 
  delete m_servListHelper;  
//  delete m_lteAppHelper;
  m_c2cIpHelper = NULL; 
  m_servListHelper = NULL; 
//  m_lteAppHelper=NULL;
}


void
LteInstaller::Install (NodeContainer container)
{
  NetDeviceContainer netDevices = DoInstall(container);
  AddInterfacesToIpInterfaceList(container);
  inf.Assign(LTE_DEVICE, netDevices);

}

void
LteInstaller::Configure (std::string filename)
{
  xmlTextReaderPtr reader = xmlNewTextReaderFilename(filename.c_str ());
  if (reader == NULL)
    {
      NS_FATAL_ERROR ("Error at xmlReaderForFile");
    }

  NS_LOG_DEBUG ("Reading config file for UMTS");

  int rc;
  rc = xmlTextReaderRead (reader);
  while (rc > 0)
    {
      const xmlChar *tag = xmlTextReaderConstName(reader);
      if (tag == 0)
        {
          NS_FATAL_ERROR ("Invalid value");
        }

      // UMTSPhy type setting
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
//            NS_LOG_DEBUG ("UMTSPhyUE attribute=" << attribute <<" value=" << value);
//            umtsPhyUE.Set((char *)attribute,StringValue((char *)value));
//          }
//          else
//          {
//             NS_LOG_DEBUG ("UMTSPhyBS attribute=" << attribute <<" value=" << value);
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
//      if (std::string ((char*)tag) == "application")
//        {
//          ProcessApplicationInstall (reader);
//        }

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

//TODO Support for applications
//void
//LteInstaller::ProcessApplicationInstall (xmlTextReaderPtr reader)
//{
//  int rc;
//  std::string appType, appName;
//
//  rc = xmlTextReaderRead (reader);
//  while (rc > 0)
//    {
//      const xmlChar *tag = xmlTextReaderConstName(reader);
//      if (tag == 0)
//	{
//	  NS_FATAL_ERROR ("Invalid value");
//	}
//
//      NS_LOG_DEBUG ("Tag read in ConfigurationFile=" << tag);

//       if (std::string ((char*)tag) == "LteApp")
//	{
//          appType = "LteApp";
//	  xmlChar *name = xmlTextReaderGetAttribute (reader, BAD_CAST "itetrisName");
//
//	  if (name != 0)
//	    {
//	      appName = std::string ((char*)name);
//	      m_lteAppHelper = new LTEAppHelper();
//	      NS_LOG_DEBUG ("LTE Application itetrisName = "<<std::string ((char*)name));
//	    }
//	  xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
//	  if (attribute != 0)
//	    {
//	      xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
//	      if (value != 0)
//		{
//		  if (m_lteAppHelper)
//		    {
//		      m_lteAppHelper->SetAttribute((char*)attribute,StringValue((char*)value));
//		      NS_LOG_DEBUG ("LTEApp attribute=" << attribute <<" value=" << value);
//		    }
//
//		    if(std::string((char*)value)=="BROADCAST"||std::string((char*)value)=="MULTICAST")
//		    {
//		      xmlChar *ip = xmlTextReaderGetAttribute (reader, BAD_CAST "ip");
//		      if (ip != 0)
//			{
//			  if (m_lteAppHelper)
//			    {
//			      m_lteAppHelper->SetAttribute((char*)attribute,StringValue((char*)ip));
//			      NS_LOG_DEBUG ("LTEApp attribute=" << attribute <<" value=" << ip);
//			    }
//
//
//			}
//			xmlFree (ip);
//		    }
//
//		}
//
//
//	      xmlFree (value);
//	    }
//	  xmlFree (name);
//	  xmlFree (attribute);
//	}
//       else if (std::string ((char*)tag) == "C2CIP")
// 	{
//           appType = "C2CIP";
// 	  
// 	  xmlChar *name = xmlTextReaderGetAttribute (reader, BAD_CAST "itetrisName");
// 	  if (name != 0)
// 	    {
// 	      appName = std::string ((char*)name);	      
// 	      m_c2cIpHelper = new C2CIPHelper("ns3::c2cl4TSocketFactory", "ns3::UdpSocketFactory");
// 	      NS_LOG_DEBUG ("Application itetrisName="<<std::string ((char*)name)); 
// 	    }
// 	  xmlChar *attribute = xmlTextReaderGetAttribute (reader, BAD_CAST "attribute");
// 	  if (attribute != 0)
// 	    {
// 	      xmlChar *value = xmlTextReaderGetAttribute (reader, BAD_CAST "value");
// 	      if (value != 0)
// 		{
// 		  if (m_c2cIpHelper)
// 		    {
// 		      m_c2cIpHelper->SetAttribute((char*)attribute,StringValue((char*)value));
// 		      NS_LOG_DEBUG ("C2CIP attribute=" << attribute <<" value=" << value); 
// 		    }
// 		}
// 	      xmlFree (value);
// 	    }
// 	  xmlFree (name);
// 	  xmlFree (attribute);
// 	}
//      else
//        if (std::string ((char*)tag) == "application")
//	{
////           if (appType=="LteApp")
////             {
//	      if(m_nodeType=="NodeUE")
//	      {
//		appName="LTE-"+appName;
//	      }
//	      m_servListHelper->Add (m_lteAppHelper, appName);
//	      NS_LOG_DEBUG ("LteApp application with itetrisName="<<appName<<" has been added to the ServiceListHelper");
//              m_lteAppHelper = NULL;
//// 	    }
//// 	    else if (appType=="C2CIP")
////             {
//// 	      m_servListHelper->Add (m_c2cIpHelper, appName);
//// 	      NS_LOG_DEBUG ("C2CIP application with itetrisName="<<appName<<" has been added to the ServiceListHelper");
////               m_c2cIpHelper = NULL;
//// 	    }
////           else
//// 	    {
//// 	      NS_FATAL_ERROR ("Type of application not valid ");
//// 	    }
//	  return;
//	}
//      rc = xmlTextReaderRead (reader);
//    }
//}

} // namespace ns3
