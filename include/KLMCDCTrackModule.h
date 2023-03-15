/**************************************************************************
 * basf2 (Belle II Analysis Software Framework)                           *
 * Author: The Belle II Collaboration                                     *
 *                                                                        *
 * See git log for contributors and copyright holders.                    *
 * This file is licensed under LGPL-3.0, see LICENSE.md.                  *
 **************************************************************************/

#pragma once

/* KLM headers. */
#include <klm/dataobjects/bklm/BKLMElementNumbers.h>
#include <klm/dataobjects/bklm/BKLMHit1d.h>
#include <klm/dataobjects/eklm/EKLMElementNumbers.h>
//#include <klm/dataobjects/KLMHit2d.h>
#include <klm/dataobjects/KLMDigit.h>
#include <klm/dataobjects/KLMChannelArrayIndex.h>
#include <klm/dataobjects/KLMElementNumbers.h>
#include <klm/dataobjects/KLMSectorArrayIndex.h>
#include <klm/bklm/geometry/GeometryPar.h>

/* Belle 2 headers. */
#include <framework/core/HistoModule.h>
#include <framework/datastore/StoreArray.h>
#include <rawdata/dataobjects/RawFTSW.h>
#include <rawdata/dataobjects/RawKLM.h>

#include <mdst/dataobjects/Track.h>
#include <mdst/dataobjects/TrackFitResult.h>
#include <tracking/dataobjects/RecoTrack.h>

#include <tracking/trackFindingCDC/eventdata/tracks/CDCTrack.h>
#include <tracking/trackFindingCDC/rootification/StoreWrappedObjPtr.h>
#include <tracking/trackFindingCDC/topology/CDCWire.h>

#include <klm/dataobjects/bklm/BKLMTrack.h>

#include <cdc/dataobjects/CDCHit.h>
#include <cdc/dataobjects/CDCRawHit.h>
#include <framework/core/Module.h>


/* ROOT headers. */
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile.h>
#include <TProfile2D.h>

namespace Belle2 {

  /**
   * KLM data quality monitor module.
   */
  class KLMCDCTrackModule : public HistoModule {

  protected:

    StoreArray<Track> m_Tracks;                   /**< Tracks. */
    StoreArray<RecoTrack> m_RecoTracks;           /**< RecoTracks. */
    std::string m_recoTrackArrayName = "" ;       /**< Belle2::RecoTrack StoreArray name. */

    //! BKLMTrack StoreArray
    StoreArray<BKLMTrack> m_storeTracks;

    /// Store Array of the input tracks
    StoreArray<RecoTrack> m_cdcRecoTracks;


  public:

    /**
     * Constructor.
     */
    KLMCDCTrackModule();

    /**
     * Destructor
     */
    ~KLMCDCTrackModule();

    /**
     * Definition of the histograms.
     */
    void defineHisto() override;

    /**
     * Initializer.
     */
    void initialize() override;

    /**
     * Called when entering a new run.
     */
    void beginRun() override;

    /**
     * This method is called for each event.
     */
    void event() override;

    /**
     * This method is called if the current run ends.
     */
    void endRun() override;

    /**
     * This method is called at the end of the event processing.
     */
    void terminate() override;

  private:
    //! bklm GeometryPar
    bklm::GeometryPar* m_GeoPar = nullptr;

    StoreObjPtr<EventMetaData> m_event_metadata;

    /* Number of hits from CDC track by eta,theta,phi */
    TProfile* m_klmcdc_eta;
    TProfile* m_klmcdc_theta;
    TProfile* m_klmcdc_phi;

    /* Same but just for the barrel */
    TProfile* m_klmcdc_b_eta;
    TProfile* m_klmcdc_b_theta;
    TProfile* m_klmcdc_b_phi;
    TH1F* m_phicounter;
    TProfile* m_BKLM_hits_mom;
    TProfile* m_BKLM_mom_phi;

    TProfile2D* m_bklm2d;
    TProfile2D* m_bklm2d_momcap;

    /* Same but just for the endcap */
    TProfile* m_klmcdc_e_eta;
    TProfile* m_klmcdc_e_theta;
    TProfile* m_klmcdc_e_phi;


    /** Directory for KLM DQM histograms in ROOT file. */
    std::string m_HistogramDirectoryName;
  };
}
