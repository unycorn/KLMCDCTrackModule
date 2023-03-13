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
#include <klm/bklm/geometry/GeometryPar.h>

#include <tracking/trackFitting/fitter/base/TrackFitter.h>
#include <cdc/geometry/CDCGeometryPar.h>

/* ROOT headers. */
#include <TDirectory.h>

#include <iostream>



using namespace Belle2;
using namespace genfit;
using namespace TrackFindingCDC;


REG_MODULE(KLMCDCTrack);

KLMCDCTrackModule::KLMCDCTrackModule() :
  HistoModule(),
  m_klmcdc_eta{nullptr},
  m_klmcdc_theta{nullptr},
  m_klmcdc_phi{nullptr},
  m_klmcdc_b_eta{nullptr},
  m_klmcdc_b_theta{nullptr},
  m_klmcdc_b_phi{nullptr},
  m_BKLM_mom_phi{nullptr},
  m_phicounter{nullptr},
  m_BKLM_hits_mom{nullptr},
  m_klmcdc_e_eta{nullptr},
  m_klmcdc_e_theta{nullptr},
  m_klmcdc_e_phi{nullptr}
  
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

  int resolution = 100;

  /* KLM General Related. */
  /* Number of hits by Eta. */
  m_klmcdc_eta = new TProfile("KLM_CDC_tracks_by_eta",
                               "Mean KLM Hits per CDC track vs. #eta",
                               resolution, -3.142, 3.142);
  m_klmcdc_eta->GetXaxis()->SetTitle("Angle #eta");
  m_klmcdc_eta->GetYaxis()->SetTitle("Mean hits (EKLM + BKLM)");

  /* Number of hits by Eta. */
  m_klmcdc_theta = new TProfile("KLM_CDC_tracks_by_theta",
                               "Mean KLM Hits per CDC track vs. #theta",
                               resolution, 0, 3.142*2);
  m_klmcdc_theta->GetXaxis()->SetTitle("Angle #theta");
  m_klmcdc_theta->GetYaxis()->SetTitle("Mean hits (EKLM + BKLM)");
  
  
  /* Number of hits by Phi. */
  m_klmcdc_phi = new TProfile("KLM_CDC_tracks_by_phi",
                               "Mean KLM Hits per CDC track vs. #phi",
                               resolution, -3.142, 3.142);
  m_klmcdc_phi->GetXaxis()->SetTitle("Angle #phi");
  m_klmcdc_phi->GetYaxis()->SetTitle("Mean hits (EKLM + BKLM)");

  /* JUST BKLM */
  /* Number of BKLM hits by Eta. */
  m_klmcdc_b_eta = new TProfile("BKLM_CDC_tracks_by_eta",
                               "Mean BKLM Hits per CDC track vs. #eta",
                               resolution, -3.142, 3.142);
  m_klmcdc_b_eta->GetXaxis()->SetTitle("Angle #eta");
  m_klmcdc_b_eta->GetYaxis()->SetTitle("Mean hits (BKLM)");

  /* Number of BKLM hits by Eta. */
  m_klmcdc_b_theta = new TProfile("BKLM_CDC_tracks_by_theta",
                               "Mean BKLM Hits per CDC track vs. #theta",
                               resolution, 0, 3.142*2);
  m_klmcdc_b_theta->GetXaxis()->SetTitle("Angle #theta");
  m_klmcdc_b_theta->GetYaxis()->SetTitle("Mean hits (BKLM)");
  
  /* Number of BKLM hits by Phi. */
  m_klmcdc_b_phi = new TProfile("BKLM_CDC_tracks_by_phi",
                               "Mean BKLM Hits per CDC track vs. #phi",
                               resolution, -3.142, 3.142);
  m_klmcdc_b_phi->GetXaxis()->SetTitle("Angle #phi");
  m_klmcdc_b_phi->GetYaxis()->SetTitle("Mean hits (BKLM)");

  /* Number of matched tracks by phi */
  m_phicounter = new TH1F("phicounter",
                               "number of tracks matching BKLM vs. #phi",
                               resolution, -3.142, 3.142);
  m_phicounter->GetXaxis()->SetTitle("Angle #phi");
  m_phicounter->GetYaxis()->SetTitle("Tracks");

  
  /* Momentum by phi */
  m_BKLM_mom_phi = new TProfile("momphi",
                               "Momentum of BKLM tracks by phi",
                               resolution, -3.142, 3.142);
  m_BKLM_mom_phi->GetXaxis()->SetTitle("Angle #phi");
  m_BKLM_mom_phi->GetYaxis()->SetTitle("Momentum");

  /* Means BKLM hits per track by momentum */
  int max_mom = 30;
  m_BKLM_hits_mom = new TProfile("BKLM_hits_mom",
                               "Mean BKLM Hits per CDC track vs. momentum",
                               max_mom * 100, 0, max_mom);
  m_BKLM_hits_mom->GetXaxis()->SetTitle("Momentum");
  m_BKLM_hits_mom->GetYaxis()->SetTitle("Mean hits (BKLM)");

  /* JUST EKLM */
  /* Number of BKLM hits by Eta. */
  m_klmcdc_e_eta = new TProfile("EKLM_CDC_tracks_by_eta",
                               "Mean EKLM Hits per CDC track vs. #eta",
                               resolution, -3.142, 3.142);
  m_klmcdc_e_eta->GetXaxis()->SetTitle("Angle #eta");
  m_klmcdc_e_eta->GetYaxis()->SetTitle("Mean hits (EKLM)");

  /* Number of BKLM hits by Eta. */
  m_klmcdc_e_theta = new TProfile("EKLM_CDC_tracks_by_theta",
                               "Mean EKLM Hits per CDC track vs. #theta",
                               resolution, 0, 3.142*2);
  m_klmcdc_e_theta->GetXaxis()->SetTitle("Angle #theta");
  m_klmcdc_e_theta->GetYaxis()->SetTitle("Mean hits (EKLM)");
  
  /* Number of BKLM hits by Phi. */
  m_klmcdc_e_phi = new TProfile("EKLM_CDC_tracks_by_phi",
                               "Mean EKLM Hits per CDC track vs. #phi",
                               resolution, -3.142, 3.142);
  m_klmcdc_e_phi->GetXaxis()->SetTitle("Angle #phi");
  m_klmcdc_e_phi->GetYaxis()->SetTitle("Mean hits (EKLM)");
}

void KLMCDCTrackModule::initialize()
{
  REG_HISTOGRAM;
}

void KLMCDCTrackModule::beginRun()
{
  m_klmcdc_eta->Reset();
  m_klmcdc_theta->Reset();
  m_klmcdc_phi->Reset();
  m_klmcdc_b_eta->Reset();
  m_klmcdc_b_theta->Reset();
  m_klmcdc_b_phi->Reset();
  m_phicounter->Reset();
  m_BKLM_hits_mom->Reset();
  m_BKLM_mom_phi->Reset();
  m_klmcdc_e_eta->Reset();
  m_klmcdc_e_theta->Reset();
  m_klmcdc_e_phi->Reset();
}

void KLMCDCTrackModule::event()
{
  //m_GeoPar = bklm::GeometryPar::instance();
  //DBObjPtr<BKLMGeometryPar> GeoPar;
  //m_GeoPar = bklm::GeometryPar::instance(*GeoPar);

  //B2INFO("Layer radius 0 " << m_GeometryBKLM->getLayerInnerRadius(0));
  //B2INFO("Layer radius 14" << m_GeometryBKLM->getLayerInnerRadius(14));
  //B2INFO("module length" << m_GeometryBKLM->getModuleLength());

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

    auto pvector = fitresult->getMomentum();

    auto track_momentum  =   pvector.R();
    auto track_eta       =   pvector.Eta();
    auto track_theta     =   pvector.Theta();
    auto track_phi       =   pvector.Phi();

    unsigned int cdc_hit_count  = track->getNumberOfCDCHits();
    unsigned int bklm_hit_count = track->getNumberOfBKLMHits();
    unsigned int eklm_hit_count = track->getNumberOfEKLMHits();

    unsigned int klm_total_hits = bklm_hit_count + eklm_hit_count;

    // Track must have hits in both CDC and KLM to be logged
    if (cdc_hit_count == 0) continue;
    if (klm_total_hits == 0) continue;

    m_BKLM_hits_mom  ->  Fill(track_momentum, bklm_hit_count);
    m_BKLM_mom_phi   ->  Fill(track_phi, track_momentum);
    // Skip low momentum tracks (like bhabhas)
    //if (track_momentum < 4.6) continue;
    //if (track_momentum > 9) continue;

    /**
     * Fill Plots with combined hit counts and exclusively barrel counts, representing each coord
     * */ 

    if(bklm_hit_count > 0) {
      m_klmcdc_b_eta   ->  Fill(track_eta, bklm_hit_count);
      m_klmcdc_b_theta ->  Fill(track_theta, bklm_hit_count);
      m_klmcdc_b_phi   ->  Fill(track_phi, bklm_hit_count);
      m_phicounter     ->  Fill(track_phi, 1);
    }
    if(eklm_hit_count > 0) {
      m_klmcdc_e_eta   ->  Fill(track_eta, eklm_hit_count);
      m_klmcdc_e_theta ->  Fill(track_theta, eklm_hit_count);
      m_klmcdc_e_phi   ->  Fill(track_phi, eklm_hit_count);
    }

    if (track_momentum < 4.7) continue;
    if(bklm_hit_count > 0) {
      m_klmcdc_eta     ->  Fill(track_eta, bklm_hit_count);
      m_klmcdc_theta   ->  Fill(track_theta, bklm_hit_count);
      m_klmcdc_phi     ->  Fill(track_phi, bklm_hit_count);
    }
  }
}

void KLMCDCTrackModule::endRun()
{
}

void KLMCDCTrackModule::terminate()
{
}
