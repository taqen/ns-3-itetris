/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 EU FP7 iTETRIS project
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
 * Author: Ramon Bauza <rbauza@umh.es>
 */

#include "mcs-tag-wifi-manager.h"
#include "ns3/mcs-tag.h"
#include "ns3/string.h"
#include "ns3/assert.h"
#include "ns3/wifi-mode.h"

#define Min(a,b) ((a < b) ? a : b)

namespace ns3 {

struct McsTagWifiRemoteStation : public WifiRemoteStation
{
};

NS_OBJECT_ENSURE_REGISTERED (McsTagWifiManager)
  ;

TypeId
McsTagWifiManager::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::McsTagWifiManager")
    .SetParent<WifiRemoteStationManager> ()
    .AddConstructor<McsTagWifiManager> ();
  return tid;
}

McsTagWifiManager::McsTagWifiManager ()
{}

McsTagWifiManager::~McsTagWifiManager ()
{}

void 
McsTagWifiManager::DoReportRxOk (WifiRemoteStation *st,
                             double rxSnr, WifiMode txMode)
{}
void 
McsTagWifiManager::DoReportRtsFailed (WifiRemoteStation *st)
{}
void 
McsTagWifiManager::DoReportDataFailed (WifiRemoteStation *st)
{}
void 
McsTagWifiManager::DoReportRtsOk (WifiRemoteStation *st,
                              double ctsSnr, WifiMode ctsMode, double rtsSnr)
{}
void 
McsTagWifiManager::DoReportDataOk (WifiRemoteStation *st,
                              double ackSnr, WifiMode ackMode, double dataSnr)
{
  m_packet = 0; 
}
void 
McsTagWifiManager::DoReportFinalRtsFailed (WifiRemoteStation *st)
{
  m_packet = 0; 
}
void 
McsTagWifiManager::DoReportFinalDataFailed (WifiRemoteStation *st)
{
  m_packet = 0; 
}

WifiTxVector
McsTagWifiManager::DoGetDataTxVector (WifiRemoteStation *st, uint32_t size)
{
  NS_ASSERT(IsLowLatency ());
  McsTagWifiRemoteStation *station = (McsTagWifiRemoteStation *)st;

  McsTag tag;
  bool found;
  WifiMode mode;
  found = m_packet->PeekPacketTag (tag);
  if (found)
    {
      mode = GetWifiMode (tag.Get());
    }
  else
    {
      mode = GetNonUnicastMode ();
    }

  return WifiTxVector (mode, GetDefaultTxPowerLevel (), GetLongRetryCount (station), GetShortGuardInterval (station), Min (GetNumberOfReceiveAntennas (station),GetNumberOfTransmitAntennas()), GetNumberOfTransmitAntennas (station), GetStbc (station));
}

WifiTxVector
McsTagWifiManager::GetRtsTxVector (Mac48Address address, const WifiMacHeader *header,
                        Ptr<const Packet> packet, uint32_t fullPacketSize)
{
  m_packet = packet;
  return WifiRemoteStationManager::GetDataTxVector (address, header, packet, fullPacketSize);

}

WifiTxVector
McsTagWifiManager::DoGetRtsTxVector (WifiRemoteStation *station)
{
  NS_ASSERT(IsLowLatency ());
  McsTag tag;
  bool found;
  WifiMode mode;
  found = m_packet->PeekPacketTag (tag);
  if (found)
    {
      mode = GetWifiMode (tag.Get());
    }
  else
    {
      mode = GetNonUnicastMode ();
    }
  return WifiTxVector (mode, GetDefaultTxPowerLevel (), GetShortRetryCount (station), GetShortGuardInterval (station), Min (GetNumberOfReceiveAntennas (station),GetNumberOfTransmitAntennas()), GetNumberOfTransmitAntennas (station), GetStbc (station));
}

WifiMode 
McsTagWifiManager::GetWifiMode (uint8_t mcs)
{
  switch (mcs) {
  case 0 :
    {
    WifiMode mode ("OfdmRate3MbpsBW10MHz");
    return mode;
    break;
    }
  case 1 :
    {
    WifiMode mode ("OfdmRate4_5MbpsBW10MHz");
    return mode;
    break;
    }
  case 2 :
    {
    WifiMode mode ("OfdmRate6MbpsBW10MHz");
    return mode;
    break;
    }
  case 3 :
    {
    WifiMode mode ("OfdmRate9MbpsBW10MHz");
    return mode;
    break;
    }
  case 4 :
    {
    WifiMode mode ("OfdmRate12MbpsBW10MHz");
    return mode;
    break;
    }
  case 5 :
    {
    WifiMode mode ("OfdmRate18MbpsBW10MHz");
    return mode;
    break;
    }
  case 6 :
    {
    WifiMode mode ("wifi-24mbs-10Mhz");
    return mode;
    break;
    }
  case 7 : 
    {
    WifiMode mode ("wifi-27mbs-10Mhz");
    return mode;
    break;
    }
  }
  WifiMode mode ("wifi-3mbs-10Mhz");
  return mode;
}

WifiRemoteStation *
McsTagWifiManager::DoCreateStation (void) const
{
  McsTagWifiRemoteStation *station = new McsTagWifiRemoteStation ();
  return station;
}

bool
McsTagWifiManager::IsLowLatency (void) const
{
  return true;
}
} // namespace ns3
