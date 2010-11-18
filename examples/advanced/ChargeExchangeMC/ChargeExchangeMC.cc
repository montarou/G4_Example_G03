/*
 * =============================================================================
 *
 *       Filename:  cexmc.cc
 *
 *    Description:  main
 *
 *        Version:  1.0
 *        Created:  10.10.2009 23:24:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Alexey Radkov (), 
 *        Company:  PNPI
 *
 * =============================================================================
 */

#include <set>
#include <boost/algorithm/string.hpp>
#include <boost/archive/archive_exception.hpp>
#ifdef CEXMC_USE_CUSTOM_FILTER
#include <boost/variant/get.hpp>
#endif
#include <G4Version.hh>
#include <G4UImanager.hh>
#include <G4String.hh>
#ifdef G4UI_USE
#include <G4UIsession.hh>
#include <G4UIterminal.hh>
#ifdef G4UI_USE_TCSH
#include <G4UItcsh.hh>
#endif
#ifdef G4UI_USE_QT
#include <G4UIQt.hh>
#endif
#endif
#ifdef G4VIS_USE
#include <G4VisExecutive.hh>
#endif
#include "CexmcRunManager.hh"
#ifdef CEXMC_USE_ROOT
#include "CexmcHistoManager.hh"
#endif
#include "CexmcSetup.hh"
#include "CexmcPhysicsList.hh"
#include "CexmcPhysicsManager.hh"
#include "CexmcPrimaryGeneratorAction.hh"
#include "CexmcTrackingAction.hh"
#include "CexmcSteppingAction.hh"
#include "CexmcEventAction.hh"
#include "CexmcRunAction.hh"
#include "CexmcMessenger.hh"
#include "CexmcException.hh"
#include "CexmcBasicPhysicsSettings.hh"
#include "CexmcCommon.hh"


namespace
{
    const G4String  CexmcVisManagerVerboseLevel( "errors" );
}


struct  CexmcCmdLineData
{
    CexmcCmdLineData() : isInteractive( false ), startQtSession( false ),
                         preinitMacro( "" ), initMacro( "" ), rProject( "" ),
                         wProject( "" ), overrideExistingProject( false ),
                         customFilter( "" )
    {}

    G4bool                  isInteractive;
    G4bool                  startQtSession;
    G4String                preinitMacro;
    G4String                initMacro;
    G4String                rProject;
    G4String                wProject;
    G4bool                  overrideExistingProject;
    CexmcOutputDataTypeSet  outputData;
    G4String                customFilter;
};


void  printUsage( void )
{
    G4cout << "Usage: cexmc [-i] "
#ifdef G4UI_USE_QT
                           "[-g] "
#endif
                           "[-p preinit_macro] [-m init_macro] "
                           "[[-y] -w project]" << G4endl <<
              "             [-r project "
#ifdef CEXMC_USE_CUSTOM_FILTER
                           "[-f filter_script] "
#endif
                           "[-o list]]" << G4endl;
    G4cout << "or     cexmc [--help | -h]" << G4endl;
    G4cout << "           -i - run in interactive mode" << G4endl;
#ifdef G4UI_USE_QT
    G4cout << "           -g - start graphical interface (Qt), implies "
                              "interactive mode " << G4endl;
#endif
    G4cout << "           -p - use specified preinit macro file " << G4endl;
    G4cout << "           -m - use specified init macro file " << G4endl;
    G4cout << "           -w - save data in specified project files" << G4endl;
    G4cout << "           -r - read data from specified project files" <<
              G4endl;
#ifdef CEXMC_USE_CUSTOM_FILTER
    G4cout << "           -f - use specified custom filter script" << G4endl;
#endif
    G4cout << "           -o - comma-separated list of data to output, "
                              "possible values:" << G4endl <<
              "                run, geom, events" << G4endl;
    G4cout << "           -y - force project override" << G4endl;
    G4cout << "  --help | -h - print this message and exit " << G4endl;
}


G4bool  parseArgs( int  argc, char ** argv, CexmcCmdLineData &  cmdLineData )
{
    if ( argc < 2 )
        return false;

    for ( G4int  i( 1 ); i < argc; ++i )
    {
        do
        {
            if ( G4String( argv[ i ] ) == "--help" )
            {
                return false;
            }
            if ( G4String( argv[ i ] ) == "-h" )
            {
                return false;
            }
            if ( G4String( argv[ i ], 2 ) == "-i" )
            {
                cmdLineData.isInteractive = true;
                break;
            }
#ifdef G4UI_USE_QT
            if ( G4String( argv[ i ], 2 ) == "-g" )
            {
                cmdLineData.isInteractive = true;
                cmdLineData.startQtSession = true;
                break;
            }
#endif
            if ( G4String( argv[ i ], 2 ) == "-p" )
            {
                cmdLineData.preinitMacro = argv[ i ] + 2;
                if ( cmdLineData.preinitMacro == "" )
                {
                    if ( ++i >= argc )
                        throw CexmcException( CexmcCmdLineParseException );
                    cmdLineData.preinitMacro = argv[ i ];
                }
                break;
            }
            if ( G4String( argv[ i ], 2 ) == "-m" )
            {
                cmdLineData.initMacro = argv[ i ] + 2;
                if ( cmdLineData.initMacro == "" )
                {
                    if ( ++i >= argc )
                        throw CexmcException( CexmcCmdLineParseException );
                    cmdLineData.initMacro = argv[ i ];
                }
                break;
            }
            if ( G4String( argv[ i ], 2 ) == "-w" )
            {
                cmdLineData.wProject = argv[ i ] + 2;
                if ( cmdLineData.wProject == "" )
                {
                    if ( ++i >= argc )
                        throw CexmcException( CexmcCmdLineParseException );
                    cmdLineData.wProject = argv[ i ];
                }
                break;
            }
            if ( G4String( argv[ i ], 2 ) == "-r" )
            {
                cmdLineData.rProject = argv[ i ] + 2;
                if ( cmdLineData.rProject == "" )
                {
                    if ( ++i >= argc )
                        throw CexmcException( CexmcCmdLineParseException );
                    cmdLineData.rProject = argv[ i ];
                }
                break;
            }
            if ( G4String( argv[ i ], 2 ) == "-y" )
            {
                cmdLineData.overrideExistingProject = true;
                break;
            }
            if ( G4String( argv[ i ], 2 ) == "-o" )
            {
                std::string  outputData( argv[ i ] + 2 );
                if ( outputData == "" )
                {
                    if ( ++i >= argc )
                        throw CexmcException( CexmcCmdLineParseException );
                    outputData = argv[ i ];
                }
                std::set< std::string >  tokens;
                boost::split( tokens, outputData, boost::is_any_of( "," ) );
                for ( std::set< std::string >::iterator  k( tokens.begin() );
                                                        k != tokens.end(); ++k )
                {
                    do
                    {
                        if ( *k == "run" )
                        {
                            cmdLineData.outputData.insert( CexmcOutputRun );
                            break;
                        }
                        if ( *k == "geom" )
                        {
                            cmdLineData.outputData.insert(
                                                          CexmcOutputGeometry );
                            break;
                        }
                        if ( *k == "events" )
                        {
                            cmdLineData.outputData.insert( CexmcOutputEvents );
                            break;
                        }
                        throw CexmcException( CexmcCmdLineParseException );
                    } while ( false );
                }
                break;
            }
#ifdef CEXMC_USE_CUSTOM_FILTER
            if ( G4String( argv[ i ], 2 ) == "-f" )
            {
                cmdLineData.customFilter = argv[ i ] + 2;
                if ( cmdLineData.customFilter == "" )
                {
                    if ( ++i >= argc )
                        throw CexmcException( CexmcCmdLineParseException );
                    cmdLineData.customFilter = argv[ i ];
                }
                break;
            }
#endif

            throw CexmcException( CexmcCmdLineParseException );

        } while ( false );
    }

    return true;
}


int  main( int  argc, char **  argv )
{
#ifdef G4UI_USE
    G4UIsession *  session( NULL );
#endif

    CexmcCmdLineData  cmdLineData;
    G4bool            outputDataOnly( false );

    try
    {
        if ( ! parseArgs( argc, argv, cmdLineData ) )
        {
            printUsage();
            return 0;
        }
        if ( cmdLineData.rProject != "" &&
             cmdLineData.rProject == cmdLineData.wProject )
            throw CexmcException( CexmcCmdLineParseException );
        if ( cmdLineData.rProject == "" && ! cmdLineData.outputData.empty() )
            throw CexmcException( CexmcCmdLineParseException );
#ifdef CEXMC_USE_CUSTOM_FILTER
        if ( cmdLineData.rProject == "" && ! cmdLineData.customFilter.empty() )
            throw CexmcException( CexmcCmdLineParseException );
#endif
        if ( cmdLineData.wProject != "" && ! cmdLineData.outputData.empty() )
            throw CexmcException( CexmcCmdLineParseException );
        outputDataOnly = ! cmdLineData.outputData.empty();
    }
    catch ( CexmcException &  e )
    {
        G4cout << e.what() << G4endl;
        return 1;
    }
    catch ( ... )
    {
        G4cout << "Unknown exception caught when parsing args" << G4endl;
        return 1;
    }

    CexmcRunManager *  runManager( NULL );
#ifdef G4VIS_USE
    G4VisManager *     visManager( NULL );
#endif

    CexmcMessenger::Instance();
#ifdef CEXMC_USE_ROOT
    CexmcHistoManager::Instance();
#endif

    try
    {
        runManager = new CexmcRunManager( cmdLineData.wProject,
                                          cmdLineData.rProject,
                                          cmdLineData.overrideExistingProject );
#ifdef CEXMC_USE_CUSTOM_FILTER
        runManager->SetCustomFilter( cmdLineData.customFilter );
#endif

        if ( outputDataOnly )
        {
            /* we will need an arbitrary physics list to get access to particle
             * table if events output was ordered */
            CexmcOutputDataTypeSet::const_iterator  found(
                            cmdLineData.outputData.find( CexmcOutputEvents ) );
            if ( found != cmdLineData.outputData.end() )
                runManager->SetUserInitialization(
                                CexmcChargeExchangePMFactory::
                                            Create( CexmcPionZeroProduction ) );
            runManager->PrintReadData( cmdLineData.outputData );
            delete runManager;
            return 0;
        }

        G4UImanager *  uiManager( G4UImanager::GetUIpointer() );

        if ( cmdLineData.preinitMacro != "" )
            uiManager->ApplyCommand( "/control/execute " +
                                     cmdLineData.preinitMacro );

        CexmcProductionModelType  productionModelType(
                                        runManager->GetProductionModelType() );

        if ( productionModelType == CexmcUnknownProductionModel )
            throw CexmcException( CexmcPreinitException );

        G4VUserPhysicsList *    physicsList( CexmcChargeExchangePMFactory::
                                                Create( productionModelType ) );
        CexmcPhysicsManager *   physicsManager(
                        dynamic_cast< CexmcPhysicsManager * >( physicsList ) );
        CexmcProductionModel *  productionModel(
                                        physicsManager->GetProductionModel() );

        if ( ! productionModel )
            throw CexmcException( CexmcWeirdException );

        G4cout << CEXMC_LINE_START << "Production model '" <<
                productionModel->GetName() << "' instantiated" << G4endl;

        runManager->SetUserInitialization( physicsList );

        CexmcSetup *   setup( new CexmcSetup( runManager->GetGdmlFileName(),
                                    runManager->ShouldGdmlFileBeValidated() ) );

        runManager->SetUserInitialization( setup );

        runManager->Initialize();

        runManager->SetPhysicsManager( physicsManager );

        runManager->SetUserAction( new CexmcPrimaryGeneratorAction(
                                                            physicsManager ) );

        runManager->SetUserAction( new CexmcEventAction( physicsManager ) );

        runManager->SetUserAction( new CexmcRunAction( physicsManager ) );

        runManager->SetUserAction( new CexmcTrackingAction( physicsManager ) );

        runManager->SetUserAction( new CexmcSteppingAction( physicsManager ) );

#ifdef CEXMC_USE_ROOT
        CexmcHistoManager::Instance()->Initialize();
#endif

#ifdef G4VIS_USE
        if ( cmdLineData.isInteractive )
        {
#if G4VERSION_NUMBER < 940
            visManager = new G4VisExecutive;
#else
            visManager = new G4VisExecutive( CexmcVisManagerVerboseLevel );
#endif
            visManager->Initialize();
        }
#endif

        if ( runManager->ProjectIsRead() )
        {
            runManager->ReadProject();
            runManager->PrintReadRunData();
        }

        if ( cmdLineData.initMacro != "" )
            uiManager->ApplyCommand( "/control/execute " +
                                     cmdLineData.initMacro );

        if ( cmdLineData.isInteractive )
        {
            productionModel->PrintInitialData();
        }

#ifdef G4UI_USE
        if ( cmdLineData.isInteractive )
        {
            if ( cmdLineData.startQtSession )
            {
#ifdef G4UI_USE_QT
                session = new G4UIQt( argc, argv );
                const G4String &  guiMacroName( runManager->GetGuiMacroName() );
                if ( guiMacroName != "" )
                    uiManager->ApplyCommand( "/control/execute " +
                                             guiMacroName );
#ifdef CEXMC_USE_ROOTQT
                runManager->EnableLiveHistograms();
#endif
#endif
            }
            else
            {
#ifdef G4UI_USE_TCSH
                session = new G4UIterminal( new G4UItcsh );
#else
                session = new G4UIterminal;
#endif
            }
            if ( session )
                session->SessionStart();
        }
#endif

        if ( runManager->ProjectIsSaved() )
        {
            runManager->SaveProject();
        }
    }
    catch ( CexmcException &  e )
    {
        G4cout << e.what() << G4endl;
    }
    catch ( boost::archive::archive_exception &  e )
    {
        G4cout << CEXMC_LINE_START << "Serialization error: " << e.what() <<
                  G4endl;
    }
#ifdef CEXMC_USE_CUSTOM_FILTER
    catch ( boost::bad_get &  e )
    {
        G4cout << CEXMC_LINE_START << "Custom filter error: " << e.what() <<
                  G4endl;
    }
#endif
    catch ( ... )
    {
        G4cout << "Unknown exception caught" << G4endl;
    }

#ifdef CEXMC_USE_ROOT
    CexmcHistoManager::Destroy();
#endif
    CexmcMessenger::Destroy();

#ifdef G4VIS_USE
    delete visManager;
#endif
    delete runManager;
#ifdef G4UI_USE
    delete session;
#endif

    return 0;
}
