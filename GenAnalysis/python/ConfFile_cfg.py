import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10000) )
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1000)

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        #'file:/afs/cern.ch/work/t/truggles/Z_to_tautau/dyjets_76x.root'
        #'file:root://cmsxrootd.fnal.gov//store/mc/RunIIFall15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/70000/002ABFCA-A0B9-E511-B9BA-0CC47A57CD6A.root',
        
        #'file:root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/40000/6A62DE01-E641-E811-AFF1-008CFAC94038.root',
        #'file:root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU2017RECOSIMstep_12Apr2018_94X_mc2017_realistic_v14_ext1-v1/70000/DA1BE8CB-5444-E811-BDBF-0025905A48FC.root',
        
        #'file:root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU2017RECOSIMstep_12Apr2018_94X_mc2017_realistic_v14_ext1-v1/70000/0C335F78-5644-E811-B6E5-0CC47A7452DA.root',
        'file:root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/GluGluHToTauTau_M125_13TeV_powheg_pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/90000/D84ED2D3-5B42-E811-B73B-0CC47A745294.root',
        #'file:root://cmsxrootd.fnal.gov//store/mc/RunIIFall17MiniAODv2/GluGluHToTauTau_M125_13TeV_powheg_pythia8/MINIAODSIM/PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/90000/FE4D4DD9-5B42-E811-A066-0CC47A4D76B6.root'


    )
)

process.tauGenJets = cms.EDProducer(
    "TauGenJetProducer",
    GenParticles =  cms.InputTag('prunedGenParticles'),
    includeNeutrinos = cms.bool( False ),
    verbose = cms.untracked.bool( False )
    )

process.tauGenJetsSelectorAllHadrons = cms.EDFilter("TauGenJetDecayModeSelector",
     src = cms.InputTag("tauGenJets"),
     select = cms.vstring('oneProng0Pi0', 
                          'oneProng1Pi0', 
                          'oneProng2Pi0', 
                          'oneProngOther',
                          'threeProng0Pi0', 
                          'threeProng1Pi0', 
                          'threeProngOther', 
                          'rare'),
     filter = cms.bool(False)
)

process.tauGenJetsSelectorElectrons = cms.EDFilter("TauGenJetDecayModeSelector",
     src = cms.InputTag("tauGenJets"),
     select = cms.vstring('electron'), 
     filter = cms.bool(False)
)

process.tauGenJetsSelectorMuons = cms.EDFilter("TauGenJetDecayModeSelector",
     src = cms.InputTag("tauGenJets"),
     select = cms.vstring('muon'), 
     filter = cms.bool(False)
)



process.rivetProducerHTXS = cms.EDProducer('HTXSRivetProducer',
  HepMCCollection = cms.InputTag('myGenerator','unsmeared'),
  LHERunInfo = cms.InputTag('externalLHEProducer'),
  #ProductionMode = cms.string('GGF'),
  ProductionMode = cms.string('AUTO'),
)



#MINIAOD

process.mergedGenParticles = cms.EDProducer("MergedGenParticleProducer",
    inputPruned = cms.InputTag("prunedGenParticles"),
    inputPacked = cms.InputTag("packedGenParticles"),
)

process.myGenerator = cms.EDProducer("GenParticles2HepMCConverter",
    genParticles = cms.InputTag("mergedGenParticles"),
    genEventInfo = cms.InputTag("generator"),
    signalParticlePdgIds = cms.vint32(25), ## for the Higgs analysis
)



#process.load("GeneratorInterface.RivetInterface.genParticles2HepMC_cff")
#process.rivetProducerHTXS = cms.EDProducer('HTXSRivetProducer',
#                                           HepMCCollection = cms.InputTag('myGenerator','unsmeared'),
#                                           LHERunInfo = cms.InputTag('externalLHEProducer'),
#                                           ProductionMode = cms.string('AUTO'),
#)

process.load("SM_Htt.GenAnalysis.CfiFile_cfi")
#process.load("GenAnalysis.AcceptanceAnalyzer.CfiFile_cfi")
process.demo = cms.EDAnalyzer('AcceptanceAnalyzer',
    hadronSrc = cms.InputTag('tauGenJetsSelectorAllHadrons'),
    electronSrc = cms.InputTag('tauGenJetsSelectorElectrons'),
    muonSrc = cms.InputTag('tauGenJetsSelectorMuons'),
    puSrc = cms.InputTag('slimmedAddPileupInfo'),
    htxsRivetSrc = cms.InputTag('rivetProducerHTXS'),
    lheSrc = cms.InputTag('externalLHEProducer')
)

process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string('ttree.root')
                                   )


#process.out = cms.OutputModule("PoolOutputModule",
#    fileName = cms.untracked.string('myOutputFile.root')
#    ,outputCommands = cms.untracked.vstring('drop *',
#      #"keep *_myProducerLabel_*_*",
#      #"keep *_slimmedMuons_*_*",
#      "keep *_*_*_Demo",
#        )
#)

process.p = cms.Path(process.tauGenJets*
            process.tauGenJetsSelectorAllHadrons*
            process.tauGenJetsSelectorElectrons*
            process.tauGenJetsSelectorMuons*
            process.mergedGenParticles*
            process.myGenerator*
            process.rivetProducerHTXS*
            process.demo)

#process.e = cms.EndPath(process.out)