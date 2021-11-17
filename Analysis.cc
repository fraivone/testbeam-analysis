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
#include <TH2F.h>
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

  residualHistogram.SaveAs(std::string(outdir+"/"+plotTitle+".root").c_str());
  residualCanvas.SaveAs(std::string(outdir+"/"+plotTitle+".eps").c_str());
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

  TH1F hPropX("hPropX", ";x propagated (mm);", 50, -48, 48);
  trackTree->Draw("prophitX>>+hPropX");
  TH1F hPropY("hPropY", ";y propagated (mm);", 50, -48, 48);
  trackTree->Draw("prophitY>>+hPropY");

  TH2F hResidualsXvsR("hResidualsXvsR", ";r rechit (mm);x residual (mm);", 100, 0, 48*1.414, 100, -1, 1);
  trackTree->Draw("rechitX-prophitX:sqrt(pow(rechitX,2)+pow(rechitY,2))>>+hResidualsXvsR");
  TCanvas residualXvsRCanvas("cResidualXvsR", "", 800, 600);
  residualXvsRCanvas.SetRightMargin(.2);
  hResidualsXvsR.Draw("cont0 colz");
  residualXvsRCanvas.SaveAs(std::string(outdir+"/residual_xvsr.eps").c_str());

  TH2F hResidualsYvsR("hResidualsYvsR", ";r rechit (mm);y residual (mm);", 100, 0, 48*1.414, 100, -3, 0);
  trackTree->Draw("rechitY-prophitY:sqrt(pow(rechitX,2)+pow(rechitY,2))>>+hResidualsYvsR");
  TCanvas residualYvsRCanvas("cResidualYvsR", "", 800, 600);
  residualYvsRCanvas.SetRightMargin(.2);
  hResidualsYvsR.Draw("colz");
  residualYvsRCanvas.SaveAs(std::string(outdir+"/residual_yvsr.eps").c_str());

  TH2F hResidualVsPropX("hResidualVsPropX", ";x propagated (mm);x residual (mm);", hPropX.GetNbinsX(), -48, 48, 50, -1, 0.6);
  trackTree->Draw("prophitX-rechitX:prophitX>>+hResidualVsPropX");
  TH2F hProp2DX("hProp2DX", ";x propagated (mm);;", hResidualVsPropX.GetNbinsX(), -48, 48, hResidualVsPropX.GetNbinsY(), -1, 0.6);
  for (int i=0; i<hProp2DX.GetNbinsX(); i++) {
    for (int j=0; j<hProp2DX.GetNbinsY(); j++) hProp2DX.SetBinContent(i, j, hPropX.GetBinContent(i));
  }
  hResidualVsPropX.Divide(&hProp2DX);
  TCanvas residualVsPropXCanvas("cResidualVsPropX", "", 800, 600);
  residualVsPropXCanvas.SetRightMargin(.2);
  hResidualVsPropX.SetMarkerStyle(1);
  hResidualVsPropX.Draw("candlex6");
  residualVsPropXCanvas.SaveAs(std::string(outdir+"/residual_vs_prophit_x.png").c_str());

  TH2F hResidualVsPropY("hResidualVsPropY", ";y propagated (mm);y residual (mm);", hPropY.GetNbinsX(), -48, 48, 50, 1, 3);
  trackTree->Draw("prophitY-rechitY:prophitY>>+hResidualVsPropY");
  TH2F hProp2DY("hProp2DY", ";y propagated (mm);;", hResidualVsPropX.GetNbinsX(), -48, 48, hResidualVsPropX.GetNbinsY(), 1, 3);
  for (int i=0; i<hProp2DY.GetNbinsY(); i++) {
    for (int j=0; j<hProp2DY.GetNbinsY(); j++) hProp2DY.SetBinContent(i, j, hPropY.GetBinContent(i));
  }
  hResidualVsPropY.Divide(&hProp2DY);
  TCanvas residualVsPropYCanvas("cResidualVsPropY", "", 800, 600);
  residualVsPropYCanvas.SetRightMargin(.2);
  hResidualVsPropY.SetMarkerStyle(1);
  hResidualVsPropY.Draw("candlex6");
  residualVsPropYCanvas.SaveAs(std::string(outdir+"/residual_vs_prophit_y.png").c_str());

  TCanvas residualVsPropCanvas("cResidualVsProp", "", 1800, 600);
  residualVsPropCanvas.Divide(2, 1);
  residualVsPropCanvas.cd(1);
  hResidualVsPropX.Draw();
  residualVsPropCanvas.cd(2);
  hResidualVsPropY.Draw();
  residualVsPropCanvas.SaveAs(std::string(outdir+"/residual_vs_prophit.png").c_str());

  std::cout << "Output files written to " << outdir << std::endl;
}