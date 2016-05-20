// -*- C++ -*-

//____________________________________________________________________________||
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/CorrMETData.h"
#include "DataFormats/PatCandidates/interface/MET.h"

#include "JetMETCorrections/Type1MET/interface/AddCorrectionsToGenericMET.h"

#include <vector>

//____________________________________________________________________________||
class CorrectedPFMETProducer : public edm::stream::EDProducer<>
{

public:

  explicit CorrectedPFMETProducer(const edm::ParameterSet& cfg)
    : corrector()
      //token_(consumes<METCollection>(cfg.getParameter<edm::InputTag>("src")))
  {
    isMiniAod = (cfg.exists("isMiniAod") ) ?  cfg.getParameter<bool>("isMiniAod"): false;
    if(isMiniAod)
    {
      token_=consumes<METCollection>(cfg.getParameter<edm::InputTag>("src"));
    }else{
      pfToken_=consumes<pfMETCollection>(cfg.getParameter<edm::InputTag>("src"));
    }



    std::vector<edm::InputTag> corrInputTags = cfg.getParameter<std::vector<edm::InputTag> >("srcCorrections");
    std::vector<edm::EDGetTokenT<CorrMETData> > corrTokens;
    for (std::vector<edm::InputTag>::const_iterator inputTag = corrInputTags.begin(); inputTag != corrInputTags.end(); ++inputTag) {
      corrTokens.push_back(consumes<CorrMETData>(*inputTag));
    }
    
    corrector.setCorTokens(corrTokens);

    produces<METCollection>("");
  }

  ~CorrectedPFMETProducer() { }

private:

  bool isMiniAod;

  AddCorrectionsToGenericMET corrector;

  typedef std::vector<pat::MET> METCollection;
  typedef std::vector<reco::PFMET> pfMETCollection;

  edm::EDGetTokenT<METCollection> token_;
  edm::EDGetTokenT<pfMETCollection> pfToken_;
 

  void produce(edm::Event& evt, const edm::EventSetup& es) override
  {
    edm::Handle<METCollection> srcMETCollection;
    edm::Handle<pfMETCollection> srcPfMETCollection;
    if(isMiniAod)
    {
      evt.getByToken(token_, srcMETCollection);
    }else{
      evt.getByToken(pfToken_, srcPfMETCollection);
    }



    if(isMiniAod){
      std::auto_ptr<METCollection> product(new METCollection);
      const reco::MET& srcMET = (*srcMETCollection)[0];
      pat::MET outMEtPat = corrector.getCorrectedMET(srcMET, evt, es);
      product->push_back(outMEtPat);
      evt.put(product);
    }else{
      std::auto_ptr<pfMETCollection> product(new pfMETCollection);
      const reco::PFMET& srcMET = (*srcPfMETCollection)[0];
      reco::PFMET outPfMEtReco = corrector.getCorrectedPFMET(srcMET, evt, es);
      product->push_back(outPfMEtReco);
      evt.put(product);
    }
        
  }

};

//____________________________________________________________________________||

DEFINE_FWK_MODULE(CorrectedPFMETProducer);
