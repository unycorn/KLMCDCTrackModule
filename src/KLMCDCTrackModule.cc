/**************************************************************************
 * basf2 (Belle II Analysis Software Framework)                           *
 * Author: The Belle II Collaboration                                     *
 *                                                                        *
 * See git log for contributors and copyright holders.                    *
 * This file is licensed under LGPL-3.0, see LICENSE.md.                  *
 **************************************************************************/

/* Own header. */
#include <klm/modules/KLMCDCTrack/KLMCDCTrackModule.h>

/* KLM headers. */
#include <klm/dataobjects/KLMChannelIndex.h>
#include <klm/dataobjects/KLMDigitRaw.h>

/* ROOT headers. */
#include <TDirectory.h>

#include <iostream>

using namespace Belle2;

REG_MODULE(KLMCDCTrack);

KLMCDCTrackModule::KLMCDCTrackModule() :
  HistoModule(),
  m_klmcdc_eta{nullptr},
  m_klmcdc_theta{nullptr}
  
{
  setDescription("KLM data quality monitor.");
  setPropertyFlags(c_ParallelProcessingCertified);
  addParam("histogramDirectoryName", m_HistogramDirectoryName,
           "Directory for KLM DQM histograms in ROOT file.",
           std::string("KLM"));
}

KLMCDCTrackModule::~KLMCDCTrackModule()
{
  
}

void KLMCDCTrackModule::defineHisto()
{
  TDirectory* newDirectory{gDirectory->mkdir(m_HistogramDirectoryName.c_str())};
  TDirectory::TContext context{gDirectory, newDirectory};

  /* Number of hits by Eta. */
  /* KLM General Related. */
  m_klmcdc_eta = new TProfile("KLM_CDC_tracks_by_eta",
                               "Mean KLM Hits per CDC track vs. #eta",
                               300, -3.142, 3.142);
  m_klmcdc_eta->GetXaxis()->SetTitle("Angle #eta");
  m_klmcdc_eta->GetYaxis()->SetTitle("Mean hits (EKLM + BKLM)");

  /* Number of hits by Eta. */
  /* KLM General Related. */
  m_klmcdc_theta = new TProfile("KLM_CDC_tracks_by_theta",
                               "Mean KLM Hits per CDC track vs. #theta",
                               300, -3.142, 3.142);
  m_klmcdc_theta->GetXaxis()->SetTitle("Angle #theta");
  m_klmcdc_theta->GetYaxis()->SetTitle("Mean hits (EKLM + BKLM)");
  
  //m_klmcdc_eta->Draw("HIST");
}

void KLMCDCTrackModule::initialize()
{
  REG_HISTOGRAM;
}

void KLMCDCTrackModule::beginRun()
{
  m_klmcdc_eta->Reset();
  m_klmcdc_theta->Reset();
}

void KLMCDCTrackModule::event()
{
  int trackCount = 0;
  for (const auto& b2track : m_Tracks) {

    // Make sure this is an actual track
    const Belle2::TrackFitResult* fitresult = b2track.getTrackFitResultWithClosestMass(Const::muon);
    if (!fitresult) {
      B2WARNING("No track fit result found.");
      continue;
    }

    Belle2::RecoTrack* track = b2track.getRelatedTo<Belle2::RecoTrack>(m_recoTrackArrayName);
    if (!track) {
      B2WARNING("Can not access RecoTrack of this Belle2::Track");
      continue;
    }

    trackCount++;  // Counter for debug purposes

    auto track_momentum = fitresult->getMomentum().Mag2();
    auto track_eta = fitresult->getMomentum().Eta();
    auto track_theta = fitresult->getMomentum().Theta();
    //B2INFO("Track Data" << LogVar("track momentum", track_momentum) << LogVar("track eta", track_eta));

    // Skip low momentum tracks (like bhabhas)
    if (track_momentum < 4) continue;

    unsigned int cdc_hit_count = track->getNumberOfCDCHits();
    unsigned int bklm_hit_count = track->getNumberOfBKLMHits();
    unsigned int eklm_hit_count = track->getNumberOfEKLMHits();

    unsigned int klm_total_hits = bklm_hit_count + eklm_hit_count;

    // Track must have hits in both CDC and KLM to be logged
    if (cdc_hit_count == 0) continue;
    if (klm_total_hits == 0) continue;

    m_klmcdc_eta->Fill(track_eta, klm_total_hits);
    m_klmcdc_theta->Fill(track_theta, klm_total_hits);
  }
  B2INFO("Track Count" << LogVar("Track Count", trackCount));
}

void KLMCDCTrackModule::endRun()
{
  //m_klmcdc_eta->Draw("HIST");
}

void KLMCDCTrackModule::terminate()
{
}
