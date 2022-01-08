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
      std::cout << "Usage: Tracks ifile outdir [--verbose] [--events n]" << std::endl;
      return 0;
    }
    std::string ifile   = argv[1];
    std::string outdir   = argv[2];
    if (mkdir(outdir.c_str(), 0755)==0) std::cout << "Created directory " << outdir << std::endl;
    else std::cout << "Failed creating directory " << outdir << ", maybe it already exists" << std::endl;
    std::string ofile = std::string(outdir+"/tracks.root");
    
    int max_events = -1;
    bool verbose = false;
    for (int iarg=0; iarg<argc; iarg++) {
      std::string arg = argv[iarg];
      if (arg=="--verbose") verbose = true;
      else if (arg=="--events") max_events = atoi(argv[iarg+1]); 
    }

    if (max_events > 0) std::cout << "Analyzing " << max_events << " events" << std::endl;
    else std::cout << "Analyzing all events" << std::endl; 

    TFile rechitFile(ifile.c_str(), "READ");
    //TFile rechitFile(ofile.c_str(), "RECREATE", "Rechit tree file");
     
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
    DetectorLarge detectorGe21(0, 4, 501.454, 659.804, 430.6, 4, 384);
    // TODO: add ME0

    detectorTrackers[0].setPosition(-0.269035, -2.06244, -(697+254+294));
    detectorTrackers[1].setPosition(+0.292939, -0.207079, -(254+294));
    detectorTrackers[2].setPosition(-0.332708, +0.577936, 170.);
    detectorTrackers[3].setPosition(+0.0905448, +0.108215, 170.+697.);
    //detectorGe21.setPosition(0., -215., 0.);
    detectorGe21.setPosition(0., 0., 0.);

    // rechit variables
    int nrechits;
    std::vector<int> *vecRechitChamber = new std::vector<int>();
    std::vector<int> *vecRechitEta = new std::vector<int>();
    std::vector<double> *vecRechitX = new std::vector<double>();
    std::vector<double> *vecRechitY = new std::vector<double>();
    std::vector<double> *vecRechitError = new std::vector<double>();
    std::vector<double> *vecRechitClusterSize = new std::vector<double>();
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
    rechitTree->SetBranchAddress("rechitClusterSize", &vecRechitClusterSize);
    // rechit2D branches
    rechitTree->SetBranchAddress("nrechits2d", &nrechits2d);
    rechitTree->SetBranchAddress("rechit2DChamber", &vecRechit2DChamber);
    rechitTree->SetBranchAddress("rechit2D_X_center", &vecRechit2D_X_Center);
    rechitTree->SetBranchAddress("rechit2D_Y_center", &vecRechit2D_Y_Center);
    rechitTree->SetBranchAddress("rechit2D_X_error", &vecRechit2D_X_Error);
    rechitTree->SetBranchAddress("rechit2D_Y_error", &vecRechit2D_Y_Error);
    rechitTree->SetBranchAddress("rechit2D_X_clusterSize", &vecRechit2D_X_ClusterSize);
    rechitTree->SetBranchAddress("rechit2D_Y_clusterSize", &vecRechit2D_Y_ClusterSize);

    const int nChambers = 4;
    // track variables
    std::array<double, nChambers> trackFitChi2;
    std::array<double, nChambers> tracks_X_slope;
    std::array<double, nChambers> tracks_Y_slope;
    std::array<double, nChambers> tracks_X_intercept;
    std::array<double, nChambers> tracks_Y_intercept;
    // rechit 2D variables
    std::array<double, nChambers> rechits2D_X;
    std::array<double, nChambers> rechits2D_Y;
    std::array<double, nChambers> rechits2D_X_Error;
    std::array<double, nChambers> rechits2D_Y_Error;
    std::array<double, nChambers> rechits2D_X_ClusterSize;
    std::array<double, nChambers> rechits2D_Y_ClusterSize;
    std::array<double, nChambers> prophits2D_X;
    std::array<double, nChambers> prophits2D_Y;
    std::array<double, nChambers> prophits2D_X_Error;
    std::array<double, nChambers> prophits2D_Y_Error;
    // rechit and prophit variables
    std::vector<int> rechitsChamber, prophitsChamber;
    std::vector<double> rechitsEta;
    std::vector<double> rechitsLocalX;
    std::vector<double> rechitsLocalY;
    std::vector<double> rechitsLocalR;
    std::vector<double> rechitsLocalPhi;
    std::vector<double> prophitsEta;
    std::vector<double> prophitsGlobalX;
    std::vector<double> prophitsGlobalY;
    std::vector<double> prophitsXError;
    std::vector<double> prophitsYError;
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
    trackTree.Branch("trackFitChi2", &trackFitChi2);
    trackTree.Branch("tracks_X_slope", &tracks_X_slope);
    trackTree.Branch("tracks_Y_slope", &tracks_Y_slope);
    trackTree.Branch("tracks_X_intercept", &tracks_X_intercept);
    trackTree.Branch("tracks_Y_intercept", &tracks_Y_intercept);
    // rechit 2D branches
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
    trackTree.Branch("rechitChamber", &rechitsChamber);
    trackTree.Branch("prophitsChamber", &prophitsChamber);
    trackTree.Branch("rechitEta", &rechitsEta);
    trackTree.Branch("rechitLocalX", &rechitsLocalX);
    trackTree.Branch("rechitLocalY", &rechitsLocalY);
    trackTree.Branch("rechitLocalR", &rechitsLocalR);
    trackTree.Branch("rechitLocalPhi", &rechitsLocalPhi);
    trackTree.Branch("prophitEta", &prophitsEta);
    trackTree.Branch("prophitGlobalX", &prophitsGlobalX);
    trackTree.Branch("prophitGlobalY", &prophitsGlobalY);
    trackTree.Branch("prophitXError", &prophitsXError);
    trackTree.Branch("prophitYError", &prophitsYError);
    trackTree.Branch("prophitLocalX", &prophitsLocalX);
    trackTree.Branch("prophitLocalY", &prophitsLocalY);
    trackTree.Branch("prophitLocalR", &prophitsLocalR);
    trackTree.Branch("prophitLocalPhi", &prophitsLocalPhi);

    Track2D track;
    Rechit rechit;
    Rechit2D rechit2d;
    Hit hit;

    gStyle->SetOptStat(0);
    TH2D profileHistograms[4];
    TCanvas *profileCanvases[4];
    for (int ichamber=0; ichamber<4; ichamber++) {
      profileCanvases[ichamber] = new TCanvas(
        std::string("h_profile_chamber"+std::to_string(ichamber)).c_str(),
        "", 800, 800
      );
      profileHistograms[ichamber] = TH2D(
        std::string("h_profile_chamber"+std::to_string(ichamber)).c_str(),
        ";x (cm);y (cm);",
        358, -44.75, 44.75, 358, -44.75, 44.75
      );
    }

    int nentries = rechitTree->GetEntries();
    int nentriesGolden = 0;

    double efficiencyGe21 = 0.; // TEMP HACK, REMOVE!

    std::cout << nentries << " total events" <<  std::endl;
    if (max_events>0) nentries = max_events;
    progressbar bar(nentries);
    signal(SIGINT, interruptHandler);
    for (int nevt=0; (!isInterrupted) && rechitTree->LoadTree(nevt)>=0; ++nevt) {
      if ((max_events>0) && (nevt>max_events)) break;

      if (verbose) std::cout << "Event " << nevt << "/" << nentries << std::endl;
      else bar.update();

      rechitsChamber.clear();
      prophitsChamber.clear();
      rechitsEta.clear();
      rechitsLocalX.clear();
      rechitsLocalY.clear();
      rechitsLocalR.clear();
      rechitsLocalPhi.clear();
      prophitsEta.clear();
      prophitsGlobalX.clear();
      prophitsGlobalY.clear();
      prophitsXError.clear();
      prophitsYError.clear();
      prophitsLocalX.clear();
      prophitsLocalY.clear();
      prophitsLocalR.clear();
      prophitsLocalPhi.clear();

      rechitTree->GetEntry(nevt);

      // process only if single rechit 2D per tracking chamber per event:
      if (nrechits2d!=nChambers) continue;
      bool isGoldenEvent = true;
      for (int irechit=0; irechit<nrechits2d; irechit++) {
        chamber = vecRechit2DChamber->at(irechit);
        if (chamber!=irechit) isGoldenEvent = false;
      }
      if (!isGoldenEvent) continue;
      nentriesGolden++;

      for (int testedChamber=0; testedChamber<nChambers; testedChamber++) {
        track.clear();
        // loop over rechits and make track:
        for (int irechit=0; irechit<nrechits2d; irechit++) {
          chamber = vecRechit2DChamber->at(irechit);
          rechit2d = Rechit2D(chamber,
            Rechit(chamber, vecRechit2D_X_Center->at(irechit), vecRechit2D_X_Error->at(irechit), vecRechit2D_X_ClusterSize->at(irechit)),
            Rechit(chamber, vecRechit2D_Y_Center->at(irechit), vecRechit2D_Y_Error->at(irechit), vecRechit2D_Y_ClusterSize->at(irechit))
          );
          detectorTrackers[chamber].mapRechit2D(&rechit2d); // apply global geometry
          if (chamber!=testedChamber) {
            track.addRechit(rechit2d);
          } else {
            // add rechit to tree
            rechits2D_X[testedChamber] = rechit2d.getGlobalX();
            rechits2D_Y[testedChamber] = rechit2d.getGlobalY();
            rechits2D_X_ClusterSize[testedChamber] = rechit2d.getClusterSizeX();
            rechits2D_Y_ClusterSize[testedChamber] = rechit2d.getClusterSizeY();
            rechits2D_X_Error[testedChamber] = rechit2d.getErrorX();
            rechits2D_Y_Error[testedChamber] = rechit2d.getErrorY();
          }
        }
        // fit and save track:
        track.fit();
        trackFitChi2[testedChamber] = track.getChi2X() + track.getChi2Y();
        tracks_X_slope[testedChamber] = track.getSlopeX();
        tracks_Y_slope[testedChamber] = track.getSlopeY();
        tracks_X_intercept[testedChamber] = track.getInterceptX();
        tracks_Y_intercept[testedChamber] = track.getInterceptY();

        // propagate to chamber under test:
        prophits2D_X[testedChamber] = track.propagateX(detectorTrackers[testedChamber].getPositionZ());
        prophits2D_Y[testedChamber] = track.propagateY(detectorTrackers[testedChamber].getPositionZ());
        prophits2D_X_Error[testedChamber] = track.propagationErrorX(detectorTrackers[testedChamber].getPositionZ());
        prophits2D_Y_Error[testedChamber] = track.propagationErrorY(detectorTrackers[testedChamber].getPositionZ());

        if (verbose) {
          std::cout << "  Chamber " << testedChamber << std::endl;
          std::cout << "    " << "track slope (" << track.getSlopeX() << "," << track.getSlopeY() << ")";
          std::cout << " " << "intercept (" << track.getInterceptX() << "," << track.getInterceptY() << ")";
          std::cout << std::endl;
          std::cout << "    " << "rechit (" << rechits2D_X[testedChamber];
          std::cout << ", " << rechits2D_Y[testedChamber] << ")";
          std::cout << "  " << "prophit (" << prophits2D_X[testedChamber];
          std::cout << ", " << prophits2D_Y[testedChamber] << ")";
          std::cout << std::endl;
        }

        // fill beam profile:
        // profileHistograms[testedChamber].Fill(rechits2D_X[testedChamber], rechits2D_Y[testedChamber]);
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

      // extrapolate track on GE2/1
      hit = track.propagate(&detectorGe21);
      prophitsChamber.push_back(detectorGe21.getChamber());
      prophitsGlobalX.push_back(hit.getGlobalX());
      prophitsGlobalY.push_back(hit.getGlobalY());
      prophitsXError.push_back(hit.getErrX());
      prophitsYError.push_back(hit.getErrY());
      prophitsLocalX.push_back(hit.getLocalX());
      prophitsLocalY.push_back(hit.getLocalY());
      prophitsLocalR.push_back(hit.getLocalR());
      prophitsLocalPhi.push_back(hit.getLocalPhi());

      if (verbose) {
        std::cout << "  Chamber GE2/1" << std::endl;
        std::cout << "    " << "track slope (" << track.getSlopeX() << "," << track.getSlopeY() << ")";
        std::cout << " " << "intercept (" << track.getInterceptX() << "," << track.getInterceptY() << ")";
        std::cout << std::endl;
        std::cout << "    " << "prophit " << "eta=" << hit.getEta() << ", ";
        std::cout << "global carthesian (" << hit.getGlobalX() << "," << hit.getGlobalY() << "), ";
        std::cout << "local carthesian (" << hit.getLocalX() << "," << hit.getLocalY() << "), ";
        std::cout << "local polar R=" << hit.getLocalR() << ", phi=" << hit.getLocalPhi();
        std::cout << std::endl;
      }
      
      // HACK TO CALCULATE EFFICIENCY, REMOVE!
      if (vecRechitChamber->size()>0) efficiencyGe21+=1.;

      // save all 1D rechits local coordinates
      for (int iRechit=0; iRechit<vecRechitChamber->size(); iRechit++) {
        chamber = vecRechitChamber->at(iRechit);
        hit = Hit::fromLocal(&detectorGe21,
          vecRechitX->at(iRechit), vecRechitY->at(iRechit), 0., 0., 0.
        );
        rechitsEta.push_back(hit.getEta());
        rechitsLocalX.push_back(hit.getLocalX());
        rechitsLocalY.push_back(hit.getLocalY());
        rechitsLocalR.push_back(hit.getLocalR());
        rechitsLocalPhi.push_back(hit.getLocalPhi());
        if (verbose) {
          std::cout << "    " << "rechit  " << "eta=" << vecRechitEta->at(iRechit) << ", ";
          std::cout << "global carthesian (" << hit.getGlobalX() << "," << hit.getGlobalY() << "), ";
          std::cout << "local carthesian (" << hit.getLocalX() << "," << hit.getLocalY() << "), ";
          std::cout << "local polar R=" << hit.getLocalR() << ", phi=" << hit.getLocalPhi();
          std::cout << std::endl;
        }
      }

      trackTree.Fill();
    }
    std::cout << std::endl;

    std::cout << "Efficiency GE2/1 ";
    std::cout << (int) efficiencyGe21 << "/" << nentriesGolden << " = ";
    efficiencyGe21 /= (double) nentriesGolden;
    std::cout << efficiencyGe21 << std::endl;

    std::cout << "Golden entries " << nentriesGolden << std::endl;

    trackTree.Write();
    trackFile.Close();

    // for (int ichamber=0; ichamber<4; ichamber++) {
    //   profileCanvases[ichamber]->cd();
    //   profileHistograms[ichamber].Draw("colz");
    //   profileCanvases[ichamber]->SaveAs(
    //     std::string(outdir+"/profile"+std::to_string(ichamber)+".png").c_str()
    //   );
    //   profileHistograms[ichamber].SaveAs(
    //     std::string(outdir+"/profile"+std::to_string(ichamber)+".root").c_str()
    //   );
    // }

    std::cout << "Output files written to " << outdir << std::endl;
}
