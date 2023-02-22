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
  m_klmcdc_theta{nullptr},
  m_klmcdc_phi{nullptr},
  m_klmcdc_b_eta{nullptr},
  m_klmcdc_b_theta{nullptr},
  m_klmcdc_b_phi{nullptr}
  
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

  /* KLM General Related. */
  /* Number of hits by Eta. */
  m_klmcdc_eta = new TProfile("KLM_CDC_tracks_by_eta",
                               "Mean KLM Hits per CDC track vs. #eta",
                               300, -3.142, 3.142);
  m_klmcdc_eta->GetXaxis()->SetTitle("Angle #eta");
  m_klmcdc_eta->GetYaxis()->SetTitle("Mean hits (EKLM + BKLM)");

  /* Number of hits by Eta. */
  m_klmcdc_theta = new TProfile("KLM_CDC_tracks_by_theta",
                               "Mean KLM Hits per CDC track vs. #theta",
                               300, -3.142, 3.142);
  m_klmcdc_theta->GetXaxis()->SetTitle("Angle #theta");
  m_klmcdc_theta->GetYaxis()->SetTitle("Mean hits (EKLM + BKLM)");
  
  
  /* Number of hits by Phi. */
  m_klmcdc_phi = new TProfile("KLM_CDC_tracks_by_phi",
                               "Mean KLM Hits per CDC track vs. #phi",
                               300, -3.142, 3.142);
  m_klmcdc_phi->GetXaxis()->SetTitle("Angle #phi");
  m_klmcdc_phi->GetYaxis()->SetTitle("Mean hits (EKLM + BKLM)");

  /* JUST BKLM */
  /* Number of BKLM hits by Eta. */
  m_klmcdc_b_eta = new TProfile("BKLM_CDC_tracks_by_eta",
                               "Mean BKLM Hits per CDC track vs. #eta",
                               300, -3.142, 3.142);
  m_klmcdc_b_eta->GetXaxis()->SetTitle("Angle #eta");
  m_klmcdc_b_eta->GetYaxis()->SetTitle("Mean hits (BKLM)");

  /* Number of BKLM hits by Eta. */
  m_klmcdc_b_theta = new TProfile("BKLM_CDC_tracks_by_theta",
                               "Mean BKLM Hits per CDC track vs. #theta",
                               300, -3.142, 3.142);
  m_klmcdc_b_theta->GetXaxis()->SetTitle("Angle #theta");
  m_klmcdc_b_theta->GetYaxis()->SetTitle("Mean hits (BKLM)");
  
  /* Number of BKLM hits by Phi. */
  m_klmcdc_b_phi = new TProfile("BKLM_CDC_tracks_by_phi",
                               "Mean BKLM Hits per CDC track vs. #phi",
                               300, -3.142, 3.142);
  m_klmcdc_b_phi->GetXaxis()->SetTitle("Angle #phi");
  m_klmcdc_b_phi->GetYaxis()->SetTitle("Mean hits (BKLM)");
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
}

void KLMCDCTrackModule::event()
{
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

    // Skip low momentum tracks (like bhabhas)
    if (track_momentum < 4) continue;

    unsigned int cdc_hit_count  = track->getNumberOfCDCHits();
    unsigned int bklm_hit_count = track->getNumberOfBKLMHits();
    unsigned int eklm_hit_count = track->getNumberOfEKLMHits();

    unsigned int klm_total_hits = bklm_hit_count + eklm_hit_count;


    // Track must have hits in both CDC and KLM to be logged
    if (cdc_hit_count == 0) continue;
    if (klm_total_hits == 0) continue;


    /**
     * Fill Plots with combined hit counts and exclusively barrel counts, representing each coord
     * */ 

    m_klmcdc_eta     ->  Fill(track_eta, klm_total_hits);
    m_klmcdc_theta   ->  Fill(track_theta, klm_total_hits);
    m_klmcdc_phi     ->  Fill(track_phi, klm_total_hits);

    m_klmcdc_b_eta   ->  Fill(track_eta, bklm_hit_count);
    m_klmcdc_b_theta ->  Fill(track_theta, bklm_hit_count);
    m_klmcdc_b_phi   ->  Fill(track_phi, bklm_hit_count);

  }
}

void KLMCDCTrackModule::endRun()
{
}

void KLMCDCTrackModule::terminate()
{
}
