/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/inci-module.h"
#include "ns3/mobility-module.h"
#include "ns3/inci-utils-module.h" 
#include "ns3/config-store-module.h"
#include <exception>

using namespace ns3;

int main (int argc, char *argv[])
{
  /**
   * Two different configuration files can be used: a file (fileGeneralParameters) with the default attribute values to be used 
   * in the simulation and a file (fileConfTechnologies) with the configuration of the iTETRIS scenario. The file paths can be passed 
   * from the command line.
   */
  std::string fileGeneralParameters = "";
  std::string fileConfTechnologies = "";
  std::string inciPort = "";
  std::string logFile = "";

  CommandLine cmd;
  cmd.AddValue("fileGeneralParameters", "Path to the configuration file", fileGeneralParameters);
  cmd.AddValue("fileConfTechnologies", "Path to the configuration file", fileConfTechnologies);
  cmd.AddValue("inciPort", "iNCI listening socket port number", inciPort);

  // logFile could not be set. In this case, do not try to read it.
//   if (argc > 4 && !inciPort.empty() && !fileConfTechnologies.empty() && !fileGeneralParameters.empty())
// 	  cmd.AddValue("logFile", "Log file path", logFile);
if (argc > 4 /*&& !inciPort.empty() && !fileConfTechnologies.empty() && !fileGeneralParameters.empty()*/)
	  cmd.AddValue("logFile", "Log file path", logFile);

  cmd.Parse (argc, argv);

  LogComponentEnable ("ConfigurationManagerXml",LOG_LEVEL_ALL);
  LogComponentEnable ("WaveInstaller",LOG_LEVEL_ALL);
  LogComponentEnable ("WaveRsuInstaller",LOG_LEVEL_ALL);
  LogComponentEnable ("RsuStaMgnt",LOG_LEVEL_LOGIC);
  LogComponentEnable ("InciPacketList",LOG_LEVEL_ALL);
  LogComponentEnable ("iTETRISApplication",LOG_LEVEL_ALL);
  LogComponentEnable ("CAMmanage",LOG_LEVEL_ALL);
  LogComponentEnable ("c2cl4TSocketImpl",LOG_LEVEL_ALL);
  LogComponentEnable ("C2CIPApp",LOG_LEVEL_ALL);
  LogComponentEnable ("iTETRISns3Facilities",LOG_LEVEL_ALL);
  LogComponentEnable ("PacketManager",LOG_LEVEL_ALL);
  LogComponentEnable ("c2cTransport",LOG_LEVEL_ALL);
//  LogComponentEnable ("ItetrisMobilityModel",LOG_LEVEL_ALL);
  LogComponentEnableAll (LOG_PREFIX_ALL);

 
//  if (fileGeneralParameters != "")
//    {
//      Config::SetDefault ("ns3::ConfigStore::Filename", StringValue ("configGenerals.txt"));
//      Config::SetDefault ("ns3::ConfigStore::Mode", StringValue ("Save"));
//      ConfigStore config;
//      config.ConfigureDefaults ();
//    }

  Config::SetDefault("ns3::WifiMacQueue::MaxDelay", TimeValue(Seconds (10.0)));
  if (fileConfTechnologies == "")
    {
      fileConfTechnologies = "scratch/configTechnologies-ics.xml";
    }

//  if (argc < 5)
//  {
//    std::cout << "No port or log file for ns-3 server introduced." << std::endl;
//    std::cout << "Format is: main-inci5 [port-number] [fileGeneralParameters] [fileConfTechnologies] OPTIONAL [log-path]" << std::endl;
//    std::cout << "Exiting..." << std::endl;
//    return 0;
//  }

//  std::cout << inciPort << std::endl;
  int port = atoi(inciPort.c_str());   

  Packet::EnablePrinting();
  ConfigurationManagerXml confManager (fileConfTechnologies);
  iTETRISNodeManager* nodeManager = new iTETRISNodeManager ();
  PacketManager* packetManager = new PacketManager (); 
  packetManager->SetNodeManager (nodeManager);
  confManager.ReadFile(nodeManager);
  SeedManager::SetSeed (confManager.GetSeed());  
  SeedManager::SetRun (confManager.GetRunNumber()); 


  try {
    ns3::Ns3Server::processCommandsUntilSimStep(port, logFile, nodeManager, packetManager); 
  } catch (int e)
  {
    std::cout << "Exiting... " << std::endl;
    return 0;
  }
  
  Simulator::Destroy ();
  
  return 0;
}
