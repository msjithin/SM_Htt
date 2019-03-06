// -*- C++ -*-
//
// Package:    Acceptance/AcceptanceAnalyzer
// Class:      AcceptanceAnalyzer
// 
/**\class AcceptanceAnalyzer AcceptanceAnalyzer.cc Acceptance/AcceptanceAnalyzer/plugins/AcceptanceAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Tyler Henry Ruggles
//         Created:  Mon, 29 Feb 2016 10:32:40 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

// Added
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TTree.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimDataFormats/HTXS/interface/HiggsTemplateCrossSections.h"



//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class AcceptanceAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit AcceptanceAnalyzer(const edm::ParameterSet&);
      ~AcceptanceAnalyzer();
  //    virtual double AcceptanceAnalyzer::DeltaPhi(double phi1, double phi2);
      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    const edm::EDGetTokenT<HTXS::HiggsClassification> getRivetInfo();
  
private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  Double_t deltaPhi(Double_t phi1, Double_t phi2);
  Double_t deltaR(Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2);
  
      // ----------member data ---------------------------
      edm::EDGetTokenT<std::vector<reco::GenJet>> genHadronicTausToken_;
      edm::EDGetTokenT<std::vector<reco::GenJet>> genElectronicTausToken_;
      edm::EDGetTokenT<std::vector<reco::GenJet>> genMuonicTausToken_;
      edm::EDGetTokenT<std::vector<PileupSummaryInfo>> puToken_;
      edm::EDGetTokenT<HTXS::HiggsClassification> htxsToken_;
      edm::EDGetTokenT<LHEEventProduct> lheToken_;
      TTree *tree;
      float genMass, ETauPass, MuTauPass, EMuPass, TauTauPass, TauTau4030Pass, MuMuPass;
      float ETauD, MuTauD, EMuD, TauTauD, MuMuD;
      float threeLeptons, nLooseTaus, nLooseElec, nLooseMu;
      float nTruePU, tauPt1, tauPt2, tauPt3;
      float tauEta1, tauEta2, tauEta3;
      float tauPhi1, tauPhi2, tauPhi3;
      float d_phi, d_eta;
      float deltaR1, deltaR2, deltaR3, deltaR4;
      float run, lumi;
      double eventD;
      float higgsPt;
  Int_t nTau , hTau_1, hTau_2;


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
AcceptanceAnalyzer::AcceptanceAnalyzer(const edm::ParameterSet& iConfig) :
    genHadronicTausToken_(consumes<std::vector<reco::GenJet>>(iConfig.getParameter<edm::InputTag>("hadronSrc"))),
    genElectronicTausToken_(consumes<std::vector<reco::GenJet>>(iConfig.getParameter<edm::InputTag>("electronSrc"))),
    genMuonicTausToken_(consumes<std::vector<reco::GenJet>>(iConfig.getParameter<edm::InputTag>("muonSrc"))),
    puToken_(consumes<std::vector<PileupSummaryInfo>>(iConfig.getParameter<edm::InputTag>("puSrc"))),
    htxsToken_(consumes<HTXS::HiggsClassification>(edm::InputTag("rivetProducerHTXS","HiggsClassification"))),
    lheToken_(consumes<LHEEventProduct>(iConfig.getParameter<edm::InputTag>("lheSrc")))
    
{
   //now do what ever initialization is needed
   //usesResource("TFileService");
   edm::Service<TFileService> fs;
   TFileDirectory subDir = fs->mkdir( "events" );
   tree = subDir.make<TTree>("Ntuple","My Analyzer Ntuple");
   tree->Branch("run",&run,"run/F");
   tree->Branch("lumi",&lumi,"lumi/F");
   tree->Branch("eventD",&eventD,"eventD/D");
   tree->Branch("genMass",&genMass,"genMass/F");
   tree->Branch("ETauPass",&ETauPass,"ETauPass/F");
   tree->Branch("ETauD",&ETauD,"ETauD/F");
   tree->Branch("MuTauPass",&MuTauPass,"MuTauPass/F");
   tree->Branch("MuTauD",&MuTauD,"MuTauD/F");
   tree->Branch("EMuPass",&EMuPass,"EMuPass/F");
   tree->Branch("EMuD",&EMuD,"EMuD/F");
   tree->Branch("TauTauPass",&TauTauPass,"TauTauPass/F");
   tree->Branch("TauTau4030Pass",&TauTau4030Pass,"TauTau4030Pass/F");
   tree->Branch("TauTauD",&TauTauD,"TauTauD/F");
   tree->Branch("MuMuPass",&MuMuPass,"MuMuPass/F");
   tree->Branch("MuMuD",&MuMuD,"MuMuD/F");
   tree->Branch("threeLeptons",&threeLeptons,"threeLeptons/F");
   tree->Branch("nLooseTaus",&nLooseTaus,"nLooseTaus/F");
   tree->Branch("nLooseElec",&nLooseElec,"nLooseElec/F");
   tree->Branch("nLooseMu",&nLooseMu,"nLooseMu/F");
   tree->Branch("nTruePU",&nTruePU,"nTruePU/F");
   tree->Branch("tauPt1",&tauPt1,"tauPt1/F");
   tree->Branch("tauPt2",&tauPt2,"tauPt2/F");
   tree->Branch("tauPt3",&tauPt3,"tauPt3/F");
   tree->Branch("tauEta1",&tauEta1,"tauEta1/F");
   tree->Branch("tauEta2",&tauEta2,"tauEta2/F");
   tree->Branch("tauEta3",&tauEta3,"tauEta3/F");
   tree->Branch("tauPhi1",&tauPhi1,"tauPhi1/F");
   tree->Branch("tauPhi2",&tauPhi2,"tauPhi2/F");
   tree->Branch("tauPhi3",&tauPhi3,"tauPhi3/F");
   tree->Branch("d_phi",&d_phi,"d_phi/F");
   tree->Branch("d_eta",&d_eta,"d_eta/F");
   tree->Branch("higgsPt",&higgsPt,"higgsPt/F");

   tree->Branch("deltaR1",&deltaR1,"deltaR1/F");
   tree->Branch("deltaR2",&deltaR2,"deltaR2/F");
   tree->Branch("deltaR3",&deltaR3,"deltaR3/F");
   tree->Branch("deltaR4",&deltaR4,"deltaR4/F");
  

   tree->Branch("nTau",&nTau);

   run = -1.0;
   lumi = -1.0;
    eventD = -1.0;
    genMass = -1.0;
    ETauPass = 0;
    MuTauPass = 0;
    EMuPass = 0;
    TauTauPass = 0;
    TauTau4030Pass = 0;
    MuMuPass = 0;
    ETauD = 0;
    MuTauD = 0;
    EMuD = 0;
    TauTauD = 0;
    MuMuD = 0;
    threeLeptons = 0;
    nLooseTaus = 0;
    nLooseElec = 0;
    nLooseMu = 0;
    nTruePU = -1.0;
    tauPt1 = -1.0;
    tauPt2 = -1.0;
    tauPt3 = -1.0;
    tauEta1 = -10.0;
    tauEta2 = -10.0;
    tauEta3 = -10.0;
    tauPhi1 = -10.0;
    tauPhi2 = -10.0;
    tauPhi3 = -10.0;
    d_phi = -10.0;
    d_eta = -10.0;
    deltaR1 = -1.0;
    deltaR2 = -1.0;
    deltaR3 = -1.0;
    deltaR4 = -1.0;
    higgsPt = -1.0;
    nTau =hTau_1= hTau_2=-1.0;

}


AcceptanceAnalyzer::~AcceptanceAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
AcceptanceAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    edm::Handle<std::vector<reco::GenJet>> hTaus;   
    iEvent.getByToken(genHadronicTausToken_, hTaus);
    edm::Handle<std::vector<reco::GenJet>> eTaus;   
    iEvent.getByToken(genElectronicTausToken_, eTaus);
    edm::Handle<std::vector<reco::GenJet>> mTaus;   
    iEvent.getByToken(genMuonicTausToken_, mTaus);
    edm::Handle<std::vector<PileupSummaryInfo>> puInfo;   
    iEvent.getByToken(puToken_, puInfo);
    edm::Handle<HTXS::HiggsClassification> htxs;
    iEvent.getByToken(htxsToken_, htxs);
    edm::Handle<LHEEventProduct> lheProd;   
    iEvent.getByToken(lheToken_, lheProd);

    //std::cout << iEvent.eventAuxiliary().event() << std::endl;
    run = iEvent.eventAuxiliary().run();
    lumi = iEvent.eventAuxiliary().luminosityBlock();
    eventD = iEvent.eventAuxiliary().event();

 
    iEvent.getByToken(htxsToken_,htxs);
    if (htxs.isValid())
     {
       //std::cout << "Rivet info higgs pt " << htxs->higgs.pt() << std::endl;
	higgsPt = htxs->higgs.pt();
      }


    // Get the number of true events
    // This is used later for pile up reweighting
    if (puInfo->size() > 0) {
        //std::cout<<"pu size = "<<puInfo->size()<<std::endl;
        //std::cout<<puInfo->at(1).getTrueNumInteractions()<<std::endl;
        nTruePU = puInfo->at(1).getTrueNumInteractions();
    }

    // Denominator section first, just check if there's the correct #
    // of the associated leptons
    if (hTaus->size() >= 2) TauTauD = 1;
    if (hTaus->size() >= 1) {
        if(eTaus->size() >= 1) ETauD = 1;
        if(mTaus->size() >= 1) MuTauD = 1;
    }
    if(eTaus->size() >= 1 && mTaus->size() >= 1) EMuD = 1;
    if(mTaus->size() >= 2) MuMuD = 1;
    
    float nLeptons = 0;
    nLeptons += hTaus->size() + eTaus->size() + mTaus->size();
    if (nLeptons >= 3) threeLeptons = 1;
    nLooseTaus = hTaus->size();
    nLooseElec = eTaus->size();
    nLooseMu = mTaus->size();


    size_t ETau_e = 0;
    size_t ETau_t = 0;
    size_t MuTau_m = 0;
    size_t MuTau_t = 0;
    size_t EMu_e = 0;
    size_t EMu_m = 0;
    size_t TauTau_t1 = 0;
    size_t TauTau_t2 = 0;
    size_t MuMu_m1 = 0;
    size_t MuMu_m2 = 0;

    //const lhef::HEPEUP lhe = lheProd.product()->hepeup();
    //    uint32_t nHTaus = hTaus->size();
    //if (hTaus->size() > 0)// std::cout << " --- N Hadronic Taus: "<<nHTaus<<std::endl;
    //{
    //const std::vector<reco::GenJet> nTaus_ = hTaus.product();
    std::vector< float > pts;
    for (const reco::GenJet &tau : *hTaus) {
        pts.push_back( tau.pt() );
	if ( TMath::Abs(tau.eta()) < 2.3 && tau.pt() > 20 ) {
	  MuTau_t += 1;
	  ETau_t += 1;}
        if ( TMath::Abs(tau.eta()) < 2.1 && tau.pt() > 40 ) TauTau_t1 += 1;
        if ( TMath::Abs(tau.eta()) < 2.1 && tau.pt() > 30 ) TauTau_t2 += 1;	
	}

    if ( pts.size() > 0 ) tauPt1 = pts.at(0);
    if ( pts.size() > 1 ) tauPt2 = pts.at(1);
    if ( pts.size() > 2 ) tauPt3 = pts.at(2);


    std::vector< float > etas;
    for (const reco::GenJet &tau : *hTaus) {
      etas.push_back( tau.eta() );
    }

    if ( etas.size() > 0 ) tauEta1 = etas.at(0);
    if ( etas.size() > 1 ) tauEta2 = etas.at(1);
    if ( etas.size() > 2 ) tauEta3 = etas.at(2);

    std::vector< float > phis;
    for (const reco::GenJet &tau : *hTaus) {
      phis.push_back( tau.phi() );
    }

    if ( phis.size() > 0 ) tauPhi1 = phis.at(0);
    if ( phis.size() > 1 ) tauPhi2 = phis.at(1);
    if ( phis.size() > 2 ) tauPhi3 = phis.at(2);

    //}
    //uint32_t nETaus = eTaus->size();
    //if (nETaus > 0)// std::cout << " ### N Electronic Taus: "<<nETaus<<std::endl;
    //{
    for (const reco::GenJet &ele : *eTaus) {
        if ( TMath::Abs(ele.eta()) < 2.5 && ele.pt() > 13 ) EMu_e += 1;
        if ( TMath::Abs(ele.eta()) < 2.1 && ele.pt() > 24 ) ETau_e += 1;
    }
    //uint32_t nMTaus = mTaus->size();
    //if (nMTaus > 0)// std::cout << " *** N Muonic Taus: "<<nMTaus<<std::endl;
    //{
    for (const reco::GenJet &mu : *mTaus) {
        if ( TMath::Abs(mu.eta()) < 2.4 && mu.pt() > 10 ) EMu_m += 1;
        if ( TMath::Abs(mu.eta()) < 2.1 && mu.pt() > 19 ) MuTau_m += 1;
        if ( TMath::Abs(mu.eta()) < 2.4 && mu.pt() > 20 ) MuMu_m1 += 1; // # of mu passing "leading" cut
        if ( TMath::Abs(mu.eta()) < 2.4 && mu.pt() < 20 && mu.pt() > 10 ) MuMu_m2 += 1; // # of mu passing only "trailing" cut
    }
    //}
    
    // Check if we have matches
    if (ETau_e > 0 && ETau_t > 0) ETauPass = 1;
    if (MuTau_m > 0 && MuTau_t > 0) MuTauPass = 1;
    if (EMu_e > 0 && EMu_m > 0) EMuPass = 1;
    if (TauTau_t1 > 1) TauTauPass = 1;
    if (TauTau_t1 == 1 && TauTau_t2 > 0) TauTau4030Pass = 1;
    if (MuMu_m1 > 1) MuMuPass = 1;
    if (MuMu_m1 > 0 && MuMu_m2 > 0) MuMuPass = 1;
    

    //std::cout << lheProd << std::endl;
    //lhe
    //std::cout << lheProd.isValid() << std::endl;
    //lhef::HEPEUP lhe;
    //if (lheProd.isValid()) {
    //  lhe = lheProd->hepeup();
    //}
    const lhef::HEPEUP lhe = lheProd.product()->hepeup();
    //std::cout << lhe.ISTUP[0] << std::endl;
    std::vector<int> outgoing;
    std::vector<TLorentzVector> invmass;
    for (uint32_t i = 0; i < lhe.ISTUP.size(); ++i) {
        if (lhe.ISTUP[i]) {
            int Id = TMath::Abs( lhe.IDUP[i] );
            if (Id==21||Id==9||Id==1||Id==2||Id==3||Id==4||Id==5||Id==6 )
	      //if (Id > 0)
	      outgoing.push_back( TMath::Abs( Id ));
            int Id2 = TMath::Abs( lhe.IDUP[i] );
            if (Id2==11||Id2==13||Id2==15) {
	      TLorentzVector l = TLorentzVector( lhe.PUP[i][0],
						 lhe.PUP[i][1],
						 lhe.PUP[i][2],
						 lhe.PUP[i][3]);
	      invmass.push_back( l );
            }
        }
    }
    if (invmass.size() == 2) 
      {
        //std::cout << "Len InvMass: "<<invmass.size()<<std::endl;
	TLorentzVector diLep = invmass[0];
	diLep += invmass[1];
	//std::cout << "m(ll) " << diLep.M() << std::endl;
	genMass = diLep.M();
	
      }
    std::vector<float> DeltaR1;
    std::vector<float> DeltaR2;
    std::vector<float> DeltaR3;
    std::vector<float> DeltaR4;

    if (ETauPass ==1){
      for (const reco::GenJet &tau : *hTaus) {
	for (const reco::GenJet &ele : *eTaus) {
	  DeltaR1.push_back(deltaR(ele.eta(), ele.phi(), tau.eta(), tau.phi() ));
	}
      }
    }
    if (MuTauPass == 1){
      for (const reco::GenJet &tau : *hTaus) {
	for (const reco::GenJet &mu : *mTaus) {
	  DeltaR2.push_back(deltaR(mu.eta(), mu.phi(), tau.eta(), tau.phi() ));
	}
      }
    }
    if (EMuPass==1){
      for (const reco::GenJet &ele : *eTaus) {
	for (const reco::GenJet &mu : *mTaus) {
	  DeltaR4.push_back(deltaR(ele.eta(), ele.phi(), mu.eta(), mu.phi() ));
	}
      }
    }

    std::vector< float > tauCand;
    nTau =  hTaus->size();
    //    tauCand.push_back(nTau);

    float  hTau_1_eta=0;
    float  hTau_2_eta=0;
    float  hTau_1_phi=0;
    float  hTau_2_phi=0;
    std::vector< float > hTau_phis;
    std::vector< float > hTau_etas;

    if (hTaus->size() >=2)
      {
	for (const reco::GenJet &tau : *hTaus) {
	  hTau_phis.push_back( tau.phi() );
	  hTau_etas.push_back( tau.eta() );
	}
	
	hTau_1_eta = hTau_etas.at(0);
	hTau_2_eta = hTau_etas.at(1);
	hTau_1_phi = hTau_phis.at(0);
	hTau_2_phi = hTau_phis.at(1);
      }
    std::cout << "tau etas  " << hTau_1_eta << " and "<< hTau_2_eta << std::endl;
    std::cout << "tau phis  " << hTau_1_phi << " and "<< hTau_2_phi << std::endl;

    if (TauTauPass ==1) { ////////// change this
      //for (const reco::GenJet &tau : *hTaus) {
      if (!(hTau_1_eta==0 && hTau_2_eta==0 && hTau_1_phi==0 && hTau_2_phi==0))
	{
      DeltaR3.push_back(deltaR(hTau_1_eta, hTau_1_phi,hTau_2_eta,hTau_2_phi ));
	}
    }

    
    
    if (DeltaR1.size() >0 )deltaR1=DeltaR1.at(0);
    if (DeltaR2.size() >0 )deltaR2=DeltaR2.at(0);
    if (DeltaR3.size() >0 )deltaR3=DeltaR3.at(0);
    if (DeltaR4.size() >0 )deltaR4=DeltaR4.at(0);

    //LogInfo("Demo") << "number of gen taus "<<nGenTaus;
    //std::cout << genTaus << std::endl;

    tree->Fill();


#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}


// ------------ method called once each job just before starting event loop  ------------
void 
AcceptanceAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
AcceptanceAnalyzer::endJob() 
{
}


Double_t AcceptanceAnalyzer::deltaPhi(Double_t phi1, Double_t phi2) {

  Double_t dPhi = phi1 - phi2;
  if (dPhi > TMath::Pi()) dPhi -= 2.*TMath::Pi();
  if (dPhi < -TMath::Pi()) dPhi += 2.*TMath::Pi();
  Double_t d_Phi_temp = TMath::Abs(dPhi);
  return d_Phi_temp;
}

Double_t AcceptanceAnalyzer::deltaR(Double_t eta1, Double_t phi1, Double_t eta2, Double_t phi2) {

  Double_t dEta, dPhi ;
  dEta = eta1 - eta2;
  dPhi = deltaPhi(phi1, phi2);

  return sqrt(dEta*dEta+dPhi*dPhi);
}


const edm::EDGetTokenT<HTXS::HiggsClassification> AcceptanceAnalyzer::getRivetInfo(){
  const edm::EDGetTokenT<HTXS::HiggsClassification> htxsRivetInfo_;
  return htxsRivetInfo_;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
AcceptanceAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(AcceptanceAnalyzer);