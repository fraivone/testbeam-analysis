#include <cstdio>
#include <iostream>
#include <cstdint>
#include <vector>
#include <array>
#include <bitset>
#include <signal.h>

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
#include "Rechit2D.h"

#include "progressbar.h"

bool isInterrupted = false;
void interruptHandler(int dummy) {
    isInterrupted = true;
}

int main (int argc, char** argv) {

    if (argc<3) {
      std::cout << "Usage: Tracks ifile outdir" << std::endl;
      return 0;
    }
    std::string ifile   = argv[1];
    std::string outdir   = argv[2];
    std::string ofile = std::string(outdir+"/tracks.root");
    
    int max_events = -1;
    if (argc>3) max_events = atoi(argv[3]);

    if (max_events > 0) std::cout << "Analyzing " << max_events << " events" << std::endl;
    else std::cout << "Analyzing all events" << std::endl; 

    TFile rechitFile(ifile.c_str(), "READ");
    //TFile rechitFile(ofile.c_str(), "RECREATE", "Rechit tree file");
     
    TTree *rechitTree = (TTree *) rechitFile.Get("rechitTree");

    TFile trackFile(ofile.c_str(), "RECREATE", "Track file");
    TTree trackTree("trackTree", "trackTree");
  
    // rechits 2d variables
    int nrechits2d;
    std::vector<int> *vecRechit2DChamber = new std::vector<int>();
    std::vector<double> *vecRechit2D_X_Center = new std::vector<double>();
    std::vector<double> *vecRechit2D_Y_Center = new std::vector<double>();
    std::vector<double> *vecRechit2D_X_Error = new std::vector<double>();
    std::vector<double> *vecRechit2D_Y_Error = new std::vector<double>();

    int chamber;
    Rechit2D rechit2D;
    double rechitX, rechitY;
    double prophitX, prophitY;
    double trackFitChi2;
    int trackFitIsValid;

    // rechit2D branches
    rechitTree->SetBranchAddress("nrechits2d", &nrechits2d);
    rechitTree->SetBranchAddress("rechit2DChamber", &vecRechit2DChamber);
    rechitTree->SetBranchAddress("rechit2D_X_center", &vecRechit2D_X_Center);
    rechitTree->SetBranchAddress("rechit2D_Y_center", &vecRechit2D_Y_Center);
    rechitTree->SetBranchAddress("rechit2D_X_error", &vecRechit2D_X_Error);
    rechitTree->SetBranchAddress("rechit2D_Y_error", &vecRechit2D_Y_Error);

    // prophit branches
    trackTree.Branch("trackFitIsValid", &trackFitIsValid, "trackFitIsValid/I");
    trackTree.Branch("trackFitChi2", &trackFitChi2, "trackFitChi2/D");
    trackTree.Branch("rechitX", &rechitX, "rechitX/D");
    trackTree.Branch("rechitY", &rechitY, "rechitY/D");
    trackTree.Branch("prophitX", &prophitX, "prophitX/D");
    trackTree.Branch("prophitY", &prophitY, "prophitY/D");

    // geometry, starting from ge2/1
    double zBari1 = -(697+254+294);
    double zBari2 = -(254+294);
    double zMe0 = -294;
    double zBari3 = 170;
    double zBari4 = 170+697;

    double zChamber[4] = { zBari1, zBari2, zBari3, zBari4 };

    double zStart = zBari1;
    double zEnd = zBari2;

    // linear track fit
    TF1 trackX("fTrackX", "[0]+[1]*x", zStart, zEnd);
    TF1 trackY("fTrackY", "[0]+[1]*x", zStart, zEnd);

    // linear track graph
    TGraphErrors graphX;
    TGraphErrors graphY;
    graphX.SetName("gTrackX");
    graphY.SetName("gTrackY");

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
    int fitGoodCount = 0, fitBadCount = 0;
    TFitResultPtr fitStatus1, fitStatus2;

    std::cout << nentries << " total events" <<  std::endl;
    progressbar bar(nentries);
    signal(SIGINT, interruptHandler);
    for (int nevt=0; (!isInterrupted) && rechitTree->LoadTree(nevt)>=0; ++nevt) {
      if ((max_events>0) && (nevt>max_events)) break;
      bar.update();

      rechitTree->GetEntry(nevt);

      // process only if single rechit per event:
      if (nrechits2d==0 || nrechits2d>4) continue;
      nentriesGolden++;

      trackX.SetParameters(0., 0.);
      trackY.SetParameters(0., 0.);

      for (int irechit=0; irechit<nrechits2d; irechit++) {
        // add to graph for fitting:
        chamber = vecRechit2DChamber->at(irechit);
        if (chamber<3) {
          graphX.SetPoint(chamber, zChamber[chamber], vecRechit2D_X_Center->at(irechit));
          graphX.SetPointError(chamber, 10, vecRechit2D_X_Error->at(irechit));
          graphY.SetPoint(chamber, zChamber[chamber], vecRechit2D_Y_Center->at(irechit));
          graphY.SetPointError(chamber, 10, vecRechit2D_Y_Error->at(irechit));
        } else if (chamber==3) {
          rechitX = vecRechit2D_X_Center->at(irechit);
          rechitY = vecRechit2D_Y_Center->at(irechit);
        }

        // fill beam profile:
        profileHistograms[chamber].Fill(
          vecRechit2D_X_Center->at(irechit),
          vecRechit2D_Y_Center->at(irechit)
        );
      }

      // fit track and propagate to chamber 3
      fitStatus1 = graphX.Fit(&trackX, "SQ");
      fitStatus2 = graphY.Fit(&trackY, "SQ");

      trackFitIsValid = fitStatus1->IsValid() && fitStatus2->IsValid();
      trackFitChi2 = fitStatus1->Chi2() * fitStatus2->Chi2();
      if (!trackFitIsValid) fitBadCount++;
      else fitGoodCount++;

      prophitX = trackX.Eval(zChamber[3]);
      prophitY = trackY.Eval(zChamber[3]);

      trackTree.Fill();
    }
    std::cout << std::endl;

    std::cout << "Golden entries " << nentriesGolden << std::endl;
    std::cout << "Failed fits " << fitBadCount << std::endl;
    std::cout << "Successful fits " << fitGoodCount << std::endl;

    trackTree.Write();
    trackFile.Close();

    for (int ichamber=0; ichamber<4; ichamber++) {
      profileCanvases[ichamber]->cd();
      profileHistograms[ichamber].Draw("colz");
      profileCanvases[ichamber]->SaveAs(
        std::string(outdir+"/profile"+std::to_string(ichamber)+".png").c_str()
      );
      profileHistograms[ichamber].SaveAs(
        std::string(outdir+"/profile"+std::to_string(ichamber)+".root").c_str()
      );
    }

    std::cout << "Output files written to " << outdir << std::endl;
}
