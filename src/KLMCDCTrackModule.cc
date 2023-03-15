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
#include <cmath>

#define Pi 3.14159265358979323846


using namespace Belle2;
using namespace genfit;
using namespace TrackFindingCDC;
using namespace std;


REG_MODULE(KLMCDCTrack);

KLMCDCTrackModule::KLMCDCTrackModule() :
  HistoModule(),
  m_klmcdc_eta{nullptr},
  m_klmcdc_theta{nullptr},
  m_klmcdc_phi{nullptr},
  m_bklm2d{nullptr},
  m_bklm2d_momcap{nullptr},
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

  m_bklm2d = new TProfile2D("bklm_2d", "BKLM Efficiency by phi and theta",
                            40, -3.142, 3.142,
                            40, 0.64, 2.27);
  m_bklm2d->GetXaxis()->SetTitle("Angle #phi");
  m_bklm2d->GetYaxis()->SetTitle("Angle #theta");

  m_bklm2d_momcap = new TProfile2D("bklm_2d_momcap", "BKLM Efficiency by phi and theta",
                            40, -3.142, 3.142,
                            40, 0.64, 2.27);
  m_bklm2d_momcap->GetXaxis()->SetTitle("Angle #phi");
  m_bklm2d_momcap->GetYaxis()->SetTitle("Angle #theta");

  /* Number of hits by Eta. */
  m_klmcdc_eta = new TProfile("bklm_eta_mcap",
                               "Mean KLM Hits per CDC track vs. #eta",
                               resolution, -3.142, 3.142, 0, 1);
  m_klmcdc_eta->GetXaxis()->SetTitle("Angle #eta");
  m_klmcdc_eta->GetYaxis()->SetTitle("Efficiency");
  m_klmcdc_eta->GetYaxis()->SetRange(0.0,1.0);

  /* Number of hits by Eta. */
  m_klmcdc_theta = new TProfile("bklm_theta_mcap",
                               "Mean KLM Hits per CDC track vs. #theta",
                               resolution, 0.64, 2.27, 0, 1);
  m_klmcdc_theta->GetXaxis()->SetTitle("Angle #theta");
  m_klmcdc_theta->GetYaxis()->SetTitle("Efficiency");
  m_klmcdc_theta->GetYaxis()->SetRange(0.0,1.0);
  
  
  /* Number of hits by Phi. */
  m_klmcdc_phi = new TProfile("bklm_phi_mcap",
                               "Mean KLM Hits per CDC track vs. #phi",
                               resolution, -3.142, 3.142, 0, 1);
  m_klmcdc_phi->GetXaxis()->SetTitle("Angle #phi");
  m_klmcdc_phi->GetYaxis()->SetTitle("Efficiency");
  m_klmcdc_phi->GetYaxis()->SetRange(0.0,1.0);

  /* JUST BKLM */
  /* Number of BKLM hits by Eta. */
  m_klmcdc_b_eta = new TProfile("bklm_eta",
                               "Mean BKLM Hits per CDC track vs. #eta",
                               resolution, -3.142, 3.142, 0, 1);
  m_klmcdc_b_eta->GetXaxis()->SetTitle("Angle #eta");
  m_klmcdc_b_eta->GetYaxis()->SetTitle("Mean hits (BKLM)");
  m_klmcdc_b_eta->GetYaxis()->SetRange(0.0,1.0);

  /* Number of BKLM hits by Eta. */
  m_klmcdc_b_theta = new TProfile("bklm_theta",
                               "Mean BKLM Hits per CDC track vs. #theta",
                               resolution, 0.64, 2.27, 0, 1);
  m_klmcdc_b_theta->GetXaxis()->SetTitle("Angle #theta");
  m_klmcdc_b_theta->GetYaxis()->SetTitle("Mean hits (BKLM)");
  m_klmcdc_b_theta->GetYaxis()->SetRange(0.0,1.0);
  
  /* Number of BKLM hits by Phi. */
  m_klmcdc_b_phi = new TProfile("bklm_phi",
                               "Mean BKLM Hits per CDC track vs. #phi",
                               resolution, -3.142, 3.142, 0, 1);
  m_klmcdc_b_phi->GetXaxis()->SetTitle("Angle #phi");
  m_klmcdc_b_phi->GetYaxis()->SetTitle("Mean hits (BKLM)");
  m_klmcdc_b_phi->GetYaxis()->SetRange(0.0,1.0);

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
                               resolution, 0, 3.142);
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
  m_bklm2d->Reset();
  m_bklm2d_momcap->Reset();
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
  
  double layerInnerRadii[] = {201.900000,
                              209.964450,
                              219.064450,
                              228.164450,
                              237.264450,
                              246.364450,
                              255.464450,
                              264.564450,
                              273.664450,
                              282.764450,
                              291.864450,
                              300.964450,
                              310.064450,
                              319.164450,
                              328.264450
                              };
  
  double min_z = -180.0;
  double max_z = 275.0;

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

    auto track_momentum  =   pvector.Mag();
    auto track_eta       =   pvector.Eta();
    auto track_theta     =   pvector.Theta();
    auto track_phi       =   pvector.Phi();

    /**
     * Determine number of barrel layers track could cross by
     * modulating phi to take advantage of octagonal symmetry,
     * projecting onto plane and determining if intersection
     * point is within bounds of the detector
     * */ 

    double modphi = abs(fmod(track_phi, (Pi / 4)));
    if (modphi > Pi/8)  modphi -= (Pi / 4) ;

    double r_max, r_min;
    int layers_denom = 0;
    for (const auto& R : layerInnerRadii) {
      r_max = max_z * cos(modphi) * tan(track_theta);
      r_min = min_z * cos(modphi) * tan(track_theta);

      if (r_min > r_max) swap(r_min, r_max);

      if(R < r_max && R > r_min) {
        layers_denom++;
      } else {
        break;
      }
    }

    unsigned int cdc_hit_count  = track->getNumberOfCDCHits();
    unsigned int bklm_hit_count = track->getNumberOfBKLMHits();
    unsigned int eklm_hit_count = track->getNumberOfEKLMHits();

    unsigned int klm_total_hits = bklm_hit_count + eklm_hit_count;

    // Track must have hits in both CDC and KLM to be logged
    if (cdc_hit_count == 0) continue;
    if (klm_total_hits == 0) continue;

    if (bklm_hit_count > layers_denom) {
      B2INFO("Barrel Layer Info" << LogVar("theta", track_theta) << LogVar("phi", track_phi)
            << LogVar("layer count", layers_denom) << LogVar("bklmHits", bklm_hit_count));
      // Extremely rarely (1 in 10,000), a plane will be hit that is techinically impossible for the given phi/theta
      // these really close edge cases when graphed but the following fixes it
      layers_denom = bklm_hit_count;
    }


    m_BKLM_hits_mom  ->  Fill(track_momentum, bklm_hit_count);
    m_BKLM_mom_phi   ->  Fill(track_phi, track_momentum);
    // Skip low momentum tracks (like bhabhas)
    //if (track_momentum < 4.6) continue;
    //if (track_momentum > 9) continue;

    /**
     * Fill Plots for barrel and endcap
     * */ 

    if(bklm_hit_count > 0) {

      if (layers_denom == 0) break;
      
      double efficiency = (double)bklm_hit_count/(double)layers_denom;
      B2INFO(LogVar("EFFICIENCY", efficiency));

      m_bklm2d         ->  Fill(track_phi, track_theta, efficiency);

      m_klmcdc_b_eta   ->  Fill(track_eta, efficiency);
      m_klmcdc_b_theta ->  Fill(track_theta, efficiency);
      m_klmcdc_b_phi   ->  Fill(track_phi, efficiency);
      m_phicounter     ->  Fill(track_phi, 1);

      if (track_momentum > 4.7) {
        m_bklm2d_momcap->  Fill(track_phi, track_theta, efficiency);

        m_klmcdc_eta   ->  Fill(track_eta, efficiency);
        m_klmcdc_theta ->  Fill(track_theta, efficiency);
        m_klmcdc_phi   ->  Fill(track_phi, efficiency);
      }
    }

    if(eklm_hit_count > 0) {
      m_klmcdc_e_eta   ->  Fill(track_eta, eklm_hit_count);
      m_klmcdc_e_theta ->  Fill(track_theta, eklm_hit_count);
      m_klmcdc_e_phi   ->  Fill(track_phi, eklm_hit_count);
    }
  }
}

void KLMCDCTrackModule::endRun()
{
}

void KLMCDCTrackModule::terminate()
{
}
