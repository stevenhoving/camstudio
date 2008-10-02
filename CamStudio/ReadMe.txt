========================================================================
       MICROSOFT FOUNDATION CLASS LIBRARY : vscap
========================================================================

AppWizard has created this vscap application for you.  This application
not only demonstrates the basics of using the Microsoft Foundation classes
but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your vscap application.

vscap.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CVscapApp application class.

vscap.cpp
    This is the main application source file that contains the application
    class CVscapApp.

vscap.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
	Developer Studio.

res\vscap.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file vscap.rc.

res\vscap.rc2
    This file contains resources that are not edited by Microsoft
	Developer Studio.  You should place all resources not
	editable by the resource editor in this file.

vscap.clw
    This file contains information used by ClassWizard to edit existing
    classes or add new classes.  ClassWizard also uses this file to store
    information needed to create and edit message maps and dialog data
    maps and to create prototype member functions.

/////////////////////////////////////////////////////////////////////////////

For the main frame window:

MainFrm.h, MainFrm.cpp
    These files contain the frame class CMainFrame, which is derived from
    CFrameWnd and controls all SDI frame features.

res\Toolbar.bmp
    This bitmap file is used to create tiled images for the toolbar.
    The initial toolbar and status bar are constructed in the
    CMainFrame class.  Edit this toolbar bitmap along with the
    array in MainFrm.cpp to add more toolbar buttons.

/////////////////////////////////////////////////////////////////////////////

AppWizard creates one document type and one view:

vscapDoc.h, vscapDoc.cpp - the document
    These files contain your CVscapDoc class.  Edit these files to
    add your special document data and to implement file saving and loading
    (via CVscapDoc::Serialize).

vscapView.h, vscapView.cpp - the view of the document
    These files contain your CVscapView class.
    CVscapView objects are used to view CVscapDoc objects.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named vscap.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Developer Studio reads and updates this file.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, and your application is
in a language other than the operating system's current language, you
will need to copy the corresponding localized resources MFC40XXX.DLL
from the Microsoft Visual C++ CD-ROM onto the system or system32 directory,
and rename it to be MFCLOC.DLL.  ("XXX" stands for the language abbreviation.
For example, MFC40DEU.DLL contains resources translated to German.)  If you
don't do this, some of the UI elements of your application will remain in the
language of the operating system.

/////////////////////////////////////////////////////////////////////////////
9/29/08 TFM:

Take note of these 2 preprocessor constants! They make many warnings go away!

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES		1
#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES		1

Still have to fix a few of the strxxx functions to get the replacement templates
to trigger but the end result are the secure functions. Have to see what happens
with UNICODE builds.

10/1/09

Produccer builds again! The fix is shift all the include files before the
redeinition of the "new" to DEBUG_NEW. Additional changes were for window classes
that cannot be global variables as they require a instance handle and the
encapsulation of the propery dialog into the view class to have the property
pages classes created properly.

- added the testver.h file to set the WIN_VER value. Alter as needed as it
may need to be extended to Vista and MSC_VER 1500.

- more code layout changes; (It never ends!)

- All the projects shoudl build in both release and debug more. SWF files
shoudl be generated with audio and if you put the controll fodler in the
output folder you should get a loadable web page with the video.
 