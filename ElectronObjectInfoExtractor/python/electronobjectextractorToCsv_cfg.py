#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""
Created on Tue Jul 24 10:37:46 2018

@author: lucasarthur
"""

import FWCore.ParameterSet.Config as cms

process = cms.Process("electronext")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.source = cms.Source("PoolSource",
     # replace 'myfile.root' with the source file you want to use
                            fileNames = cms.untracked.vstring(
                                    'file:myfile.root'
                                    )
                            )

process.electronextractorToCsv = cms.EDAnalyzer('ElectronInfoExtractorToCsv',
                                                InputCollection = cms.InputTag("electrons"),
                                                maxNumberElectrons = cms.untracked.int32(10)
                                                )

process.p = cms.Path(process.electronextractorToCsv)