// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4StateManager.cc,v 1.3 2001-03-06 15:56:52 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// ------------------------------------------------------------
//      GEANT 4 class implementation file 
//
//      For information related to this code contact:
//      Geant4 Collaboration
//      ---------------- G4StateManager ----------------
//             by Gabriele Cosmo, November 1996
// ------------------------------------------------------------

#include "G4StateManager.hh"

// Initialization of the static pointer of the single class instance
G4StateManager* G4StateManager::theStateManager = 0;

G4StateManager::G4StateManager()
 : theCurrentState(PreInit),
   thePreviousState(PreInit),
   theBottomDependent(0)
{
}

G4StateManager::~G4StateManager()
{
  G4VStateDependent* state=0;

  while (theDependentsList.size()>0)
  {
    state = theDependentsList.back();
    theDependentsList.pop_back();
    for (G4std::vector<G4VStateDependent*>::iterator
         i=theDependentsList.begin(); i!=theDependentsList.end(); i++)
    {
      if (*i==state)
      {
	theDependentsList.erase(i);
	i--;
      }
    } 
    if ( state ) delete state;    
  } 
}

G4StateManager::G4StateManager(const G4StateManager &right)
  : theCurrentState(right.theCurrentState),
    thePreviousState(right.thePreviousState),
    theDependentsList(right.theDependentsList),
    theBottomDependent(right.theBottomDependent)
{
}

G4StateManager&
G4StateManager::operator=(const G4StateManager &right)
{
   if (&right == this) return *this;

   theCurrentState = right.theCurrentState;
   thePreviousState = right.thePreviousState;
   theDependentsList = right.theDependentsList;
   theBottomDependent = right.theBottomDependent;

   return *this;
}

G4int
G4StateManager::operator==(const G4StateManager &right) const
{
   return (this == &right);
}

G4int
G4StateManager::operator!=(const G4StateManager &right) const
{
   return (this != &right);
}


G4StateManager*
G4StateManager::GetStateManager()
{
    if (!theStateManager)
    {
      theStateManager = new G4StateManager;
    }
    return theStateManager;    
}

G4bool
G4StateManager::RegisterDependent(G4VStateDependent* aDependent, G4bool bottom)
{
    G4bool ack=true;
    if(!bottom)
    {
      theDependentsList.push_back(aDependent);
    }
    else
    { 
      if(theBottomDependent)
      {
        theDependentsList.push_back(theBottomDependent);
      }
      theBottomDependent = aDependent;
    }
    return ack;
}

G4bool
G4StateManager::DeregisterDependent(G4VStateDependent* aDependent)
{
  G4VStateDependent* tmp = 0;
  G4std::vector<G4VStateDependent*>::iterator i;
  for (i=theDependentsList.begin(); i!=theDependentsList.end(); i++)
    {
      if (**i==*aDependent) 
	{
	  tmp = *i;
	  theDependentsList.erase(i);
	} 
    }
  return (tmp != 0);
}

G4ApplicationState
G4StateManager::GetCurrentState() const
{
   return theCurrentState;
}

G4ApplicationState
G4StateManager::GetPreviousState() const
{
   return thePreviousState;
}

G4bool
G4StateManager::SetNewState(G4ApplicationState requestedState)
{
   size_t i=0;
   G4bool ack = true;
   G4ApplicationState savedState = thePreviousState;
   thePreviousState = theCurrentState;
   theCurrentState = requestedState;
   while ((ack) && (i<theDependentsList.size()))
   {
     ack = theDependentsList[i]->Notify(requestedState);
     i++;
   }
   if(theBottomDependent)
   {
     ack = theBottomDependent->Notify(requestedState);
   }

   if(!ack)
   {
     theCurrentState = thePreviousState;
     thePreviousState = savedState;
   }
   return ack;
}

G4VStateDependent*
G4StateManager::RemoveDependent(const G4VStateDependent* aDependent)
{
  G4VStateDependent* tmp = 0;
  G4std::vector<G4VStateDependent*>::iterator i;
  for (i=theDependentsList.begin(); i!=theDependentsList.end(); i++)
    {
      if (**i==*aDependent) 
	{
	  tmp = *i;
	  theDependentsList.erase(i);
	} 
    }
  return tmp;
}

G4String
G4StateManager::GetStateString(G4ApplicationState aState) const
{
  G4String stateName;
  switch(aState)
  {
    case PreInit:
     stateName = "PreInit"; break;
    case Init:
     stateName = "Init"; break;
    case Idle:
     stateName = "Idle"; break;
    case GeomClosed:
     stateName = "GeomClosed"; break;
    case EventProc:
     stateName = "EventProc"; break;
    case Quit:
     stateName = "Quit"; break;
    case Abort:
     stateName = "Abort"; break;
    default:
     stateName = "Unknown";
  }
  return stateName;
}

//void G4StateManager::Pause()
//{
//  Pause("G4_pause> ");
//}
//
//void G4StateManager::Pause(const char* msg)
//{
//  G4String msgS = msg;
//  Pause(msgS);
//}
//
//void G4StateManager::Pause(G4String msg)
//{
//  G4UImanager::GetUIpointer()->PauseSession(msg);
//}
