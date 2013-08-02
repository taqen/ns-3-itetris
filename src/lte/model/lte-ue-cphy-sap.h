/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011, 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Nicola Baldo <nbaldo@cttc.es>,
 *         Marco Miozzo <mmiozzo@cttc.es>
 */

#ifndef LTE_UE_CPHY_SAP_H
#define LTE_UE_CPHY_SAP_H

#include <stdint.h>
#include <ns3/ptr.h>

#include <ns3/lte-rrc-sap.h>

namespace ns3 {


class LteEnbNetDevice;

/**
 * Service Access Point (SAP) offered by the UE PHY to the UE RRC for control purposes
 *
 * This is the PHY SAP Provider, i.e., the part of the SAP that contains
 * the PHY methods called by the MAC
 */
class LteUeCphySapProvider
{
public:

  /** 
   * destructor
   */
  virtual ~LteUeCphySapProvider ();

  /** 
   * reset the PHY
   * 
   */
  virtual void Reset () = 0;

  /**
   * \brief Tell the PHY entity to listen to PSS from surrounding cells and
   *        measure the RSRP.
   *
   * This function will instruct this PHY instance to listen to the DL channel
   * over the bandwidth of 6 RB.
   *
   * After this, it will start receiving Primary Synchronization Signal (PSS)
   * and periodically returning measurement reports to RRC via
   * LteUeCphySapUser::ReportUeMeasurements function.
   */
  virtual void StartCellSearch (uint16_t dlEarfcn) = 0;

  /**
   * \brief Tell the PHY entity to synchronize with a given eNodeB over the
   *        currently active EARFCN for communication purposes.
   * \param cellId the ID of the eNodeB
   *
   * By synchronizing, the PHY will start receiving various information
   * transmitted by the eNodeB. For instance, when receiving system information,
   * the message will be relayed to RRC via
   * LteUeCphySapUser::RecvMasterInformationBlock and
   * LteUeCphySapUser::RecvSystemInformationBlockType1 functions.
   *
   * Initially, the PHY will be configured to listen to 6 RBs of BCH.
   * LteUeCphySapProvider::SetDlBandwidth can be called afterwards to increase
   * the bandwidth.
   */
  virtual void SynchronizeWithEnb (uint16_t cellId) = 0;

  /**
   * \brief Tell the PHY entity to align to the given EARFCN and synchronize
   *        with a given eNodeB for communication purposes.
   * \param cellId the ID of the eNodeB
   * \param dlEarfcn the downlink carrier frequency (EARFCN)
   *
   * By synchronizing, the PHY will start receiving various information
   * transmitted by the eNodeB. For instance, when receiving system information,
   * the message will be relayed to RRC via
   * LteUeCphySapUser::RecvMasterInformationBlock and
   * LteUeCphySapUser::RecvSystemInformationBlockType1 functions.
   *
   * Initially, the PHY will be configured to listen to 6 RBs of BCH.
   * LteUeCphySapProvider::SetDlBandwidth can be called afterwards to increase
   * the bandwidth.
   */
  virtual void SynchronizeWithEnb (uint16_t cellId, uint16_t dlEarfcn) = 0;

  /**
   * \param dlBandwidth the DL bandwidth in number of PRBs
   */
  virtual void SetDlBandwidth (uint8_t dlBandwidth) = 0;

  /** 
   * \brief Configure uplink (normally done after reception of SIB2)
   * 
   * \param ulEarfcn the uplink carrier frequency (EARFCN)
   * \param ulBandwidth the UL bandwidth in number of PRBs
   */
  virtual void ConfigureUplink (uint16_t ulEarfcn, uint8_t ulBandwidth) = 0;

  /** 
   * 
   * \param rnti the cell-specific UE identifier
   */
  virtual void SetRnti (uint16_t rnti) = 0;

  /**
   * \param txMode the transmissionMode of the user
   */
  virtual void SetTransmissionMode (uint8_t txMode) = 0;

  /**
   * \param txMode the transmissionMode of the user
   */
  virtual void SetSrsConfigurationIndex (uint16_t   srcCi) = 0;

};


/**
 * Service Access Point (SAP) offered by the UE PHY to the UE RRC for control purposes
 *
 * This is the CPHY SAP User, i.e., the part of the SAP that contains the RRC
 * methods called by the PHY
*/
class LteUeCphySapUser
{
public:

  /** 
   * destructor
   */
  virtual ~LteUeCphySapUser ();


  /**
   * Parameters of the ReportUeMeasurements primitive: RSRP [dBm] and RSRQ [dB]
   * See section 5.1.1 and 5.1.3 of TS 36.214
   */
  struct UeMeasurementsElement
  {
    uint16_t m_cellId;
    double m_rsrp;  // [dBm]
    double m_rsrq;  // [dB]
  };

  struct UeMeasurementsParameters
  {
    std::vector <struct UeMeasurementsElement> m_ueMeasurementsList;
  };


  /** 
   * 
   * \param mib the Master Information Block received on the BCH
   */
  virtual void RecvMasterInformationBlock (uint16_t cellId,
                                           LteRrcSap::MasterInformationBlock mib) = 0;

  /**
   *
   * \param sib1 the System Information Block Type 1 received on the BCH
   */
  virtual void RecvSystemInformationBlockType1 (uint16_t cellId,
                                                LteRrcSap::SystemInformationBlockType1 sib1) = 0;

  /**
   *
   * \param params the structure containing the vector of cellId, RSRP and RSRQ
   */
  virtual void ReportUeMeasurements (UeMeasurementsParameters params) = 0;
};




/**
 * Template for the implementation of the LteUeCphySapProvider as a member
 * of an owner class of type C to which all methods are forwarded
 * 
 */
template <class C>
class MemberLteUeCphySapProvider : public LteUeCphySapProvider
{
public:
  MemberLteUeCphySapProvider (C* owner);

  // inherited from LteUeCphySapProvider
  virtual void Reset ();
  virtual void StartCellSearch (uint16_t dlEarfcn);
  virtual void SynchronizeWithEnb (uint16_t cellId);
  virtual void SynchronizeWithEnb (uint16_t cellId, uint16_t dlEarfcn);
  virtual void SetDlBandwidth (uint8_t dlBandwidth);
  virtual void ConfigureUplink (uint16_t ulEarfcn, uint8_t ulBandwidth);
  virtual void SetRnti (uint16_t rnti);
  virtual void SetTransmissionMode (uint8_t txMode);
  virtual void SetSrsConfigurationIndex (uint16_t srcCi);

private:
  MemberLteUeCphySapProvider ();
  C* m_owner;
};

template <class C>
MemberLteUeCphySapProvider<C>::MemberLteUeCphySapProvider (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberLteUeCphySapProvider<C>::MemberLteUeCphySapProvider ()
{
}

template <class C>
void 
MemberLteUeCphySapProvider<C>::Reset ()
{
  m_owner->DoReset ();
}

template <class C>
void
MemberLteUeCphySapProvider<C>::StartCellSearch (uint16_t dlEarfcn)
{
  m_owner->DoStartCellSearch (dlEarfcn);
}

template <class C>
void
MemberLteUeCphySapProvider<C>::SynchronizeWithEnb (uint16_t cellId)
{
  m_owner->DoSynchronizeWithEnb (cellId);
}

template <class C>
void
MemberLteUeCphySapProvider<C>::SynchronizeWithEnb (uint16_t cellId, uint16_t dlEarfcn)
{
  m_owner->DoSynchronizeWithEnb (cellId, dlEarfcn);
}

template <class C>
void
MemberLteUeCphySapProvider<C>::SetDlBandwidth (uint8_t dlBandwidth)
{
  m_owner->DoSetDlBandwidth (dlBandwidth);
}

template <class C>
void 
MemberLteUeCphySapProvider<C>::ConfigureUplink (uint16_t ulEarfcn, uint8_t ulBandwidth)
{
  m_owner->DoConfigureUplink (ulEarfcn, ulBandwidth);
}

template <class C>
void 
MemberLteUeCphySapProvider<C>::SetRnti (uint16_t rnti)
{
  m_owner->DoSetRnti (rnti);
}

template <class C>
void 
MemberLteUeCphySapProvider<C>::SetTransmissionMode (uint8_t txMode)
{
  m_owner->DoSetTransmissionMode (txMode);
}

template <class C>
void 
MemberLteUeCphySapProvider<C>::SetSrsConfigurationIndex (uint16_t srcCi)
{
  m_owner->DoSetSrsConfigurationIndex (srcCi);
}



/**
 * Template for the implementation of the LteUeCphySapUser as a member
 * of an owner class of type C to which all methods are forwarded
 * 
 */
template <class C>
class MemberLteUeCphySapUser : public LteUeCphySapUser
{
public:
  MemberLteUeCphySapUser (C* owner);

  // methods inherited from LteUeCphySapUser go here
  virtual void RecvMasterInformationBlock (uint16_t cellId,
                                           LteRrcSap::MasterInformationBlock mib);
  virtual void RecvSystemInformationBlockType1 (uint16_t cellId,
                                                LteRrcSap::SystemInformationBlockType1 sib1);
  virtual void ReportUeMeasurements (LteUeCphySapUser::UeMeasurementsParameters params);

private:
  MemberLteUeCphySapUser ();
  C* m_owner;
};

template <class C>
MemberLteUeCphySapUser<C>::MemberLteUeCphySapUser (C* owner)
  : m_owner (owner)
{
}

template <class C>
MemberLteUeCphySapUser<C>::MemberLteUeCphySapUser ()
{
}

template <class C> 
void 
MemberLteUeCphySapUser<C>::RecvMasterInformationBlock (uint16_t cellId,
                                                       LteRrcSap::MasterInformationBlock mib)
{
  m_owner->DoRecvMasterInformationBlock (cellId, mib);
}

template <class C>
void
MemberLteUeCphySapUser<C>::RecvSystemInformationBlockType1 (uint16_t cellId,
                                                            LteRrcSap::SystemInformationBlockType1 sib1)
{
  m_owner->DoRecvSystemInformationBlockType1 (cellId, sib1);
}

template <class C>
void
MemberLteUeCphySapUser<C>::ReportUeMeasurements (LteUeCphySapUser::UeMeasurementsParameters params)
{
  m_owner->DoReportUeMeasurements (params);
}


} // namespace ns3


#endif // LTE_UE_CPHY_SAP_H
