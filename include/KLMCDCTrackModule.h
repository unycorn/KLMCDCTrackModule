#pragma once

/* KLM headers. */
#include <klm/dataobjects/bklm/BKLMElementNumbers.h>
#include <klm/dataobjects/bklm/BKLMHit1d.h>
#include <klm/dataobjects/eklm/EKLMElementNumbers.h>
#include <klm/dataobjects/KLMHit2d.h>
#include <klm/dataobjects/KLMDigit.h>
#include <klm/dataobjects/KLMChannelArrayIndex.h>
#include <klm/dataobjects/KLMElementNumbers.h>
#include <klm/dataobjects/KLMSectorArrayIndex.h>

/* Belle 2 headers. */
#include <framework/core/HistoModule.h>
#include <framework/datastore/StoreArray.h>
#include <rawdata/dataobjects/RawFTSW.h>
#include <rawdata/dataobjects/RawKLM.h>

#include <mdst/dataobjects/Track.h>
#include <mdst/dataobjects/TrackFitResult.h>
#include <tracking/dataobjects/RecoTrack.h>

#include <cdc/dataobjects/CDCHit.h>
#include <cdc/dataobjects/CDCRawHit.h>
#include <framework/core/Module.h>


/* ROOT headers. */
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile.h>

namespace Belle2 {

  /**
   * KLM data quality monitor module.
   */
  class KLMCDCTrackModule : public HistoModule {

  protected:

    StoreArray<Track> m_Tracks;                   /**< Tracks. */
    StoreArray<RecoTrack> m_RecoTracks;           /**< RecoTracks. */
    std::string m_recoTrackArrayName = "" ;       /**< Belle2::RecoTrack StoreArray name. */


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
    /* Number of hits from CDC track by eta,theta,phi */
    TProfile* m_klmcdc_eta;
    TProfile* m_klmcdc_theta;
    TProfile* m_klmcdc_phi;

    /* Same but just for the barrel */
    TProfile* m_klmcdc_b_eta;
    TProfile* m_klmcdc_b_theta;
    TProfile* m_klmcdc_b_phi;

    /** Directory for KLM DQM histograms in ROOT file. */
    std::string m_HistogramDirectoryName;
  };
}
