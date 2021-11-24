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
#include <TH1F.h>
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
      std::cout << "Usage: Tracks ifile ofile" << std::endl;
      return 0;
    }
    std::string ifile   = argv[1];
    std::string ofile   = argv[2];
    
    const int ntrackers = 4;

    int max_events = -1;
    if (argc>3) max_events = atoi(argv[3]);

    if (max_events > 0) std::cout << "Analyzing " << max_events << " events" << std::endl;
    else std::cout << "Analyzing all events" << std::endl; 

    TFile rechitFile(ifile.c_str(), "READ");
    TTree *rechitTree = (TTree *) rechitFile.Get("rechitTree");

    TFile alignmentFile(ofile.c_str(), "RECREATE", "Alignment file");
    //TTree trackTree("trackTree", "trackTree");
  
    // rechits2D variables
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
    
    // geometry, starting from ge2/1
    double zBari1 = -(697+254+294);
    double zBari2 = -(254+294);
    double zMe0 = -294;
    double zBari3 = 170;
    double zBari4 = 170+697;

    double zChamber[4] = { zBari1, zBari2, zBari3, zBari4 };

    double zStart = zBari1;
    double zEnd = zBari2;

    // alignment parameters:
    std::array<double, ntrackers> correctionsX;
    std::array<double, ntrackers> correctionsY;
    // plot of alignment parameter trend
    std::array<TGraphErrors, ntrackers> alignmentGraphX;
    std::array<TGraphErrors, ntrackers> alignmentGraphY;
    for (int i=0; i<ntrackers; i++) {
      alignmentGraphX[i].SetName(std::string("alignmentX_chamber"+std::to_string(i)).c_str());
      alignmentGraphY[i].SetName(std::string("alignmentY_chamber"+std::to_string(i)).c_str());
    }

    int nentries = rechitTree->GetEntries();
    std::cout << nentries << " total events" <<  std::endl;
    int nsteps = 10;
    for (int istep=0; istep<nsteps; istep++) {

      std::cout << "Beginning step " << istep << std::endl;

      std::array<TH1F, ntrackers> residualHistogramsX;
      std::array<TH1F, ntrackers> residualHistogramsY;
      std::array<TH1F, ntrackers> chi2HistogramsX;
      std::array<TH1F, ntrackers> chi2HistogramsY;
      std::array<TH2F, ntrackers> residualVsChi2HistogramsX;
      std::array<TH2F, ntrackers> residualVsChi2HistogramsY;
      std::array<TH2F, ntrackers> residualVsClusterSizeHistogramsX;
      std::array<TH2F, ntrackers> residualVsClusterSizeHistogramsY;

      if (max_events>0) nentries = max_events;

      // set chamber under alignment, then fit using the others:
      for (int testedTracker=0; testedTracker<ntrackers; testedTracker++) {
        std::cout << "Aligning chamber " << testedTracker << std::endl;
        
        // linear track fit
        TF1 trackX("fTrackX", "[0]+[1]*x", zStart, zEnd);
        TF1 trackY("fTrackY", "[0]+[1]*x", zStart, zEnd);
        // linear track graph
        TGraphErrors graphX;
        TGraphErrors graphY;
        graphX.SetName("gTrackX");
        graphY.SetName("gTrackY");
        // support variables for extrapolated and rechits
        int chamber;
        double rechitX, rechitY;
        double propagatedX, propagatedY;
        int fitGoodCount, fitBadCount;
        double trackFitChi2;
        int trackFitIsValid;
        TFitResultPtr fitStatusX, fitStatusY;
        
        residualHistogramsX[testedTracker] = TH1F(
          std::string("residualHistogramX_chamber"+std::to_string(testedTracker)+"_step"+std::to_string(istep)).c_str(),
          ";residual x (mm);", 200, -5., 5.
        );
        residualHistogramsY[testedTracker] = TH1F(
          std::string("residualHistogramY_chamber"+std::to_string(testedTracker)+"_step"+std::to_string(istep)).c_str(),
          ";residual y (mm);", 200, -5., 5.
        );
        chi2HistogramsX[testedTracker] = TH1F(
          std::string("chi2HistogramX_chamber"+std::to_string(testedTracker)+"_step"+std::to_string(istep)).c_str(),
          ";#chi^{2} track x;", 100, -0.01, 4.
        );
        chi2HistogramsY[testedTracker] = TH1F(
          std::string("chi2HistogramY_chamber"+std::to_string(testedTracker)+"_step"+std::to_string(istep)).c_str(),
          ";#chi^{2} track y;", 100, -0.01, 4.
        );
        residualVsChi2HistogramsX[testedTracker] = TH2F(
          std::string("residualVsChi2HistogramX_chamber"+std::to_string(testedTracker)+"_step"+std::to_string(istep)).c_str(),
          ";residual x (mm);#chi^{2} track x", 1000, -10., 10., 1000, -0.01, 1
        );
        residualVsChi2HistogramsY[testedTracker] = TH2F(
          std::string("residualVsChi2HistogramY_chamber"+std::to_string(testedTracker)+"_step"+std::to_string(istep)).c_str(),
          ";residual y (mm);#chi^{2} track y", 1000, -10., 10., 1000, -0.01, 1
        );
        residualVsClusterSizeHistogramsX[testedTracker] = TH2F(
          std::string("residualVsClusterSizeHistogramX_chamber"+std::to_string(testedTracker)+"_step"+std::to_string(istep)).c_str(),
          ";residual x (mm);cluster size", 1000, -10., 10., 30, 0, 30
        );
        residualVsClusterSizeHistogramsY[testedTracker] = TH2F(
          std::string("residualVsClusterSizeHistogramY_chamber"+std::to_string(testedTracker)+"_step"+std::to_string(istep)).c_str(),
          ";residual y (mm);cluster size", 1000, -10., 10., 30, 0, 30
        );

        progressbar bar(nentries);
        signal(SIGINT, interruptHandler);
        for (int nevt=0; (!isInterrupted) && rechitTree->LoadTree(nevt)>=0; ++nevt) {
          if ((max_events>0) && (nevt>max_events)) break;
          bar.update();

          rechitTree->GetEntry(nevt);

          // process event only if single rechit per chamber, cluster size < 5 per chamber
          bool goodEvent = true;
          for (int itracker=0; itracker<ntrackers; itracker++) {
            if (count(vecRechit2DChamber->begin(), vecRechit2DChamber->end(), itracker)>1) {
              goodEvent = false;
              break;
            }
          }
          for (int irechit=0; irechit<nrechits2d; irechit++) {
            if (vecRechit2D_X_ClusterSize->at(irechit)>5 or vecRechit2D_Y_ClusterSize->at(irechit)>5) {
              goodEvent = false;
              break;
            }
            // if (((int)vecRechit2D_X_ClusterSize->at(irechit))%2==1 or ((int)vecRechit2D_Y_ClusterSize->at(irechit))%2==1) {
            //   goodEvent = false;
            //   break;
            // }
          }
          if (!goodEvent) continue;

          //for (auto ch:*vecRechit2DChamber) std::cout << ch << " ";
          //std::cout << std::endl;

          trackX.SetParameters(0., 0.);
          trackY.SetParameters(0., 0.);

          for (int irechit=0; irechit<nrechits2d; irechit++) {
            // add to graph for fitting:
            chamber = vecRechit2DChamber->at(irechit);
            if (chamber==testedTracker) continue;
            rechitX = vecRechit2D_X_Center->at(irechit)-correctionsX[chamber];
            rechitY = vecRechit2D_Y_Center->at(irechit)-correctionsY[chamber];
            graphX.SetPoint(chamber, zChamber[chamber], rechitX);
            graphX.SetPointError(chamber, 1., vecRechit2D_X_Error->at(irechit));
            graphY.SetPoint(chamber, zChamber[chamber], rechitY);
            graphY.SetPointError(chamber, 1., vecRechit2D_Y_Error->at(irechit));
            //std::cout << chamber << " " << irechit << std::endl;
          }

          // fit track and propagate to all chambers
          fitStatusX = graphX.Fit(&trackX, "SQ");
          fitStatusY = graphY.Fit(&trackY, "SQ");

          trackFitIsValid = fitStatusX->IsValid() && fitStatusY->IsValid();
          trackFitChi2 = fitStatusX->Chi2() * fitStatusY->Chi2();
          if (fitStatusX->Chi2()>0.02 || fitStatusY->Chi2()>0.02) continue;
          if (!trackFitIsValid) fitBadCount++;
          else fitGoodCount++;

          for (int irechit=0; irechit<nrechits2d; irechit++) {
            // fill residual plot
            chamber = vecRechit2DChamber->at(irechit);
            if (chamber!=testedTracker) continue;
            rechitX = vecRechit2D_X_Center->at(irechit)-correctionsX[chamber];
            rechitY = vecRechit2D_Y_Center->at(irechit)-correctionsY[chamber];
            propagatedX = trackX.Eval(zChamber[chamber]);
            propagatedY = trackY.Eval(zChamber[chamber]);
            residualHistogramsX[chamber].Fill(rechitX-propagatedX);
            residualHistogramsY[chamber].Fill(rechitY-propagatedY);
            chi2HistogramsX[chamber].Fill(fitStatusX->Chi2());
            chi2HistogramsY[chamber].Fill(fitStatusY->Chi2());
            residualVsChi2HistogramsX[chamber].Fill(rechitX-propagatedX, fitStatusX->Chi2());
            residualVsChi2HistogramsY[chamber].Fill(rechitY-propagatedY, fitStatusY->Chi2());
            residualVsClusterSizeHistogramsX[chamber].Fill(rechitX-propagatedX, vecRechit2D_X_ClusterSize->at(irechit));
            residualVsClusterSizeHistogramsY[chamber].Fill(rechitY-propagatedY, vecRechit2D_Y_ClusterSize->at(irechit));
          }
        }
        std::cout << std::endl;

        // fit the residual histograms and find corrections:
        TF1 twoGaussX(
          std::string("twoGaussX_chamber"+std::to_string(testedTracker)).c_str(), "gaus(0)+gaus(3)",
          -10, 10
        );
        twoGaussX.SetParameters(
          1, residualHistogramsX[testedTracker].GetMean(), residualHistogramsX[testedTracker].GetRMS(),
          .1, residualHistogramsX[testedTracker].GetMean(), residualHistogramsX[testedTracker].GetRMS()*10
        );
        residualHistogramsX[testedTracker].Fit(&twoGaussX, "Q");
        correctionsX[testedTracker] += residualHistogramsX[testedTracker].GetMean();
        //correctionsX[testedTracker] += twoGaussX.GetParameter(1);
        TF1 twoGaussY(
          std::string("twoGaussY_chamber"+std::to_string(testedTracker)).c_str(), "gaus(0)+gaus(3)",
          -10, 10
        );
        twoGaussY.SetParameters(
          1, residualHistogramsY[testedTracker].GetMean(), residualHistogramsY[testedTracker].GetRMS(),
          .1, residualHistogramsY[testedTracker].GetMean(), residualHistogramsY[testedTracker].GetRMS()*10
        );
        residualHistogramsY[testedTracker].Fit(&twoGaussY, "Q");
        correctionsY[testedTracker] += residualHistogramsY[testedTracker].GetMean();
        //correctionsY[testedTracker] += twoGaussY.GetParameter(1);
      }
      std::cout << std::endl;
      std::cout << "Corrections for step " << istep << ":" << std::endl;
      for (int testedTracker=0; testedTracker<ntrackers; testedTracker++) {
        std::cout << testedTracker << " (" << correctionsX[testedTracker] << "," << correctionsY[testedTracker] << ")" << std::endl;
        alignmentGraphX[testedTracker].SetPoint(istep, istep, correctionsX[testedTracker]);
        alignmentGraphY[testedTracker].SetPoint(istep, istep, correctionsY[testedTracker]);
        residualHistogramsX[testedTracker].Write();
        residualHistogramsY[testedTracker].Write();
        chi2HistogramsX[testedTracker].Write();
        chi2HistogramsY[testedTracker].Write();
        residualVsChi2HistogramsX[testedTracker].Write();
        residualVsChi2HistogramsY[testedTracker].Write();
        residualVsClusterSizeHistogramsX[testedTracker].Write();
        residualVsClusterSizeHistogramsY[testedTracker].Write();
      }
    }

    for (int i=0; i<ntrackers; i++) {
      alignmentGraphX[i].Write();
      alignmentGraphY[i].Write();
    }
    alignmentFile.Close();

    std::cout << "Output files written to " << ofile << std::endl;
}
