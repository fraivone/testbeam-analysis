#include <cstdio>
#include <iostream>
#include <cstdint>
#include <vector>
#include <array>
#include <bitset>

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>

int main (int argc, char** argv) {

  gStyle->SetOptStat(0);

  if (argc<3) {
    std::cout << "Usage: Analysis ifile outdir" << std::endl;
    return 0;
  }
  std::string ifile   = argv[1];
  std::string outdir   = argv[2];
  
  int max_events = -1;
  if (argc>3) max_events = atoi(argv[3]);

  if (max_events > 0) std::cout << "Analyzing " << max_events << " events" << std::endl;
  else std::cout << "Analyzing all events" << std::endl; 

  TFile trackFile(ifile.c_str(), "READ");     
  TTree *trackTree = (TTree *) trackFile.Get("trackTree");

  TH1F hResidualsX("hResidualsX", ";x residual (mm);", 100, -4, 4);
  trackTree->Draw("rechitX-prophitX>>+hResidualsX");

  TF1 twoGauss("twoGauss", "gaus(0)+gaus(3)", -10, 10);
  twoGauss.SetParameters(1, hResidualsX.GetMean(), hResidualsX.GetRMS(), .1, hResidualsX.GetMean(), hResidualsX.GetRMS()*10);
  hResidualsX.Fit(&twoGauss);

  TF1 gauss1("g1", "gaus(0)", -10, 10);
  gauss1.SetParameters(twoGauss.GetParameter(0), twoGauss.GetParameter(1), twoGauss.GetParameter(2));
  TF1 gauss2("g2", "gaus(0)", -10, 10);
  gauss2.SetParameters(twoGauss.GetParameter(3), twoGauss.GetParameter(4), twoGauss.GetParameter(5));

  TCanvas cResidualsX("cResidualsX", "", 800, 600);
  hResidualsX.Draw("e");
  gauss1.Draw("same");
  gauss2.Draw("same");
  hResidualsX.SaveAs(std::string(outdir+"residuals_x.root").c_str());
  cResidualsX.SaveAs(std::string(outdir+"residuals_x.eps").c_str());

  std::cout << "Output files written to " << outdir << std::endl;
}
