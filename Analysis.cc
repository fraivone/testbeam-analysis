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
#include <TLatex.h>

#include "src/tdrstyle.c"

void saveResidualPlot(TH1F residualHistogram, std::string plotTitle, std::string outdir) {
  TF1 twoGauss("twoGauss", "gaus(0)+gaus(3)", -10, 10);
  twoGauss.SetParameters(1, residualHistogram.GetMean(), residualHistogram.GetRMS(), .1, residualHistogram.GetMean(), residualHistogram.GetRMS()*10);
  residualHistogram.Fit(&twoGauss, "0");
  int spaceResolution = 1e3*twoGauss.GetParameter(2);

  TF1 gauss1("g1", "gaus(0)", -10, 10);
  TF1 gauss2("g2", "gaus(0)", -10, 10);
  gauss1.SetParameters(twoGauss.GetParameter(0), twoGauss.GetParameter(1), twoGauss.GetParameter(2));
  gauss2.SetParameters(twoGauss.GetParameter(3), twoGauss.GetParameter(4), twoGauss.GetParameter(5));
  gauss2.SetLineStyle(7);
  gauss2.SetLineColor(1);

  TCanvas residualCanvas(std::string("c"+plotTitle).c_str(), "", 800, 600);
  residualHistogram.Draw("e");
  gauss1.Draw("same");
  gauss2.Draw("same");

  TLatex latex;
  latex.SetTextFont(42);
  latex.SetTextSize(.04);

  latex.SetTextAlign(33);
  latex.DrawLatexNDC(.98, .985, "GEM-10x10-380XY-BARI-04");

  latex.SetTextAlign(31);
  latex.DrawLatexNDC(.93, .6, std::string("Space resolution "+std::to_string(spaceResolution)+"  #mum").c_str());

  residualHistogram.SaveAs(std::string(outdir+plotTitle+".root").c_str());
  residualCanvas.SaveAs(std::string(outdir+plotTitle+".eps").c_str());
}

int main (int argc, char** argv) {

  gStyle->SetOptStat(0);
  setTDRStyle();

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

  TH1F hResidualsX("hResidualsX", ";x residual (mm);", 100, -2.5, 4);
  trackTree->Draw("rechitX-prophitX>>+hResidualsX");
  saveResidualPlot(hResidualsX, "residuals_x", outdir);

  TH1F hResidualsY("hResidualsY", ";y residual (mm);", 100, -5, 3);
  trackTree->Draw("rechitY-prophitY>>+hResidualsY");
  saveResidualPlot(hResidualsY, "residuals_y", outdir);

  std::cout << "Output files written to " << outdir << std::endl;
}