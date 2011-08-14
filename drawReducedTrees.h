// -*- C++ -*-

//useful for playing around with plots in interactive ROOT
TH1D* hinteractive=0;
TH2D* h2d=0;

TLatex* text1=0;
TLatex* text2=0;

//holds a list of the *active* samples (to be plotted)
std::vector<TString> samples_;
//hold a list of all samples
std::set<TString> samplesAll_;
std::vector<TString> configDescriptions_;
//these maps use the sample names as keys
//std::map<TString, TFile*> files_;
std::map<TString, std::map<TString, TFile*> > files_;
std::map<TString, TH1D*> histos_;
std::map<TString, UInt_t> sampleColor_;
std::map<TString, TString> sampleOwenName_;
std::map<TString, TString> sampleLabel_;
std::map<TString, UInt_t> sampleMarkerStyle_;
TChain* dtree=0;
TH1D* hdata=0;

TString currentConfig_;

//default selection
TString selection_ ="cutHT==1 && cutPV==1 && cutTrigger==1 && cut3Jets==1 && cutEleVeto==1 && cutMuVeto==1";

float leg_x1 = 0.696, leg_x2=0.94, leg_y1=0.5, leg_y2=0.92;

//special containers for holding the "search regions of interest"
class SearchRegion {
public:
  SearchRegion(TString btagSel,TString htSel,TString metSel,TString oId,bool isSig=true);
  ~SearchRegion();
  void Print() const;

  TString htSelection;
  TString metSelection;
  TString btagSelection;

  TString owenId;
  bool isSIG;
};
SearchRegion::SearchRegion(TString btagSel,TString htSel,TString metSel,TString oId,bool isSig) : 
  htSelection(htSel),metSelection(metSel),btagSelection(btagSel),owenId(oId),isSIG(isSig) {}
SearchRegion::~SearchRegion() {}
void SearchRegion::Print() const {
  cout<<" == "<<btagSelection<<" "<<htSelection<<" "<<metSelection<<endl;

}

std::vector<SearchRegion > searchRegions_;
std::vector<SearchRegion > sbRegions_;
bool searchRegionsSet_=false;
void setSearchRegions() {
  if (searchRegionsSet_) return;

  //nb: some of the code *depends* on the fact that for there are equal numbers of corresponding
  //sbRegions and searchRegions, with the only difference being the MET selection!

  sbRegions_.push_back( SearchRegion( "ge1b","HT>=350","MET>=150&&MET<200","Loose",false)); //loose SB
  searchRegions_.push_back( SearchRegion( "ge1b","HT>=350","MET>=200","Loose")); //loose Sig

  sbRegions_.push_back( SearchRegion( "ge1b","HT>=500","MET>=150&&MET<200","Tight",false)); //tight SB
  searchRegions_.push_back( SearchRegion( "ge1b","HT>=500","MET>=300","Tight")); //tight Sig

  sbRegions_.push_back( SearchRegion( "ge2b","HT>=350","MET>=150&&MET<200","Loose",false)); //loose SB
  searchRegions_.push_back( SearchRegion( "ge2b","HT>=350","MET>=200","Loose")); //loose Sig

  sbRegions_.push_back( SearchRegion( "ge2b","HT>=500","MET>=150&&MET<200","Tight",false)); //tight SB
  searchRegions_.push_back( SearchRegion( "ge2b","HT>=500","MET>=300","Tight")); //tight Sig

/* new regions

  sbRegions_.push_back( SearchRegion( "ge1b","HT>=1100","MET>=150&&MET<200","HighHT",false));
  searchRegions_.push_back( SearchRegion( "ge1b","HT>=1100","MET>=200","HighHT"));

  sbRegions_.push_back( SearchRegion( "ge1b","HT>=400","MET>=150&&MET<200","HighMET",false));
  searchRegions_.push_back( SearchRegion( "ge1b","HT>=400","MET>=400","HighMET"));

  sbRegions_.push_back( SearchRegion( "ge3b","HT>=400","MET>=150&&MET<200","Threeb",false));
  searchRegions_.push_back( SearchRegion( "ge3b","HT>=400","MET>=200","Threeb"));
*/

  searchRegionsSet_=true;
}

struct SignalEffData {
  double rawYield;

  double effCorr;
  double totalSystematic;
};

struct OwenData {
  double Nsig; //number in signal region , data //done
  double Nsb; // number in SB, data             //done
  double Nsig_sl; //number in SL SIG, data  //done
  double Nsb_sl; // number in SL SB, data   //done
  double Nsig_ldp; //number in SIG, fail DP //done
  double Nsb_ldp;   // number in SB, fail DP //done

  //owen didn't ask for these
  //  double  Nlsb ;
  //  double  Nlsb_ldp;
  double  Nlsb_0b ;      // done
  double  Nlsb_0b_ldp;   // done

  double Nttbarmc_sig_ldp; //done
  double Nttbarmc_sb_ldp; //done

  double lsf_WJmc; //done
  double NWJmc_sig_ldp; //done
  double NWJmc_sb_ldp; //done

  double lsf_Znnmc; //done
  double NZnnmc_sig_ldp; //done
  double NZnnmc_sb_ldp; //done

  double lsf_Zjmc;
  double NZjmc_sig_ldp;
  double NZjmc_sb_ldp;

  double Nsingletopmc_sig_ldp;
  double Nsingletopmc_sb_ldp;

  //don't need DataLumi...that's just lumiScale_
} ;

std::map<TString, OwenData> owenMap_;

void printOwen(const TString& owenKey) {

  cout<< " === "<<owenKey<<" === "<<endl;

  cout<<"Nsig              "<<  owenMap_[owenKey].Nsig<<endl;
  cout<<"Nsb               "<<  owenMap_[owenKey].Nsb<<endl;

  cout<<"Nsig_sl           "<<  owenMap_[owenKey].Nsig_sl<<endl;
  cout<<"Nsb_sl            "<<  owenMap_[owenKey].Nsb_sl<<endl;

  cout<<"Nsig_ldp          "<<  owenMap_[owenKey].Nsig_ldp<<endl;
  cout<<"Nsb_ldp           "<<  owenMap_[owenKey].Nsb_ldp<<endl;

  cout<<"Nlsb_0b           "<<  owenMap_[owenKey].Nlsb_0b<<endl;
  cout<<"Nlsb_0b_ldp       "<<  owenMap_[owenKey].Nlsb_0b_ldp<<endl;

  cout<<"Nttbarmc_sig_ldp  "<<  owenMap_[owenKey].Nttbarmc_sig_ldp<<endl;
  cout<<"Nttbarmc_sb_ldp   "<<  owenMap_[owenKey].Nttbarmc_sb_ldp<<endl;

  cout<<"Nsingletopmc_sig_ldp  "<<  owenMap_[owenKey].Nsingletopmc_sig_ldp<<endl;
  cout<<"Nsingletopmc_sb_ldp   "<<  owenMap_[owenKey].Nsingletopmc_sb_ldp<<endl;

  cout<<"lsf_WJmc          "<<  owenMap_[owenKey].lsf_WJmc<<endl;
  cout<<"NWJmc_sig_ldp     "<<  owenMap_[owenKey].NWJmc_sig_ldp<<endl;
  cout<<"NWJmc_sb_ldp      "<<  owenMap_[owenKey].NWJmc_sb_ldp<<endl;

  cout<<"lsf_Znnmc         "<<  owenMap_[owenKey].lsf_Znnmc<<endl;
  cout<<"NZnnmc_sig_ldp    "<<  owenMap_[owenKey].NZnnmc_sig_ldp<<endl;
  cout<<"NZnnmc_sb_ldp     "<<  owenMap_[owenKey].NZnnmc_sb_ldp<<endl;

  cout<<"lsf_Zjmc         "<<  owenMap_[owenKey].lsf_Zjmc<<endl;
  cout<<"NZjmc_sig_ldp    "<<  owenMap_[owenKey].NZjmc_sig_ldp<<endl;
  cout<<"NZjmc_sb_ldp     "<<  owenMap_[owenKey].NZjmc_sb_ldp<<endl;

}


bool quiet_=false;
//bool quiet_=true;
bool doRatio_=false;
bool logy_=false;
bool dostack_=true;
bool doleg_=true;
bool dodata_=true;
bool addOverflow_=true;
//bool doSubtraction_=false;
bool drawMCErrors_=false;
bool renormalizeBins_=false;//no setter function
bool owenColor_ = false;

int m0_=0;
int m12_=0;
TString susyCrossSectionVariation_="";

bool normalized_=false;

bool useFlavorHistoryWeights_=false;//no setter function
float flavorHistoryScaling_=-1;

bool usePUweight_=false;
bool useHLTeff_ = false;
TString btagSFweight_="1";

bool savePlots_ = true; //no setter function
bool drawTotalSM_=false; //no setter function
bool drawTotalSMSusy_=false;//no setter function
bool drawSusyOnly_=false;//no setter function
bool drawMarkers_=true;//no setter function

bool doVerticalLine_=false;
double verticalLinePosition_=0;

bool doCustomPlotMax_=false;
double customPlotMax_=0;

bool doCustomPlotMin_=false;
double customPlotMin_=0;

float maxScaleFactor_ = 1.05;

bool latexMode_=false;
//bool latexMode_=true;
const TString pm = latexMode_ ? " \\pm " : " +/- ";

TCanvas* thecanvas=0;
//TCanvas* cratio=0;
TLegend* leg=0;
THStack* thestack=0;
TH1D* totalsm=0;
TH1D* totalsmsusy=0;
TH1D* totalewk=0;
TH1D* totalqcdttbar=0;
TH1D* totalnonttbar=0;
TH1D* totalnonqcd=0;
TH1D* totalqcd=0; //ben - just for ease of doing event counts with drawPlots
TH1D* ratio=0; float ratioMin=0; float ratioMax=2;
TGraphErrors* mcerrors=0;
bool loaded_=false; //bookkeeping
bool loadedSusyHistos_=false;//bookkeeping

// == set configuration options ==
void setQuiet(bool q) {
  quiet_ = q;
}

void doRatioPlot(bool doIt) {
  doRatio_=doIt;
}

void setPlotMaximum(double max) {
  customPlotMax_=max;
  doCustomPlotMax_=true;
}

void resetPlotMaximum() {
  doCustomPlotMax_=false;
}

void setPlotMinimum(double min) {
  customPlotMin_=min;
  doCustomPlotMin_=true;
}

void resetPlotMinimum() {
  doCustomPlotMin_=false;
}

void enableVerticalLine(double position) {
  doVerticalLine_=true;
  verticalLinePosition_ =position;
}

// void showDataMinusMC(bool dosub) {
//   doSubtraction_=dosub;
// }

void resetVerticalLine() {
  doVerticalLine_=false;
}

void doOverflowAddition(bool doOv) {
  addOverflow_ = doOv;
}

void setLogY(bool dolog) {
  logy_=dolog;
  if (logy_) maxScaleFactor_=3;
  else maxScaleFactor_=1.05;
}

void setStackMode(bool dostack, bool normalized=false) {
  dostack_=dostack;
  normalized_=normalized;
}

void doData(bool dodata) {
  dodata_=dodata;
}

void drawLegend(bool doleg) {
  doleg_=doleg;
}

void setLumiScale(double lumiscale){
  lumiScale_ = lumiscale;
}

map<pair<int,int>, TH1D* >  scanProcessTotalsMap;
void loadSusyScanHistograms() {
  if (loadedSusyHistos_) return;

  TFile* susyfile = 0;
  for (unsigned int isample=0; isample<samples_.size(); isample++) {
    if ( samples_[isample].Contains("mSUGRA")) {
      susyfile =  files_[currentConfig_][samples_[isample]];
      cout<<" Loading SUSY scan histograms from file: "<<susyfile->GetName()<<endl;
    }
  }

  if (susyfile==0) {cout<<"did not find mSugra in loadSusyScanHistograms!"<<endl; return;}

  for (int i=0; i<susyfile->GetListOfKeys()->GetEntries(); i++) {
    TString objname=  susyfile->GetListOfKeys()->At(i)->GetName();
    if (objname.BeginsWith("scanProcessTotals") ) {
      TString m0str=   objname.Tokenize("_")->At(1)->GetName();
      TString m12str=   objname.Tokenize("_")->At(2)->GetName();
      int m0 = m0str.Atoi();
      int m12 = m12str.Atoi();
      scanProcessTotalsMap[make_pair(m0,m12)] = (TH1D*) susyfile->Get(objname);
    }
  }


  loadedSusyHistos_=true;
}

void drawPlotHeader() {

  const float ypos = 0.97;

  // i'm gonna leave this out for now
  if (text1 != 0 ) delete text1;
  text1 = new TLatex(3.570061,23.08044,"CMS Preliminary");
  text1->SetNDC();
  text1->SetTextAlign(13);
  text1->SetX(0.68);
  text1->SetY(ypos);
  text1->SetTextFont(42);
  text1->SetTextSizePixels(24);
  text1->Draw();


  if (normalized_ == false) {
    TString astring;
    astring.Form("%.0f pb^{-1} at #sqrt{s} = 7 TeV",lumiScale_);
    if (text2 != 0 ) delete text2;
    text2 = new TLatex(3.570061,23.08044,astring);
    text2->SetNDC();
    text2->SetTextAlign(13);
    text2->SetX(0.17);
    text2->SetY(ypos+0.005);//0.88); //0.005 is a kluge to make things look right. i don't get it
    text2->SetTextFont(42);
    text2->SetTextSizePixels(24);
    text2->Draw();
  }

}

TString getVariedSubstring(TString currentVariation) {

  TObjArray* baseline = configDescriptions_[1].Tokenize("_");

  TObjArray* mine = currentVariation.Tokenize("_");

  TString output="";
  for (int i=0; i<baseline->GetEntries(); i++) {
    TString b=baseline->At(i)->GetName();
    TString m=mine->At(i)->GetName();
    if (b!=m) {
      output+=b;
    }
  }
  return output;
}


int mainpadWidth; int mainpadHeight;
int ratiopadHeight = 250;
// TPad* mainPad=0;
// TPad* ratioPad=0;
void renewCanvas(const TString opt="") {
  if (thecanvas!=0) delete thecanvas;

  int canvasWidth = mainpadWidth;
  int canvasHeight = opt.Contains("ratio") ? mainpadHeight+ratiopadHeight : mainpadHeight;

  thecanvas= new TCanvas("thecanvas","the canvas",canvasWidth,canvasHeight);
  thecanvas->cd()->SetRightMargin(0.04);
  thecanvas->cd()->SetTopMargin(0.07); //test

  if (opt.Contains("ratio")) {
    thecanvas->Divide(1,2);
    const float padding=0.01; const float ydivide=0.2;
    thecanvas->GetPad(1)->SetPad( padding, ydivide + padding, 1-padding, 1-padding);
    thecanvas->GetPad(2)->SetPad( padding, padding, 1-padding, ydivide-padding);
    if (!quiet_)  cout<< thecanvas->GetPad(1)->GetXlowNDC() <<"\t"
		      << thecanvas->GetPad(1)->GetWNDC() <<"\t"
		      << thecanvas->GetPad(1)->GetYlowNDC() <<"\t"
		      << thecanvas->GetPad(1)->GetHNDC() <<endl;
    if (logy_) thecanvas->GetPad(1)->SetLogy();
  }
  else { if (logy_) thecanvas->SetLogy(); }


  int cdarg = opt.Contains("ratio") ? 1 : 0;
  thecanvas->cd(cdarg);

}

void resetPadDimensions() {
  mainpadWidth = 600; 
  mainpadHeight=550;
}

void setPadDimensions(int x, int y) {

  mainpadWidth = x; 
  mainpadHeight= y;
}

void renewLegend() {

  if (leg!=0) delete leg;
  leg = new TLegend(leg_x1, leg_y1, leg_x2, leg_y2);
  leg->SetBorderSize(0);
  leg->SetLineStyle(0);
  leg->SetTextFont(42);
  leg->SetFillStyle(0);

}

void resetHistos() {
  for ( std::map<TString, TH1D*>::iterator i = histos_.begin(); i!=histos_.end(); ++i) {
    if (i->second != 0) {
      delete  i->second;
      i->second= 0;
    }
  }
}

double findOverallMax(const TH1D* hh) {

  double max=-1e9;

  for (int i=1; i<= hh->GetNbinsX(); i++) {
    double val = hh->GetBinContent(i) + hh->GetBinError(i);
    if (val>max) max=val;
  }
  return max;
}

//code largely lifted from Owen
//returned string is the y title of the renormalized histo
TString renormBins( TH1D* hp, int refbin ) {

  if ( hp==0 ) return "PROBLEM";

  double refbinwid = hp->GetBinLowEdge( refbin+1 ) - hp->GetBinLowEdge( refbin ) ;
  if (!quiet_)  printf(" reference bin: [%6.1f,%6.1f], width = %6.3f\n",  hp->GetBinLowEdge( refbin ), hp->GetBinLowEdge( refbin+1 ), refbinwid ) ;
  
  for ( int bi=1; bi<= hp->GetNbinsX(); bi++ ) {
    double binwid = hp->GetBinLowEdge( bi+1 ) - hp->GetBinLowEdge( bi ) ;
    double sf = refbinwid / binwid ;
    if (!quiet_)    printf("  bin %d : width= %6.2f, sf=%7.3f\n", bi, binwid, sf ) ;
    hp->SetBinContent( bi, sf*(hp->GetBinContent( bi )) ) ;
    hp->SetBinError( bi, sf*(hp->GetBinError( bi )) ) ;
  } // bi.

  TString ytitle;
  ytitle.Form("(Events / bin) * (%5.1f / bin width)", refbinwid );

  return ytitle;
}

void fillFlavorHistoryScaling() {

  TTree* tree=0;
  for (unsigned int isample=0; isample<samples_.size(); isample++) {
    if ( samples_[isample].Contains("WJets")) { //will pick out WJets or WJetsZ2
      tree = (TTree*) files_[currentConfig_][samples_[isample]]->Get("reducedTree");
    }
  }
  if (tree==0) {cout<<"Did not find a WJets sample!"<<endl; return;}

  gROOT->cd();
  TH1D dummyU("dummyU","",1,0,1e9);
  TH1D dummyk("dummyk","",1,0,1e9);
  assert(0);//please check that the following cuts have the weights and cuts handled correctly
  tree->Draw("HT>>dummyU","1","goff");
  tree->Draw("HT>>dummyk","flavorHistoryWeight","goff");
  flavorHistoryScaling_ = dummyU.Integral() / dummyk.Integral();
  if (!quiet_) cout<<"flavor history scaling factor = "<<flavorHistoryScaling_<<endl;

}

TString getCutString(double lumiscale, TString extraWeight="", TString thisSelection="", TString extraSelection="", int pdfWeightIndex=0,TString pdfSet="CTEQ", bool isSusyScan=false, int susySubProcess=-1) {
  TString weightedcut="weight"; 
  
  weightedcut += "*(";
  weightedcut +=lumiscale;
  weightedcut+=")";
  
  if (extraWeight=="flavorHistoryWeight") {
    if (flavorHistoryScaling_ <0) {
      fillFlavorHistoryScaling();
    }
    extraWeight.Form("flavorHistoryWeight*%f",flavorHistoryScaling_);
  }
  if (extraWeight!="") {
    weightedcut += "*(";
    weightedcut +=extraWeight;
    weightedcut+=")";
  }
  if (pdfWeightIndex != 0) {
    TString pdfString;
    pdfString.Form("*pdfWeights%s[%d]",pdfSet.Data(),pdfWeightIndex);
    weightedcut += pdfString;
  }
  if (usePUweight_) {
    weightedcut +="*PUweight";
  }
  if (useHLTeff_) {
    weightedcut +="*hltHTeff";
  }
  if (btagSFweight_=="") btagSFweight_="1";
  weightedcut += "*";
  weightedcut += btagSFweight_;
 
  if (thisSelection!="") {
    weightedcut += "*(";
    weightedcut+=thisSelection;
    if (extraSelection != "") {
      weightedcut += " && ";
      weightedcut +=extraSelection;
    }
    if (isSusyScan) {
      weightedcut += " && m0==";
      weightedcut +=m0_;
      weightedcut += " && m12==";
      weightedcut +=m12_;
    }
    weightedcut+=")";
  }
  else if (extraSelection !="") {
    weightedcut += "*(";
    weightedcut +=extraSelection;
    if (isSusyScan) {
      weightedcut += " && m0==";
      weightedcut +=m0_;
      weightedcut += " && m12==";
      weightedcut +=m12_;
    }
    weightedcut+=")";
  }

  //the easy part is done above -- keep only events at the current scan point
  //the hard part is to properly weight the events according to NLO cross sections
  if (isSusyScan) {
    //the cross section for the subprocess is stored in scanCrossSection (systematics in scanCrossSectionPlus and scanCrossSectionMinus)
    //the normalization for the subprocess is stored in TH1D scanProcessTotals_<m0>_<m12>
    loadSusyScanHistograms();
    TH1D* thishist = scanProcessTotalsMap[make_pair(m0_,m12_)];
    if (thishist==0) cout<<"We've got a problem in getCutString!"<<endl;
    TString susyprocessweight = "(";
    int lowbound=0; int highbound=10;
    if (susySubProcess>=0) { lowbound=susySubProcess; highbound=susySubProcess;}
    for (int i=0; i<=10; i++ ) {
      char thisweight[50];
      int thisn = TMath::Nint(thishist->GetBinContent(i));
      if (thisn==0) thisn=1; //avoid div by 0. if there are no events anyway then any value is ok
      sprintf(thisweight, "((SUSY_process==%d)*scanCrossSection%s/%d)",i,susyCrossSectionVariation_.Data(),thisn);
      susyprocessweight += thisweight;
      if (i!=10)    susyprocessweight += " + ";
    }
    susyprocessweight += ")";
    weightedcut+="*";
    weightedcut += susyprocessweight;
  }
  else if (susySubProcess>=0) {
    char thisweight[50];
    sprintf(thisweight, "*((SUSY_process==%d)*scanCrossSection%s)",susySubProcess,susyCrossSectionVariation_.Data());
    weightedcut += thisweight;
  }

  if (!quiet_)  cout<<weightedcut<<endl;
  return weightedcut;
}

//add an interface more like the old one, but with an addition for the data/MC lumi scaling
TString getCutString(bool isData, TString extraSelection="",TString extraWeight="",int pdfWeightIndex=0,TString pdfSet="CTEQ", bool isSusyScan=false, int susySubProcess=-1) {

  double ls = isData ? 1 : lumiScale_;
  return    getCutString(ls, extraWeight, selection_,extraSelection, pdfWeightIndex,pdfSet,isSusyScan,susySubProcess) ;
}

void addOverflowBin(TH1D* theHist) {
  //this code was written for when there was a customizable plot range (post-histo creation)
  //it could be made a lot simpler now

  int lastVisibleBin = theHist->GetNbinsX();
  //  cout<<theHist<<"  "<<lastVisibleBin<<"\t";

  //in case there is no custom range, the code should just add the overflow bin to the last bin of the histo
  double lastBinContent = theHist->GetBinContent(lastVisibleBin);
  double lastBinError = pow(theHist->GetBinError(lastVisibleBin),2); //square in prep for addition

  //  cout<<"Overflow addition: "<<lastBinContent<<" +/- "<<sqrt(lastBinError)<<" --> ";

  //now loop over the bins that aren't being shown at the moment (including the overflow bin)
  for (int ibin = lastVisibleBin+1; ibin <= 1 + theHist->GetNbinsX() ; ++ibin) {
    lastBinContent += theHist->GetBinContent( ibin);
    lastBinError += pow(theHist->GetBinError( ibin),2);
  }
  lastBinError = sqrt(lastBinError);

  theHist->SetBinContent(lastVisibleBin,lastBinContent);
  theHist->SetBinError(lastVisibleBin,lastBinError);
}


void addOverflowBin(TH1F* theHist) {
  //this code was written for when there was a customizable plot range (post-histo creation)
  //it could be made a lot simpler now
  //this one is copied from the function of the same name that takes a TH1D

  int lastVisibleBin = theHist->GetNbinsX();
  //  cout<<theHist<<"  "<<lastVisibleBin<<"\t";

  //in case there is no custom range, the code should just add the overflow bin to the last bin of the histo
  double lastBinContent = theHist->GetBinContent(lastVisibleBin);
  double lastBinError = pow(theHist->GetBinError(lastVisibleBin),2); //square in prep for addition

  //  cout<<"Overflow addition: "<<lastBinContent<<" +/- "<<sqrt(lastBinError)<<" --> ";

  //now loop over the bins that aren't being shown at the moment (including the overflow bin)
  for (int ibin = lastVisibleBin+1; ibin <= 1 + theHist->GetNbinsX() ; ++ibin) {
    lastBinContent += theHist->GetBinContent( ibin);
    lastBinError += pow(theHist->GetBinError( ibin),2);
  }
  lastBinError = sqrt(lastBinError);

  theHist->SetBinContent(lastVisibleBin,lastBinContent);
  theHist->SetBinError(lastVisibleBin,lastBinError);
}

void drawVerticalLine() {
  if (thecanvas==0) return;

  //this is a fine example of ROOT idiocy
  TVirtualPad* thePad = thecanvas->GetPad(0); //needs fixing for ratio plots
  double xmin,ymin,xmax,ymax;
  thePad->GetRangeAxis(xmin,ymin,xmax,ymax);
  //for academic interest, can get the same numbers using e.g. thePad->GetUymax()
  if (logy_) {
    ymax = pow(10, ymax);
    ymin = pow(10, ymin);
  }
  TLine theLine(verticalLinePosition_,ymin,verticalLinePosition_,ymax);
  theLine.SetLineColor(kBlue);
  theLine.SetLineWidth(3);

  theLine.DrawClone();

}

//add a sample to be plotted to the *end* of the list
void addSample(const TString & newsample) {

  //see if it is already there
  for (std::vector<TString>::iterator it = samples_.begin(); it!=samples_.end(); ++it) {
    if ( *it == newsample) {
      cout<<newsample<<" is already on the list!"<<endl;
      return;
    }
  }
  //if it isn't there, go ahead and add it

  if ( samplesAll_.find(newsample) != samplesAll_.end() ) {
    samples_.push_back(newsample);
  }
  else {
    cout<<"Could not find sample with name "<<newsample<<endl;
  }
}

void clearSamples() {
  samples_.clear();
}

void removeSample(const TString & sample) {

  for (std::vector<TString>::iterator it = samples_.begin(); it!=samples_.end(); ++it) {
    if ( *it == sample) {
      if (!quiet_) cout<<sample<<" removed from plotting list!"<<endl;
      samples_.erase(it);
      return;
    }
  }

  //if we get down here then we didn't find the sample
  cout<<sample<<" could not be found on the plotting list, so I did not remove it!"<<endl;
}

void setColorScheme(const TString & name) {
  if (name == "stack") {
    sampleColor_["LM13"] = kGray;
    sampleColor_["LM9"] =kGray;
    sampleColor_["mSUGRAtanb40"] =kGray;
    sampleColor_["QCD"] = kYellow;
    sampleColor_["PythiaQCD"] = kYellow;
    sampleColor_["PythiaPUQCD"] = kYellow;
    sampleColor_["PythiaPUQCDFlat"] = kYellow;
    sampleColor_["TTbarJets"]=kRed+1;
    sampleColor_["SingleTop"] = kMagenta;
    sampleColor_["WJets"] = kGreen-3;
    sampleColor_["WJetsZ2"] = kGreen-3;
    sampleColor_["ZJets"] = kAzure-2;
    sampleColor_["Zinvisible"] = kOrange-3;
    sampleColor_["SingleTop-sChannel"] = kMagenta+1; //for special cases
    sampleColor_["SingleTop-tChannel"] = kMagenta+2; //for special cases
    sampleColor_["SingleTop-tWChannel"] = kMagenta+3; //for special cases
    sampleColor_["TotalSM"] = kBlue+2;
    sampleColor_["Total"] = kGreen+3;
    sampleColor_["VV"] = kCyan+1;

  }
  else if (name == "nostack" || name=="owen") {
    sampleColor_["LM13"] = kBlue+2;
    sampleColor_["LM9"] = kCyan+2;
    sampleColor_["mSUGRAtanb40"] =kCyan+2;
    sampleColor_["QCD"] = 2;
    sampleColor_["PythiaQCD"] = 2;
    sampleColor_["PythiaPUQCD"] =2;
    sampleColor_["PythiaPUQCDFlat"] =2;
    sampleColor_["TTbarJets"]=4;
    sampleColor_["SingleTop"] = kMagenta;
    sampleColor_["WJets"] = kOrange;
    sampleColor_["WJetsZ2"] = kOrange;
    sampleColor_["ZJets"] = 7;
    sampleColor_["Zinvisible"] = kOrange+7;
    sampleColor_["SingleTop-sChannel"] = kMagenta+1; //for special cases
    sampleColor_["SingleTop-tChannel"] = kMagenta+2; //for special cases
    sampleColor_["SingleTop-tWChannel"] = kMagenta+3; //for special cases
    sampleColor_["TotalSM"] =kGreen+2; //owen requested 3
    sampleColor_["Total"] = 6;
    sampleColor_["VV"] = kOrange-3;
  }
  else {
    cout<<"Sorry, color scheme "<<name<<" is not known!"<<endl;
  }

}

void resetSamples(bool joinSingleTop=true) {

  samples_.clear();
  //this block controls what samples will enter your plot
  //order of this vector controls order of samples in stack

  //careful -- QCD must have 'QCD' in its name somewhere.
  //samples_.push_back("QCD"); //madgraph
  //samples_.push_back("PythiaQCD");
  samples_.push_back("PythiaPUQCD");
  //samples_.push_back("PythiaPUQCDFlat");

  samples_.push_back("TTbarJets");
  //flip this bool to control whether SingleTop is loaded as one piece or 3
  if (joinSingleTop) samples_.push_back("SingleTop");
  else {
    samples_.push_back("SingleTop-sChannel");
    samples_.push_back("SingleTop-tChannel");
    samples_.push_back("SingleTop-tWChannel");
  }
  samples_.push_back("WJets");
  samples_.push_back("ZJets");
  samples_.push_back("VV");
  samples_.push_back("Zinvisible");
  samples_.push_back("LM9");

}

void loadSamples(bool joinSingleTop=true) {
  if (loaded_) return;
  loaded_=true;

  resetPadDimensions();

  resetSamples(joinSingleTop);
  //samplesAll_ should have *every* available sample
  //also note that there's no harm in failing to load one of these samples, 
  //as long as you don't actually try to draw it

  //samplesAll_.insert("QCD");
  //samplesAll_.insert("PythiaQCD");
  samplesAll_.insert("PythiaPUQCD");
  //samplesAll_.insert("PythiaPUQCDFlat");
  samplesAll_.insert("TTbarJets");
  samplesAll_.insert("WJets");
  samplesAll_.insert("ZJets");
  samplesAll_.insert("Zinvisible");
  samplesAll_.insert("SingleTop");
  samplesAll_.insert("SingleTop-sChannel");
  samplesAll_.insert("SingleTop-tChannel");
  samplesAll_.insert("SingleTop-tWChannel");

  //  samplesAll_.insert("WJetsZ2");
  //  samplesAll_.insert("ZJetsZ2");
  samplesAll_.insert("VV");


  samplesAll_.insert("LM13");
  samplesAll_.insert("LM9");

  samplesAll_.insert("mSUGRAtanb40");

  //  configDescriptions_.push_back("SSVHPT");
  configDescriptions_.push_back("SSVHPT_PF2PATjets_JES0_JER0_PFMET_METunc0_PUunc0_BTagEff0_HLTEff0");
  configDescriptions_.push_back("SSVHPT_PF2PATjets_JES0_JERbias_PFMET_METunc0_PUunc0_BTagEff0_HLTEff0");

  //JES
  configDescriptions_.push_back("SSVHPT_PF2PATjets_JESdown_JERbias_PFMET_METunc0_PUunc0_BTagEff0_HLTEff0");
  configDescriptions_.push_back("SSVHPT_PF2PATjets_JESup_JERbias_PFMET_METunc0_PUunc0_BTagEff0_HLTEff0");
  //JER
  //  configDescriptions_.push_back("SSVHPT_PF2PATjets_JES0_JERdown_PFMET_METunc0_PUunc0_BTagEff0_HLTEff0");
  //  configDescriptions_.push_back("SSVHPT_PF2PATjets_JES0_JERup_PFMET_METunc0_PUunc0_BTagEff0_HLTEff0");

  //unclustered MET
  configDescriptions_.push_back("SSVHPT_PF2PATjets_JES0_JERbias_PFMET_METuncDown_PUunc0_BTagEff0_HLTEff0");
  configDescriptions_.push_back("SSVHPT_PF2PATjets_JES0_JERbias_PFMET_METuncUp_PUunc0_BTagEff0_HLTEff0");

  //PU
  //  configDescriptions_.push_back("SSVHPT_PF2PATjets_JES0_JERbias_PFMET_METunc0_PUuncDown_BTagEff0_HLTEff0");
  //  configDescriptions_.push_back("SSVHPT_PF2PATjets_JES0_JERbias_PFMET_METunc0_PUuncUp_BTagEff0_HLTEff0");

  //btag eff
  configDescriptions_.push_back("SSVHPT_PF2PATjets_JES0_JERbias_PFMET_METunc0_PUunc0_BTagEffdown_HLTEff0");
  configDescriptions_.push_back("SSVHPT_PF2PATjets_JES0_JERbias_PFMET_METunc0_PUunc0_BTagEffup_HLTEff0");

  //HLT eff
  //  configDescriptions_.push_back("SSVHPT_PF2PATjets_JES0_JERbias_PFMET_METunc0_PUunc0_BTagEff0_HLTEffdown");
  //  configDescriptions_.push_back("SSVHPT_PF2PATjets_JES0_JERbias_PFMET_METunc0_PUunc0_BTagEff0_HLTEffup");

  //convention is that the [0] one should always be the "nominal" one while others are for systematics
  currentConfig_=configDescriptions_[0];

  //these blocks are just a "dictionary"
  //no need to ever comment these out
  setColorScheme("stack");

  sampleLabel_["mSUGRAtanb40"] = "tan #beta = 40";
  sampleLabel_["LM13"] = "LM13";
  sampleLabel_["LM9"] = "LM9";
  sampleLabel_["QCD"] = "QCD (madgraph)";
  sampleLabel_["PythiaQCD"] = "QCD (no PU)";
  sampleLabel_["PythiaPUQCDFlat"] = "QCD"; 
  sampleLabel_["PythiaPUQCD"] = "QCD";
  sampleLabel_["TTbarJets"]="t#bar{t}";
  sampleLabel_["SingleTop"] = "Single-Top";
  sampleLabel_["WJets"] = "W#rightarrowl#nu";
  sampleLabel_["WJetsZ2"] = "W#rightarrowl#nu (Z2)";
  sampleLabel_["ZJets"] = "Z/#gamma*#rightarrowl^{+}l^{-}";
  sampleLabel_["Zinvisible"] = "Z#rightarrow#nu#nu";
  sampleLabel_["SingleTop-sChannel"] = "Single-Top (s)";
  sampleLabel_["SingleTop-tChannel"] = "Single-Top (t)";
  sampleLabel_["SingleTop-tWChannel"] = "Single-Top (tW)";
  sampleLabel_["VV"] = "Diboson";
  sampleLabel_["TotalSM"] = "SM";
  sampleLabel_["Total"] = "SM + LM13"; //again, this is a hack

  sampleMarkerStyle_["mSUGRAtanb40"] = kFullStar;
  sampleMarkerStyle_["LM13"] = kFullStar;
  sampleMarkerStyle_["LM9"] = kFullStar;
  sampleMarkerStyle_["QCD"] = kFullCircle;
  sampleMarkerStyle_["PythiaQCD"] = kOpenCircle;
  sampleMarkerStyle_["PythiaPUQCDFlat"] = kOpenCircle;  
  sampleMarkerStyle_["PythiaPUQCD"] = kOpenCircle;
  sampleMarkerStyle_["TTbarJets"]= kFullSquare;
  sampleMarkerStyle_["SingleTop"] = kOpenSquare;
  sampleMarkerStyle_["WJets"] = kMultiply;
  sampleMarkerStyle_["WJetsZ2"] = kMultiply;
  sampleMarkerStyle_["ZJets"] = kFullTriangleUp;
  sampleMarkerStyle_["Zinvisible"] = kFullTriangleDown;
  sampleMarkerStyle_["VV"] = kOpenCross;
  sampleMarkerStyle_["SingleTop-sChannel"] = kOpenSquare;
  sampleMarkerStyle_["SingleTop-tChannel"] = kOpenSquare;
  sampleMarkerStyle_["SingleTop-tWChannel"] = kOpenSquare;
  sampleMarkerStyle_["TotalSM"] = kOpenCross; //FIXME?
  sampleMarkerStyle_["Total"] = kDot; //FIXME?

  sampleOwenName_["mSUGRAtanb40"] = "msugra40";
  sampleOwenName_["LM13"] = "lm13";
  sampleOwenName_["LM9"] = "lm9";
  sampleOwenName_["QCD"] = "qcd";
  sampleOwenName_["PythiaQCD"] = "qcd";
  sampleOwenName_["PythiaPUQCDFlat"] = "qcd"; 
  sampleOwenName_["PythiaPUQCD"] = "qcd";
  sampleOwenName_["TTbarJets"]="ttbar";
  sampleOwenName_["SingleTop"] = "singletop";
  sampleOwenName_["WJets"] = "wjets";
  sampleOwenName_["WJetsZ2"] = "wjets";
  sampleOwenName_["ZJets"] = "zjets";
  sampleOwenName_["Zinvisible"] = "zinvis";
  sampleOwenName_["VV"] = "vv";
  sampleOwenName_["SingleTop-sChannel"] = "singletops";
  sampleOwenName_["SingleTop-tChannel"] = "singletopt";
  sampleOwenName_["SingleTop-tWChannel"] = "singletoptw";
  sampleOwenName_["TotalSM"] = "totalsm";
  sampleOwenName_["Total"] = "total";  

  for (std::vector<TString>::iterator iconfig=configDescriptions_.begin(); iconfig!=configDescriptions_.end(); ++iconfig) {
    for (std::set<TString>::iterator isample=samplesAll_.begin(); isample!=samplesAll_.end(); ++isample) {
      TString fname="reducedTree.";
      fname += *iconfig;
      fname+=".";
      fname += *isample;
      fname+=".root";
      fname.Prepend(inputPath);
      files_[*iconfig][*isample] = new TFile(fname);
      if (files_[*iconfig][*isample]->IsZombie() ) {cout<<"file error with "<<*isample<<endl; files_[*iconfig][*isample]=0;}
      else { if (!quiet_)    cout<<"Added sample: "<<*iconfig<<"\t"<<*isample<<endl;}
    }
  }

  //load data file too
  TString dname="reducedTree.";
  dname+=currentConfig_;
  //dname+=".data.root";
  //dname+=".ht_run2011a_SUM_promptrecov4only_uptojun24.root";
  //dname+=".data_promptrecoThroughJul1.root";
  dname+=".ht*.root";
  //dname+="*.root";
  //dname+=".ht_run2011a_SUM_promptrecov4only_uptojul1.root";
  dname.Prepend(dataInputPath);
  dname.ReplaceAll("JERbias","JER0"); //JERbias not relevant for data
  if ( dodata_) {
    dtree = new TChain("reducedTree");
    dtree->Add(dname);
  }

}

//if something is passed to varbins, then low and high will be ignored
float drawSimple(const TString var, const int nbins, const float low, const float high, const TString filename, 
		 const TString histname , const TString samplename, const float* varbins=0) {

  loadSamples();
  gROOT->SetStyle("CMS");
//I would rather implement this functionality via drawPlots(), but I think it will be simpler
//to just write something simple

//no presentation, just fill the histogram and save
  TTree* tree=0;
  if (samplename=="data") {
    tree = dtree;
  }
  else {
    for (unsigned int isample=0; isample<samples_.size(); isample++) {
      if ( samples_[isample] == samplename) {
	if (!quiet_) cout <<samples_[isample]<<endl;
	tree = (TTree*) files_[currentConfig_][samples_[isample]]->Get("reducedTree");
      }
    }
  }
  if (tree==0) {cout<<"Something went wrong finding your sample!"<<endl; return 0;}
  gROOT->cd();
  
  TH1D* hh=0;
  if (varbins==0) {
    hh = new TH1D(histname,histname,nbins,low,high);
  }
  else {
    hh = new TH1D(histname,histname,nbins,varbins);
  }
  hh->Sumw2();
   
  TString optfh= useFlavorHistoryWeights_ && samplename.Contains("WJets") ? "flavorHistoryWeight" : "";
  if(samplename=="data") tree->Project(histname,var,getCutString(1.,optfh,selection_,"",0).Data());
  else tree->Project(histname,var,getCutString(lumiScale_,optfh,selection_,"",0,"", samplename.Contains("mSUGRA")).Data());
  float theIntegral = hh->Integral(0,nbins+1);

  if (addOverflow_)  addOverflowBin( hh ); //manipulates the TH1D

  //at this point i've got a histogram. what more could i want?
  TFile fout(filename,"UPDATE");
  hh->Write();
  fout.Close();
  delete hh; //deleting ROOT objects can be dangerous...but i've tried carefully to avoid a double deletion here by doing gROOT->cd() before the creation of hh
  return theIntegral;
}

float drawSimple(const TString var, const int nbins, const float* varbins, const TString filename, 
		 const TString histname , const TString samplename) {
  return drawSimple(var, nbins, 0, 1, filename, histname, samplename, varbins);
}


void draw2d(const TString var, const int nbins, const float low, const float high, 
	    const TString vary, const int nbinsy, const float lowy, const float highy,
	    const TString xtitle, TString ytitle, TString filename="") {

  //for now hard-code this to plot only the first sample and only COLZ!

  loadSamples();
  if (filename=="") filename=var;
  gROOT->SetStyle("CMS");

  renewCanvas();
  if (h2d!=0) delete h2d;
  const TString hname="h2d";
  h2d = new TH2D(hname,"",nbins,low,high,nbinsy,lowy,highy);
  h2d->Sumw2();
  TString opt="colz";
  for (unsigned int isample=0; isample<1 ; isample++) { //plot only the first sample!
    gROOT->cd();
    TTree* tree = (TTree*) files_[currentConfig_][samples_[isample]]->Get("reducedTree");
    gROOT->cd();
    TString weightopt= useFlavorHistoryWeights_ && samples_[isample].Contains("WJets") ? "flavorHistoryWeight" : "";
    TString drawstring=vary;
    drawstring+=":";
    drawstring+=var;
    tree->Project(hname,drawstring,getCutString(false,"",weightopt,0,"",samples_[isample].Contains("mSUGRA") ).Data());
    //now the histo is filled
    
    h2d->SetXTitle(xtitle);
    h2d->SetYTitle(ytitle);
  }
  h2d->Draw(opt);

  TString savename = filename;
  if (logy_) savename += "-logY";

  savename += "-draw2d";

  //amazingly, \includegraphics cannot handle an extra dot in the filename. so avoid it.
  if (savePlots_) {
    thecanvas->SaveAs(savename+".eps"); //for me
    //  thecanvas->Print(savename+".C");    //for formal purposes
    thecanvas->SaveAs(savename+".pdf"); //for pdftex
    thecanvas->SaveAs(savename+".png"); //for twiki
  }

}

bool isSampleSM(const TString & name) {

  if (name.Contains("LM")) return false;

  if (name.Contains("SUGRA")) return false;

  return true;
}

void drawPlots(const TString var, const int nbins, const float low, const float high, const TString xtitle, TString ytitle, TString filename="", const float* varbins=0) {
  //  cout<<"[drawPlots] var = "<<var<<endl;

  loadSamples();

  if (filename=="") filename=var;

  //  TH1D* thestackH=0;

  gROOT->SetStyle("CMS");
  //gStyle->SetHatchesLineWidth(1);

  TString canvasOpt = doRatio_ ? "ratio" : "";
  const int mainPadIndex = doRatio_ ? 1 : 0;
  renewCanvas(canvasOpt);

  thecanvas->cd(mainPadIndex);
  renewLegend();

  if (dostack_) {
    if (thestack!= 0 ) delete thestack;
    thestack = new THStack("thestack","--");
    if (doRatio_) {
      if (ratio!=0) delete ratio;
      ratio = (varbins==0) ? new TH1D("ratio","data/(SM MC)",nbins,low,high) : new TH1D("ratio","",nbins,varbins);
      ratio->Sumw2();
    }
  }
  if (totalsm!=0) delete totalsm;
  totalsm = (varbins==0) ? new TH1D("totalsm","",nbins,low,high) : new TH1D("totalsm","",nbins,varbins);
  totalsm->Sumw2();
  if (totalsmsusy!=0) delete totalsmsusy;
  totalsmsusy = (varbins==0) ? new TH1D("totalsmsusy","",nbins,low,high) : new TH1D("totalsmsusy","",nbins,varbins);
  totalsmsusy->Sumw2();
  if (totalewk!=0) delete totalewk;
  totalewk = (varbins==0) ? new TH1D("totalewk","",nbins,low,high) : new TH1D("totalewk","",nbins,varbins);
  totalewk->Sumw2();
  if (totalqcdttbar!=0) delete totalqcdttbar;
  totalqcdttbar = (varbins==0) ? new TH1D("totalqcdttbar","",nbins,low,high) : new TH1D("totalqcdttbar","",nbins,varbins);
  totalqcdttbar->Sumw2();
  if (totalnonttbar!=0) delete totalnonttbar;
  totalnonttbar = (varbins==0) ? new TH1D("totalnonttbar","",nbins,low,high) : new TH1D("totalnonttbar","",nbins,varbins);
  totalnonttbar->Sumw2();
  if (totalnonqcd!=0) delete totalnonqcd;
  totalnonqcd = (varbins==0) ? new TH1D("totalnonqcd","",nbins,low,high) : new TH1D("totalnonqcd","",nbins,varbins);
  totalnonqcd->Sumw2();
  if (totalqcd!=0) delete totalqcd;
  totalqcd = (varbins==0) ? new TH1D("totalqcd","",nbins,low,high) : new TH1D("totalqcd","",nbins,varbins);
  totalqcd->Sumw2();

  totalsm->SetMarkerColor(sampleColor_["TotalSM"]);
  totalsm->SetLineColor(sampleColor_["TotalSM"]);
  totalsm->SetLineWidth(2);
  totalsm->SetMarkerStyle(sampleMarkerStyle_["TotalSM"]);
  if (!drawMarkers_)  totalsm->SetMarkerSize(0); //no marker for this one

  totalsmsusy->SetMarkerColor(sampleColor_["Total"]);
  totalsmsusy->SetLineColor(sampleColor_["Total"]);
  totalsmsusy->SetLineWidth(2);
  totalsmsusy->SetMarkerStyle(sampleMarkerStyle_["Total"]);
  if (!drawMarkers_)  totalsmsusy->SetMarkerSize(0); //no marker for this one

  //here is the part that is really different from the previous implementation
  //need to make new histograms
  resetHistos(); //delete existing histograms
  TString opt="hist e";
  double histMax=-1e9;
  for (unsigned int isample=0; isample<samples_.size(); isample++) {
    if (!quiet_)   cout <<samples_[isample]<<endl;

    gROOT->cd();
    //should each histo have a different name? maybe
    TString hname = jmt::fortranize(var); hname += "_"; hname += samples_[isample];
    histos_[samples_[isample]] = (varbins==0) ? new TH1D(hname,"",nbins,low,high) : new TH1D(hname,"",nbins,varbins);
    histos_[samples_[isample]]->Sumw2();

    TTree* tree = (TTree*) files_[currentConfig_][samples_[isample]]->Get("reducedTree");
    gROOT->cd();
    TString weightopt= useFlavorHistoryWeights_ && samples_[isample].Contains("WJets") ? "flavorHistoryWeight" : "";
    tree->Project(hname,var,getCutString(lumiScale_,weightopt,selection_,"",0,"",samples_[isample].Contains("mSUGRA")).Data());
    //now the histo is filled
    
    if (renormalizeBins_) ytitle=renormBins(histos_[samples_[isample]],2 ); //manipulates the TH1D //FIXME hard-coded "2"
    if (addOverflow_)  addOverflowBin( histos_[samples_[isample]] ); //manipulates the TH1D
    histos_[samples_[isample]]->SetXTitle(xtitle);
    histos_[samples_[isample]]->SetYTitle(ytitle);

    hinteractive = histos_[samples_[isample]];// hinteractive will point to the last sample's histo

    if (isSampleSM(samples_[isample])) {
      totalsm->Add(histos_[samples_[isample]]);
      //      if (!quiet_)    cout << "totalsm: " << samples_[isample] << endl;
    }
    if (!samples_[isample].Contains("LM") && !samples_[isample].Contains("QCD") && !samples_[isample].Contains("TTbar") && !samples_[isample].Contains("SUGRA")) {
      totalewk->Add(histos_[samples_[isample]]);
      //      if (!quiet_) cout << "totalewk: " << samples_[isample] << endl;
    }
    if (samples_[isample].Contains("QCD") || samples_[isample].Contains("TTbar")){
      totalqcdttbar->Add(histos_[samples_[isample]]);
      //      if (!quiet_) cout << "totalqcdttbar: " << samples_[isample] << endl;
    }
    if (!samples_[isample].Contains("TTbar") && !samples_[isample].Contains("LM") && !samples_[isample].Contains("SUGRA")) {
      totalnonttbar->Add(histos_[samples_[isample]]);
      //      if (!quiet_) cout << "totalnonttbar: " << samples_[isample] << endl;
    }
    if (!samples_[isample].Contains("QCD") && !samples_[isample].Contains("LM")&& !samples_[isample].Contains("SUGRA")){
       totalnonqcd->Add(histos_[samples_[isample]]);
       //      if (!quiet_) cout << "totalnonqcd: " << samples_[isample] << endl;
    }
    if (samples_[isample].Contains("QCD")){
       totalqcd->Add(histos_[samples_[isample]]);
       //      if (!quiet_) cout << "totalqcd: " << samples_[isample] << endl;
    }
    totalsmsusy->Add(histos_[samples_[isample]]); //add everything!

    //now just do a bunch of histogram formatting
    if (!dostack_) {
      //set line color instead of fill color for this type of plot
      histos_[samples_[isample]]->SetLineColor(sampleColor_[samples_[isample]]);
      histos_[samples_[isample]]->SetMarkerStyle(sampleMarkerStyle_[samples_[isample]]);
      histos_[samples_[isample]]->SetMarkerColor(sampleColor_[samples_[isample]]);
      if (!drawMarkers_) histos_[samples_[isample]]->SetMarkerSize(0);

      //ad hoc additions
      histos_[samples_[isample]]->SetLineWidth(2);
    }
    else {
      histos_[samples_[isample]]->SetFillColor(sampleColor_[samples_[isample]]);
      histos_[samples_[isample]]->SetMarkerSize(0);
    }

    if (dostack_) { //add histo to stack
      leg->AddEntry(histos_[samples_[isample]], sampleLabel_[samples_[isample]]);
      thestack->Add(histos_[samples_[isample]] );
    }
    else { //draw non-stacked histo
      //normalize
      if ( normalized_ && histos_[samples_[isample]]->Integral() >0)  histos_[samples_[isample]]->Scale( 1.0 / histos_[samples_[isample]]->Integral() );
      if (!drawSusyOnly_ || samples_[isample].Contains("LM")|| samples_[isample].Contains("SUGRA")) { //drawSusyOnly_ means don't draw SM
	//set max
	if ( findOverallMax( histos_[samples_[isample]]) > histMax) histMax = findOverallMax(histos_[samples_[isample]]);
	
	leg->AddEntry(histos_[samples_[isample]], sampleLabel_[samples_[isample]]);

	histos_[samples_[isample]]->Draw(opt);
	if (!opt.Contains("same")) opt+=" same";
      }
    }
  } //loop over samples and fill histograms

  if (drawTotalSM_) leg->AddEntry(totalsm, sampleLabel_["TotalSM"]);
  if (drawTotalSMSusy_) leg->AddEntry(totalsmsusy, sampleLabel_["Total"]);

  if (!dostack_) {
    //this is all a re-implemenataion of stuff done is HistHolder. Oh well.

    if (drawTotalSM_) histMax = totalsm->GetMaximum();
    for (unsigned int isample=0; isample<samples_.size(); isample++) {
      double pmx= doCustomPlotMax_ ? customPlotMax_ : histMax*maxScaleFactor_;
      histos_[samples_[isample]]->SetMaximum(pmx);
      if (doCustomPlotMin_) histos_[samples_[isample]]->SetMinimum(customPlotMin_);
    }

    if (drawTotalSM_) { 
      totalsm->Draw(opt); 
      if (doCustomPlotMax_) totalsm->SetMaximum(customPlotMax_);
    }
    if (drawTotalSMSusy_) {
      totalsmsusy->Draw(opt); 
      if (doCustomPlotMax_) totalsmsusy->SetMaximum(customPlotMax_);
    }
  }
  else {
    thestack->Draw("hist");
    thestack->GetHistogram()->GetXaxis()->SetTitle(xtitle);
    thestack->GetHistogram()->GetYaxis()->SetTitle(ytitle);

    if (doVerticalLine_) drawVerticalLine(); //i want to draw the data last

    if (drawMCErrors_) {
      if (mcerrors!=0) delete mcerrors;
      mcerrors = new TGraphErrors(totalsm);
      mcerrors->SetFillStyle(3353); //3353 3544
      mcerrors->SetFillColor(1);
      //ack. TGraphs and TH1s use different conventions for numbering.
      for ( int ibin=1; ibin<=totalsm->GetNbinsX(); ibin++) {
	double yerr = mcerrors->GetErrorY(ibin-1);
	double xerr = totalsm->GetBinCenter(ibin) - totalsm->GetBinLowEdge(ibin);
	mcerrors->SetPointError(ibin-1,xerr,yerr);
      }
      mcerrors->Draw("2 same");
    }

    if (doCustomPlotMax_) thestack->SetMaximum(customPlotMax_);
    if (doCustomPlotMin_) thestack->SetMinimum(customPlotMin_);
  } //if doStack_

  if (dodata_) {
    gROOT->cd();
    //    if (!quiet_)     cout<<"Drawing data!"<<endl;
    if (hdata != 0) delete hdata;
    TString hname = jmt::fortranize(var); hname += "_"; hname += "data";
    hdata = (varbins==0) ? new TH1D(hname,"",nbins,low,high) : new TH1D(hname,"",nbins,varbins);
    hdata->Sumw2();
    gROOT->cd();
    dtree->Project(hname,var,selection_.Data());
    //now the histo is filled
    
    hdata->UseCurrentStyle(); //maybe not needed anymore
    hdata->SetMarkerColor(kBlack);
    hdata->SetLineWidth(2);
    hdata->SetMarkerStyle(kFullCircle);
    hdata->SetMarkerSize(1);
    if (renormalizeBins_) renormBins(hdata,2 ); //manipulates the histogram //FIXME hard-coded "2"
    if (addOverflow_)     addOverflowBin(hdata); // manipulates the histogram!
    leg->AddEntry(hdata,"Data");

    if (!quiet_)    cout<<"Data underflow: " <<hdata->GetBinContent(0)<<endl;//BEN
    hdata->Draw("SAME");
    if (!doCustomPlotMax_) {
      double mymax=-1e9;
      if (dostack_) mymax = thestack->GetMaximum();
      if ( findOverallMax(totalsm) >mymax) mymax = findOverallMax(totalsm);
      if (findOverallMax(hdata) > mymax) mymax = findOverallMax(hdata);
      if (dostack_) thestack->SetMaximum( maxScaleFactor_*mymax);
      else { //i don't like repeating this loop constantly; at a minimum it should be abstracted
	for (unsigned int isample=0; isample<samples_.size(); isample++)  histos_[samples_[isample]]->SetMaximum(maxScaleFactor_*mymax);
      }
    }


    if (!quiet_ && !renormalizeBins_) {
      cout<<"Integral of data, EW, total SM: "<<hdata->Integral()<<" ; "<<totalewk->Integral()<<" ; "<<totalsm->Integral()<<endl;
      cout<<"Chi^2 Test results: "<<hdata->Chi2Test(totalsm,"UW P")<<endl;
      cout<<"KS Test results: "<<hdata->KolmogorovTest(totalsm,"N")<<endl;;
    }
    if (doRatio_) {
      thecanvas->cd(2);
      ratio->Divide(hdata,totalsm);
      ratio->SetMinimum(ratioMin);
      ratio->SetMaximum(ratioMax);
      ratio->GetYaxis()->SetNdivisions(200 + int(ratioMax-ratioMin)+1);    //set ticks ; to be seen if this really works
      ratio->GetYaxis()->SetLabelSize(0.2); //make y label bigger
      ratio->Draw();
      thecanvas->GetPad(2)->SetTopMargin(0.1);
    }
  }
  
  if (!quiet_ && dostack_ && dodata_ && nbins<11 && doRatio_) {//BEN - 11 is an arbitrary number that isn't too big so we don't print out too much stuff.
    for(int i=1; i<=nbins; i++){
      cout << "data: " << hdata->GetBinContent(i) << " +- " << hdata->GetBinError(i) << ", totalsm: " << totalsm->GetBinContent(i) << " +- " << totalsm->GetBinError(i) << ", ratio: " << ratio->GetBinContent(i) << " +- " << ratio->GetBinError(i) << endl;
    }
  }
  

  thecanvas->cd(mainPadIndex);
  if (doleg_)  leg->Draw();
  drawPlotHeader();

  //  if (doSubtraction_) savename+="-MCSub";
  TString savename = filename;
  if (logy_) savename += "-logY";
  //  savename += scaleAppendToFilename;

  if (!dostack_ && !normalized_)      savename += "-drawPlain";
  else if (!dostack_ && normalized_)  savename += "-drawNorm";
  else savename += "-drawStack";

  //amazingly, \includegraphics cannot handle an extra dot in the filename. so avoid it.
  if (savePlots_) {
    thecanvas->SaveAs(savename+".eps"); //for me
    //  thecanvas->Print(savename+".C");    //for formal purposes
    thecanvas->SaveAs(savename+".pdf"); //for pdftex
    thecanvas->SaveAs(savename+".png"); //for twiki
  }

  //dump some event counts to the screen
  if (!quiet_) {
    for (unsigned int isample=0; isample<samples_.size(); isample++) {
      cout<<samples_[isample]<<" =\t "<<histos_[samples_[isample]]->Integral()<<" +/- "<<jmt::errOnIntegral(histos_[samples_[isample]])<<endl;
    }
    cout<<"total SM =\t "<<totalsm->Integral()<<" +/- "<<jmt::errOnIntegral(totalsm)<<endl;
  }
  
}

void drawPlots(const TString var, const int nbins, const float* varbins, const TString xtitle, const TString ytitle, TString filename="") {
  //provide a more natural modification to the argument list....
  drawPlots( var, nbins, 0, 1, xtitle,ytitle, filename,  varbins);
}

TH1D* getHist(const TString & sample) {
  TH1D* h=0;
  if (sample=="totalsm") h=totalsm;
  else if (sample=="data") h=hdata;
  else h = histos_[sample];

  return h;
}

double getIntegral(const TString & sample) {
  return getHist(sample)->Integral();
}

double getIntegralErr(const TString & sample) {
  return jmt::errOnIntegral(getHist(sample));
}

double getSumOfIntegrals(const std::vector<TString> & samples) {
  double sum=0;
  for (unsigned int j= 0; j<samples.size(); j++) {
    sum += getIntegral(samples.at(j));
  }
  return sum;
}

double getSumOfIntegralsErr(const std::vector<TString> & samples) {
  double sum=0;
  for (unsigned int j= 0; j<samples.size(); j++) {
    sum += pow(getIntegralErr(samples.at(j)),2);
  }
  return sqrt(sum);
}

void drawSignificance(const TString & var, const int nbins, const float low, const float high, const TString & savename) {

  bool oldSaveSetting = savePlots_;
  savePlots_=false;
  drawPlots(var,nbins,low,high,var,"",savename);

  TH1D* hsusy=0;
  for (std::vector<TString>::iterator it = samples_.begin(); it!=samples_.end(); ++it) {
    if ( (*it).Contains("LM") ||(*it).Contains("SUGRA")  ) hsusy = histos_[ *it];
  }
  if (hsusy==0) {
    cout<<"Didn't find a signal sample"<<endl;
    return;
  }

  for (int ibin= 1; ibin<=nbins; ibin++) {
    double B = totalsm->Integral(ibin,nbins);
    double S = hsusy->Integral(ibin,nbins);
    if (B>0)    cout<<totalsm->GetBinLowEdge(ibin)<<"\t"<<S/sqrt(B)<<endl;//"\t"<<totalsm->GetBinContent(ibin)<<endl;
    else     cout<<ibin<<" B is zero"<<endl;
  }



  savePlots_=oldSaveSetting;
}

//could add xtitle and ytitle
void drawR(const TString vary, const float cutVal, const TString var, const int nbins, const float low, const float high, const TString& savename, const float* varbins=0) {
  const TString ytitle="N pass / N fail";
  bool dataOnly = false;

  //const TString var = "MET"; //hardcoded for now
  cout << "x axis: " << var << endl;
  TString cstring1 = vary, cstring2=vary;
  cstring1 += " >= ";
  cstring2 += " < ";
  cstring1 += cutVal;
  cstring2 += cutVal;

  //terrible hack to decide if bias correction should be calculated
  bool calcBiasCorr = false;
  if(nbins==4 && low>-0.01 && low<0.01 && high>199.99 && high<200.01) calcBiasCorr=true;
  float cb_qcd=0, cb_qcd_err=0, cb_sm=0, cb_sm_err=0, cb_data=0, cb_data_err=0;
  float cp_qcd=0, cp_qcd_err=0, cp_sm=0, cp_sm_err=0, cp_data=0, cp_data_err=0;
  float n_qcd_sb = 0, n_qcd_sb_err = 0, n_qcd_sig = 0, n_qcd_sig_err = 0;
  float n_qcd_a = 0, n_qcd_a_err = 0, n_qcd_d = 0, n_qcd_d_err = 0;

  loadSamples();

  gROOT->SetStyle("CMS");

  TString opt=doRatio_? "ratio":"";
  renewCanvas(opt);

  //in first incarnation, make a separate r(MET) plot for each sample in the list

//   TH1D* qcdPass = new TH1D("qcdPass","",nbins,low,high);
//   TH1D* qcdFail = new TH1D("qcdFail","",nbins,low,high);
//   TH1D* qcdRatio = new TH1D("qcdRatio","",nbins,low,high);

//   qcdPass->Sumw2();
//   qcdFail->Sumw2();
//   qcdRatio->Sumw2();

  resetHistos(); //delete existing histograms

  renewLegend();


  // === begin correlation hack ====
  gROOT->cd();
  TH2D totalsm2d_50("totalsm2d_50","",50,50,100,50,0,TMath::Pi());
  TH2D totalsm2d_SB("totalsm2d_SB","",50,100,150,50,0,TMath::Pi());
  totalsm2d_50.Sumw2();
  totalsm2d_SB.Sumw2();
  TH2D data2d_50("data2d_50","",50,50,100,50,0,TMath::Pi());
  TH2D data2d_SB("data2d_SB","",50,100,150,50,0,TMath::Pi());
  data2d_50.Sumw2();
  data2d_SB.Sumw2();
  // === end correlation hack ===

  TH1D  totalsm_pass("totalsm_pass","",nbins,low,high);
  TH1D  totalsm_fail("totalsm_fail","",nbins,low,high);
  totalsm_pass.Sumw2(); 
  totalsm_fail.Sumw2(); 
  if (totalsm!=0) delete totalsm;
  totalsm =  (varbins==0) ? new TH1D("totalsm","",nbins,low,high) : new TH1D("totalsm","",nbins,varbins);
  totalsm->Sumw2();

  totalsm->SetMarkerColor(sampleColor_["TotalSM"]);
  totalsm->SetLineColor(sampleColor_["TotalSM"]);
  totalsm->SetLineWidth(2);
  totalsm->SetMarkerStyle(0);
  totalsm->SetYTitle(ytitle);

  TString drawopt="hist e";
  float max=-1e9; TString firsthist="";
  for (unsigned int isample=0; isample<samples_.size(); isample++) {

    if (!quiet_) cout <<samples_[isample]<<endl;
    TTree* tree = (TTree*) files_[currentConfig_][samples_[isample]]->Get("reducedTree");

    gROOT->cd();

    //need Pass, Fail, and Ratio for each sample
    TString hnameP = var; hnameP += "_"; hnameP += samples_[isample];
    hnameP += "_Pass";
    histos_[hnameP] = (varbins==0) ? new TH1D(hnameP,"",nbins,low,high) : new TH1D(hnameP,"",nbins,varbins);
    histos_[hnameP]->Sumw2();

    TString hnameF = var; hnameF += "_"; hnameF += samples_[isample];
    hnameF += "_Fail";
    histos_[hnameF] = (varbins==0) ? new TH1D(hnameF,"",nbins,low,high) : new TH1D(hnameF,"",nbins,varbins);
    histos_[hnameF]->Sumw2();

    TString hnameR = var; hnameR += "_"; hnameR += samples_[isample];
    hnameR += "_Ratio";
    histos_[hnameR] = (varbins==0) ? new TH1D(hnameR,"",nbins,low,high) : new TH1D(hnameR,"",nbins,varbins);
    histos_[hnameR]->Sumw2();

    //Fill histos
    if (useFlavorHistoryWeights_) assert(0); // this needs to be implemented
    tree->Project(hnameP,var,getCutString(lumiScale_,"",selection_,cstring1,0,"",samples_[isample].Contains("mSUGRA")).Data());
    tree->Project(hnameF,var,getCutString(lumiScale_,"",selection_,cstring2,0,"",samples_[isample].Contains("mSUGRA")).Data());
    
    if (addOverflow_)  addOverflowBin( histos_[hnameP] );
    if (addOverflow_)  addOverflowBin( histos_[hnameF] );

    cout<<"Bug check!"<<endl;
    for (int ib=1; ib<3; ib++) {
      cout<<histos_[hnameP]->GetBinContent(ib)<<"\t";
      cout<<histos_[hnameF]->GetBinContent(ib)<<"\t";
      cout<<histos_[hnameR]->GetBinContent(ib)<<endl;
    }
    //compute ratio
    histos_[hnameR]->Divide(histos_[hnameP], histos_[hnameF]);

    if (isSampleSM(samples_[isample])) {
      totalsm_pass.Add(histos_[hnameP]);
      totalsm_fail.Add(histos_[hnameF]);

      //comment out filling of these for now to save time
      TH2D this2d_SB("this2d_SB","",50,100,150,50,0,TMath::Pi());
      //      tree->Project("this2d_SB","minDeltaPhi:MET",getCutString().Data());
      TH2D this2d_50("this2d_50","",50,50,100,50,0,TMath::Pi());
      //      tree->Project("this2d_50","minDeltaPhi:MET",getCutString().Data());

      totalsm2d_SB.Add(&this2d_SB);
      totalsm2d_50.Add(&this2d_50);
    }

    //   cout<<"content of bin 2: "<<histos_[hnameP]->GetBinContent(2)<<" / "<< histos_[hnameF]->GetBinContent(2)<<" = "<<histos_[hnameR]->GetBinContent(2)<<endl;

    //now format the histograms
    if (!quiet_) cout<<"setting color to: "<<sampleColor_[samples_[isample]]<<endl;
    histos_[hnameR]->SetLineColor(sampleColor_[samples_[isample]]);
    histos_[hnameR]->SetMarkerStyle(sampleMarkerStyle_[samples_[isample]]);
    histos_[hnameR]->SetMarkerColor(sampleColor_[samples_[isample]]);
    histos_[hnameR]->SetYTitle(ytitle);
    histos_[hnameR]->SetXTitle(var);

    //ad hoc additions
    histos_[hnameR]->SetLineWidth(2);

    //draw
    thecanvas->cd(1);
    if (hnameR.Contains("QCD") && !dataOnly) { //HACK draw only qcd
      histos_[hnameR]->Draw(drawopt);
      if (!drawopt.Contains("same")) drawopt+=" same";
      
      //hack to save
      //TH1D* hQCD = (TH1D*)histos_[hnameR]->Clone(savename+"_qcd");
      //hQCD->SaveAs(savename+"_qcd.root");
      
      if (firsthist="") firsthist = hnameR;
      if (histos_[hnameR]->GetMaximum() > max) max = histos_[hnameR]->GetMaximum();
      leg->AddEntry(histos_[hnameR], sampleLabel_[samples_[isample]]);
      
      if(calcBiasCorr){
	cp_qcd = histos_[hnameR]->GetBinContent(3)/ histos_[hnameR]->GetBinContent(2);
	cp_qcd_err = jmt::errAoverB( histos_[hnameR]->GetBinContent(3), histos_[hnameR]->GetBinError(3), histos_[hnameR]->GetBinContent(2), histos_[hnameR]->GetBinError(2)); 
	cb_qcd = histos_[hnameR]->GetBinContent(4)/ histos_[hnameR]->GetBinContent(3);
	cb_qcd_err = jmt::errAoverB( histos_[hnameR]->GetBinContent(4), histos_[hnameR]->GetBinError(4), histos_[hnameR]->GetBinContent(3), histos_[hnameR]->GetBinError(3)); 
	n_qcd_sb = histos_[hnameP]->GetBinContent(3);
	n_qcd_sb_err = histos_[hnameP]->GetBinError(3);
	n_qcd_sig = histos_[hnameP]->GetBinContent(4);
	n_qcd_sig_err = histos_[hnameP]->GetBinError(4);
     	n_qcd_a = histos_[hnameF]->GetBinContent(3);
	n_qcd_a_err = histos_[hnameF]->GetBinError(3);
	n_qcd_d = histos_[hnameF]->GetBinContent(4);
	n_qcd_d_err = histos_[hnameF]->GetBinError(4);
      }
    }
    
  }

  if(!dataOnly){
    histos_[firsthist]->SetMaximum( max*maxScaleFactor_);
    hinteractive =  histos_[firsthist];
  }

  totalsm->Divide(&totalsm_pass,&totalsm_fail);
  if (drawTotalSM_ && !dataOnly) {
    totalsm->Draw("hist e same");
    //    leg->Clear();
    leg->AddEntry(totalsm,sampleLabel_["TotalSM"]);
  }
  if(calcBiasCorr){
    cp_sm = totalsm->GetBinContent(3)/totalsm->GetBinContent(2);
    cp_sm_err = jmt::errAoverB(totalsm->GetBinContent(3),totalsm->GetBinError(3),totalsm->GetBinContent(2),totalsm->GetBinError(2));
    cb_sm = totalsm->GetBinContent(4)/totalsm->GetBinContent(3);
    cb_sm_err = jmt::errAoverB(totalsm->GetBinContent(4),totalsm->GetBinError(4),totalsm->GetBinContent(3),totalsm->GetBinError(3));
  }


  if (dodata_) {
    gROOT->cd();
    if (!quiet_)   cout<<"Drawing data!"<<endl;
    if (hdata != 0) delete hdata;

    TString hname = var; hname += "_"; hname += "data";
    hdata = (varbins==0) ? new TH1D(hname,"",nbins,low,high) : new TH1D(hname,"",nbins,varbins);
    hdata->Sumw2();

    TString hnameP = var; hnameP += "_"; hnameP += "dataPass";
    histos_[hnameP] = (varbins==0) ? new TH1D(hnameP,"",nbins,low,high) : new TH1D(hnameP,"",nbins,varbins);
    histos_[hnameP]->Sumw2();

    TString hnameF = var; hnameF += "_"; hnameF += "dataFail";
    histos_[hnameF] = (varbins==0) ? new TH1D(hnameF,"",nbins,low,high) : new TH1D(hnameF,"",nbins,varbins);
    histos_[hnameF]->Sumw2();

    gROOT->cd();
    dtree->Project(hnameP,var,getCutString(1.,"",selection_,cstring1,0).Data());
    dtree->Project(hnameF,var,getCutString(1.,"",selection_,cstring2,0).Data());
    if (addOverflow_)  addOverflowBin( histos_[hnameP] );
    if (addOverflow_)  addOverflowBin( histos_[hnameF] );
    //compute ratio
    hdata->Divide(histos_[hnameP], histos_[hnameF]);

    dtree->Project("data2d_SB","minDeltaPhi:MET",getCutString(1.,"",selection_,"",0).Data());
    dtree->Project("data2d_50","minDeltaPhi:MET",getCutString(1.,"",selection_,"",0).Data());

    //    hdata->UseCurrentStyle(); //maybe not needed anymore
    hdata->SetMarkerColor(kBlack);
    hdata->SetLineWidth(2);
    hdata->SetMarkerStyle(kFullCircle);
    hdata->SetMarkerSize(1);

    thecanvas->cd(1);
    hdata->SetYTitle(ytitle);
    hdata->SetXTitle(var);
    if(dataOnly) hdata->Draw();
    else hdata->Draw("SAME");
    leg->AddEntry(hdata,"Data");

    if (hdata->GetMaximum() > max && !dataOnly)  {
      histos_[firsthist]->SetMaximum( maxScaleFactor_*hdata->GetMaximum());
      totalsm->SetMaximum(maxScaleFactor_*hdata->GetMaximum());
    }
    else if (doCustomPlotMax_ && dataOnly) {
      hdata->SetMaximum(customPlotMax_);
    }
    else if (doCustomPlotMax_) {
      histos_[firsthist]->SetMaximum( customPlotMax_);
      totalsm->SetMaximum(customPlotMax_);
    }
    if (doCustomPlotMin_ && dataOnly) {
      hdata->SetMinimum(customPlotMin_);
    }
    else if (doCustomPlotMin_) {
      histos_[firsthist]->SetMinimum( customPlotMin_);
      totalsm->SetMinimum(customPlotMin_);
    }
    
    //    cratio->cd();
    thecanvas->cd(2);
    if (ratio!=0) delete ratio;
    if(!dataOnly){
      ratio = (varbins==0) ? new TH1D("ratio","data/(SM MC)",nbins,low,high) : new TH1D("ratio","data/(SM MC)",nbins,varbins);
      ratio->Sumw2();
      ratio->Divide(hdata,totalsm); 
      ratio->SetMinimum(ratioMin);
      ratio->SetMaximum(ratioMax);
      ratio->Draw();
    }
    cout<<"KS Test results (shape only): "<<hdata->KolmogorovTest(totalsm)<<endl;;

    if(calcBiasCorr){
      cp_data = hdata->GetBinContent(3)/hdata->GetBinContent(2);
      cp_data_err = jmt::errAoverB(hdata->GetBinContent(3),hdata->GetBinError(3),hdata->GetBinContent(2),hdata->GetBinError(2)); 
      cb_data = hdata->GetBinContent(4)/hdata->GetBinContent(3);
      cb_data_err = jmt::errAoverB(hdata->GetBinContent(4),hdata->GetBinError(4),hdata->GetBinContent(3),hdata->GetBinError(3)); 
    }
  }
  else {
    if (doCustomPlotMax_) {
      histos_[firsthist]->SetMaximum( customPlotMax_);
      totalsm->SetMaximum(customPlotMax_);
    }
    if (doCustomPlotMin_) {
      histos_[firsthist]->SetMinimum( customPlotMin_);
      totalsm->SetMinimum(customPlotMin_);
    }
  }
  thecanvas->cd(1);
   if (doleg_)  leg->Draw();

  thecanvas->SaveAs("mindpPassOverFail-"+savename+".eps");
  thecanvas->SaveAs("mindpPassOverFail-"+savename+".pdf");
  thecanvas->SaveAs("mindpPassOverFail-"+savename+".png");

//   TCanvas* c2d=new TCanvas("c2d","2d",800,800);
//   c2d->Divide(2,2);
//   c2d->cd(1);
//   totalsm2d_50.DrawCopy("colz");
//   c2d->cd(2);
//   totalsm2d_SB.DrawCopy("colz");
//   c2d->cd(3);
//   data2d_50.DrawCopy("colz");
//   c2d->cd(4);
//   data2d_SB.DrawCopy("colz");
  cout<<"Total SM MC correlation [50<MET<100]  = "<<totalsm2d_50.GetCorrelationFactor()<<endl;
  cout<<"Total SM MC correlation [100<MET<150] = "<<totalsm2d_SB.GetCorrelationFactor()<<endl;
  cout<<"Data correlation [50<MET<100]         = "<<data2d_50.GetCorrelationFactor()<<endl;
  cout<<"Data correlation [100<MET<150]        = "<<data2d_SB.GetCorrelationFactor()<<endl;
  if(calcBiasCorr){
    cout<<endl;
    cout<<"Pseudo bias correction (using 50<MET<100 and 100<MET<150):" << endl;
    cout<<"QCD MC: "<<cp_qcd<<" +/- "<<cp_qcd_err<<endl;
    cout<<"SM MC: "<<cp_sm<<" +/- "<<cp_sm_err<<endl;
    cout<<"Data: "<<cp_data<<" +/- "<<cp_data_err<<endl;
    cout<<endl;
    cout<<"True bias correction (using 100<MET<150 and 150<MET):" << endl;
    cout<<"QCD MC: "<<cb_qcd<<" \\pm "<<cb_qcd_err<<endl;
    cout<<"SM MC: "<<cb_sm<<" \\pm "<<cb_sm_err<<endl;
    cout<<"Data: "<<cb_data<<" \\pm "<<cb_data_err<<endl;
    cout << endl;
    cout << "QCD Event Counts" << endl;
    cout << "SB: " << n_qcd_sb << " +- " << n_qcd_sb_err << endl;
    cout << "SIG: " << n_qcd_sig << " +- " << n_qcd_sig_err << endl;
    cout << "A: " << n_qcd_a << " +- " << n_qcd_a_err << endl;
    cout << "D: " << n_qcd_d << " +- " << n_qcd_d_err << endl;
    cout << endl;
  }
  cout<<"End of drawR()"<<endl;
}

void drawR(const TString vary, const float cutVal, const int nbins, const float low, const float high, const TString& savename) {//for backwards compatibility
  drawR(vary, cutVal, "MET", nbins, low, high, savename, 0); 
}

void drawR(const TString vary, const float cutVal, const TString var, const int nbins, const float* varbins=0, const TString& savename="") {//more natural
  drawR(vary, cutVal, var, nbins, 0, 1, savename, varbins);
}

//utility function for making output more readable
TString format_nevents(double n,double e) {

  //  const bool moreDigits = false;
  const bool moreDigits = true;
  const int eCutoff = moreDigits ? 10 : 1;
  const int extraDigits = moreDigits ? 1:0;

  TString mathmode = latexMode_ ? "$" : "";
  
  char out[100];
  if (e >= eCutoff || e < 0.00001) { //show whole numbers only
    sprintf(out,"%s%.0f%s%.0f%s",mathmode.Data(),n,pm.Data(),e,mathmode.Data());
  }
  else {
    int nfig = ceil(fabs(log10(e))) + extraDigits;
    TString form="%s%.";
    form+=nfig; form+="f%s%.";
    form+=nfig; form+="f%s";
    sprintf(out,form.Data(),mathmode.Data(),n,pm.Data(),e,mathmode.Data());
  }
  return TString(out);
}

typedef map<pair<int,int>, pair<double,double> > susyScanYields;
susyScanYields getSusyScanYields(const TString & sampleOfInterest) {
  //sample is an argument
  //other important things are defined by the usual global variables
  if (!quiet_) cout<<sampleOfInterest<<" "<<currentConfig_<<endl;

  TString varx="m0"; TString xtitle=varx;
  int  nbinsx=210; float lowx=-0.5; float highx=2100-0.5;

  TString vary="m12"; TString ytitle=vary;
  int  nbinsy=110; float lowy=-0.5; float highy=1100-0.5;
  TString drawstring = vary+":"+varx;

  TTree* thetree = (TTree*) files_[currentConfig_][sampleOfInterest]->Get("reducedTree");


  vector<TH2D*> raw0;
  for (int i=0; i<=10; i++) {
    TString hname="raw0_";
    hname += i;
    raw0.push_back(new TH2D(hname,"raw event counts",nbinsx,lowx,highx,nbinsy,lowy,highy));
    raw0[i]->Sumw2();
    TString thecut = getCutString(false,"","",0,"",false,i);
    thetree->Project(hname,drawstring,thecut.Data());
  }

  //at this point, each bin contains Npass_i * sigma_i for that (m0,m12)
  //need to divide by N_i for each (m0,m12)

  //loop over i and histo bins
  for (int i=0; i<=10; i++) {
    for (map<pair<int,int>, TH1D* >::iterator iscanpoint = scanProcessTotalsMap.begin(); iscanpoint!=scanProcessTotalsMap.end(); ++iscanpoint) {
      int m0=iscanpoint->first.first;
      int m12=iscanpoint->first.second;
      TH1D* thishist = scanProcessTotalsMap[make_pair(m0,m12)];
      int thisn = TMath::Nint(thishist->GetBinContent(i));
      int bin=  raw0[i]->FindBin(m0,m12);
      double N_i_thispoint = raw0[i]->GetBinContent(bin);
      double err_i_thispoint = raw0[i]->GetBinError(bin);
      if (thisn == 0) {
	if (N_i_thispoint > 0.0000001) cout<<"Possible problem: "<<m0<<" "<<m12<<" "<<i<<" "<< N_i_thispoint<<" "<<thisn<<endl;
	thisn=1; //prevent divide by zero
	N_i_thispoint = 0; //need to come back to what is going wrong h
      }
      N_i_thispoint /= thisn;
      err_i_thispoint /= thisn;
      raw0[i]->SetBinContent(bin,N_i_thispoint);
      raw0[i]->SetBinError(bin,err_i_thispoint);
    }
  }

  //now we have Npass_i * sigma_i * lumi / Ngen_i 
  //all that is left is to make the sum over i

  susyScanYields theYields;
  for (map<pair<int,int>, TH1D* >::iterator iscanpoint = scanProcessTotalsMap.begin(); iscanpoint!=scanProcessTotalsMap.end(); ++iscanpoint) {
    double Nraw = 0, errraw=0;

    int bin=  raw0[0]->FindBin(iscanpoint->first.first , iscanpoint->first.second);
    for (unsigned int i=0; i<raw0.size(); i++) {
      Nraw += raw0[i]->GetBinContent(bin);
      errraw += pow(raw0[i]->GetBinError(bin),2);
    }
    //cout<<iscanpoint->first.first<<" "<<iscanpoint->first.second<<" "<<Nraw<< " +/- "<<sqrt(errraw)<<endl;
    theYields[iscanpoint->first] = make_pair(Nraw,sqrt(errraw));
  }

  //try to clean up
  for (unsigned int i=0; i<raw0.size(); i++) {
    delete raw0[i];
  }

  return theYields;
}


void getCutStringForCutflow(vector<TString> &vectorOfCuts, vector<TString> &stageCut, bool isTightSelection){

  //careful -- doesn't support mSUGRA right now

  vectorOfCuts.clear(); stageCut.clear();

  //define the HT and MET thresholds
  TString minHT; TString minMET;
  if (!isTightSelection) {minHT = "350"; minMET = "200";}
  else {minHT = "500"; minMET = "300";}

  TString cut;
  TString thisSelection;
  TString selectionPreB;
  TString selectionEq1bLoose;
  TString selectionGe1bLoose;
  TString selectionGe2bLoose;
  //  TString selectionGe3b;
  TString selectionEq1bTight;
  TString selectionGe1bTight;
  TString selectionGe2bTight;  

  /*
  //inclusive
  thisSelection="";
  cut=getCutString(lumiScale_,thisSelection);
  vectorOfCuts.push_back(cut);
  stageCut.push_back("Inclusive");

  //trigger
  if (thisSelection=="") thisSelection += "cutTrigger==1";
  else thisSelection += " && cutTrigger";
  cut=getCutString(lumiScale_,thisSelection);
  vectorOfCuts.push_back(cut);
  stageCut.push_back("Trigger");

  //PV
  if (thisSelection=="") thisSelection += "cutPV==1";
  else thisSelection += " && cutPV==1";
  cut=getCutString(lumiScale_,thisSelection);
  vectorOfCuts.push_back(cut);
  stageCut.push_back("PV");
  */

  //HT
  if (thisSelection=="") {thisSelection += "HT>="; thisSelection += minHT;}
  else { thisSelection += " && HT>="; thisSelection += minHT;}
  cut=getCutString(lumiScale_,thisSelection);
  vectorOfCuts.push_back(cut);
  if (latexMode_) stageCut.push_back("HT$\\ge$"+minHT);
  else stageCut.push_back("HT>="+minHT);

  //3 or more jets
  if (thisSelection=="") thisSelection += "cut3Jets==1";
  else thisSelection += " && cut3Jets==1";
  cut=getCutString(lumiScale_,thisSelection);
  vectorOfCuts.push_back(cut);
  if (latexMode_) stageCut.push_back("$\\ge$ 3 jets");
  else stageCut.push_back(">= 3 jets");

  //Ele veto
  if (thisSelection=="") thisSelection += "cutEleVeto==1";
  else thisSelection += " && cutEleVeto==1";
  cut=getCutString(lumiScale_,thisSelection);
  vectorOfCuts.push_back(cut);
  stageCut.push_back("e veto");

  //Mu veto
  if (thisSelection=="") thisSelection += "cutMuVeto==1";
  else thisSelection += " && cutMuVeto==1";
  cut=getCutString(lumiScale_,thisSelection);
  vectorOfCuts.push_back(cut);
  if (latexMode_) stageCut.push_back("$\\mu$ veto");
  else stageCut.push_back("Mu veto");

  //MET
  if (thisSelection=="") {thisSelection += "MET>="; thisSelection += minMET; }
  else {thisSelection += " && MET>="; thisSelection += minMET;}
  cut=getCutString(lumiScale_,thisSelection);
  vectorOfCuts.push_back(cut);
  if (latexMode_) stageCut.push_back("\\MET$\\ge$"+minMET);
  else stageCut.push_back("MET>="+minMET);

  //angular cuts

  //  //deltaPhi
  //  if (thisSelection=="") thisSelection += "cutDeltaPhi==1";
  //  else thisSelection += " && cutDeltaPhi==1";
  //  cut=getCutString(lumiScale_,thisSelection);
  //  vectorOfCuts.push_back(cut);
  //  stageCut.push_back("DeltaPhi");

  //deltaPhiN
  //  if (thisSelection=="") thisSelection += "cutDeltaPhiN==1";
  //  else thisSelection += " && cutDeltaPhiN==1";
  if (thisSelection=="") thisSelection += "minDeltaPhiN>4";
  else thisSelection += " && minDeltaPhiN>=4";
  cut=getCutString(lumiScale_,thisSelection);
  vectorOfCuts.push_back(cut);
  if (latexMode_) stageCut.push_back("$\\minDeltaPhiN>$4");
  else stageCut.push_back("minDeltaPhiN>4");

  //  //deltaPhi(MET,taus)
  //  if (thisSelection=="") thisSelection += "cutDeltaPhiTaus==1";
  //  else thisSelection += " && cutDeltaPhiTaus==1";
  //  cut=getCutString(lumiScale_,thisSelection);
  //  vectorOfCuts.push_back(cut);
  //  stageCut.push_back("DeltaPhiTaus");
  
  //Cleaning
//   if (thisSelection=="") thisSelection += "cutCleaning==1";
//   else thisSelection +=" && cutCleaning==1";
//   cut=getCutString(lumiScale_,thisSelection);
//   vectorOfCuts.push_back(cut);
//   stageCut.push_back("Cleaning");

  //store selection string pre b cut
  selectionPreB=thisSelection;

  //loose selection
  //>= 1 b
  selectionGe1bLoose=selectionPreB; selectionGe1bLoose += " && nbjetsSSVHPT>=1";
  cut=getCutString(lumiScale_,selectionGe1bLoose);
  vectorOfCuts.push_back(cut);
  if (latexMode_) stageCut.push_back("HT$\\ge$350, \\MET$\\ge$200, $\\ge$1 b");
  else stageCut.push_back("HT>=350, MET>=200, >= 1 b");

  //==1b
  selectionEq1bLoose=selectionPreB; selectionEq1bLoose +=" && nbjetsSSVHPT==1";
  cut=getCutString(lumiScale_,selectionEq1bLoose);
  vectorOfCuts.push_back(cut);
  if (latexMode_) stageCut.push_back("HT$\\ge$350, \\MET$\\ge$200, $==$1 b");
  else stageCut.push_back("HT>=350, MET>=200, == 1 b");

  //>= 2 b
  selectionGe2bLoose=selectionPreB; selectionGe2bLoose +=" && nbjetsSSVHPT>=2";
  cut=getCutString(lumiScale_,selectionGe2bLoose);
  vectorOfCuts.push_back(cut);
  if (latexMode_) stageCut.push_back("HT$\\ge$350, \\MET$\\ge$200, $\\ge$2 b");
  else stageCut.push_back("HT>=350, MET>=200, >= 2 b");

  //  //>= 3 b
  //  selectionGe3bLoose=selectionPreB; selectionGe3bLoose +=" && nbjetsSSVHPT>=3";
  //  cut=getCutString(lumiScale_,selectionGe3bLoose);
  //  vectorOfCuts.push_back(cut);
  //  if (latexMode_) stageCut.push_back("HT$\\ge$350, \\MET$\\ge$200, $\\ge$3 b");
  //  else stageCut.push_back("HT>=350, MET>=200, >= 3 b");

  //tight selection
  if (!isTightSelection){ //print out the results for the tight selection anyway
    //>=1 b
    selectionGe1bTight=selectionPreB; selectionGe1bTight += " && nbjetsSSVHPT>=1 && HT>=500 && MET>=300"; //hard-coded!
    cut=getCutString(lumiScale_,selectionGe1bTight);
    vectorOfCuts.push_back(cut);
    if (latexMode_) stageCut.push_back("HT$\\ge$500, \\MET$\\ge$300, $\\ge$1 b");
    else stageCut.push_back("HT>=500, MET>=300, >= 1 b");
    
    //==1 b
    selectionGe1bTight=selectionPreB; selectionGe1bTight += " && nbjetsSSVHPT==1 && HT>=500 && MET>=300"; //hard-coded!
    cut=getCutString(lumiScale_,selectionGe1bTight);
    vectorOfCuts.push_back(cut);
    if (latexMode_) stageCut.push_back("HT$\\ge$500, \\MET$\\ge$300, $==$1 b");
    else stageCut.push_back("HT>=500, MET>=300, == 1 b");
    
    //>=2 b
    selectionGe1bTight=selectionPreB; selectionGe1bTight += " && nbjetsSSVHPT>=2 && HT>=500 && MET>=300"; //hard-coded!
    cut=getCutString(lumiScale_,selectionGe1bTight);
    vectorOfCuts.push_back(cut);
    if (latexMode_) stageCut.push_back("HT$\\ge$500, \\MET$\\ge$300, $\\ge$2 b");
    else stageCut.push_back("HT>=500, MET>=300, >= 2 b");  
  }
  
  //  //check output
  //  for (unsigned int istage=0; istage<vectorOfCuts.size(); istage++){
  //    cout<<stageCut[istage]<<endl;
  //    cout<<vectorOfCuts[istage]<<endl;
  //  }
  
}

void cutflow(bool isTightSelection){

  loadSamples();
  resetHistos();

  vector<TString> vectorOfCuts; //each element is a successive cut string for the cutflow table
  vector<TString> stageCut; //name of cut at each stage
  getCutStringForCutflow(vectorOfCuts, stageCut, isTightSelection); //fills vectorOfCuts

  vector< vector<float> > cutflowEntries; //stores events by stage and sample
  vector< vector<float> > cutflowEntriesE; //stores error

  TString var = "HT";
  const float* varbins=0;
  const int nbins=1; const float low=0; const float high=100000000000;

  vector<float> nSumSM;
  vector<float> nSumSME;

  //loop over cuts
  for (unsigned int istage=0; istage<vectorOfCuts.size(); istage++){
    TString thisStageCut = vectorOfCuts[istage];
    resetHistos();

    vector<float> nPass; //number of events passing current cut in each sample
    vector<float> nPassE; //error on events passing cuts

    //for total background
    if (totalsm!=0) delete totalsm;
    totalsm = (varbins==0) ? new TH1D("totalsm","",nbins,low,high) : new TH1D("totalsm","",nbins,varbins);
    totalsm->Sumw2();

    //loop over samples, copied from drawPlots()
    for (unsigned int isample=0; isample<samples_.size(); isample++) {
      if (!quiet_)   cout <<samples_[isample]<<endl;

      gROOT->cd();
      //should each histo have a different name? maybe
      TString hname = jmt::fortranize(var); hname += "_"; hname += samples_[isample];
      histos_[samples_[isample]] = (varbins==0) ? new TH1D(hname,"",nbins,low,high) : new TH1D(hname,"",nbins,varbins);
      histos_[samples_[isample]]->Sumw2();

      //qcd reweighting not implemented yet

      TTree* tree = (TTree*) files_[currentConfig_][samples_[isample]]->Get("reducedTree");
      gROOT->cd();
      TString weightopt= useFlavorHistoryWeights_ && samples_[isample].Contains("WJets") ? "flavorHistoryWeight" : "";
      tree->Project(hname,var,thisStageCut);
      //now the histo is filled

      if (isSampleSM(samples_[isample])) {
	totalsm->Add(histos_[samples_[isample]]);
	if (!quiet_)    cout << "totalsm: " << samples_[isample] << endl;
      }

      nPass.push_back(histos_[samples_[isample]]->GetBinContent(1)); //to get total entries with weighting applied
      nPassE.push_back(histos_[samples_[isample]]->GetBinError(1)); //error
            
    }//end loop over samples

    cutflowEntries.push_back(nPass);
    cutflowEntriesE.push_back(nPassE);
    nSumSM.push_back(totalsm->GetBinContent(1));
    nSumSME.push_back(totalsm->GetBinError(1));

  }//end current cut 

  //now print out the cutflow table
  if (isTightSelection) cout<<"tight selection (HT>=500, MET>=300)"<<endl;
  else cout<<"baseline selection (HT>=350, MET>=200)"<<endl; 

  TString col_start; TString col; TString col_end; TString hline; TString hhline;

  if (latexMode_){
    col_start=""; col=" & "; col_end=" \\\\ "; hline="\\hline"; hhline="\\hline \\hline";
  }
  else{
    col_start=" | "; col=" | "; col_end=" | "; hline=""; hhline="";
  }

  cout<<hhline<<endl;
  //list sample names
  cout<<col_start<<"Cut "<<col;
  for (unsigned int isample=0; isample<samples_.size(); isample++) {
    if ( isSampleSM(samples_[isample])) { //skip LM points to add total background column first 
      if ( samples_[isample].Contains("VV") ) cout<<"Diboson"<<col; 
      else if ( samples_[isample].Contains("QCD") ) cout<<"QCD"<<col;
      else if ( samples_[isample]=="SingleTop" ) cout<<"Single Top"<<col;
      else if ( samples_[isample].Contains("TTbarJets") ){
	if (latexMode_) cout<<"\\ttbar"<<col;
	else cout<<samples_[isample]<<col;
      }
      else if ( samples_[isample].Contains("WJets") ){
	if (latexMode_)cout<<"\\WJets"<<col;
	else cout<<samples_[isample]<<col;
      }
      else if ( samples_[isample].Contains("ZJets") ){
	if (latexMode_)cout<<"\\ZJets"<<col;
        else cout<<samples_[isample]<<col;
      }
      else if ( samples_[isample].Contains("Zinvisible") ){
        if (latexMode_)cout<<"\\Zinvisible"<<col;
        else cout<<samples_[isample]<<col;
      }
      else cout<<samples_[isample]<<col;
    }
  }
  //now add total background, LM
  cout<<"Total SM";
  for (unsigned int isample=0; isample<samples_.size(); isample++) {
    if ( !isSampleSM(samples_[isample])  ) cout<<col<<samples_[isample];
  }
  cout<<col_end<<endl;
  
  //fill table
  for (unsigned int istage=0; istage<vectorOfCuts.size(); istage++){
    cout<<col_start<<stageCut[istage]<<col;
    for (unsigned int isample=0; isample<samples_.size(); isample++){
      if ( isSampleSM(samples_[isample]) ) {
	cout<<format_nevents(cutflowEntries[istage][isample],cutflowEntriesE[istage][isample])<<col;
      }
    }//end loop over samples
    //now add total background, LM
    cout<<format_nevents(nSumSM[istage],nSumSME[istage]);
    for (unsigned int isample=0; isample<samples_.size(); isample++) {
      if ( !isSampleSM(samples_[isample]) ) cout<<col<<format_nevents(cutflowEntries[istage][isample],cutflowEntriesE[istage][isample]);
    }
    cout<<col_end<<endl;
    if (stageCut[istage]=="Cleaning") cout<<hline<<endl;

    if (latexMode_){
      if (stageCut[istage]=="HT$\\ge$350, \\MET$\\ge$200, $\\ge$2 b" && !isTightSelection) cout<<hline<<endl;
    }
    else {
      if (stageCut[istage]=="HT>=350, MET>=200, >= 2 b" && !isTightSelection) cout<<hline<<endl;
    }

  }//end loop over cuts
  cout<<hhline<<endl;
  
}
