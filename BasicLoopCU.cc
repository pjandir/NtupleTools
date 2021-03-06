//-----------------------------------------------------------------------------
// File:        BasicLoopCU.cc
// Description: Analyzer for ntuples created by TheNtupleMaker
// Created:     Fri Jun  3 16:38:31 2011 by mkntanalyzer.py
// Author:      Ben Kreis
//-----------------------------------------------------------------------------

//#include "BasicLoopTools.h"
#include "EventCalculator.C"

#ifdef PROJECT_NAME
#include "PhysicsTools/TheNtupleMaker/interface/pdg.h"
#else
#include "pdg.h"
#endif

using namespace std;
//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
 
  //code for sampleName_ and output path

  TString outputDir = "./"; //this is where reducedTrees will go, use "./" for T3
  //TString outputDir = "/uscmst1b_scratch/lpc1/3DayLifetime/wdteo/"; //this is where reducedTrees will go
  //TString outputDir = "/tmp/joshmt/";

  TString options="";
  if (argc>2) options = argv[2];

  stringstream ss;
  TString fileArg;
  *argv++;
  ss<<*argv;
  ss>>fileArg;
  if (fileArg.Contains(".")) fileArg.Remove(fileArg.Last('.'));
  //  setSampleName_(fileArg);
  cout << fileArg << endl;
  *argv--;
  
  // Get file list and histogram filename from command line
  commandLine cmdline;
  decodeCommandLine(argc, argv, cmdline);

  // Get names of ntuple files to be processed and open chain of ntuples
  vector<string> filenames = getFilenames(cmdline.filelist);
  itreestream stream(filenames, "Events");
  if ( !stream.good() ) error("unable to open ntuple file(s)");

  // Get number of events to be read
  int nevents = stream.size();
  cout << "Number of events: " << nevents << endl;

  // Select variables to be read
  selectVariables(stream);

  EventCalculator ec(fileArg, EventCalculator::kPF2PAT, EventCalculator::kPFMET);
  //ec.setBTaggerType(EventCalculator::kSSVHPT);
  ec.setBTaggerType(EventCalculator::kCSVM);
  ec.setOptions(options);
  ec.reducedTree(outputDir, stream);

  //ec.cutflow(stream, -1);
  //ec.sampleAnalyzer(stream);
  //ec.plotBTagEffMC(stream);

  return 0;

}



  //sampleAnalyzer(stream);

  //makeReducedTrees(argc, argv); // currently not working with multiple files (problem with treestream?)

  // The root application is needed to make canvases visible during
  // program execution. If this is not needed, just comment out the following
  // line

  //TApplication app("analyzer", &argc, argv);

  /*
	 Notes:
	
	 1. Use
	   ofile = outputFile(cmdline.outputfile, stream)
	
	 to skim events to output file in addition to writing out histograms.
	
	 2. Use
	   ofile.addEvent(event-weight)
	
	 to specify that the current event is to be added to the output file. If
	 omitted, the event-weight is taken to 1.
	
	 3. Use
	    ofile.count(cut-name, event-weight)
	
	 to keep track, in the count histogram, of the number of events passing
	 a given cut. If omitted, the event-weight is taken to be 1. If you want
	 the counts in the count histogram to appear in a given order, specify the
	 order, before entering the event loop, as in the example below
	 
	    ofile.count("NoCuts", 0)
		ofile.count("GoodEvent", 0)
		ofile.count("Vertex", 0)
		ofile.count("MET", 0)
  */
  
  //outputFile ofile(cmdline.outputfilename);

  //---------------------------------------------------------------------------
  // Declare histograms
  //---------------------------------------------------------------------------



  //---------------------------------------------------------------------------
  // Loop over events
  //---------------------------------------------------------------------------

  /*
  for(int entry=0; entry < nevents; ++entry)
	{
	  // Read event into memory
	  stream.read(entry);

	  // Uncomment the following line if you wish to copy variables into
	  // structs. See the header file BasicLoopCU.h to find out what structs
	  // are available.
	  // fillObjects();
	  
	  // ---------------------
	  // -- event selection --
	  // ---------------------


	  // ---------------------
	  // -- fill histograms --
	  // ---------------------	  

	}
  */
  //stream.close();
  //ofile.close();
