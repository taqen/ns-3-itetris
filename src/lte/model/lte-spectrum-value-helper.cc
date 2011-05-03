/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 TELEMATICS LAB, DEE - Politecnico di Bari
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
 * Author: Giuseppe Piro  <g.piro@poliba.it>
 *         Nicola Baldo <nbaldo@cttc.es>
 */


#include <ns3/log.h>
#include <cmath>

#include "lte-spectrum-value-helper.h"

NS_LOG_COMPONENT_DEFINE ("LteSpectrumValueHelper");

namespace ns3 {

/**
 * Table 5.7.3-1 "E-UTRA channel numbers" from 3GPP TS 36.101
 * The table was converted to C syntax doing a cut & paste from TS 36.101 and running the following filter:
 * awk '{if ((NR % 7) == 1) printf("{"); printf ("%s",$0); if ((NR % 7) == 0) printf("},\n"); else printf(", ");}' | sed 's/ – /, /g' 
 */
struct EutraChannelNumbers
{
  uint8_t band;  
  double fDlLow;    
  uint16_t nOffsDl; 
  uint16_t rangeNdl1;
  uint16_t rangeNdl2;
  double fUlLow;    
  uint16_t nOffsUl; 
  uint16_t rangeNul1;
  uint16_t rangeNul2;  
} g_eutraChannelNumbers[] = {
  {1, 2110, 0, 0, 599, 1920, 18000, 18000, 18599},
  {2, 1930, 600, 600, 1199, 1850, 18600, 18600, 19199},
  {3, 1805, 1200, 1200, 1949, 1710, 19200, 19200, 19949},
  {4, 2110, 1950, 1950, 2399, 1710, 19950, 19950, 20399},
  {5, 869, 2400, 2400, 2649, 824, 20400, 20400, 20649},
  {6, 875, 2650, 2650, 2749, 830, 20650, 20650, 20749},
  {7, 2620, 2750, 2750, 3449, 2500, 20750, 20750, 21449},
  {8, 925, 3450, 3450, 3799, 880, 21450, 21450, 21799},
  {9, 1844.9, 3800, 3800, 4149, 1749.9, 21800, 21800, 22149},
  {10, 2110, 4150, 4150, 4749, 1710, 22150, 22150, 22749},
  {11, 1475.9, 4750, 4750, 4949, 1427.9, 22750, 22750, 22949},
  {12, 728, 5000, 5000, 5179, 698, 23000, 23000, 23179},
  {13, 746, 5180, 5180, 5279, 777, 23180, 23180, 23279},
  {14, 758, 5280, 5280, 5379, 788, 23280, 23280, 23379},
  {17, 734, 5730, 5730, 5849, 704, 23730, 23730, 23849},
  {18, 860, 5850, 5850, 5999, 815, 23850, 23850, 23999},
  {19, 875, 6000, 6000, 6149, 830, 24000, 24000, 24149},
  {20, 791, 6150, 6150, 6449, 832, 24150, 24150 - 24449},
  {21, 1495.9, 6450, 6450, 6599, 1447.9, 24450, 24450, 24599},
  {33, 1900, 36000, 36000, 36199, 1900, 36000, 36000, 36199},
  {34, 2010, 36200, 36200, 36349, 2010, 36200, 36200, 36349},
  {35, 1850, 36350, 36350, 36949, 1850, 36350, 36350, 36949},
  {36, 1930, 36950, 36950, 37549, 1930, 36950, 36950, 37549},
  {37, 1910, 37550, 37550, 37749, 1910, 37550, 37550, 37749},
  {38, 2570, 37750, 37750, 38249, 2570, 37750, 37750, 38249},
  {39, 1880, 38250, 38250, 38649, 1880, 38250, 38250, 38649},
  {40, 2300, 38650, 38650, 39649, 2300, 38650, 38650, 39649}
};

#define NUM_EUTRA_BANDS (sizeof (g_eutraChannelNumbers) / sizeof (EutraChannelNumbers))

double 
LteSpectrumValueHelper::GetCarrierFrequency (uint16_t earfcn)
{
  NS_LOG_FUNCTION (earfcn);
  if (earfcn < 7000)
    {
      // FDD downlink
      return GetDownlinkCarrierFrequency (earfcn);
    }
  else 
    {
      // either FDD uplink or TDD (for which uplink & downlink have same frequency)
      return GetUplinkCarrierFrequency (earfcn);      
    }
}

double 
LteSpectrumValueHelper::GetDownlinkCarrierFrequency (uint16_t nDl)
{
  NS_LOG_FUNCTION (nDl);
  for (uint16_t i = 0; i < NUM_EUTRA_BANDS; ++i)
    {
      if ((g_eutraChannelNumbers[i].rangeNdl1 <= nDl) && 
          (g_eutraChannelNumbers[i].rangeNdl2 >= nDl))
        {
          NS_LOG_LOGIC ("entry " << i << " fDlLow=" << g_eutraChannelNumbers[i].fDlLow);
          return g_eutraChannelNumbers[i].fDlLow + 0.1 * (nDl - g_eutraChannelNumbers[i].nOffsDl);
        }
    }
  NS_LOG_ERROR ("invalid EARFCN " << nDl);
  return 0.0;
}

double 
LteSpectrumValueHelper::GetUplinkCarrierFrequency (uint16_t nUl)
{
  NS_LOG_FUNCTION (nUl);
  for (uint16_t i = 0; i < NUM_EUTRA_BANDS; ++i)
    {
      if ((g_eutraChannelNumbers[i].rangeNul1 <= nUl) && 
          (g_eutraChannelNumbers[i].rangeNul2 >= nUl))
        {
          NS_LOG_LOGIC ("entry " << i << " fUlLow=" << g_eutraChannelNumbers[i].fUlLow);
          return g_eutraChannelNumbers[i].fUlLow + 0.1 * (nUl - g_eutraChannelNumbers[i].nOffsUl);
        }
    }
  NS_LOG_ERROR ("invalid EARFCN " << nUl);
  return 0.0;
}



Ptr<SpectrumModel> LteDownlinkSpectrumModel;
Ptr<SpectrumModel> LteUplinkSpectrumModel;

class static_LteDownlinkSpectrumModel_initializer
{
public:
  static_LteDownlinkSpectrumModel_initializer ()
  {

    /*
     * Operating  Bands for the UL: 1920 MHz – 1980 MHz
     * see for details TR.36.101 - Tab 5.5-1
     *
     */

    std::vector<double> freqs;
    /* WILD HACK
    * banwidth 25
    */
    for (int i = 0; i < 25; ++i)
      {

        double centralFrequencyOfPRB = 1.920 + (i * 0.00018);
        freqs.push_back (centralFrequencyOfPRB * 1e9);
      }

    LteDownlinkSpectrumModel = Create<SpectrumModel> (freqs);
  }

} static_LteDownlinkSpectrumModel_initializer_instance;



class static_LteUplinkSpectrumModel_initializer
{
public:
  static_LteUplinkSpectrumModel_initializer ()
  {

    /*
     * Operating  Bands for the DL: 2110 MHz – 2170 MHz
     * see for details TR.36.101 - Tab 5.5-1
     *
     */

    std::vector<double> freqs;
    /* WILD HACK
    * banwidth 25
    */
    for (int i = 0; i < 25; ++i)
      {

        double centralFrequencyOfPRB = 2.110 + (i * 0.00018);
        freqs.push_back (centralFrequencyOfPRB * 1e9);
      }

    LteUplinkSpectrumModel = Create<SpectrumModel> (freqs);
  }

} static_LteUplinkSpectrumModel_initializer_instance;




Ptr<SpectrumValue>
LteSpectrumValueHelper::CreateDownlinkTxPowerSpectralDensity (double powerTx, std::vector<int> channels)
{
  Ptr<SpectrumValue> txPsd = Create <SpectrumValue> (LteDownlinkSpectrumModel);

  // powerTx is expressed in dBm. We must convert it into natural unit.
  powerTx = pow (10., (powerTx - 30) / 10);

  double txPowerDensity = (powerTx / channels.size ()) / 180000;

  for (std::vector <int>::iterator it = channels.begin (); it != channels.end (); it++)
    {
      int idSubChannel = (*it);
      (*txPsd)[idSubChannel] = txPowerDensity;
    }

  return txPsd;
}


Ptr<SpectrumValue>
LteSpectrumValueHelper::CreateUplinkTxPowerSpectralDensity (double powerTx, std::vector<int> channels)
{
  Ptr<SpectrumValue> txPsd = Create <SpectrumValue> (LteUplinkSpectrumModel);

  // powerTx is expressed in dBm. We must convert it into natural unit.
  powerTx = pow (10., (powerTx - 30) / 10);

  double txPowerDensity = (powerTx / channels.size ()) / 180000;

  for (std::vector <int>::iterator it = channels.begin (); it != channels.end (); it++)
    {
      int idSubChannel = (*it);
      (*txPsd)[idSubChannel] = txPowerDensity;
    }

  return txPsd;
}


Ptr<SpectrumValue>
LteSpectrumValueHelper::CreateDownlinkNoisePowerSpectralDensity (double noiseFigure)
{
  return  CreateNoisePowerSpectralDensity (noiseFigure, LteDownlinkSpectrumModel);
}

Ptr<SpectrumValue>
LteSpectrumValueHelper::CreateUplinkNoisePowerSpectralDensity (double noiseFigure)
{
  return  CreateNoisePowerSpectralDensity (noiseFigure, LteUplinkSpectrumModel);
}

Ptr<SpectrumValue>
LteSpectrumValueHelper::CreateNoisePowerSpectralDensity (double noiseFigureDb, Ptr<SpectrumModel> spectrumModel)
{
  double noiseFigureLinear = pow (10.0, noiseFigureDb / 10.0);
  static const double BOLTZMANN = 1.3803e-23;
  static const double ROOM_TEMPERATURE = 290.0;
  double noisePowerSpectralDensity = noiseFigureLinear * BOLTZMANN * ROOM_TEMPERATURE; //  W/Hz

  Ptr<SpectrumValue> noisePsd = Create <SpectrumValue> (spectrumModel);
  (*noisePsd) = noisePowerSpectralDensity;
  return noisePsd;
}

} // namespace ns3
