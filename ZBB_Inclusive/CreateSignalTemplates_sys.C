#include "Common.h"

using namespace RooFit;

std::ofstream outtext("signal_out.txt");

void CreateSignalPDF(int iCAT,
		     std::map<TString, TNtuple*> treeGGH,
		     std::map<TString, TNtuple*> treeVBF,
		     RooRealVar theta_JES,
		     RooRealVar theta_JER,
		     RooWorkspace * w) {

  TH1::SetDefaultSumw2(true);

  std::map<TString,double> mapMean;
  std::map<TString,double> mapSigma;
  std::map<TString,double> mapNormGGH;
  std::map<TString,double> mapNormVBF;

  TCanvas * dummy = new TCanvas("dummy","",800,700);

  double B0 = 0.5;
  double B1 = 0.5;
  double B2 = 0.5;
  double Alpha_CB = 2.0;
  double N_CB = 2.0;
  double FSig = 0.9;

  for (unsigned int i=0; i<5; ++i) {

    TString sysName = sysNames[i];

    std::cout << "++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << std::endl;
    std::cout << "Systematics name : " << sysName << std::endl;
    std::cout << std::endl;

    TString nameGGHHist = "mbb_ggh";//+namesCAT[iCAT]+"_"+sysName;
    TString nameVBFHist = "mbb_vbf";//+namesCAT[iCAT]+"_"+sysName;

    TH1D * histGGH = new TH1D(nameGGHHist,"",NbinsSig,xmin,xmax);
    TH1D * histVBF = new TH1D(nameVBFHist,"",NbinsSig,xmin,xmax);
    
    treeGGH[sysName]->Draw("mbb>>"+nameGGHHist,"weight*0.7078*(1113215491.0/1662168704.0)*("+cuts[iCAT]+")");
    treeVBF[sysName]->Draw("mbb>>"+nameVBFHist,"weight*("+cuts[iCAT]+")");

    mapNormGGH[sysName] = histGGH->GetSumOfWeights();
    mapNormVBF[sysName] = histVBF->GetSumOfWeights();

    delete histGGH;
    delete histVBF;

    histGGH = new TH1D(nameGGHHist,"",NbinsSig,xmin,xmax);
    histVBF = new TH1D(nameVBFHist,"",NbinsSig,xmin,xmax);
    
    treeGGH[sysName]->Draw("mbb>>"+nameGGHHist,"weight");
    treeVBF[sysName]->Draw("mbb>>"+nameVBFHist,"weight");

    RooRealVar mbbx("mbb","mass(bb)",xmin,xmax);

    RooRealVar meanx("mean","Mean",125,80,200);
    RooRealVar sigmax("sigma","Width",10,0,30);
    RooRealVar alphax("alpha","Alpha",1,0,20);
    RooRealVar nx("n","n",2,0,20);
    
    RooRealVar b0x("b0","b0",0.5,0,1);
    RooRealVar b1x("b1","b1",0.5,0,1);
    RooRealVar b2x("b2","b2",0.5,0,1);

    RooRealVar fsigx("fsig","fsig",0.5,0,1);

    if (!sysName.Contains("Nom")) {

      b0x.setVal(B0);
      b1x.setVal(B1);
      b2x.setVal(B2);
      alphax.setVal(Alpha_CB);
      nx.setVal(N_CB);
      fsigx.setVal(FSig);

      b0x.setConstant(kTRUE);
      b1x.setConstant(kTRUE);
      b2x.setConstant(kTRUE);
      alphax.setConstant(kTRUE);
      nx.setConstant(kTRUE);
      fsigx.setConstant(kTRUE);

    }

    RooBernstein BRNx("brn","Bernstein",mbbx,RooArgList(b0x,b1x,b2x));
    RooCBShape cbx("cb","CBshape",mbbx,meanx,sigmax,alphax,nx);
    RooGaussian gausx("gaus","Gauss",mbbx,meanx,sigmax);
    RooAddPdf signalx("signal","signal",RooArgList(cbx,BRNx),fsigx);

    histVBF->Add(histGGH,histVBF);
    RooDataHist data("dataGGH","dataGGH",mbbx,histVBF);
    RooFitResult * res = signalx.fitTo(data,Save(),SumW2Error(kTRUE));

    if (sysName.Contains("Nom")) {
      B0 = b0x.getValV();
      B1 = b1x.getValV();
      B2 = b2x.getValV();
      Alpha_CB = alphax.getValV();
      N_CB = nx.getValV();
      FSig = fsigx.getValV();
    }

    mapMean[sysName]  = meanx.getValV();
    mapSigma[sysName] = sigmax.getValV();

    delete histGGH;
    delete histVBF;
    
  }
  delete dummy;

  RooRealVar mean("mean_sig_"+names[iCAT],"mean",125,80,200);
  RooRealVar sigma("sigma_sig_"+names[iCAT],"sigma",10,0,30);
  RooRealVar alpha("alpha_sig_"+names[iCAT],"alpha",0.5,0.,10.);
  RooRealVar n("n_sig_"+names[iCAT],"n",10,0,20);
  RooRealVar fsig("f_sig_"+names[iCAT],"fsig",0.5,0,1);
  RooRealVar b0("b0_sig_"+names[iCAT],"b0",0.5,0,1);
  RooRealVar b1("b1_sig_"+names[iCAT],"b1",0.5,0,1);
  RooRealVar b2("b2_sig_"+names[iCAT],"b2",0.5,0,1);
  RooRealVar mbb("mbb_"+names[iCAT],"mass(bb)",xmin,xmax);

  double Mean = 0.5*(mapMean["JESUp"]+mapMean["JESDown"]);
  double d_Mean = mapMean["JESUp"] - Mean;
  
  double Sigma = 0.5*(mapSigma["JERUp"]+mapSigma["JERDown"]);
  double d_Sigma = mapSigma["JERUp"] - Sigma;

  mean.setVal(Mean);
  sigma.setVal(Sigma);
  alpha.setVal(Alpha_CB);
  n.setVal(N_CB);
  fsig.setVal(FSig);
  b0.setVal(B0);
  b1.setVal(B1);
  b2.setVal(B2);

  RooRealVar shift_JES("shift_JES_sig_"+names[iCAT],"shift_JES",d_Mean,d_Mean-1.0,d_Mean+1.0);
  RooRealVar shift_JER("shift_JER_sig_"+names[iCAT],"shift_JER",d_Sigma,d_Sigma-1.0,d_Sigma+1.0);

  RooFormulaVar mean_shifted_ggH("mean_shifted_ggH_" +names[iCAT],
				 "@0 + @1*@2",
				 RooArgList(mean,theta_JES,shift_JES));
  RooFormulaVar mean_shifted_qqH("mean_shifted_qqH_" +names[iCAT],
				 "@0 + @1*@2",
				 RooArgList(mean,theta_JES,shift_JES));

  RooFormulaVar sigma_shifted_ggH("sigma_shifted_ggH_"+names[iCAT],
				  "@0 + @1*@2",
				  RooArgList(sigma,theta_JER,shift_JER));
  RooFormulaVar sigma_shifted_qqH("sigma_shifted_qqH_"+names[iCAT],
				  "@0 + @1*@2",
				  RooArgList(sigma,theta_JER,shift_JER));
    
  RooBernstein BRN_ggH("brn_ggH_"+names[iCAT],"Bernstein",mbb,RooArgList(b0,b1,b2));
  RooCBShape cb_ggH("cb_ggH_"+names[iCAT],"CBshape",mbb,mean_shifted_ggH,sigma_shifted_ggH,alpha,n);
  RooGaussian gaus_ggH("gaus_ggH_"+names[iCAT],"Gaussian",mbb,mean_shifted_ggH,sigma_shifted_ggH);
  RooAddPdf signal_ggH("ggH_"+names[iCAT],"ggH",RooArgList(cb_ggH,BRN_ggH),fsig);

  RooBernstein BRN_qqH("brn_qqH_"+names[iCAT],"Bernstein",mbb,RooArgList(b0,b1,b2));
  RooCBShape cb_qqH("cb_qqH_"+names[iCAT],"CBshape",mbb,mean_shifted_qqH,sigma_shifted_qqH,alpha,n);
  RooGaussian gaus_qqH("gaus_qqH_"+names[iCAT],"Gaussian",mbb,mean_shifted_qqH,sigma_shifted_qqH);
  RooAddPdf signal_qqH("qqH_"+names[iCAT],"qqH",RooArgList(cb_qqH,BRN_qqH),fsig);

  double normGGH_Central = mapNormGGH["Nom"];
  double d_normGGH_JES_Up = mapNormGGH["JESUp"] - mapNormGGH["Nom"];
  double d_normGGH_JES_Down = mapNormGGH["Nom"] - mapNormGGH["JESDown"];
  double d_normGGH_JER_Up = mapNormGGH["JERUp"] - mapNormGGH["Nom"];
  double d_normGGH_JER_Down = mapNormGGH["Nom"] - mapNormGGH["JERDown"];

  double normVBF_Central = mapNormVBF["Nom"];
  double d_normVBF_JES_Up = mapNormVBF["JESUp"] - mapNormVBF["Nom"];
  double d_normVBF_JES_Down = mapNormVBF["Nom"] - mapNormVBF["JESDown"];
  double d_normVBF_JER_Up = mapNormVBF["JERUp"] - mapNormVBF["Nom"];
  double d_normVBF_JER_Down = mapNormVBF["Nom"] - mapNormVBF["JERDown"];

  RooRealVar normGGH_Nom("norm_GGH_Nom_sig_"+names[iCAT],"norm_GGH_Nom",normGGH_Central,0.,2.*normGGH_Central);
  RooRealVar normGGH_JES_Up("normGGH_JES_Up_sig_"+names[iCAT],"normGGH_JES_Up",d_normGGH_JES_Up,-100.,100.);
  RooRealVar normGGH_JES_Down("normGGH_JES_Down_sig_"+names[iCAT],"normGGH_JES_Down",d_normGGH_JES_Down,-100.,100.);
  RooRealVar normGGH_JER_Up("normGGH_JER_Up_sig_"+names[iCAT],"normGGH_JER_Up",d_normGGH_JER_Up,-100.,100.);
  RooRealVar normGGH_JER_Down("normGGH_JER_Down_sig_"+names[iCAT],"normGGH_JER_Down",d_normGGH_JER_Down,-100.,100.);

  RooRealVar normVBF_Nom("norm_VBF_Nom_sig_"+names[iCAT],"norm_VBF_Nom",normVBF_Central,0.,2.*normVBF_Central);
  RooRealVar normVBF_JES_Up("normVBF_JES_Up_sig_"+names[iCAT],"normVBF_JES_Up",d_normVBF_JES_Up,-100.,100.);
  RooRealVar normVBF_JES_Down("normVBF_JES_Down_sig_"+names[iCAT],"normVBF_JES_Down",d_normVBF_JES_Down,-100.,100.);
  RooRealVar normVBF_JER_Up("normVBF_JER_Up_sig_"+names[iCAT],"normVBF_JER_Up",d_normVBF_JER_Up,-100.,100.);
  RooRealVar normVBF_JER_Down("normVBF_JER_Down_sig_"+names[iCAT],"normVBF_JER_Down",d_normVBF_JER_Down,-100.,100.);

  RooFormulaVar yield_GGH("ggH_"+names[iCAT]+"_norm",
			  "@0 + (@1>=0.0)*@1*@2 + (@1<0.0)*@1*@3 + (@4>=0.0)*@4*@5 + (@4<0.0)*@4*@6",
			  RooArgList(normGGH_Nom,theta_JES,normGGH_JES_Up,normGGH_JES_Down,theta_JER,normGGH_JER_Up,normGGH_JER_Down));
  RooFormulaVar yield_VBF("qqH_"+names[iCAT]+"_norm",
			  "@0 + (@1>=0.0)*@1*@2 + (@1<0.0)*@1*@3 + (@4>=0.0)*@4*@5 + (@4<0.0)*@4*@6",
			  RooArgList(normVBF_Nom,theta_JES,normVBF_JES_Up,normVBF_JES_Down,theta_JER,normVBF_JER_Up,normVBF_JER_Down));

  alpha.setConstant(true);
  mean.setConstant(true);
  sigma.setConstant(true);
  n.setConstant(true);

  fsig.setConstant(true);

  b0.setConstant(true);
  b1.setConstant(true);
  b2.setConstant(true);

  shift_JES.setConstant(true);
  shift_JER.setConstant(true);

  normGGH_Nom.setConstant(true);
  normGGH_JES_Up.setConstant(true);
  normGGH_JES_Down.setConstant(true);
  normGGH_JER_Up.setConstant(true);
  normGGH_JER_Down.setConstant(true);
  
  normVBF_Nom.setConstant(true);
  normVBF_JES_Up.setConstant(true);
  normVBF_JES_Down.setConstant(true);
  normVBF_JER_Up.setConstant(true);
  normVBF_JER_Down.setConstant(true);
  
  outtext << "+++++++++++++ " << names[iCAT] << " ++++++++++++++++" << endl;
  outtext << endl;
  outtext << "Mean  (ggH)  = " << mean_shifted_ggH.getVal() << endl;
  outtext << "Mean  (qqH)  = " << mean_shifted_qqH.getVal() << endl;
  outtext << "Mean  (nom)  = " << mapMean["Nom"] << endl;
  outtext << "Mean  (ave)  = " << Mean << endl;
  outtext << "Mean  (Up)   = " << mapMean["JESUp"] << endl;
  outtext << "Mean  (Down) = " << mapMean["JESDown"] << endl;
  outtext << endl;
  outtext << "Sigma (ggH)  = " << sigma_shifted_ggH.getVal() << endl;
  outtext << "Sigma (qqH)  = " << sigma_shifted_qqH.getVal() << endl;
  outtext << "Sigma (nom)  = " << mapSigma["Nom"] << endl;
  outtext << "Sigma (ave)  = " << Sigma << endl;
  outtext << "Sigma (Up)   = " << mapSigma["JERUp"] << endl;
  outtext << "Sigma (Down) = " << mapSigma["JERDown"] << endl;
  outtext << endl;
  outtext << "Norm    (ggH)  = " << mapNormGGH["Nom"] << endl;
  outtext << "JESUp   (ggH)  = " << mapNormGGH["JESUp"] << endl;
  outtext << "JESDown (ggH)  = " << mapNormGGH["JESDown"] << endl;
  outtext << "JERUp   (ggH)  = " << mapNormGGH["JERUp"] << endl;
  outtext << "JERDown (ggH)  = " << mapNormGGH["JERDown"] << endl;
  outtext << endl;
  outtext << "Norm    (qqH)  = " << mapNormVBF["Nom"] << endl;
  outtext << "JESUp   (qqH)  = " << mapNormVBF["JESUp"] << endl;
  outtext << "JESDown (qqH)  = " << mapNormVBF["JESDown"] << endl;
  outtext << "JERUp   (qqH)  = " << mapNormVBF["JERUp"] << endl;
  outtext << "JERDown (qqH)  = " << mapNormVBF["JERDown"] << endl;
  outtext << endl;

  w->import(signal_ggH);
  w->import(signal_qqH);
  w->import(yield_GGH);
  w->import(yield_VBF);

}

void CreateSignalTemplates_sys() {
  
  std::map<TString,TNtuple *> treeGGH;
  std::map<TString,TNtuple *> treeVBF;

  for (int i=0; i<5; ++i) {
    TFile * file = new TFile(dirName+"/"+FileNamesBDT[i]);
    TString sysName = sysNames[i];
    treeGGH[sysName] = (TNtuple*)file->Get("Mass_and_BDT_ggF_Hbb");
    treeVBF[sysName] = (TNtuple*)file->Get("Mass_and_BDT_VBF_Hbb");
  }

  TFile * fileOutput = new TFile("rootshape/signal_doubleb_shapes.root","recreate");
  fileOutput->cd("");
  RooWorkspace * w = new RooWorkspace("w","signal");

  RooRealVar theta_JES("CMS_JES_2018","CMS_JES_2018",0.,-5.,5.);
  RooRealVar theta_JER("CMS_JER_2018","CMS_JER_2018",0.,-5.,5.);

  for (int i=0; i<4; ++i) 
    CreateSignalPDF(i,treeGGH,treeVBF,theta_JES,theta_JER,w);

  w->Write("w");
  fileOutput->Write();
  fileOutput->Close();
}
