#include <vector>
#include "TTree.h"
#include "TMath.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include <fstream>
#include <cmath>
#include <string>
#include "TString.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TChain.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
//#include "Analyzer.h"
#include "EventCalculator_cfA.h"

/*
general syntax is

./run_Analysis <file> [optionstring]

For making b-tag eff histograms, do:
./run_Analysis <file> btageff

*/

using namespace std;

// === code borrowed from Harrison and Sezen ==
//(i see no reason to reinvent the wheel)

struct commandLine
{
  std::string progname;
  std::string filelist;
  std::string outputfilename;
};

std::string strip(std::string line)
{
  int l = line.size();
  if ( l == 0 ) return std::string("");
  int n = 0;
  while (((line[n] == 0)    ||
	  (line[n] == ' ' ) ||
	  (line[n] == '\n') ||
	  (line[n] == '\t')) && n < l) n++;

  int m = l-1;
  while (((line[m] == 0)    ||
	  (line[m] == ' ')  ||
	  (line[m] == '\n') ||
	  (line[m] == '\t')) && m > 0) m--;
  return line.substr(n,m-n+1);
}

void decodeCommandLine(int argc, char** argv, commandLine& cl)
{
  cl.progname = std::string(argv[0]);

  // 1st (optional) argument
  if ( argc > 1 )
	cl.filelist = std::string(argv[1]);
  else
	cl.filelist = std::string("filelist.txt");


}

// Read ntuple filenames from file list
std::vector<std::string> getFilenames(std::string filelist)
{
  std::ifstream stream(filelist.c_str());
  if ( !stream.good() ) cout<<"ERROR: unable to open file: " <<filelist<<endl;;

  // Get list of ntuple files to be processed

  std::vector<std::string> v;
  std::string filename;
  while ( stream >> filename )
	if ( strip(filename) != "" ) v.push_back(filename);
  return v;
}

// == main program ==

int main(int argc, char* argv[])
{

  cout << "Very begining of run_Analysis" << endl;

  TString outputDir = "./"; //this is where reducedTrees will go, use "./" for T3
  //parse command line arguments: <file list> <options list>
  TString options="";
  if (argc>2) options = argv[2];

  //code imported from the old BasicLoopCU.cc
  stringstream ss;
  TString fileArg;
  *argv++;
  ss<<*argv;
  ss>>fileArg;
  if (fileArg.Contains(".")) fileArg.Remove(fileArg.Last('.'));
  cout << fileArg << endl;
  *argv--;
  
  // Get file list and histogram filename from command line
  commandLine cmdline;
  decodeCommandLine(argc, argv, cmdline);

  // Get names of ntuple files to be processed and open chain of ntuples
  vector<string> filenames = getFilenames(cmdline.filelist);
    cout<<" n files = "<<filenames.size()<<endl;
  //for convenience I will continue to pass 'fileArg' but then I will also pass this filename list

  //Instantiate and run class
  if (options=="slim") {
    //special mode

    for (unsigned int ifile=0; ifile<filenames.size(); ifile++) {
      cout<<ifile<<endl;
      cout<<"Processing "<<filenames[ifile]<<endl;
      std::vector<std::string> afilename;
      afilename.push_back(filenames[ifile]);
      EventCalculator ec(fileArg,afilename, EventCalculator::kPF2PAT, EventCalculator::kPFMETTypeI, EventCalculator::kIso0);
      bool statusOk =   ec.slim();
      if (statusOk ) {
	cout<<"done ok  "<<filenames[ifile]<<endl;
      }
      else {
	cout<<"NOT ok   "<<filenames[ifile]<<endl;
      }
    }
  }
  else { //normal mode
    EventCalculator ec(fileArg,filenames, EventCalculator::kPF2PAT, EventCalculator::kPFMETTypeI, EventCalculator::kIso0);
    ec.setBTaggerType(EventCalculator::kCSVM);
    if (options=="btageff") {
      ec.plotBTagEffMC();
    }
    else if (options=="scanSMSngen") {
      ec.fillSMShist();
    }
    else if (options=="debug1") {
      ec.debugSMS();
    }
    else if (options=="slimCheck") ec.slimCheck();
    else {
      ec.setOptions(options);
      ec.reducedTree(outputDir);
    }
  }


  cout<<"Back in main(). now really terminate"<<endl;  
  return 0;
}
