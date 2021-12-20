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
    if (mkdir(outdir.c_str(), 0755)==0) std::cout << "Created directory " << outdir << std::endl;
    else std::cout << "Failed creating directory " << outdir << ", maybe it already exists" << std::endl;
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
    std::vector<double> *vecRechit2D_X_ClusterSize = new std::vector<double>();
    std::vector<double> *vecRechit2D_Y_ClusterSize = new std::vector<double>();
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
    // track & rechit 2D variables
    std::array<double, nChambers> rechits2D_X;
    std::array<double, nChambers> rechits2D_Y;
    std::array<double, nChambers> rechits2D_X_Error;
    std::array<double, nChambers> rechits2D_Y_Error;
    std::array<double, nChambers> rechits2D_X_ClusterSize;
    std::array<double, nChambers> rechits2D_Y_ClusterSize;
    std::array<double, nChambers> trackFitChi2;
    std::array<int, nChambers> trackFitIsValid;
    std::array<double, nChambers> tracks_X_slope;
    std::array<double, nChambers> tracks_Y_slope;
    std::array<double, nChambers> tracks_X_intercept;
    std::array<double, nChambers> tracks_Y_intercept;
    std::array<double, nChambers> prophits2D_X;
    std::array<double, nChambers> prophits2D_Y;
    std::array<double, nChambers> prophits2D_X_Error;
    std::array<double, nChambers> prophits2D_Y_Error;

    int chamber;
    Rechit2D rechit2D;
    double rechitX, rechitY;
    double rechitX_clusterSize, rechitY_clusterSize;
    double prophitX, prophitY;
    double propErrorX, propErrorY;

    // track branches
    trackTree.Branch("rechits2D_X", &rechits2D_X);
    trackTree.Branch("rechits2D_Y", &rechits2D_Y);
    trackTree.Branch("rechits2D_X_Error", &rechits2D_X_Error);
    trackTree.Branch("rechits2D_Y_Error", &rechits2D_Y_Error);
    trackTree.Branch("rechits2D_X_ClusterSize", &rechits2D_X_ClusterSize);
    trackTree.Branch("rechits2D_Y_ClusterSize", &rechits2D_Y_ClusterSize);
    trackTree.Branch("trackFitIsValid", &trackFitIsValid);
    trackTree.Branch("trackFitChi2", &trackFitChi2);
    trackTree.Branch("tracks_X_slope", &tracks_X_slope);
    trackTree.Branch("tracks_Y_slope", &tracks_Y_slope);
    trackTree.Branch("tracks_X_intercept", &tracks_X_intercept);
    trackTree.Branch("tracks_Y_intercept", &tracks_Y_intercept);
    trackTree.Branch("prophits2D_X", &prophits2D_X);
    trackTree.Branch("prophits2D_Y", &prophits2D_Y);
    trackTree.Branch("prophits2D_X_Error", &prophits2D_X_Error);
    trackTree.Branch("prophits2D_Y_Error", &prophits2D_Y_Error);

    // prophit branches
    // trackTree.Branch("trackFitIsValid", &trackFitIsValid, "trackFitIsValid/I");
    // trackTree.Branch("trackFitChi2", &trackFitChi2, "trackFitChi2/D");
    // trackTree.Branch("rechitX", &rechitX, "rechitX/D");
    // trackTree.Branch("rechitY", &rechitY, "rechitY/D");
    // trackTree.Branch("rechitXClusterSize", &rechitX_clusterSize, "rechitX_clusterSize/D");
    // trackTree.Branch("rechitYClusterSize", &rechitY_clusterSize, "rechitY_clusterSize/D");
    // trackTree.Branch("prophitX", &prophitX, "prophitX/D");
    // trackTree.Branch("prophitY", &prophitY, "prophitY/D");
    // trackTree.Branch("propErrorX", &propErrorX, "propErrorX/D");
    // trackTree.Branch("propErrorY", &propErrorY, "propErrorY/D");

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

    // sample track used for propagation error
    TF1 sampleTrackX("fSampleTrackX", "[0]+[1]*x", zStart, zEnd);
    TF1 sampleTrackY("fSampleTrackY", "[0]+[1]*x", zStart, zEnd);
    srand(time(NULL));
    // sample track graph
    TGraphErrors sampleGraphX;
    TGraphErrors sampleGraphY;
    sampleGraphX.SetName("gSampleTrackX");
    sampleGraphY.SetName("gSampleTrackY");

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
    if (max_events>0) nentries = max_events;
    progressbar bar(nentries);
    signal(SIGINT, interruptHandler);
    for (int nevt=0; (!isInterrupted) && rechitTree->LoadTree(nevt)>=0; ++nevt) {
      if ((max_events>0) && (nevt>max_events)) break;
      bar.update();

      rechitTree->GetEntry(nevt);

      for (int i=0; i<nChambers; i++) {
        graphX.RemovePoint(i);
        graphY.RemovePoint(i);
      }

      // process only if single rechit per chamber per event:
      if (nrechits2d!=nChambers) continue;
      bool isGoldenEvent = true;
      for (int irechit=0; irechit<nrechits2d; irechit++) {
        chamber = vecRechit2DChamber->at(irechit);
        if (chamber!=irechit) isGoldenEvent = false;
      }
      if (!isGoldenEvent) continue;
      nentriesGolden++;

      for (int testedChamber=0; testedChamber<nChambers; testedChamber++) {
        // reset track:
        trackX.SetParameters(0., 0.);
        trackY.SetParameters(0., 0.);
        // loop over rechits and make track:
        for (int irechit=0; irechit<nrechits2d; irechit++) {
          chamber = vecRechit2DChamber->at(irechit);
          if (chamber!=testedChamber) {
            // add to graph for fitting:
            graphX.SetPoint(graphX.GetN(), zChamber[chamber], vecRechit2D_X_Center->at(irechit));
            graphY.SetPoint(graphY.GetN(), zChamber[chamber], vecRechit2D_Y_Center->at(irechit));
            graphX.SetPointError(graphX.GetN()-1, 1, vecRechit2D_X_Error->at(irechit)/0.866*0.07217);
            graphY.SetPointError(graphY.GetN()-1, 1, vecRechit2D_Y_Error->at(irechit)/0.866*0.07217);
          } else {
            // add rechit to tree
            rechits2D_X[testedChamber] = vecRechit2D_X_Center->at(irechit);
            rechits2D_Y[testedChamber] = vecRechit2D_Y_Center->at(irechit);
            rechits2D_X_ClusterSize[testedChamber] = vecRechit2D_X_ClusterSize->at(irechit);
            rechits2D_Y_ClusterSize[testedChamber] = vecRechit2D_Y_ClusterSize->at(irechit);
            rechits2D_X_Error[testedChamber] = vecRechit2D_X_Error->at(irechit)/0.866*0.07217;
            rechits2D_Y_Error[testedChamber] = vecRechit2D_Y_Error->at(irechit)/0.866*0.07217;
          }
        }
        // fit track and propagate to chamber under test:
        fitStatus1 = graphX.Fit(&trackX, "SQ");
        fitStatus2 = graphY.Fit(&trackY, "SQ");
        tracks_X_slope[testedChamber] = trackX.GetParameter(0);
        tracks_Y_slope[testedChamber] = trackY.GetParameter(0);
        tracks_X_intercept[testedChamber] = trackX.GetParameter(1);
        tracks_Y_intercept[testedChamber] = trackY.GetParameter(1);

        trackFitIsValid[testedChamber] = fitStatus1->IsValid() && fitStatus2->IsValid();
        trackFitChi2[testedChamber] = fitStatus1->Chi2() * fitStatus2->Chi2();
        if (!trackFitIsValid[testedChamber]) fitBadCount++;
        else fitGoodCount++;

        prophits2D_X[testedChamber] = trackX.Eval(zChamber[testedChamber]);
        prophits2D_Y[testedChamber] = trackY.Eval(zChamber[testedChamber]);

        /*//
        // Calculate propagation error by weird procedure
        // 
        // reset support graph:
        for (int j=0; j<4; j++) {
          sampleGraphX.RemovePoint(j);
          sampleGraphY.RemovePoint(j);
        }
        // sample a random track:
        double randomSlope = ((double)rand())/RAND_MAX;
        double randomIntercept = ((double)rand())/RAND_MAX;
        sampleTrackX.SetParameters(randomSlope, randomIntercept);
        sampleTrackY.SetParameters(randomSlope, randomIntercept);
        double zValuesX[4], zValuesY[4];
        // find points on chambers along this track and
        // build sample point collection:
        for (int j=0; j<4; j++) {
          zValuesX[j] = sampleTrackX.Eval(zChamber[j]);
          zValuesY[j] = sampleTrackY.Eval(zChamber[j]);
          if (j==testedChamber) continue;
          sampleGraphX.SetPoint(sampleGraphX.GetN(), zChamber[j], zValuesX[j]);
          sampleGraphY.SetPoint(sampleGraphY.GetN(), zChamber[j], zValuesY[j]);
          // use real errors for sampled points:
          sampleGraphX.SetPointError(sampleGraphY.GetN()-1, 0., rechits2D_X_Error[j]);
          sampleGraphY.SetPointError(sampleGraphY.GetN()-1, 0., rechits2D_Y_Error[j]);
        }
        // refit sample track:
        sampleGraphX.Fit(&sampleTrackX, "SQ");
        sampleGraphY.Fit(&sampleTrackY, "SQ");
        // extrapolate to test chamber and use difference as error:
        prophits2D_X_Error[testedChamber] = sampleTrackX.Eval(zChamber[testedChamber]) - zValuesX[testedChamber];
        prophits2D_Y_Error[testedChamber] = sampleTrackY.Eval(zChamber[testedChamber]) - zValuesY[testedChamber];*/

        prophits2D_X_Error[testedChamber] = sqrt(pow(trackX.GetParError(0),2)+pow(zChamber[testedChamber]*trackX.GetParError(1),2));
        prophits2D_Y_Error[testedChamber] = sqrt(pow(trackY.GetParError(0),2)+pow(zChamber[testedChamber]*trackY.GetParError(1),2));

        // fill beam profile:
        profileHistograms[testedChamber].Fill(rechits2D_X[testedChamber], rechits2D_Y[testedChamber]);
      }

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
