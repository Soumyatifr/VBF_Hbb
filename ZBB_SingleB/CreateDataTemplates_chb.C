#include "Common.h"

using namespace RooFit;

void CreatePDF (int iCAT,
		int iORDER,
		TTree * tree,
                TNtuple * tree_tt,
                TNtuple * tree_zj,
		RooWorkspace * w) {

  TH1::SetDefaultSumw2(true);

  TCanvas * dummy = new TCanvas("dummy","",800,700);
  TString nameHist = "mbb_"+names[iCAT];
  TString nameHist_tt = "mbb_tt_"+names[iCAT];
  TString nameHist_zj = "mbb_zj_"+names[iCAT];
  TH1D * hist = new TH1D(nameHist,"",Nbins,xmin,xmax);
  TH1D * hist_tt = new TH1D(nameHist_tt,"",Nbins,xmin,xmax);
  TH1D * hist_zj = new TH1D(nameHist_zj,"",Nbins,xmin,xmax);
  tree->Draw("mbb>>"+nameHist,cuts[iCAT]);
  tree_tt->Draw("mbb>>"+nameHist_tt,"weight*("+cuts[iCAT]+")");
  tree_zj->Draw("mbb>>"+nameHist_zj,"weight*("+cuts[iCAT]+")");

  TH1D * histData = (TH1D*)hist->Clone("dataHist_"+names[iCAT]);

  hist->Add(hist_tt,-1);
  hist->Add(hist_zj,-1);

  delete dummy;

  RooRealVar mbb("mbb_"+names[iCAT],"mass(bb)",xmin,xmax);
  RooRealVar b0("b0_"+names[iCAT],"b0",-1.0,1.0);
  RooRealVar b1("b1_"+names[iCAT],"b1",-1.0,1.0);
  RooRealVar b2("b2_"+names[iCAT],"b2",-1.0,1.0);
  RooRealVar b3("b3_"+names[iCAT],"b3",-1.0,1.0);
  RooRealVar b4("b4_"+names[iCAT],"b4",-1.0,1.0);
  RooRealVar b5("b5_"+names[iCAT],"b5",-1.0,1.0);
  b0.setConstant(false);
  b1.setConstant(false);
  b2.setConstant(false);
  b3.setConstant(false);
  b4.setConstant(false);
  b5.setConstant(false);

  Float_t yield = hist->GetSumOfWeights();
  RooRealVar qcd_yield("qcd_"+names[iCAT]+"_norm","Yield",yield,0.5*yield,2*yield);

  RooArgList argList(b0,b1);
  if (iORDER>=3)
    argList.add(b2);
  if (iORDER>=4)
    argList.add(b3);
  if (iORDER>=5)
    argList.add(b4);
  if (iORDER>=6)
    argList.add(b5);
  
  RooChebychev CHV("qcd_"+names[iCAT],"qcd_"+names[iCAT],mbb,argList);
  RooDataHist dataSubtr("data_subtr_"+names[iCAT],"data",mbb,hist);
  RooDataHist data("data_"+names[iCAT],"data",mbb,histData);  

  RooFitResult * res = CHV.fitTo(dataSubtr,Save(),SumW2Error(kTRUE));  

  cout << endl;
  cout << "+++++++++++++++ " << names[iCAT] << " +++++++++++++++++++" << endl;
  cout << endl;
  b0.Print();
  b1.Print();
  b2.Print();
  b3.Print();
  b4.Print();
  b5.Print();
  res->Print();
  cout << "TOTAL YIELD = " << yield << endl;
  cout << endl;
  cout << "+++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << endl;

  w->import(CHV);
  w->import(data);
  w->import(qcd_yield);

}

void CreateDataTemplates_chb() {

  TFile * file = new TFile("/afs/cern.ch/user/t/tumasyan/public/For_Soumya/Systematics_ROOTS/mbb_and_bdt_all_Nom_JER.root");
//   TFile * file = new TFile("/afs/cern.ch/user/t/tumasyan/public/New_SingleBTag/mbb_and_bdt_all_Central_JEC.root");

//  TFile * file = new TFile("/afs/cern.ch/user/t/tumasyan/public/New_SingleBTag_PowHeg/mbb_and_bdt_all_Central_JEC.root");  
  TTree * tree = (TTree*)file->Get("Mass_and_BDT_DATA");
  TNtuple * tree_tt = (TNtuple*)file->Get("Mass_and_BDT_tt");
  TNtuple * tree_zj = (TNtuple*)file->Get("Mass_and_BDT_ZJets");

  TFile * fileOutput = new TFile("rootshape/data_singleb_shapes.root","recreate");
//  TFile * fileOutput = new TFile("workspace_singleb_powheg/data_singleb_shapes.root","recreate");


  fileOutput->cd("");
  RooWorkspace * w = new RooWorkspace("w","data");
  int iorder[5] = {6,5,5,4,4};

//  int iorder[5] = {3,2,2,2,2};
  for (int i=0; i<5; ++i) 
    CreatePDF(i,iorder[i],tree,tree_tt,tree_zj,w);
  
  w->Write("w");
  fileOutput->Write();
  fileOutput->Close();
}
