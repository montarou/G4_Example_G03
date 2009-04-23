//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: G4FTFPPiKBuilder.cc,v 1.5 2009-04-23 18:54:57 japost Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4FTFPPiKBuilder
//
// Author: 2002 J.P. Wellisch
//
// Modified:
// 30.03.2009 V.Ivanchenko create cross section by new
//
//----------------------------------------------------------------------------
//
#include "G4FTFPPiKBuilder.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"

G4FTFPPiKBuilder::
G4FTFPPiKBuilder(G4bool quasiElastic) 
 {
   thePiData = new G4PiNuclearCrossSection;
   theMin = 4*GeV;
   theMax = 100*TeV;
   theModel = new G4TheoFSGenerator("FTFP");

   theStringModel = new G4FTFModel;
   theStringDecay = new G4ExcitedStringDecay(new G4LundStringFragmentation);
   theStringModel->SetFragmentationModel(theStringDecay);

   theCascade = new G4GeneratorPrecompoundInterface;
   thePreEquilib = new G4PreCompoundModel(new G4ExcitationHandler);
   theCascade->SetDeExcitation(thePreEquilib);  

   theModel->SetHighEnergyGenerator(theStringModel);
   if (quasiElastic)
   {
      theQuasiElastic=new G4QuasiElasticChannel;
      theModel->SetQuasiElasticChannel(theQuasiElastic);
   } else 
   {  theQuasiElastic=0;}  

   theModel->SetTransport(theCascade);
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(100*TeV);
 }

G4FTFPPiKBuilder:: ~G4FTFPPiKBuilder() 
 {
   delete theCascade;
   delete theStringDecay;
   delete theStringModel;
   delete theModel;
   if ( theQuasiElastic ) delete theQuasiElastic;
 }

void G4FTFPPiKBuilder::
Build(G4HadronElasticProcess * ) {}

void G4FTFPPiKBuilder::
Build(G4PionPlusInelasticProcess * aP)
 {
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
   aP->AddDataSet(thePiData);
   aP->RegisterMe(theModel);
 }

void G4FTFPPiKBuilder::
Build(G4PionMinusInelasticProcess * aP)
 {
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
   aP->AddDataSet(thePiData);
   aP->RegisterMe(theModel);
 }

void G4FTFPPiKBuilder::
Build(G4KaonPlusInelasticProcess * aP)
 {
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
   aP->RegisterMe(theModel);
 }

void G4FTFPPiKBuilder::
Build(G4KaonMinusInelasticProcess * aP)
 {
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
   aP->RegisterMe(theModel);
 }

void G4FTFPPiKBuilder::
Build(G4KaonZeroLInelasticProcess * aP)
 {
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
   aP->RegisterMe(theModel);
 }

void G4FTFPPiKBuilder::
Build(G4KaonZeroSInelasticProcess * aP)
 {
   theModel->SetMinEnergy(theMin);
   theModel->SetMaxEnergy(theMax);
   aP->RegisterMe(theModel);
 }

 // 2002 by J.P. Wellisch
