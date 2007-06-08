#---------------------------------------------------------------  
# ATLAS electromagnetic: sampling Pb-LAr.
#                        92 layers: Pb (1.53 mm) - LAr (4.2 mm) 
#                        (total Pb+LAr : 27.6 X0).
#
# ATLAS hadronic endcap: sampling Cu-LAr.
#                        60 layers: 25 mm (Cu) - 8.5 mm (LAr).
#                        (total about 10 lambda).
#
# Radius of the cylinder: 75 cm
#                         (diameter about 10 lambda)
#---------------------------------------------------------------  
#
###/random/resetEngineFrom start.rndm
/random/setSavingFlag 1
#
/run/verbose 1 
/event/verbose 0 
/tracking/verbose 0 
#
#/gun/particle mu-
#/gun/particle e-
/gun/particle pi-
#/gun/particle proton
#
/gun/energy 5 GeV
#
/mydet/trackerLength 0.0
#
/mydet/emAbsorberMaterial Lead
/mydet/emActiveMaterial LiquidArgon
/mydet/isEmCalHomogeneous 0
/mydet/emAbsorberTotalLength 140.76
/mydet/emActiveLayerNumber 92
/mydet/emActiveLayerSize 4.2
#
/mydet/hadAbsorberMaterial Copper
/mydet/hadActiveMaterial LiquidArgon
/mydet/isHadCalHomogeneous 0
/mydet/hadAbsorberTotalLength 1500.0
/mydet/hadActiveLayerNumber 60
/mydet/hadActiveLayerSize 8.5
#
/mydet/muonLength 0.0
#
/mydet/detectorRadius 750.0
#
/mydet/update
#
/run/beamOn 100
#

