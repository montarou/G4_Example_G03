//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//

/**
 * @author Mark Donszelmann, Joseph Perl
 */

#include <stdio.h>

#include "globals.hh"
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

//HepRep
#include "HEPREP/HepRep.h"

//G4
#include "G4Types.hh"
#include "G4Point3D.hh"
#include "G4Normal3D.hh"
#include "G4Polyline.hh"
#include "G4Polymarker.hh"
#include "G4Polyhedron.hh"
#include "G4Circle.hh"
#include "G4Square.hh"
#include "G4Text.hh"
#include "G4NURBS.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"
#include "G4VTrajectory.hh"
#include "G4VTrajectoryPoint.hh"
#include "G4VHit.hh"
#include "G4Scene.hh"
#include "G4Material.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"

// JHepRep
#include "XMLHepRepFactory.h"

// This
#include "G4HepRep.hh"
#include "G4HepRepSceneHandler.hh"
#include "G4HepRepViewer.hh"


using namespace HEPREP;
using namespace std;

G4int G4HepRepSceneHandler::sceneCount = 0;

//#define SDEBUG 1
//#define PDEBUG 1

G4HepRepSceneHandler::G4HepRepSceneHandler (G4VGraphicsSystem& system, const G4String& name)
        : G4VSceneHandler (system, sceneCount++, name),
          geometryLayer         ("Geometry"),
          eventLayer            ("Event"),
          calHitLayer           ("CalHit"),
          trajectoryLayer       ("Trajectory"),
          trajectoryPointLayer  ("TrajectoryPoint"),
          hitLayer              ("Hit"),
          rootVolumeName        ("Geometry"),
          eventNumber           (1),
          currentDepth          (0),
          currentPV             (0),
          currentLV             (0),
          _heprep               (NULL)
{

#ifdef SDEBUG
    cout << "G4HepRepSceneHandler::G4HepRepSceneHandler: " << system << endl;
#endif

    materialState[kStateSolid]      = G4String("Solid");
    materialState[kStateLiquid]     = G4String("Liquid");
    materialState[kStateGas]        = G4String("Gas");
    materialState[kStateUndefined]  = G4String("Undefined");    

    factory = new XMLHepRepFactory();
    writer = NULL;

    open(GetScene() == NULL ? G4String("G4HepRepOutput.heprep.zip") : GetScene()->GetName());
    openHepRep();
}


G4HepRepSceneHandler::~G4HepRepSceneHandler () {
#ifdef SDEBUG
    cout << "G4HepRepSceneHandler::~G4HepRepSceneHandler() " << endl;
#endif
    close();

    delete factory;
    factory = NULL;

    dynamic_cast<G4HepRep*>(GetGraphicsSystem())->removeSceneHandler();
}


void G4HepRepSceneHandler::open(G4String name) {
    if (writer != NULL) return;

    if (strcmp(name, "stdout") == 0) {
#ifdef SDEBUG
        cout << "G4HepRepSceneHandler::Open() stdout" << endl;
#endif
        writer = factory->createHepRepWriter(&cout, false, false);
        out  = NULL;
    } else if (strcmp(name, "stderr") == 0) {
#ifdef SDEBUG
        cout << "G4HepRepSceneHandler::Open() stderr" << endl;
#endif
        writer = factory->createHepRepWriter(&cerr, false, false);
        out = NULL;
    } else {
#ifdef SDEBUG
        cout << "G4HepRepSceneHandler::Open() " << name << endl;
#endif
        out = new ofstream(name.c_str(), std::ios::out | std::ios::binary );
        bool heprep = name.substr(name.size()-7, 7) == G4String(".heprep");
        bool heprepxml = name.substr(name.size()-11, 11) == G4String(".heprep.xml");
        bool heprepzip = name.substr(name.size()-11, 11) == G4String(".heprep.zip");
        bool heprepgz = name.substr(name.size()-10, 10) == G4String(".heprep.gz");
        
        bool zip = name.substr(name.size()-4, 4) == G4String(".zip");
        bool gz = name.substr(name.size()-3, 3) == G4String(".gz");
        
        writer = factory->createHepRepWriter(out, heprepzip || zip, heprepzip || heprepgz || zip || gz);
        
        // write warning
        if (!heprep && !heprepxml && !heprepzip && !heprepgz) {
            cout << "WARNING HepRep file: '" << name << "' cannot be read by JAS/WIRED, use extensions .heprep, .heprep.xml, .heprep.zip or .heprep.gz" << endl;
        }
    }
}


void G4HepRepSceneHandler::openHepRep() {
#ifdef SDEBUG
    cout << "G4HepRepSceneHandler::OpenHepRep() " << endl;
#endif

    if (_heprep != NULL) return;

    // all done on demand, once pointers are set to NULL
    _geometryInstanceTree = NULL;
    _geometryRootInstance = NULL;
    _geometryInstance.clear();
    _geometryTypeTree     = NULL;
    _geometryRootType     = NULL;
    _geometryTypeName.clear();
    _geometryType.clear();
    _eventInstanceTree    = NULL;
    _eventInstance        = NULL;
    _eventTypeTree        = NULL;
    _eventType            = NULL;
    _trajectoryType       = NULL;
    _trajectoryPointType  = NULL;
    _hitType              = NULL;
    _calHitType           = NULL;
    _calHitFaceType       = NULL;     
}


/**
 * Returns true if the HepRep was (already) closed, false if the HepRep is still open
 */
bool G4HepRepSceneHandler::closeHepRep() {
    if (_heprep == NULL) return true;

#ifdef SDEBUG
    cout << "G4HepRepSceneHandler::CloseHepRep() start" << endl;
#endif

    // add geometry to the heprep
    G4HepRepViewer* viewer = dynamic_cast<G4HepRepViewer*>(GetCurrentViewer());
    viewer->ProcessScene();

    // Give this HepRep all of the layer order info for both geometry and event,
    // since these will both end up in a single HepRep.
    if (_geometryRootType    != NULL) _heprep->addLayer(geometryLayer);
    if (_eventType           != NULL) _heprep->addLayer(eventLayer);
    if (_calHitType          != NULL) _heprep->addLayer(calHitLayer);
    if (_trajectoryType      != NULL) _heprep->addLayer(trajectoryLayer);
    if (_trajectoryPointType != NULL) _heprep->addLayer(trajectoryPointLayer);
    if (_hitType             != NULL) _heprep->addLayer(hitLayer);

    // write out the heprep
    char eventName[255];
    sprintf(eventName, "event%010d.heprep", eventNumber);
    writer->write(_heprep, eventName);
    eventNumber++;

    delete _heprep;
    _heprep = NULL;

#ifdef SDEBUG
    cout << "G4HepRepSceneHandler::CloseHepRep() end" << endl;
#endif
    return true;
}


void G4HepRepSceneHandler::close() {

#ifdef SDEBUG
    cout << "G4HepRepSceneHandler::Close() " << endl;
#endif

    if (writer == NULL) return;

    closeHepRep();

    writer->close();
    delete writer;
    writer = NULL;

    delete out;
    out = NULL;
}


void G4HepRepSceneHandler::EstablishSpecials(G4PhysicalVolumeModel& model) {
#ifdef SDEBUG
    cout << "G4HepRepSceneHandler::EstablishSpecials(G4PhysicalVolumeModel&) " << endl;
#endif
    model.DefinePointersToWorkingSpace(&currentDepth, &currentPV, &currentLV);
}


void G4HepRepSceneHandler::BeginModeling() {
#ifdef SDEBUG
    cout << "G4HepRepSceneHandler::BeginModeling() " << endl;
#endif
    G4VSceneHandler::BeginModeling();
}


void G4HepRepSceneHandler::EndModeling() {
#ifdef SDEBUG
    cout << "G4HepRepSceneHandler::EndModeling() " << endl;
#endif
    G4VSceneHandler::EndModeling();
}


void G4HepRepSceneHandler::AddPrimitive (const G4Polyline& line) {

#ifdef PDEBUG
    cout << "G4HepRepSceneHandler::AddPrimitive(G4Polyline&) " << line.size() << endl;
#endif

    HepRepInstance* instance = factory->createHepRepInstance(getEventInstance(), getTrajectoryType());

    setColor(instance, GetColor(line));

    setLine(instance, line);

    for (size_t i=0; i < line.size(); i++) {
        G4Point3D vertex = transform * line[i];
        factory->createHepRepPoint(instance, vertex.x(), vertex.y(), vertex.z());
    }
}


void G4HepRepSceneHandler::AddPrimitive (const G4Polymarker& line) {

#ifdef PDEBUG
    cout << "G4HepRepSceneHandler::AddPrimitive(G4Polymarker&) " << line.size() << endl;
#endif

    HepRepInstance* instance = factory->createHepRepInstance(getEventInstance(), getHitType());

    setColor(instance, GetColor(line));

    setMarker(instance, line);

    // Default MarkName is set to Circle for this Type.
    switch (line.GetMarkerType()) {
        case line.dots:
            setAttribute(instance, "Fill", true);
            setColor(instance, GetColor(line), G4String("FillColor"));
            break;
        case line.circles:
            break;
        case line.squares:
            setAttribute(instance, "MarkName", G4String("Box"));
            break;
        case line.line:
        default:
            setAttribute(instance, "MarkName", G4String("Plus"));
            break;
    }

    for (size_t i=0; i < line.size(); i++) {
        G4Point3D vertex = transform * line[i];
        factory->createHepRepPoint(instance, vertex.x(), vertex.y(), vertex.z());
    }
}


void G4HepRepSceneHandler::AddPrimitive (const G4Circle& circle) {
#ifdef PDEBUG
    cout << "G4HepRepSceneHandler::AddPrimitive(G4Circle&) " << endl;
#endif

    HepRepInstance* instance = factory->createHepRepInstance(getEventInstance(), getHitType());

    G4Point3D center = transform * circle.GetPosition();

    setColor (instance, GetColor(circle));

    setMarker(instance, circle);

    factory->createHepRepPoint(instance, center.x(), center.y(), center.z());
}


void G4HepRepSceneHandler::AddPrimitive (const G4Polyhedron& polyhedron) {

#ifdef PDEBUG
    cout << "G4HepRepSceneHandler::AddPrimitive(G4Polyhedron&) " << endl;
#endif
    G4Normal3D surfaceNormal;
    G4Point3D vertex;

    if (polyhedron.GetNoFacets()==0) return;

    HepRepInstance* instance;
    if (isEventData()) {
        instance = factory->createHepRepInstance(getEventInstance(), getCalHitType());
    } else {
        instance = getGeometryInstance(currentLV, currentDepth);
    }
    
    G4bool notLastFace;
    do {
        HepRepInstance* face;
        if (isEventData()) {
            face = factory->createHepRepInstance(instance, getCalHitType());
        } else {
            face = getGeometryInstance("*Face", currentDepth+1);
        }
        
        setLine(face, polyhedron);
        setColor(face, GetColor(polyhedron));
        if (isEventData()) setColor(face, GetColor(polyhedron), G4String("FillColor"));

        notLastFace = polyhedron.GetNextNormal (surfaceNormal);

        G4int edgeFlag = 1;
        G4bool notLastEdge;
        do {
	        notLastEdge = polyhedron.GetNextVertex (vertex, edgeFlag);
            vertex = transform * vertex;
            factory->createHepRepPoint(face, vertex.x(), vertex.y(), vertex.z());
        } while (notLastEdge);
    } while (notLastFace);
}


void G4HepRepSceneHandler::AddPrimitive (const G4Text&) {
#ifdef PDEBUG
    cout << "G4HepRepSceneHandler::AddPrimitive(G4Text&) " << endl;
#endif
    cout << "G4HepRepSceneHandler::AddPrimitive G4Text : not yet implemented. " << endl;
}


void G4HepRepSceneHandler::AddPrimitive (const G4Square& square) {
#ifdef PDEBUG
    cout << "G4HepRepSceneHandler::AddPrimitive(G4Square&) " << endl;
#endif

    HepRepInstance* instance = factory->createHepRepInstance(getEventInstance(), getHitType());

    G4Point3D center = transform * square.GetPosition();

    setColor (instance, GetColor(square));

    setMarker(instance, square);

    factory->createHepRepPoint(instance, center.x(), center.y(), center.z());
}


//Method for handling G4NURBS objects for drawing solids.
//Knots and Ctrl Pnts MUST be arrays of GLfloats.
void G4HepRepSceneHandler::AddPrimitive (const G4NURBS&) {
#ifdef PDEBUG
    cout << "G4HepRepSceneHandler::AddPrimitive(G4NURBS&) " << endl;
#endif
    cout << "G4HepRepSceneHandler::AddPrimitive G4NURBS : not yet implemented. " << endl;
}


void G4HepRepSceneHandler::AddThis (const G4VTrajectory& trajectory) {
#ifdef PDEBUG
    cout << "G4HepRepSceneHandler::AddThis(G4VTrajectory&) " << endl;
#endif

    vector<G4AttValue>* trajectoryAttValues = trajectory.CreateAttValues();
    const map<G4String,G4AttDef>* trajectoryAttDefs = trajectory.GetAttDefs();

    HepRepInstance* trajectoryInstance = factory->createHepRepInstance(getEventInstance(), 
                                                        getTrajectoryType(trajectoryAttDefs));
    addAttVals(trajectoryInstance, trajectoryAttValues);
    delete trajectoryAttValues;

    // Set the LineColor attribute according to the particle charge. GEANT-11
    // FIXME move this to converter method, should the user put this color?    
    float red = 0.;
    float green = 0.;
    float blue = 0.;
    const G4double charge = trajectory.GetCharge();
    if(charge>0.0)      blue =  1.0; // Blue = positive.
    else if(charge<0.0) red  =  1.0; // Red = negative.
    else                green = 1.0; // Green = neutral.
    setColor(trajectoryInstance, G4Color(red, green, blue));
    
    setAttribute(trajectoryInstance, "LineWidth", 2.0); // change to 1.0 for GEANT-13

    // Specify the polyline by using the trajectory points.
    G4int i;
    for (i = 0; i < trajectory.GetPointEntries(); i++) {
        G4VTrajectoryPoint* trajectoryPoint = trajectory.GetPoint(i);
        G4Point3D vertex = trajectoryPoint->GetPosition();
        HepRepPoint* point = factory->createHepRepPoint(trajectoryInstance, vertex.x(), vertex.y(), vertex.z());

        vector<G4AttValue>* pointAttValues = trajectoryPoint->CreateAttValues();
/* This uses way too much memory...
        // add all points also as separate instances (MD -> JP: why?), with single points
        const map<G4String,G4AttDef>* pointAttDefs = trajectoryPoint->GetAttDefs();
        HepRepInstance* pointInstance = factory->createHepRepInstance(trajectoryInstance, 
                                                        getTrajectoryPointType(pointAttDefs));

        // Specify the position of the trajectory point.
        factory->createHepRepPoint(pointInstance, vertex.x(), vertex.y(), vertex.z());
*/

// We cannot call addAttVals since it will only accept an instance and not a point.
// For now we just copy the attvals on a point and do not search the instance or type.
//        addAttVals(point, pointAttValues);
        if (pointAttValues != NULL) {
            // Copy the point's G4AttValues to HepRepAttValues.
            vector<G4AttValue>::iterator attValIterator;
            for (attValIterator = pointAttValues->begin(); attValIterator != pointAttValues->end(); attValIterator++) {
                // Pos already in points being written
                if (attValIterator->GetName() == "Pos") continue;
                
                point->addAttValue(attValIterator->GetName(), attValIterator->GetValue());
            }
        }
        delete pointAttValues;

    }
}



void
G4HepRepSceneHandler::PreAddThis (const G4Transform3D& objectTransformation,
				  const G4VisAttributes& visAttribs) {

    G4VSceneHandler::PreAddThis (objectTransformation, visAttribs);

    transform = objectTransformation;
#ifdef SDEBUG
    cout << "G4HepRepSceneHandler::PreAddThis(G4Transform3D&, G4VisAttributes&)" << endl;
#endif
}


void G4HepRepSceneHandler::PostAddThis () {
#ifdef SDEBUG
    cout << "G4HepRepSceneHandler::PostAddThis()" << endl;
#endif
    G4VSceneHandler::PostAddThis();
}


void G4HepRepSceneHandler::BeginPrimitives (const G4Transform3D& objectTransformation) {
#ifdef SDEBUG
    cout << "G4HepRepSceneHandler::BeginPrimitives(G4Transform3D&)" << endl;
#endif

    G4VSceneHandler::BeginPrimitives (objectTransformation);
    transform = objectTransformation;
}


void G4HepRepSceneHandler::EndPrimitives () {
#ifdef SDEBUG
    cout << "G4HepRepSceneHandler::EndPrimitives" << endl;
#endif
    G4VSceneHandler::EndPrimitives ();
}


void G4HepRepSceneHandler::setColor (HepRepInstance *instance,
				      const G4Color& color,
				      const G4String& key) {
#ifdef CDEBUG
    cout << "G4HepRepSceneHandler::setColor : red : " << color.GetRed ()   <<
                                  " green : " << color.GetGreen () <<
                                  " blue : " << color.GetBlue ()   << endl;
#endif

    setAttribute(instance, key, color.GetRed(), color.GetGreen(), color.GetBlue(),color.GetAlpha());
}


void G4HepRepSceneHandler::setLine (HepRepInstance *instance, const G4Visible& visible) {
    G4VisAttributes atts = visible.GetVisAttributes();
    
    setAttribute(instance, "LineWidth", atts.GetLineWidth());
    
    switch (atts.GetLineStyle()) {
        case G4VisAttributes::dotted:
            setAttribute(instance, "LineStyle", G4String("Dotted"));
            break;
        case G4VisAttributes::dashed:
            setAttribute(instance, "LineStyle", G4String("Dashed"));
            break;
        case G4VisAttributes::unbroken:
        default:
            break;
    }
}


void G4HepRepSceneHandler::setMarker (HepRepInstance *instance, const G4VMarker& marker) {
    MarkerSizeType markerType;
    G4double size = GetMarkerRadius( marker , markerType );
// FIXME make things visible by default, multiply by 4, remove for GEANT-13
    setAttribute(instance, "MarkSize", size*4);

    if (markerType == screen) setAttribute(instance, "MarkType", G4String("Symbol"));
    if (marker.GetFillStyle() == G4VMarker::noFill) {
        setAttribute(instance, "Fill", false);
    } else {
        setColor(instance, GetColor(marker), G4String("FillColor"));
    }
}

void G4HepRepSceneHandler::setAttribute(HepRepInstance *instance, G4String name, G4String value) {
    HepRepAttValue* attValue = instance->getAttValue(name);
    if ((attValue == NULL) || (attValue->getString() != value)) {
        HepRepAttribute* attribute = instance;
        // look for definition on type (node only)
        if (instance->getType()->getAttValueFromNode(name) == NULL) {
            attribute = instance->getType();
        }   
        
        attribute->addAttValue(name, value);
    }
}

void G4HepRepSceneHandler::setAttribute(HepRepInstance *instance, G4String name, bool value) {
    HepRepAttValue* attValue = instance->getAttValue(name);
    if ((attValue == NULL) || (attValue->getBoolean() != value)) {
        HepRepAttribute* attribute = instance;
        // look for definition on type (node only)
        if (instance->getType()->getAttValueFromNode(name) == NULL) {
            attribute = instance->getType();
        }   
        
        attribute->addAttValue(name, value);
    }
}

void G4HepRepSceneHandler::setAttribute(HepRepInstance *instance, G4String name, double value) {
    HepRepAttValue* attValue = instance->getAttValue(name);
    if ((attValue == NULL) || (attValue->getDouble() != value)) {
        HepRepAttribute* attribute = instance;
        // look for definition on type (node only)
        if (instance->getType()->getAttValueFromNode(name) == NULL) {
            attribute = instance->getType();
        }   
        
        attribute->addAttValue(name, value);
    }
}

void G4HepRepSceneHandler::setAttribute(HepRepInstance *instance, G4String name, int value) {
    HepRepAttValue* attValue = instance->getAttValue(name);
    if ((attValue == NULL) || (attValue->getInteger() != value)) {
        HepRepAttribute* attribute = instance;
        // look for definition on type (node only)
        if (instance->getType()->getAttValueFromNode(name) == NULL) {
            attribute = instance->getType();
        }   
        
        attribute->addAttValue(name, value);
    }
}

void G4HepRepSceneHandler::setAttribute(HepRepInstance *instance, G4String name, double red, double green, double blue, double alpha) {
    HepRepAttValue* attValue = instance->getAttValue(name);
    vector<double> color;
    if (attValue != NULL) color = attValue->getColor();
    if ((color.size() == 0) ||
        (color[0] != red) ||
        (color[1] != green) ||
        (color[2] != blue) ||
        ((color.size() > 3) && (color[3] != alpha))) {
        
        HepRepAttribute* attribute = instance;
        // look for definition on type (node only)
        if (instance->getType()->getAttValueFromNode(name) == NULL) {
            attribute = instance->getType();
        }   
        
        attribute->addAttValue(name, red, green, blue, alpha);
    }
}

void G4HepRepSceneHandler::addAttDefs(HepRepDefinition* definition, const map<G4String,G4AttDef>* attDefs) {
    if (attDefs == NULL) return;

    // Specify additional attribute definitions.  Assumes that all attributes for
    //  the given type can be found from the first instance of this type. GEANT-3
    map<G4String,G4AttDef>::const_iterator attDefIterator = attDefs->begin();
    while (attDefIterator != attDefs->end()) {
	    // Protect against incorrect use of Category.  Anything value other than the
	    // standard ones will be considered to be in the physics category.
	    G4String category = attDefIterator->second.GetCategory();
	    if ((category == "Draw") ||
    		(category == "Physics") ||
    		(category == "Association") ||
    		(category == "PickAction")) {

            category = "Physics";
        }
        definition->addAttDef(attDefIterator->first, attDefIterator->second.GetDesc(),
                        category, attDefIterator->second.GetExtra());
        attDefIterator++;
    }
}

void G4HepRepSceneHandler::addAttVals(HepRepInstance* instance, vector<G4AttValue>* attValues) {
    if (attValues == NULL) return;

    // Copy the instance's G4AttValues to HepRepAttValues.
    vector<G4AttValue>::iterator attValIterator;
    for (attValIterator = attValues->begin(); attValIterator != attValues->end(); attValIterator++) {
        // Use GetDesc rather than GetName once WIRED can handle names with spaces in them.
        //attribute->addAttValue(iAttDef->second.GetDesc(), iAttVal->GetValue());
        
        G4String name = attValIterator->GetName();

        // Pos already in points being written
        if (name == "Pos") continue;
        
        // NTP already in points being written
        if (name == "NTP") continue;
        
        // FIXME get type from attDef GEANT-12
        if ((name == "Ch") ||
            (name == "ID") ||
            (name == "NTP") ||
            (name == "PDG") ||
            (name == "PID")) {
            // Ch, ID, NTP, PDG and PID are of type int
            setAttribute(instance, attValIterator->GetName(), atoi(attValIterator->GetValue()));           
        } else {
            // rest is String           
            setAttribute(instance, attValIterator->GetName(), attValIterator->GetValue());
        }
    }
}


bool G4HepRepSceneHandler::isEventData () {
    return !currentPV || fReadyForTransients;
}

HepRep* G4HepRepSceneHandler::getHepRep() {
    if (_heprep == NULL) {
        // Create the HepRep that holds the Trees.
        _heprep = factory->createHepRep();
    }
    return _heprep;
}   

HepRepInstanceTree* G4HepRepSceneHandler::getGeometryInstanceTree() {
    if (_geometryInstanceTree == NULL) {
        // Create the Goemetry InstanceTree.
        _geometryInstanceTree = factory->createHepRepInstanceTree("G4GeometryData", "1.0", getGeometryTypeTree());
        getHepRep()->addInstanceTree(_geometryInstanceTree);
    }
    return _geometryInstanceTree;
}

HepRepInstance* G4HepRepSceneHandler::getGeometryRootInstance() {
    if (_geometryRootInstance == NULL) {
        // Create the top level Geometry Instance.
        _geometryRootInstance = factory->createHepRepInstance(getGeometryInstanceTree(), getGeometryRootType());
    }
    return _geometryRootInstance;
}

HepRepInstance* G4HepRepSceneHandler::getGeometryInstance(G4LogicalVolume* volume, int depth) {
    HepRepInstance* instance = getGeometryInstance(volume->GetName(), depth);

    setAttribute(instance, "LVol",     volume->GetName());
    setAttribute(instance, "Solid",    volume->GetSolid()->GetName());
    setAttribute(instance, "EType",    volume->GetSolid()->GetEntityType());
    setAttribute(instance, "Material", volume->GetMaterial()->GetName());
    setAttribute(instance, "Density",  volume->GetMaterial()->GetDensity());
    setAttribute(instance, "Radlen",   volume->GetMaterial()->GetRadlen());
    
    G4String state = materialState[volume->GetMaterial()->GetState()];
    setAttribute(instance, "State", state);
    
    return instance;
}

HepRepInstance* G4HepRepSceneHandler::getGeometryInstance(G4String volumeName, int depth) {
    // no extra checks since these are done in the geometryType already

    // adjust depth, also pop the current instance
    while ((int)_geometryInstance.size() > depth) {
        _geometryInstance.pop_back();
    }
    
    // get parent
    HepRepInstance* parent = (_geometryInstance.empty()) ? getGeometryRootInstance() : _geometryInstance.back();
    
    // get type
    HepRepType* type = getGeometryType(volumeName, depth);
    
    // create instance
    HepRepInstance* instance = factory->createHepRepInstance(parent, type);
    _geometryInstance.push_back(instance);

    return instance;
}

HepRepTypeTree* G4HepRepSceneHandler::getGeometryTypeTree() {
    if (_geometryTypeTree == NULL) {
        // Create the Geometry TypeTree.
        HepRepTreeID* geometryTreeID = factory->createHepRepTreeID("G4GeometryTypes", "1.0");
        _geometryTypeTree = factory->createHepRepTypeTree(geometryTreeID);
        getHepRep()->addTypeTree(_geometryTypeTree);
    }
    return _geometryTypeTree;
}

HepRepType* G4HepRepSceneHandler::getGeometryRootType() {
    if (_geometryRootType == NULL) {
        // Create the top level Geometry Type.
        _geometryRootType = factory->createHepRepType(NULL, rootVolumeName);
        _geometryRootType->addAttValue("Layer", geometryLayer);
    
        // Add attdefs used by all geometry types.
        _geometryRootType->addAttDef  ("LVol", "Logical Volume", "Physics","");
        _geometryRootType->addAttValue("LVol", "");
        _geometryRootType->addAttDef  ("Solid", "Solid Name", "Physics","");
        _geometryRootType->addAttValue("Solid", "");
        _geometryRootType->addAttDef  ("EType", "Entity Type", "Physics","");
        _geometryRootType->addAttValue("EType", "G4Box");
        _geometryRootType->addAttDef  ("Material", "Material Name", "Physics","");
        _geometryRootType->addAttValue("Material", "Air");
        _geometryRootType->addAttDef  ("Density", "Material Density", "Physics","");
        _geometryRootType->addAttValue("Density", 0.0);
        _geometryRootType->addAttDef  ("State", "Material State", "Physics","");
        _geometryRootType->addAttValue("State", "Gas");
        _geometryRootType->addAttDef  ("Radlen", "Material Radiation Length", "Physics","");
        _geometryRootType->addAttValue("Radlen", 0.0);
        
        // add defaults for Geometry
        _geometryRootType->addAttValue("Color", 0.8, 0.8, 0.8, 1.0);
        _geometryRootType->addAttValue("FillColor", 0.8, 0.8, 0.8, 1.0);
        _geometryRootType->addAttValue("LineWidth", 1.0);
        _geometryRootType->addAttValue("DrawAs", G4String("Polygon"));
        
        // remove definition for GEANT-13
        _geometryRootType->addAttDef  ("MarkSizeMultiplier",  "Multiplier for Marker Size", "Draw","");
        _geometryRootType->addAttValue("MarkSizeMultiplier", 1.0);  // Should be 4.0 for GEANT-13
        // remove definition for GEANT-13
        _geometryRootType->addAttDef  ("LineWidthMultiplier", "Multiplier for Line Width", "Draw","");
        _geometryRootType->addAttValue("LineWidthMultiplier", 1.0);
        
        getGeometryTypeTree()->addType(_geometryRootType);
        
        _geometryType["/"+_geometryRootType->getName()] = _geometryRootType;
    }
    return _geometryRootType;
}

HepRepType* G4HepRepSceneHandler::getGeometryType(G4String volumeName, int depth) {
    // make sure we have a root
    getGeometryRootType();   

    // construct the full name for this volume
    G4String name = getFullTypeName(volumeName, depth);
    
    // lookup type and create if necessary
    HepRepType* type = _geometryType[name];    
    if (type == NULL) {
        G4String parentName = getParentTypeName(depth);
        HepRepType* parentType = _geometryType[parentName];
        // FIXME should have short path element name...(volumeName)
        type = factory->createHepRepType(parentType, name);
        _geometryType[name] = type;
    }
    return type;   
}

G4String G4HepRepSceneHandler::getFullTypeName(G4String volumeName, int depth) {
    // check for name depth
    if (depth > (int)_geometryTypeName.size()) {
        // there is a problem, book this type under problems
        G4String problem = "HierarchyProblem";
        if (_geometryType["/"+problem] == NULL) {
            // FIXME should have short path element name...
            HepRepType* type = factory->createHepRepType(getGeometryRootType(), "/"+problem);
            _geometryType["/"+problem] = type;
        }
        return "/" + problem + "/" + volumeName;
    }
    
    // adjust name depth, also pop the current volumeName
    while ((int)_geometryTypeName.size() > depth) {
        _geometryTypeName.pop_back();
    }
    
    // construct full name and push it
    G4String name = (_geometryTypeName.empty()) ? G4String("/"+rootVolumeName) : _geometryTypeName.back();
    name = name + "/" + volumeName;
    _geometryTypeName.push_back(name);
    return name;
}

G4String G4HepRepSceneHandler::getParentTypeName(int depth) {
    return (depth >= 1) ? _geometryTypeName[depth-1] : G4String("/"+rootVolumeName);
}       

HepRepInstanceTree* G4HepRepSceneHandler::getEventInstanceTree() {
    if (_eventInstanceTree == NULL) {
        // Create the Event InstanceTree.
        _eventInstanceTree = factory->createHepRepInstanceTree("G4EventData", "1.0", getEventTypeTree());
        getHepRep()->addInstanceTree(_eventInstanceTree);
    }
    return _eventInstanceTree;
}

HepRepInstance* G4HepRepSceneHandler::getEventInstance() {
    if (_eventInstance == NULL) {
        // Create the top level Event Instance.
        _eventInstance = factory->createHepRepInstance(getEventInstanceTree(), getEventType());
    }
    return _eventInstance;
}

HepRepTypeTree* G4HepRepSceneHandler::getEventTypeTree() {
    if (_eventTypeTree == NULL) {
        // Create the Event TypeTree.
        HepRepTreeID* eventTreeID = factory->createHepRepTreeID("G4EventTypes", "1.0");
        _eventTypeTree = factory->createHepRepTypeTree(eventTreeID);
        getHepRep()->addTypeTree(_eventTypeTree);
    }
    return _eventTypeTree;
}

HepRepType* G4HepRepSceneHandler::getEventType() {
    if (_eventType == NULL) {
        // Create the top level Event Type.
        _eventType = factory->createHepRepType(NULL, "Event");
        _eventType->addAttValue("Layer", eventLayer);

        // add defaults for Events
        _eventType->addAttValue("Color", 1.0, 1.0, 1.0, 1.0);
        _eventType->addAttValue("FillColor", 1.0, 1.0, 1.0, 1.0);
        _eventType->addAttValue("LineWidth", 1.0);
        _eventType->addAttValue("HasFrame", true);

        // remove definition for GEANT-13
        _eventType->addAttDef  ("MarkSizeMultiplier",  "Multiplier for Marker Size", "Draw","");
        _eventType->addAttValue("MarkSizeMultiplier", 1.0);  // Should be 4.0 for GEANT-13
        // remove definition for GEANT-13
        _eventType->addAttDef  ("LineWidthMultiplier", "Multiplier for Line Width", "Draw","");
        _eventType->addAttValue("LineWidthMultiplier", 1.0);
            
        getEventTypeTree()->addType(_eventType);
    }
    return _eventType;
}

HepRepType* G4HepRepSceneHandler::getTrajectoryType(const map<G4String,G4AttDef>* attDefs) {
    if (_trajectoryType == NULL) {
        _trajectoryType = factory->createHepRepType(getEventType(), "Trajectory");
        
        _trajectoryType->addAttValue("Ch", 0);
        // FIXME should call color converter
        _trajectoryType->addAttValue("Color", 0.0, 1.0, 0.0, 1.0);  // green
        _trajectoryType->addAttValue("ID", -1);
        _trajectoryType->addAttValue("IMom", "");
        _trajectoryType->addAttValue("PDG", -1);
        _trajectoryType->addAttValue("PN", "");
        _trajectoryType->addAttValue("PID", -1);

        _trajectoryType->addAttValue("Layer", trajectoryLayer);
        _trajectoryType->addAttValue("DrawAs", G4String("Line"));

        _trajectoryType->addAttValue("LineWidthMultiplier", 1.0);   // Change to 2.0 for GEANT-13

        addAttDefs(_trajectoryType, attDefs);
    }
    return _trajectoryType;
}

HepRepType* G4HepRepSceneHandler::getTrajectoryPointType(const map<G4String,G4AttDef>* attDefs) {
    if (_trajectoryPointType == NULL) {
        _trajectoryPointType = factory->createHepRepType(getTrajectoryType(), "Trajectory/Point");
        _trajectoryPointType->addAttValue("Layer", trajectoryPointLayer);
        _trajectoryPointType->addAttValue("DrawAs", G4String("Point"));
        _trajectoryPointType->addAttValue("MarkName", G4String("Box"));
        _trajectoryPointType->addAttValue("MarkSize", 4);
        _trajectoryPointType->addAttValue("MarkType", G4String("Symbol"));
        _trajectoryPointType->addAttValue("Fill", true);
        _trajectoryPointType->addAttValue("Visibility", true);

        addAttDefs(_trajectoryPointType, attDefs);
    }
    return _trajectoryPointType;
}

HepRepType* G4HepRepSceneHandler::getHitType(const map<G4String,G4AttDef>* attDefs) {
    if (_hitType == NULL) {
        _hitType = factory->createHepRepType(getEventType(), "Hit");
        _hitType->addAttValue("Layer", hitLayer);
        _hitType->addAttValue("DrawAs", G4String("Point"));
        _hitType->addAttValue("MarkName", G4String("Box"));
        _hitType->addAttValue("MarkSize", 4.0);
        _hitType->addAttValue("MarkType", G4String("Symbol"));
        _hitType->addAttValue("Fill", true);

        addAttDefs(_hitType, attDefs);
    }
    return _hitType;
}

HepRepType* G4HepRepSceneHandler::getCalHitType(const map<G4String,G4AttDef>* attDefs) {
    if (_calHitType == NULL) {
        _calHitType = factory->createHepRepType(getEventType(), "CalHit");
        _calHitType->addAttValue("Layer", calHitLayer);
        _calHitType->addAttValue("Fill", true);
        _calHitType->addAttValue("DrawAs", "Polygon");

        addAttDefs(_calHitType, attDefs);
    }
    return _calHitType;
}

