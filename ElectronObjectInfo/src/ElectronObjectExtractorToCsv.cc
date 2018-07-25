// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//classes included to extract electron information
#include "DataFormats/ElectronReco/interface/Electron.h"
#include "DataFormats/ElectronReco/interface/ElectronFwd.h" 

//classes included to extract tracking for the electrons
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

//additional classes for storage, containers and operations
#include<vector>
#include<string>
#include "TFile.h"
#include "TTree.h"
#include <stdlib.h>
#include<iostream>
#include<fstream>
#include<sstream>



//
// class declaration
//

class ElectronObjectInfoExtractorToCsv : public edm::EDAnalyzer {
   public:
      explicit ElectronObjectInfoExtractorToCsv(const edm::ParameterSet&);
      ~ElectronObjectInfoExtractorToCsv();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      virtual void endRun(edm::Run const&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
 
 //declare a function to do the electron analysis
      void analyzeElectrons(const edm::Event& iEvent, const edm::Handle<reco::ElectronCollection> &electrons);
  //function to store info in csv
  void dumpElectronsToCsv();
  //declare the input tag for the electrons collection to be used (read from cofiguration)
  edm::InputTag electronsInput;
  int maxNumObjt;

  //Declare some variables for storage
  std::ofstream myfile;
  int maxpart;
  std::ostringstream oss;
  std::string theHeader;


  //and declare variable that will go into the root tree
  int runno; //run number
  int evtno; //event number
  int nmu;//number of electrons in the event
  std::string electron_partype; //type of particle
  std::vector<float> electron_e;
  std::vector<float> electron_pt;
  std::vector<float> electron_px;
  std::vector<float> electron_py;
  std::vector<float> electron_pz;
  std::vector<float> electron_eta;
  std::vector<float> electron_phi;
  std::vector<float> electron_ch;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
ElectronObjectInfoExtractorToCsv::ElectronObjectInfoExtractorToCsv(const edm::ParameterSet& iConfig)

{
  //This should match the configuration in the corresponding python file
  electronsInput = iConfig.getParameter<edm::InputTag>("InputCollection");
  maxNumObjt = iConfig.getUntrackedParameter<int>("maxNumberElectrons",5);

}


ElectronObjectInfoExtractorToCsv::~ElectronObjectInfoExtractorToCsv()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
ElectronObjectInfoExtractorToCsv::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   //get the global information first
   runno = iEvent.id().run();
   evtno  = iEvent.id().event();

   //Declare a container (or handle) where to store your electrons.
   //https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideDataFormatRecoElectron
   Handle<reco::ElectronCollection> myelectrons;

   //This is where the information gets extracted from the EDM file
   //Essentially, this corresponds to the information stored in a specific
   //branch within the EDM files.  For example, for recoElectrons one could get
   //just "electrons", which would be the most used reco electrons collection,
   //but also "electronsFromCosmics", which could be used to study fakes.
   //If you explore the branches in the EDM file with a ROOT TBrowser, 
   //you can indeed find a
   //"recoElectrons_electrons__RECO" branch and a "recoElectrons_electronsFromCosmics__RECO" one.
   //Therefore, following the documentation
   //(https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideEDMGetDataFromEvent?rev=20),
   //one could simply write "electrons" instead of 
   //the electronsInput variable, which is extracted from
   //the configuration above.  However, using such a configuration variable
   //allows one to access a different branch or type of electrons, some cosmic electrons
   //for example, without having to recompile the code.
   iEvent.getByLabel(electronsInput, myelectrons); 

   //Now, to keep it orderly, pass the collection to a subroutine that extracts
   //some of  the electron information
   //We do need to pass the event.  We could have also passed
   //the event setup if it were needed.
   analyzeElectrons(iEvent,myelectrons);
   dumpElectronsToCsv();
   return;

}

// ------------ function to analyze electrons
void 
ElectronObjectInfoExtractorToCsv::analyzeElectron(const edm::Event& iEvent, const edm::Handle<reco::ElectronCollection> &electrons)
{
  //clear the storage containers for this objects in this event
  nelectron=0;
  electron_e.clear();
  electron_px.clear();
  electron_py.clear();
  electron_pz.clear();
  electron_pt.clear();
  electron_eta.clear();
  electron_phi.clear();
  electron_ch.clear();

  //check if the collection is valid
  if(electrons.isValid()){
    //get the number of electrons in the event
	//loop over all the electrons in this event
    int idx = 0;
	for (reco::ElectronCollection::const_iterator recoElectron = electrons->begin(); recoElectron!=electrons->end(); ++recoElectron){
      //find only globlal electrons for this specific example
      //https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookElectronAnalysis?rev=88
	  if(recoElectron->isGlobalElectron()) {
	    electron_partype = "G"; 
	    electron_e.push_back(recoElectron->energy());
	    electron_pt.push_back(recoElectron->pt());
	    electron_px.push_back(recoElectron->px());
	    electron_py.push_back(recoElectron->py());
	    electron_pz.push_back(recoElectron->pz());
	    electron_eta.push_back(recoElectron->eta());
	    electron_phi.push_back(recoElectron->phi());
	    electron_ch.push_back(recoElectron->charge());
	    ++idx;
	  }
	}
	nmu = idx;
  }
  
}

// ------------ function to analyze electron
void ElectronObjectInfoExtractorToCsv::dumpElectronsToCsv()
{
  unsigned int maxnumobjt = maxNumObjt;
  if(nmu>0){
  oss.str("");oss.clear();oss<<runno;
  myfile<<oss.str();
  oss.str("");oss.clear();oss<<evtno;
  myfile<<","<<oss.str();
    for (unsigned int j=0;j<maxnumobjt;j++){
      oss.str("");oss.clear();oss<<electron_partype;
      myfile<<","<<oss.str();
      oss.str("");oss.clear();oss<<electron_e[j];
      j<electron_e.size() ? myfile<<","<<oss.str():myfile<<",0.0";
      //      std::cout<<maxnumobjt<<"\t"<<nmu<<"\t"<<electron_e.size()<<"\t"<<electron_e[j]<<"\t"<<oss.str()<<std::endl;
      oss.str("");oss.clear();oss<<electron_px[j];
      j<electron_px.size() ? myfile<<","<<oss.str():myfile<<",0.0";
      oss.str("");oss.clear();oss<<electron_py[j];
      j<electron_py.size() ? myfile<<","<<oss.str():myfile<<",0.0";
      oss.str("");oss.clear();oss<<electron_pz[j];
      j<electron_pz.size() ? myfile<<","<<oss.str():myfile<<",0.0";
      oss.str("");oss.clear();oss<<electron_pt[j];
      j<electron_pt.size() ? myfile<<","<<oss.str():myfile<<",0.0";
      oss.str("");oss.clear();oss<<electron_eta[j];
      j<electron_eta.size() ? myfile<<","<<oss.str():myfile<<",0.0";
      oss.str("");oss.clear();oss<<electron_phi[j];
      j<electron_phi.size() ? myfile<<","<<oss.str():myfile<<",0.0";
      oss.str("");oss.clear();oss<<electron_ch[j];
      j<electron_ch.size() ? myfile<<","<<oss.str():myfile<<",0.0";
    }
  myfile<<"\n";
  }
}


// ------------ method called once each job just before starting event loop  ------------
void 
ElectronObjectInfoExtractorToCsv::beginJob()
{
  //Define storage
  myfile.open("ElectronObjectInfo.csv");
  //Write the header.
  //create the header string accordingly
  theHeader = "Run,Event";
  for(int j =1;j<maxNumObjt+1;j++){
    oss.str(""); oss<<j;
    std::string idxstr = oss.str();
    theHeader += ",type"+idxstr+",E"+idxstr+",px"+idxstr+",py"+idxstr+",pz"+idxstr+",pt"+idxstr+",eta"+idxstr+",phi"+idxstr+",Q"+idxstr;
  }
  
  myfile<<theHeader<<"\n";

}

// ------------ method called once each job just after ending the event loop  ------------
void 
ElectronObjectInfoExtractorToCsv::endJob() 
{

  //save file
  myfile.close();

}

// ------------ method called when starting to processes a run  ------------
void 
ElectronObjectInfoExtractorToCsv::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
ElectronObjectInfoExtractorToCsv::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
ElectronObjectInfoExtractorToCsv::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
ElectronObjectInfoExtractorToCsv::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
ElectronObjectInfoExtractorToCsv::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(ElectronObjectInfoExtractorToCsv);std::ostringstream oss;
