#include <cstdio>
#include <iostream>
#include <cstdint>
#include <vector>
#include <array>
#include <bitset>
#include <signal.h>
#include <math.h>
#include <sys/stat.h>

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH2D.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>

#include "Digi.h"
#include "Cluster.h"
#include "Rechit.h"
#include "Rechit2D.h"
#include "Hit.h"
#include "Track2D.h"
#include "DetectorTracker.h"
#include "DetectorLarge.h"

#include "progressbar.h"

bool isInterrupted = false;
void interruptHandler(int dummy) {
    isInterrupted = true;
}

int main (int argc, char** argv) {

    if (argc<3) {
      std::cout << "Usage: Tracks ifile ofile [--verbose] [--events n]" << std::endl;
      return 0;
    }
    std::string ifile   = argv[1];
    std::string ofile   = argv[2];
    
    int max_events = -1;
    bool verbose = false;
    double trackerAngles[4] = {
      .00102455514842, -.00105457956694, -.00002029043069, .00080694014877
    };
    for (int iarg=0; iarg<argc; iarg++) {
      std::string arg = argv[iarg];
      if (arg=="--verbose") verbose = true;
      else if (arg=="--events") max_events = atoi(argv[iarg+1]);
      else if (arg=="--angles") {
        std::cout << "angles: ";
        for (int iangle=0; iangle<4; iangle++) {
          trackerAngles[iangle] = atof(argv[iarg+iangle+1]);
          std::cout << trackerAngles[iangle] << " ";
        }
        std::cout << std::endl;
      }
    }

    if (max_events > 0) std::cout << "Analyzing " << max_events << " events" << std::endl;
    else std::cout << "Analyzing all events" << std::endl; 

    TFile rechitFile(ifile.c_str(), "READ");     
    TTree *rechitTree = (TTree *) rechitFile.Get("rechitTree");

    TFile trackFile(ofile.c_str(), "RECREATE", "Track file");
    TTree trackTree("trackTree", "trackTree");
  
    // create geometry objects
    DetectorTracker detectorTrackers[4] = {
      DetectorTracker(2, 0, 89.5, 89.5, 358),
      DetectorTracker(2, 1, 89.5, 89.5, 358),
      DetectorTracker(3, 2, 89.5, 89.5, 358),
      DetectorTracker(3, 3, 89.5, 89.5, 358),
    };
    DetectorLarge detectorsLarge[3] = {
      DetectorLarge(0, 4, 488.8, 628.8, 390.9, 4, 384), // GE2/1
      DetectorLarge(0, 5, 127.584, 434.985, 868.18, 8, 384), // ME0 blank
      DetectorLarge(0, 6, 127.584, 434.985, 868.18, 8, 384), // ME0 random
    };
    std::map<int, DetectorGeometry*> detectorsMap = {
      {4, &detectorsLarge[0]},
      {5, &detectorsLarge[1]},
      {6, &detectorsLarge[2]}
    };

    detectorTrackers[0].setPosition(+2.06244, +0.269035, -(697+254+294), trackerAngles[0]);
    detectorTrackers[1].setPosition(+0.207079, -0.292939, -(254+294), trackerAngles[1]);
    detectorTrackers[2].setPosition(-0.577936, +0.332708, 170., trackerAngles[2]);
    detectorTrackers[3].setPosition(-0.108215, -0.0905448, 170.+697., trackerAngles[3]);
    //detectorsLarge[0].setPosition(0., 0., 0., 0.); // todo: x and y alignment
    detectorsLarge[0].setPosition(-159.606, -214.9289, 0., -17.69e-3);
    //detectorsLarge[0].setPosition(-159.606, -220.9289, 0., 7*17.69e-3);
    //detectorsLarge[0].setPosition(-161.658, -219.081, 0., 0.);
    //detectorsLarge[0].setPosition(0., 0., 0., 0.015515778476258502);
    detectorsLarge[1].setPosition(0., 0., 0., 1.5707963267948966); // ME0 tilted by 90°, todo: x and y alignment
    detectorsLarge[2].setPosition(0., 0., 518.5, 0); // todo: x and y alignment
    
    // rechit variables
    int nrechits;
    std::vector<int> *vecRechitChamber = new std::vector<int>();
    std::vector<int> *vecRechitEta = new std::vector<int>();
    std::vector<double> *vecRechitX = new std::vector<double>();
    std::vector<double> *vecRechitY = new std::vector<double>();
    std::vector<double> *vecRechitError = new std::vector<double>();
    std::vector<double> *vecRechitClusterSize = new std::vector<double>();
    std::vector<int> *vecClusterCenter = new std::vector<int>();
    std::vector<int> *vecDigiStrip = new std::vector<int>();
    std::vector<int> *vecRawChannel = new std::vector<int>();
    // rechit2D variables
    int nrechits2d;
    std::vector<int> *vecRechit2DChamber = new std::vector<int>();
    std::vector<double> *vecRechit2D_X_Center = new std::vector<double>();
    std::vector<double> *vecRechit2D_Y_Center = new std::vector<double>();
    std::vector<double> *vecRechit2D_X_Error = new std::vector<double>();
    std::vector<double> *vecRechit2D_Y_Error = new std::vector<double>();
    std::vector<double> *vecRechit2D_X_ClusterSize = new std::vector<double>();
    std::vector<double> *vecRechit2D_Y_ClusterSize = new std::vector<double>();
    // rechit branches
    rechitTree->SetBranchAddress("nrechits", &nrechits);
    rechitTree->SetBranchAddress("rechitChamber", &vecRechitChamber);
    rechitTree->SetBranchAddress("rechitEta", &vecRechitEta);
    rechitTree->SetBranchAddress("rechitX", &vecRechitX);
    rechitTree->SetBranchAddress("rechitY", &vecRechitY);
    rechitTree->SetBranchAddress("rechitError", &vecRechitError);
    rechitTree->SetBranchAddress("clusterCenter", &vecClusterCenter);
    rechitTree->SetBranchAddress("rechitClusterSize", &vecRechitClusterSize);
    rechitTree->SetBranchAddress("digiStrip", &vecDigiStrip);
    rechitTree->SetBranchAddress("rawChannel", &vecRawChannel);
    // rechit2D branches
    rechitTree->SetBranchAddress("nrechits2d", &nrechits2d);
    rechitTree->SetBranchAddress("rechit2DChamber", &vecRechit2DChamber);
    rechitTree->SetBranchAddress("rechit2D_X_center", &vecRechit2D_X_Center);
    rechitTree->SetBranchAddress("rechit2D_Y_center", &vecRechit2D_Y_Center);
    rechitTree->SetBranchAddress("rechit2D_X_error", &vecRechit2D_X_Error);
    rechitTree->SetBranchAddress("rechit2D_Y_error", &vecRechit2D_Y_Error);
    rechitTree->SetBranchAddress("rechit2D_X_clusterSize", &vecRechit2D_X_ClusterSize);
    rechitTree->SetBranchAddress("rechit2D_Y_clusterSize", &vecRechit2D_Y_ClusterSize);

    const int nTrackingChambers = 4;
    // track variables
    std::vector<double> tracks_X_chi2;
    std::vector<double> tracks_Y_chi2;
    std::vector<double> tracks_X_slope;
    std::vector<double> tracks_Y_slope;
    std::vector<double> tracks_X_intercept;
    std::vector<double> tracks_Y_intercept;
    std::vector<double> tracks_X_covariance;
    std::vector<double> tracks_Y_covariance;
    // rechit 2D variables
    std::vector<int> rechits2D_Chamber;
    std::vector<double> rechits2D_X;
    std::vector<double> rechits2D_Y;
    std::vector<double> rechits2D_X_Error;
    std::vector<double> rechits2D_Y_Error;
    std::vector<double> rechits2D_X_ClusterSize;
    std::vector<double> rechits2D_Y_ClusterSize;
    std::vector<double> prophits2D_X;
    std::vector<double> prophits2D_Y;
    std::vector<double> prophits2D_X_Error;
    std::vector<double> prophits2D_Y_Error;
    // rechit and prophit variables
    std::vector<int> rechitsChamber, prophitsChamber;
    double trackChi2X, trackChi2Y;
    double trackCovarianceX, trackCovarianceY;
    double trackSlopeX, trackSlopeY;
    double trackInterceptX, trackInterceptY;
    std::vector<double> rechitsEta;
    std::vector<double> rechitsLocalX;
    std::vector<double> rechitsLocalY;
    std::vector<double> rechitsLocalR;
    std::vector<double> rechitsLocalPhi;
    std::vector<double> rechitsGlobalX;
    std::vector<double> rechitsGlobalY;
    std::vector<double> rechitsClusterSize;
    std::vector<double> prophitsEta;
    std::vector<double> prophitsGlobalX;
    std::vector<double> prophitsGlobalY;
    std::vector<double> prophitsErrorX;
    std::vector<double> prophitsErrorY;
    std::vector<double> prophitsLocalX;
    std::vector<double> prophitsLocalY;
    std::vector<double> prophitsLocalR;
    std::vector<double> prophitsLocalPhi;

    int chamber;
    double rechitX, rechitY;
    double rechitX_clusterSize, rechitY_clusterSize;
    double prophitX, prophitY;
    double propErrorX, propErrorY;

    // track branches
    trackTree.Branch("tracks_X_chi2", &tracks_X_chi2);
    trackTree.Branch("tracks_Y_chi2", &tracks_Y_chi2);
    trackTree.Branch("tracks_X_slope", &tracks_X_slope);
    trackTree.Branch("tracks_Y_slope", &tracks_Y_slope);
    trackTree.Branch("tracks_X_intercept", &tracks_X_intercept);
    trackTree.Branch("tracks_Y_intercept", &tracks_Y_intercept);
    trackTree.Branch("tracks_X_covariance", &tracks_X_covariance);
    trackTree.Branch("tracks_Y_covariance", &tracks_Y_covariance);

    // rechit 2D branches
    trackTree.Branch("rechits2D_Chamber", &rechits2D_Chamber);
    trackTree.Branch("rechits2D_X", &rechits2D_X);
    trackTree.Branch("rechits2D_Y", &rechits2D_Y);
    trackTree.Branch("rechits2D_X_Error", &rechits2D_X_Error);
    trackTree.Branch("rechits2D_Y_Error", &rechits2D_Y_Error);
    trackTree.Branch("rechits2D_X_ClusterSize", &rechits2D_X_ClusterSize);
    trackTree.Branch("rechits2D_Y_ClusterSize", &rechits2D_Y_ClusterSize);
    trackTree.Branch("prophits2D_X", &prophits2D_X);
    trackTree.Branch("prophits2D_Y", &prophits2D_Y);
    trackTree.Branch("prophits2D_X_Error", &prophits2D_X_Error);
    trackTree.Branch("prophits2D_Y_Error", &prophits2D_Y_Error);

    // rechit and prophit branches
    trackTree.Branch("trackChi2X", &trackChi2X, "trackChi2X/D");
    trackTree.Branch("trackChi2Y", &trackChi2Y, "trackChi2Y/D");
    trackTree.Branch("trackCovarianceX", &trackCovarianceX, "trackCovarianceX/D");
    trackTree.Branch("trackCovarianceY", &trackCovarianceY, "trackCovarianceY/D");
    trackTree.Branch("trackSlopeX", &trackSlopeX, "trackSlopeX/D");
    trackTree.Branch("trackSlopeY", &trackSlopeY, "trackSlopeY/D");
    trackTree.Branch("trackInterceptX", &trackInterceptX, "trackInterceptX/D");
    trackTree.Branch("trackInterceptY", &trackInterceptY, "trackInterceptY/D");
    trackTree.Branch("rechitChamber", &rechitsChamber);
    trackTree.Branch("prophitChamber", &prophitsChamber);
    trackTree.Branch("rechitEta", &rechitsEta);
    trackTree.Branch("rechitClusterCenter", vecClusterCenter);
    trackTree.Branch("rechitDigiStrip", vecDigiStrip);
    trackTree.Branch("rechitRawChannel", vecRawChannel);
    trackTree.Branch("rechitLocalX", &rechitsLocalX);
    trackTree.Branch("rechitLocalY", &rechitsLocalY);
    trackTree.Branch("rechitLocalR", &rechitsLocalR);
    trackTree.Branch("rechitLocalPhi", &rechitsLocalPhi);
    trackTree.Branch("rechitGlobalX", &rechitsGlobalX);
    trackTree.Branch("rechitGlobalY", &rechitsGlobalY);
    trackTree.Branch("rechitClusterSize", &rechitsClusterSize);
    trackTree.Branch("prophitEta", &prophitsEta);
    trackTree.Branch("prophitGlobalX", &prophitsGlobalX);
    trackTree.Branch("prophitGlobalY", &prophitsGlobalY);
    trackTree.Branch("prophitErrorX", &prophitsErrorX);
    trackTree.Branch("prophitErrorY", &prophitsErrorY);
    trackTree.Branch("prophitLocalX", &prophitsLocalX);
    trackTree.Branch("prophitLocalY", &prophitsLocalY);
    trackTree.Branch("prophitLocalR", &prophitsLocalR);
    trackTree.Branch("prophitLocalPhi", &prophitsLocalPhi);

    Track2D track;
    Rechit rechit;
    Rechit2D rechit2d;
    Hit hit;

    int nentries = rechitTree->GetEntries();
    int nentriesGolden = 0, nentriesNice = 0;
    // support array to exclude events with more than one hit per tracker:
    std::array<double, nTrackingChambers> hitsPerTrackingChamber;

    std::cout << nentries << " total events" <<  std::endl;
    if (max_events>0) nentries = max_events;
    progressbar bar(nentries);
    signal(SIGINT, interruptHandler);
    for (int nevt=0; (!isInterrupted) && rechitTree->LoadTree(nevt)>=0; ++nevt) {
      if ((max_events>0) && (nevt>max_events)) break;

      if (verbose) std::cout << "Event " << nevt << "/" << nentries << std::endl;
      else bar.update();

      /* reset support variables */
      for (int i=0; i<nTrackingChambers; i++) {
        hitsPerTrackingChamber[i] = 0;
      }
      /* reset branch variables */
      // tracker branches:
      tracks_X_chi2.clear();
      tracks_Y_chi2.clear();
      tracks_X_slope.clear();
      tracks_Y_slope.clear();
      tracks_X_intercept.clear();
      tracks_Y_intercept.clear();
      tracks_X_covariance.clear();
      tracks_Y_covariance.clear();
      rechits2D_Chamber.clear();
      rechits2D_X.clear();
      rechits2D_Y.clear();
      rechits2D_X_Error.clear();
      rechits2D_Y_Error.clear();
      rechits2D_X_ClusterSize.clear();
      rechits2D_Y_ClusterSize.clear();
      prophits2D_X.clear();
      prophits2D_Y.clear();
      prophits2D_X_Error.clear();
      prophits2D_Y_Error.clear();

      // large chamber branches:
      rechitsChamber.clear();
      prophitsChamber.clear();
      rechitsEta.clear();
      rechitsLocalX.clear();
      rechitsLocalY.clear();
      rechitsGlobalX.clear();
      rechitsGlobalY.clear();
      rechitsLocalR.clear();
      rechitsLocalPhi.clear();
      rechitsClusterSize.clear();
      prophitsEta.clear();
      prophitsGlobalX.clear();
      prophitsGlobalY.clear();
      prophitsErrorX.clear();
      prophitsErrorY.clear();
      prophitsLocalX.clear();
      prophitsLocalY.clear();
      prophitsLocalR.clear();
      prophitsLocalPhi.clear();

      rechitTree->GetEntry(nevt);

      // process event only if at least one 2D rechit per tracking chamber:
      bool isNiceEvent = true;
      for (int irechit=0; isNiceEvent && irechit<nrechits2d; irechit++) {
        chamber = vecRechit2DChamber->at(irechit);
        if (hitsPerTrackingChamber[chamber]>0) isNiceEvent = false;
        else hitsPerTrackingChamber[chamber]++;
        if (verbose) std::cout << "  Rechit in chamber " << chamber << std::endl;
      }

      // skip if it is not a nice event:
      if (!isNiceEvent) {
        if (verbose) {
          std::cout << "  Not nice, skipping event..." << std::endl; 
        }
        continue;
      }
      nentriesNice++;

      for (int testedChamber=0; testedChamber<nTrackingChambers; testedChamber++) {
        track.clear();
        // loop over rechits and make track:
        for (int irechit=0; irechit<nrechits2d; irechit++) {
          chamber = vecRechit2DChamber->at(irechit);
          rechit2d = Rechit2D(chamber,
            Rechit(chamber, vecRechit2D_X_Center->at(irechit), vecRechit2D_X_Error->at(irechit), vecRechit2D_X_ClusterSize->at(irechit)),
            Rechit(chamber, vecRechit2D_Y_Center->at(irechit), vecRechit2D_Y_Error->at(irechit), vecRechit2D_Y_ClusterSize->at(irechit))
          );
          // apply global geometry:
          detectorTrackers[chamber].mapRechit2D(&rechit2d);
          if (chamber!=testedChamber) {
            track.addRechit(rechit2d);
          } else {
            // add rechit to tree
            rechits2D_Chamber.push_back(chamber);
            rechits2D_X.push_back(rechit2d.getGlobalX());
            rechits2D_Y.push_back(rechit2d.getGlobalY());
            rechits2D_X_ClusterSize.push_back(rechit2d.getClusterSizeX());
            rechits2D_Y_ClusterSize.push_back(rechit2d.getClusterSizeY());
            rechits2D_X_Error.push_back(rechit2d.getErrorX());
            rechits2D_Y_Error.push_back(rechit2d.getErrorY());
          }
        }
        // fit and save track:
        track.fit();
        tracks_X_chi2.push_back(track.getChi2X());
        tracks_Y_chi2.push_back(track.getChi2Y());
        tracks_X_slope.push_back(track.getSlopeX());
        tracks_Y_slope.push_back(track.getSlopeY());
        tracks_X_intercept.push_back(track.getInterceptX());
        tracks_Y_intercept.push_back(track.getInterceptY());
        tracks_X_covariance.push_back(track.getCovarianceX());
        tracks_Y_covariance.push_back(track.getCovarianceY());

        // propagate to chamber under test:
        prophits2D_X.push_back(track.propagateX(detectorTrackers[testedChamber].getPositionZ()));
        prophits2D_Y.push_back(track.propagateY(detectorTrackers[testedChamber].getPositionZ()));
        prophits2D_X_Error.push_back(track.propagationErrorX(detectorTrackers[testedChamber].getPositionZ()));
        prophits2D_Y_Error.push_back(track.propagationErrorY(detectorTrackers[testedChamber].getPositionZ()));

        if (verbose) {
          std::cout << "  Chamber " << testedChamber << std::endl;
          std::cout << "    " << "track slope (" << track.getSlopeX() << "," << track.getSlopeY() << ")";
          std::cout << " " << "intercept (" << track.getInterceptX() << "," << track.getInterceptY() << ")";
          std::cout << std::endl;
          std::cout << "    " << "rechit (" << rechits2D_X.back();
          std::cout << ", " << rechits2D_Y.back() << ")";
          std::cout << "  " << "prophit (" << prophits2D_X.back();
          std::cout << ", " << prophits2D_Y.back() << ")";
          std::cout << std::endl;
        }
      }

      // build track with all 4 trackers
      track.clear();
      for (int irechit=0; irechit<nrechits2d; irechit++) {
        chamber = vecRechit2DChamber->at(irechit);
        rechit2d = Rechit2D(chamber,
          Rechit(chamber, vecRechit2D_X_Center->at(irechit), vecRechit2D_X_Error->at(irechit), vecRechit2D_X_ClusterSize->at(irechit)),
          Rechit(chamber, vecRechit2D_Y_Center->at(irechit), vecRechit2D_Y_Error->at(irechit), vecRechit2D_Y_ClusterSize->at(irechit))
        );
        detectorTrackers[chamber].mapRechit2D(&rechit2d); // apply local geometry
        track.addRechit(rechit2d);
      }
      track.fit();
      if (!track.isValid()) {
        trackChi2X = -1.;
        trackChi2Y = -1.;
      } else {
        trackChi2X = track.getChi2ReducedX();
        trackChi2Y = track.getChi2ReducedY();
        trackCovarianceX = track.getCovarianceX();
        trackCovarianceY = track.getCovarianceY();
        trackSlopeX = track.getSlopeX();
        trackSlopeY = track.getSlopeY();
        trackInterceptX = track.getInterceptX();
        trackInterceptY = track.getInterceptY();

        // extrapolate track on large detectors
        for (auto detector:detectorsLarge) {
          hit = track.propagate(&detector);
          prophitsChamber.push_back(detector.getChamber());
          prophitsEta.push_back(hit.getEta());
          prophitsGlobalX.push_back(hit.getGlobalX());
          prophitsGlobalY.push_back(hit.getGlobalY());
          prophitsErrorX.push_back(hit.getErrX());
          prophitsErrorY.push_back(hit.getErrY());
          prophitsLocalX.push_back(hit.getLocalX());
          prophitsLocalY.push_back(hit.getLocalY());
          prophitsLocalR.push_back(hit.getLocalR());
          prophitsLocalPhi.push_back(hit.getLocalPhi());

          if (verbose) {
            std::cout << "  Chamber " << detector.getChamber() << std::endl;
            std::cout << "    " << "track slope (" << track.getSlopeX() << "," << track.getSlopeY() << ")";
            std::cout << " " << "intercept (" << track.getInterceptX() << "," << track.getInterceptY() << ")";
            std::cout << std::endl;
            std::cout << "    " << "prophit " << "eta=" << hit.getEta() << ", ";
            std::cout << "global carthesian (" << hit.getGlobalX() << "," << hit.getGlobalY() << "), ";
            std::cout << "local carthesian (" << hit.getLocalX() << "," << hit.getLocalY() << "), ";
            std::cout << "local polar R=" << hit.getLocalR() << ", phi=" << hit.getLocalPhi();
            std::cout << std::endl;
          }
        }
      }

      // save all 1D rechits local coordinates
      for (int iRechit=0; iRechit<vecRechitChamber->size(); iRechit++) {
        chamber = vecRechitChamber->at(iRechit);
        if (verbose) std::cout << "  Chamber " << chamber << std::endl;
        if (detectorsMap.count(chamber)>0) {
          hit = Hit::fromLocal(detectorsMap.at(chamber),
            vecRechitX->at(iRechit), vecRechitY->at(iRechit), 0., 0., 0.
          );
        } else {
          if (verbose) std::cout << "    Skipping, no mapping found" << std::endl;
          continue;
        }
        rechitsChamber.push_back(chamber);
        rechitsEta.push_back(hit.getEta());
        rechitsLocalX.push_back(hit.getLocalX());
        rechitsLocalY.push_back(hit.getLocalY());
        rechitsLocalR.push_back(hit.getLocalR());
        rechitsLocalPhi.push_back(hit.getLocalPhi());
        rechitsGlobalX.push_back(hit.getGlobalX());
        rechitsGlobalY.push_back(hit.getGlobalY());
        rechitsClusterSize.push_back(vecRechitClusterSize->at(iRechit));
        if (verbose) {
          std::cout << "    " << "rechit  " << "eta=" << vecRechitEta->at(iRechit) << ", ";
          std::cout << "global carthesian (" << rechitsGlobalX.back() << "," << rechitsGlobalY.back() << "), ";
          std::cout << "local carthesian (" << hit.getLocalX() << "," << hit.getLocalY() << "), ";
          std::cout << "local polar R=" << hit.getLocalR() << ", phi=" << hit.getLocalPhi();
          std::cout << std::endl;
        }
      }

      trackTree.Fill();
    }
    std::cout << std::endl;
    std::cout << "Nice entries " << nentriesNice << std::endl;

    trackTree.Write();
    trackFile.Close();
    std::cout << "Output files written to " << ofile << std::endl;
}
