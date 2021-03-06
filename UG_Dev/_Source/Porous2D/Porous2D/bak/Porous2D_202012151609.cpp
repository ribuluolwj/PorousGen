//==============================================================================
//  WARNING!!  This file is overwritten by the Block UI Styler while generating
//  the automation code. Any modifications to this file will be lost after
//  generating the code again.
//
//       Filename:  G:\RPWorkspace\PorousGen\UG_Dev\Application\Porous2D.cpp
//
//        This file was generated by the NX Block UI Styler
//        Created by: RenPeng
//              Version: NX 1919
//              Date: 12-12-2020  (Format: mm-dd-yyyy)
//              Time: 18:02 (Format: hh-mm)
//
//==============================================================================

//==============================================================================
//  Purpose:  This TEMPLATE file contains C++ source to guide you in the
//  construction of your Block application dialog. The generation of your
//  dialog file (.dlx extension) is the first step towards dialog construction
//  within NX.  You must now create a NX Open application that
//  utilizes this file (.dlx).
//
//  The information in this file provides you with the following:
//
//  1.  Help on how to load and display your Block UI Styler dialog in NX
//      using APIs provided in NXOpen.BlockStyler namespace
//  2.  The empty callback methods (stubs) associated with your dialog items
//      have also been placed in this file. These empty methods have been
//      created simply to start you along with your coding requirements.
//      The method name, argument list and possible return values have already
//      been provided for you.
//==============================================================================

//------------------------------------------------------------------------------
//These includes are needed for the following template code
//------------------------------------------------------------------------------
#include "Porous2D.hpp"
//#include "PorousLib.hpp"
using namespace NXOpen;
using namespace NXOpen::BlockStyler;

//------------------------------------------------------------------------------
// Initialize static variables
//------------------------------------------------------------------------------
Session *(Porous2D::theSession) = NULL;
UI *(Porous2D::theUI) = NULL;
//------------------------------------------------------------------------------
// Constructor for NX Styler class
//------------------------------------------------------------------------------
Porous2D::Porous2D()
{
    try
    {
        // Initialize the NX Open C++ API environment
        Porous2D::theSession = NXOpen::Session::GetSession();
        Porous2D::theUI = UI::GetUI();
        theDlxFileName = "Porous2D.dlx";
        theDialog = Porous2D::theUI->CreateDialog(theDlxFileName);
        // Registration of callback functions
        theDialog->AddApplyHandler(make_callback(this, &Porous2D::apply_cb));
        theDialog->AddOkHandler(make_callback(this, &Porous2D::ok_cb));
        theDialog->AddUpdateHandler(make_callback(this, &Porous2D::update_cb));
        theDialog->AddInitializeHandler(make_callback(this, &Porous2D::initialize_cb));
        theDialog->AddDialogShownHandler(make_callback(this, &Porous2D::dialogShown_cb));
    }
    catch(exception& ex)
    {
        //---- Enter your exception handling code here -----
        throw;
    }
}

//------------------------------------------------------------------------------
// Destructor for NX Styler class
//------------------------------------------------------------------------------
Porous2D::~Porous2D()
{
    if (theDialog != NULL)
    {
        delete theDialog;
        theDialog = NULL;
    }
}
//------------------------------- DIALOG LAUNCHING ---------------------------------
//
//    Before invoking this application one needs to open any part/empty part in NX
//    because of the behavior of the blocks.
//
//    Make sure the dlx file is in one of the following locations:
//        1.) From where NX session is launched
//        2.) $UGII_USER_DIR/application
//        3.) For released applications, using UGII_CUSTOM_DIRECTORY_FILE is highly
//            recommended. This variable is set to a full directory path to a file 
//            containing a list of root directories for all custom applications.
//            e.g., UGII_CUSTOM_DIRECTORY_FILE=$UGII_BASE_DIR\ugii\menus\custom_dirs.dat
//
//    You can create the dialog using one of the following way:
//
//    1. USER EXIT
//
//        1) Create the Shared Library -- Refer "Block UI Styler programmer's guide"
//        2) Invoke the Shared Library through File->Execute->NX Open menu.
//
//------------------------------------------------------------------------------
extern "C" DllExport void  ufusr(char *param, int *retcod, int param_len)
{
    Porous2D *thePorous2D = NULL;
    try
    {
        thePorous2D = new Porous2D();
        // The following method shows the dialog immediately
        thePorous2D->Show();
    }
    catch(exception& ex)
    {
        //---- Enter your exception handling code here -----
        Porous2D::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
    if(thePorous2D != NULL)
    {
        delete thePorous2D;
        thePorous2D = NULL;
    }
}

//------------------------------------------------------------------------------
// This method specifies how a shared image is unloaded from memory
// within NX. This method gives you the capability to unload an
// internal NX Open application or user  exit from NX. Specify any
// one of the three constants as a return value to determine the type
// of unload to perform:
//
//
//    Immediately : unload the library as soon as the automation program has completed
//    Explicitly  : unload the library from the "Unload Shared Image" dialog
//    AtTermination : unload the library when the NX session terminates
//
//
// NOTE:  A program which associates NX Open applications with the menubar
// MUST NOT use this option since it will UNLOAD your NX Open application image
// from the menubar.
//------------------------------------------------------------------------------
extern "C" DllExport int ufusr_ask_unload()
{
    //return (int)Session::LibraryUnloadOptionExplicitly;
    return (int)Session::LibraryUnloadOptionImmediately;
    //return (int)Session::LibraryUnloadOptionAtTermination;
}

//------------------------------------------------------------------------------
// Following method cleanup any housekeeping chores that may be needed.
// This method is automatically called by NX.
//------------------------------------------------------------------------------
extern "C" DllExport void ufusr_cleanup(void)
{
    try
    {
        //---- Enter your callback code here -----
    }
    catch(exception& ex)
    {
        //---- Enter your exception handling code here -----
        Porous2D::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
}

int Porous2D::Show()
{
    try
    {
        theDialog->Show();
    }
    catch(exception& ex)
    {
        //---- Enter your exception handling code here -----
        Porous2D::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
    return 0;
}

//------------------------------------------------------------------------------
//---------------------Block UI Styler Callback Functions--------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//Callback Name: initialize_cb
//------------------------------------------------------------------------------
void Porous2D::initialize_cb()
{
    try
    {
        groupSheetStartPoint = dynamic_cast<NXOpen::BlockStyler::Group*>(theDialog->TopBlock()->FindBlock("groupSheetStartPoint"));
        pointSheetStart = dynamic_cast<NXOpen::BlockStyler::SpecifyPoint*>(theDialog->TopBlock()->FindBlock("pointSheetStart"));
        groupShape = dynamic_cast<NXOpen::BlockStyler::Group*>(theDialog->TopBlock()->FindBlock("groupShape"));
        enumParticleShape = dynamic_cast<NXOpen::BlockStyler::Enumeration*>(theDialog->TopBlock()->FindBlock("enumParticleShape"));
        groupGenMethod = dynamic_cast<NXOpen::BlockStyler::Group*>(theDialog->TopBlock()->FindBlock("groupGenMethod"));
        enumGenMethod = dynamic_cast<NXOpen::BlockStyler::Enumeration*>(theDialog->TopBlock()->FindBlock("enumGenMethod"));
        groupSize = dynamic_cast<NXOpen::BlockStyler::Group*>(theDialog->TopBlock()->FindBlock("groupSize"));
        expressionFaceLength = dynamic_cast<NXOpen::BlockStyler::ExpressionBlock*>(theDialog->TopBlock()->FindBlock("expressionFaceLength"));
        expressionFaceWidth = dynamic_cast<NXOpen::BlockStyler::ExpressionBlock*>(theDialog->TopBlock()->FindBlock("expressionFaceWidth"));
        expressionParticleSize = dynamic_cast<NXOpen::BlockStyler::ExpressionBlock*>(theDialog->TopBlock()->FindBlock("expressionParticleSize"));
        groupParticle = dynamic_cast<NXOpen::BlockStyler::Group*>(theDialog->TopBlock()->FindBlock("groupParticle"));
        tabControl = dynamic_cast<NXOpen::BlockStyler::TabControl*>(theDialog->TopBlock()->FindBlock("tabControl"));
        tabPage = dynamic_cast<NXOpen::BlockStyler::Group*>(theDialog->TopBlock()->FindBlock("tabPage"));
        integerParticleNumber = dynamic_cast<NXOpen::BlockStyler::IntegerBlock*>(theDialog->TopBlock()->FindBlock("integerParticleNumber"));
        doublePorousRatio = dynamic_cast<NXOpen::BlockStyler::DoubleBlock*>(theDialog->TopBlock()->FindBlock("doublePorousRatio"));
        toggleUsePorousRatio = dynamic_cast<NXOpen::BlockStyler::Toggle*>(theDialog->TopBlock()->FindBlock("toggleUsePorousRatio"));
        tabPageVoronoi = dynamic_cast<NXOpen::BlockStyler::Group*>(theDialog->TopBlock()->FindBlock("tabPageVoronoi"));
        integerBSplineOrder = dynamic_cast<NXOpen::BlockStyler::IntegerBlock*>(theDialog->TopBlock()->FindBlock("integerBSplineOrder"));
        integerScaleRatio = dynamic_cast<NXOpen::BlockStyler::IntegerBlock*>(theDialog->TopBlock()->FindBlock("integerScaleRatio"));
        toggleScale = dynamic_cast<NXOpen::BlockStyler::Toggle*>(theDialog->TopBlock()->FindBlock("toggleScale"));
        tabPage1 = dynamic_cast<NXOpen::BlockStyler::Group*>(theDialog->TopBlock()->FindBlock("tabPage1"));
        expressionRowGap = dynamic_cast<NXOpen::BlockStyler::ExpressionBlock*>(theDialog->TopBlock()->FindBlock("expressionRowGap"));
        expressionColumGap = dynamic_cast<NXOpen::BlockStyler::ExpressionBlock*>(theDialog->TopBlock()->FindBlock("expressionColumGap"));
        group = dynamic_cast<NXOpen::BlockStyler::Group*>(theDialog->TopBlock()->FindBlock("group"));
        buttonGenSheet = dynamic_cast<NXOpen::BlockStyler::Button*>(theDialog->TopBlock()->FindBlock("buttonGenSheet"));
        buttonGenParticle = dynamic_cast<NXOpen::BlockStyler::Button*>(theDialog->TopBlock()->FindBlock("buttonGenParticle"));
        buttonTrimParticle = dynamic_cast<NXOpen::BlockStyler::Button*>(theDialog->TopBlock()->FindBlock("buttonTrimParticle"));
        buttonOnekeyGen = dynamic_cast<NXOpen::BlockStyler::Button*>(theDialog->TopBlock()->FindBlock("buttonOnekeyGen"));
    }
    catch(exception& ex)
    {
        //---- Enter your exception handling code here -----
        Porous2D::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
}

//------------------------------------------------------------------------------
//Callback Name: dialogShown_cb
//This callback is executed just before the dialog launch. Thus any value set 
//here will take precedence and dialog will be launched showing that value. 
//------------------------------------------------------------------------------
void Porous2D::dialogShown_cb()
{
    try
    {
        //---- Enter your callback code here -----
    }
    catch(exception& ex)
    {
        //---- Enter your exception handling code here -----
        Porous2D::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
}

//------------------------------------------------------------------------------
//Callback Name: apply_cb
//------------------------------------------------------------------------------
int Porous2D::apply_cb()
{
    int errorCode = 0;
    try
    {
        //---- Enter your callback code here -----
    }
    catch(exception& ex)
    {
        //---- Enter your exception handling code here -----
        errorCode = 1;
        Porous2D::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
    return errorCode;
}

//------------------------------------------------------------------------------
//Callback Name: update_cb
//------------------------------------------------------------------------------
int Porous2D::update_cb(NXOpen::BlockStyler::UIBlock* block)
{
    NXOpen::Part* workPart(theSession->Parts()->Work());
    NXOpen::Part* displayPart(theSession->Parts()->Display());
    try
    {
        if(block == pointSheetStart)
        {
        //---------Enter your code here-----------
        }
        else if(block == enumParticleShape)
        {
        //---------Enter your code here-----------
        }
        else if(block == enumGenMethod)
        {
        //---------Enter your code here-----------
        }
        else if(block == expressionFaceLength)
        {
        //---------Enter your code here-----------
        }
        else if(block == expressionFaceWidth)
        {
        //---------Enter your code here-----------
        }
        else if(block == expressionParticleSize)
        {
        //---------Enter your code here-----------
        }
        else if(block == integerParticleNumber)
        {
        //---------Enter your code here-----------
        }
        else if(block == doublePorousRatio)
        {
        //---------Enter your code here-----------
        }
        else if(block == toggleUsePorousRatio)
        {
        //---------Enter your code here-----------
        }
        else if(block == integerBSplineOrder)
        {
        //---------Enter your code here-----------
        }
        else if(block == integerScaleRatio)
        {
        //---------Enter your code here-----------
        }
        else if(block == toggleScale)
        {
        //---------Enter your code here-----------
        }
        else if(block == expressionRowGap)
        {
        //---------Enter your code here-----------
        }
        else if(block == expressionColumGap)
        {
        //---------Enter your code here-----------
        }
        else if(block == buttonGenSheet)
        {
        //---------Enter your code here-----------
            //--Genarate sheet porous party
            GenPorousSheet(workPart);
        }
        else if(block == buttonGenParticle)
        {
        //---------Enter your code here-----------
        //--Create particle boundary curves
            GenParticleCurve(workPart);
        }
        else if(block == buttonTrimParticle)
        {
        //---------Enter your code here-----------
        //--Trim sheet to get porous
            TrimPorousSheet(workPart);
        }
        else if(block == buttonOnekeyGen)
        {
        //---------Enter your code here-----------
            //--Genarate sheet porous party
            GenPorousSheet(workPart);
            //--Create particle boundary curves
            GenParticleCurve(workPart);
            //--Trim sheet to get porous
            TrimPorousSheet(workPart);
        }
    }
    catch(exception& ex)
    {
        //---- Enter your exception handling code here -----
        Porous2D::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
    return 0;
}

//------------------------------------------------------------------------------
//Callback Name: ok_cb
//------------------------------------------------------------------------------
int Porous2D::ok_cb()
{
    int errorCode = 0;
    try
    {
        errorCode = apply_cb();
    }
    catch(exception& ex)
    {
        //---- Enter your exception handling code here -----
        errorCode = 1;
        Porous2D::theUI->NXMessageBox()->Show("Block Styler", NXOpen::NXMessageBox::DialogTypeError, ex.what());
    }
    return errorCode;
}

//------------------------------------------------------------------------------
//Function Name: GetBlockProperties
//Description: Returns the propertylist of the specified BlockID
//------------------------------------------------------------------------------
PropertyList* Porous2D::GetBlockProperties(const char *blockID)
{
    return theDialog->GetBlockProperties(blockID);
}
