// TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Test.h"
#include "TestDlg.h"
#include "ProgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_DrawingWindow.m_pDrawing = &drawing;
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestDlg)
	DDX_Control(pDX, IDC_DRAWINGWINDOW, m_DrawingWindow);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	//{{AFX_MSG_MAP(CTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON9, OnCloseButton)
	ON_BN_CLICKED(IDC_BUTTON5, OnZoomInButton)
	ON_BN_CLICKED(IDC_BUTTON6, OnZoomOutButton)
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDC_BUTTON7, OnZoomExtentsButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg message handlers

BOOL CTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestDlg::OnButton1() 
{
	static char szFilter[] = "DXF file format(*.dxf) |*.dxf|All Files (*.*)|*.*||";

	CFileDialog SaveDlg( FALSE, NULL, NULL/*LastFilePath*/, OFN_HIDEREADONLY | OFN_EXPLORER, szFilter, NULL );
	SaveDlg.m_ofn.lpstrTitle = "Save DXF File As";

	if(SaveDlg.DoModal()==IDOK)
	{	// Saving sample dxf file data
		CString DxfFileName( SaveDlg.GetPathName() );
		CDxfFileWrite dxffile;

		if(SaveDlg.GetFileExt().IsEmpty())
			DxfFileName += ".dxf";

		BOOL result=TRUE;

		// Create dxf file
		result &= dxffile.Create( DxfFileName );

		// Header Section ------------------------------------------
		result &= dxffile.BeginSection(SEC_HEADER);
		result &= dxffile.EndSection();
		// close HEADER section ------------------------------------

		// Tables Section ------------------------------------------
		result &= dxffile.BeginSection(SEC_TABLES);

		//  LTYPE table type -------------------------
		result &= dxffile.BeginTableType(TAB_LTYPE);

		DXFLTYPE	ltype;

		//  Continuous
		ZeroMemory(&ltype, sizeof(ltype));
		strcpy(ltype.Name, "Continuous");
		strcpy(ltype.DescriptiveText, "Solid line");
		result &= dxffile.AddLinetype(&ltype);

		//  DASHDOT2
		ZeroMemory(&ltype, sizeof(ltype));
		strcpy(ltype.Name, "DASHDOT2");
		strcpy(ltype.DescriptiveText, "Dash dot (.5x) _._._._._._._._._._._._._._._.");
		ltype.ElementsNumber = 4;
		ltype.TotalPatternLength = 0.5;
		ltype.Elements[0] = 0.25;
		ltype.Elements[1] = -0.125;
		ltype.Elements[2] = 0.0;
		ltype.Elements[3] = -0.125;
		result &= dxffile.AddLinetype(&ltype);

		result &= dxffile.EndTableType();
		//  close LTYPE table type -------------------

		//  LAYER table type -------------------------
		result &= dxffile.BeginTableType(TAB_LAYER);
		result &= dxffile.AddLayer("Layer1", 1, "Continuous");
		result &= dxffile.AddLayer("Layer2", 2, "Continuous");
		result &= dxffile.AddLayer("Layer3", 3, "Continuous");
		result &= dxffile.AddLayer("Layer4", 4, "Continuous");
		result &= dxffile.EndTableType();
		//  close LAYER table type -------------------

		//  STYLE table type -------------------------
		result &= dxffile.BeginTableType(TAB_STYLE);

		DXFSTYLE tstyle;
		ZeroMemory(&tstyle, sizeof(tstyle));
		strcpy(tstyle.Name, "Style1");
		strcpy(tstyle.PrimaryFontFilename, "TIMES.TTF");
		tstyle.Height = 0.3;
		tstyle.WidthFactor = 1;
		result &= dxffile.AddTextStyle(&tstyle);

		result &= dxffile.EndTableType();
		//  close STYLE table type -------------------

		//  DIMSTYLE table type ----------------------
		result &= dxffile.BeginTableType(TAB_DIMSTYLE);
		
		DXFDIMSTYLE	dimstyle;

		//  DIM1
		ZeroMemory(&dimstyle, sizeof(dimstyle));
		strcpy(dimstyle.Name, "DIM1");			// DimStyle Name
		dimstyle.DIMCLRD = 2;					// Dimension line & Arrow heads color
		dimstyle.DIMDLE = 0.0000;				// Dimension line size after Extensionline
		dimstyle.DIMCLRE = 2;					// Extension line color
		dimstyle.DIMEXE = 0.1800;				// Extension line size after Dimline
		dimstyle.DIMEXO = 0.0625;				// Offset from origin
		strcpy(dimstyle.DIMBLK1,"ClosedFilled");// 1st Arrow head
		strcpy(dimstyle.DIMBLK2,"ClosedFilled");// 2nd Arrow head
		dimstyle.DIMASZ = 0.1800;				// Arrow size
		strcpy(dimstyle.DIMTXSTY, "Style1");	// Text style
		dimstyle.DIMCLRT = 3;					// Text color
		dimstyle.DIMTXT = 0.1800;				// Text height
		dimstyle.DIMTAD = 1;					// Vertical Text Placement
		dimstyle.DIMGAP = 0.0900;				// Offset from dimension line
		result &= dxffile.AddDimStyle(&dimstyle);

		result &= dxffile.EndTableType();
		//  close DIMSTYLE table type ----------------

		result &= dxffile.EndSection();
		// close TABLES section ------------------------------------

		// Entities Section ----------------------------------------
		result &= dxffile.BeginSection(SEC_ENTITIES);

		// set current textstyle to Style1
		result &= dxffile.SetCurrentTextStyle("Style1");

		// default current layer is 0
		// draw text
		result &= dxffile.Text("Solid", 11.4, 4.6, 0.3);

		// set current layer to Layer2
		result &= dxffile.SetCurrentLayer("Layer2");
		// draw a line
		result &= dxffile.Line(1.2, 3.3, 7.5, 7.7);

		// set current layer to Layer1 and linetype to DASHDOT2
		result &= dxffile.SetCurrentLayer("Layer1", "DASHDOT2");
		// draw a circle
		result &= dxffile.Circle(7.8, 4.3, 1.75);

		// set current layer to Layer2
		result &= dxffile.SetCurrentLayer("Layer2");
		// draw text
		result &= dxffile.Text("Circle", 7.2, 4.1, 0.3, 0);

		// set current layer to Layer4
		result &= dxffile.SetCurrentLayer("Layer4");
		// draw a solid
		REALPOINT points[4];
		points[0].x = 10.4; points[0].y = 7.2;
		points[1].x = 13.6; points[1].y = 7.4;
		points[2].x = 13.1; points[2].y = 4.9;
		points[3].x = 10.9; points[3].y = 5.9;
		result &= dxffile.Solid(4, points);

		// set current layer to Layer3
		result &= dxffile.SetCurrentLayer("Layer3");
		// draw text
		result &= dxffile.Text("Line", 5.9, 6.7, 0.3, 35);
		// draw a dimension line
		result &= dxffile.SetCurrentDimStyle("DIM1");
		result &= dxffile.DimLinear(6.05, 3, 9.55, 3, 9.55, 2, 0, "3.50");
		
		result &= dxffile.EndSection();
		// close ENTITIES section ----------------------------------

		// Close dxf file
		result &= dxffile.Close();

		if(!result)
			MessageBox("Error while writing dxf file!", "Error", MB_ICONERROR | MB_OK);
		else
			MessageBox("Writing dxf file done.", "Dxf Test", MB_ICONINFORMATION | MB_OK);
	}
}

void CTestDlg::OnButton2() 
{
	static char szFilter[] = "DXF file format(*.dxf) |*.dxf|All Files (*.*)|*.*||";

	CFileDialog SaveDlg( FALSE, NULL, NULL/*LastFilePath*/, OFN_HIDEREADONLY | OFN_EXPLORER, szFilter, NULL );
	SaveDlg.m_ofn.lpstrTitle = "Save DXF File As";

	if(SaveDlg.DoModal()==IDOK)
	{	// Saving sample dxf file data
		CString DxfFileName( SaveDlg.GetPathName() );
//		CDrawing drw;

		if(SaveDlg.GetFileExt().IsEmpty())
			DxfFileName += ".dxf";

		BOOL result=TRUE;

		// Create Drawing
		if(!drawing.Create())
		{
			MessageBox("Fatal error on creating CDrawing!", "Error", MB_ICONERROR | MB_OK);
			return;
		}

		// Tables Section ------------------------------------------
		//  LTYPE table type -------------------------
		LTYPE		ltype;
		OBJHANDLE	objhandle1;

		//  Continuous
		ZeroMemory(&ltype, sizeof(ltype));
		strcpy(ltype.Name, "Continuous");
		strcpy(ltype.DescriptiveText, "Solid line");
		objhandle1 = drawing.AddLinetype(&ltype);

		//  DASHDOT2
		ZeroMemory(&ltype, sizeof(ltype));
		strcpy(ltype.Name, "DASHDOT2");
		strcpy(ltype.DescriptiveText, "Dash dot (.5x) _._._._._._._._._._._._._._._.");
		ltype.ElementsNumber = 4;
		ltype.PatternLength = 0.5;
		ltype.Elements[0] = 0.25;
		ltype.Elements[1] = -0.125;
		ltype.Elements[2] = 0.0;
		ltype.Elements[3] = -0.125;
		drawing.AddLinetype(&ltype);

		//  LAYER table type -------------------------
		LAYER	layer;

		// Layer1
		ZeroMemory(&layer, sizeof(layer));
		strcpy(layer.Name, "Layer1");
		layer.Color = 1;
		layer.LineTypeObjhandle = objhandle1; // Continuous
		drawing.AddLayer(&layer);

		// Layer2
		ZeroMemory(&layer, sizeof(layer));
		strcpy(layer.Name, "Layer2");
		layer.Color = 2;
		layer.LineTypeObjhandle = objhandle1; // Continuous
		drawing.AddLayer(&layer);

		// Layer3
		ZeroMemory(&layer, sizeof(layer));
		strcpy(layer.Name, "Layer3");
		layer.Color = 3;
		layer.LineTypeObjhandle = objhandle1; // Continuous
		drawing.AddLayer(&layer);

		// Layer4
		ZeroMemory(&layer, sizeof(layer));
		strcpy(layer.Name, "Layer4");
		layer.Color = 4;
		layer.LineTypeObjhandle = objhandle1; // Continuous
		drawing.AddLayer(&layer);

		//  STYLE table type -------------------------
		STYLE	style;

		ZeroMemory(&style, sizeof(style));
		strcpy(style.Name, "Style1");
		strcpy(style.PrimaryFontFilename, "TIMES.TTF");
		style.LastHeightUsed = 0.3;
		style.WidthFactor = 1;
		objhandle1 = drawing.AddTextStyle(&style);

		//  DIMSTYLE table type ----------------------
		DIMSTYLE	dimstyle;

		//  DIM1
		ZeroMemory(&dimstyle, sizeof(dimstyle));
		strcpy(dimstyle.Name, "DIM1");			// DimStyle Name
		dimstyle.dimclrd = 2;					// Dimension line & Arrow heads color
		dimstyle.dimdle = 0.0000;				// Dimension line size after Extensionline
		dimstyle.dimclre = 2;					// Extension line color
		dimstyle.dimexe = 0.1800;				// Extension line size after Dimline
		dimstyle.dimexo = 0.0625;				// Offset from origin
		strcpy(dimstyle.dimblk1, "ClosedFilled");// 1st Arrow head
		strcpy(dimstyle.dimblk2, "ClosedFilled");// 2nd Arrow head
		dimstyle.dimasz = 0.1800;				// Arrow size
		dimstyle.dimtxstyObjhandle = objhandle1;// Text style
		dimstyle.dimclrt = 3;					// Text color
		dimstyle.dimtxt = 0.1800;				// Text height
		dimstyle.dimtad = 1;					// Vertical Text Placement
		dimstyle.dimgap = 0.0900;				// Offset from dimension line
		drawing.AddDimStyle(&dimstyle);

		// Entities Section ----------------------------------------
		// set current textstyle to Style1
		drawing.SetTextStyle("Style1");

		// default current layer is 0
		// draw text
		result &= drawing.Text("Solid", 11.4, 4.6, 0.3) > 0;

		// set current layer to Layer2
		result &= drawing.SetLayer("Layer2");
		// draw a line
		result &= drawing.Line(1.8, 3.1, 8.1, 7.5) > 0;

		// set current layer to Layer1 and linetype to DASHDOT2
		result &= drawing.SetLayer("Layer1");
		result &= drawing.SetLineType("DASHDOT2");
		// draw a circle
		result &= drawing.Circle(8.1, 4.4, 1.75) > 0;

		// set current layer to Layer2
		result &= drawing.SetLayer("Layer2");
		// draw text
		result &= drawing.Text("Circle", 7.5, 4.2, 0.3, 0) > 0;

		// set current layer to Layer4
		result &= drawing.SetLayer("Layer4");
		// draw a solid
		REALPOINT points[4];
		points[0].x = 10.4; points[0].y = 7.2;
		points[1].x = 13.6; points[1].y = 7.4;
		points[2].x = 13.1; points[2].y = 4.9;
		points[3].x = 10.9; points[3].y = 5.9;
		result &= drawing.Solid(points[0], points[1], points[2], points[3]) > 0;
		// draw polyline
		result &= drawing.SetColor(3);
		ENTVERTEX vertex[5];
		ZeroMemory(vertex, sizeof(ENTVERTEX)*5);
		vertex[0].Point.x = 1.2125; vertex[0].Point.y = 5.1811;
		vertex[1].Point.x = 2.5888; vertex[1].Point.y = 4.9520;
		vertex[2].Point.x = 2.7910; vertex[2].Point.y = 7.1188;
		vertex[3].Point.x = 4.7204; vertex[3].Point.y = 6.6414;
		vertex[4].Point.x = 5.0015; vertex[4].Point.y = 8.0484;
		result &= drawing.PolyLine(vertex, 5);
		// draw text
		result &= drawing.SetColor(4);
		result &= drawing.Text("Polyline", 2.97, 5.7, 0.3, 30) > 0;

		// set current layer to Layer3
		result &= drawing.SetLayer("Layer3");
		result &= drawing.SetColor(COLOR_BYLAYER);
		// draw text
		result &= drawing.Text("Line", 6.5, 6.5, 0.3, 35) > 0;
		// draw a dimension line
		result &= drawing.SetDimStyle("DIM1");
		result &= drawing.DimLinear(6.35, 3, 9.85, 3, 9.85, 2, 0, "3.50") > 0;
		
		// Save data to dxf file
		if(!drawing.SaveDXFFile(DxfFileName))
		{
			result = FALSE;
			MessageBox("Error while saving dxf file!", "Error", MB_ICONERROR | MB_OK);
		}

		// Initialize Drawing View Window
		CRect r;
		m_DrawingWindow.GetClientRect(r);
		drawing.InitView(0, 0, r.Width(), r.Height());
		m_DrawingWindow.RedrawWindow();

		// Destroy CDrawing and free allocated memory
//		result &= drawing.Destroy();

		if(result)
			MessageBox("Writing dxf file done.", "Dxf Test (CDrawing)", MB_ICONINFORMATION | MB_OK);
		else
			MessageBox("Error!", "Error", MB_ICONERROR | MB_OK);

	}
}

void CTestDlg::OnButton3() 
{
	static char szFilter[] = "DXF file format(*.dxf) |*.dxf|All Files (*.*)|*.*||";

	CFileDialog OpenDlg( TRUE, NULL, NULL/*LastFilePath*/, OFN_HIDEREADONLY | OFN_EXPLORER, szFilter, NULL );
	OpenDlg.m_ofn.lpstrTitle = "Open DXF File";

	if(OpenDlg.DoModal()==IDOK)
	{	// Saving sample dxf file data
		CString DxfFileName( OpenDlg.GetPathName() );
		CDrawing drw;

		BOOL result=TRUE;

		// Create Drawing
		if(!drw.Create())
		{
			MessageBox("Fatal error on creating CDrawing!", "Error", MB_ICONERROR | MB_OK);
			return;
		}
	
		// Initialize Progressbar
		CProgressDlg ProgDlg;
		ProgDlg.Create(this);
		ProgDlg.SetRange(0, 100);
		ProgDlg.SetWindowText("Loading DXF data...");

		// Load data from dxf file
		if(!drw.LoadDXFFile(DxfFileName, ProgDlg.m_hWnd))
		{
			ProgDlg.DestroyWindow();
			result = FALSE;
			MessageBox("Error while loading dxf file!", "Error", MB_ICONERROR | MB_OK);
			return;
		}

		// Close Progress window
		ProgDlg.DestroyWindow();

		// Add a circle
		result &= drw.Circle(10, 10, 10) > 0;

		// Save data to dxf file
		CFileDialog SaveDlg( FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_EXPLORER, szFilter, NULL );
		SaveDlg.m_ofn.lpstrTitle = "Save New DXF File As";

		if(SaveDlg.DoModal()==IDOK)
		{	// Saving sample dxf file data
			DxfFileName = SaveDlg.GetPathName();

			if(SaveDlg.GetFileExt().IsEmpty())
				DxfFileName += ".dxf";

			// Initialize Progressbar
			ProgDlg.Create(this);
			ProgDlg.SetRange(0, 100);
			ProgDlg.SetWindowText("Saving DXF data...");
			ProgDlg.SetPos(0);

			// Save data to dxf file
			if(!drw.SaveDXFFile(DxfFileName, ProgDlg.m_hWnd))
			{
				ProgDlg.DestroyWindow();
				result = FALSE;
				MessageBox("Error while saving dxf file!", "Error", MB_ICONERROR | MB_OK);
			}

			// Close Progress window
			ProgDlg.DestroyWindow();

		}
		else
		{
			MessageBox("New dxf file was not saved!", "Dxf Test (Load & Save Test)", MB_ICONERROR | MB_OK);
			return;
		}

		// Destroy CDrawing and free allocated memory
		result &= drw.Destroy();

		if(result)
			MessageBox("Test was done successfully.", "Dxf Test (Load & Save Test)", MB_ICONINFORMATION | MB_OK);
		else
			MessageBox("Error!", "Error", MB_ICONERROR | MB_OK);

	}
}

void CTestDlg::OnButton4() 
{
	static char szFilter[] = "DXF file format(*.dxf) |*.dxf|All Files (*.*)|*.*||";

	CFileDialog OpenDlg( TRUE, NULL, NULL/*LastFilePath*/, OFN_HIDEREADONLY | OFN_EXPLORER, szFilter, NULL );
	OpenDlg.m_ofn.lpstrTitle = "Open DXF File";

	if(OpenDlg.DoModal()==IDOK)
	{	// Openning sample dxf file
		CString DxfFileName( OpenDlg.GetPathName() );

		if(drawing.isOpen())
			drawing.Destroy();

		// Create Drawing
		if(!drawing.Create())
		{
			MessageBox("Fatal error on creating CDrawing!", "Error", MB_ICONERROR | MB_OK);
			return;
		}
	
		// Initialize Progressbar
		CProgressDlg ProgDlg;
		ProgDlg.Create(this);
		ProgDlg.SetRange(0, 100);

		// Load data from dxf file
		if(!drawing.LoadDXFFile(DxfFileName, ProgDlg.m_hWnd))
		{
			ProgDlg.DestroyWindow();
			MessageBox("Error while loading dxf file!", "Error", MB_ICONERROR | MB_OK);
			return;
		}

		// Close Progress window
		ProgDlg.DestroyWindow();

		// Initiaze Drawing View Window
		CRect r;
		m_DrawingWindow.GetClientRect(r);
		drawing.InitView(0, 0, r.Width(), r.Height());
		drawing.ZoomExtents();
		m_DrawingWindow.RedrawWindow();
	}
}

void CTestDlg::OnCloseButton() 
{
	drawing.Destroy();
	m_DrawingWindow.RedrawWindow();
}

void CTestDlg::OnZoomInButton() 
{
	if(drawing.isOpen())
	{
		drawing.SetZoomLevel( drawing.GetZoomLevel() * 1.5 );
		m_DrawingWindow.RedrawWindow();
	}
}

void CTestDlg::OnZoomOutButton() 
{
	if(drawing.isOpen())
	{
		drawing.SetZoomLevel( drawing.GetZoomLevel() / 1.5 );
		m_DrawingWindow.RedrawWindow();
	}
}

void CTestDlg::OnZoomExtentsButton() 
{
	if(drawing.isOpen())
	{
		drawing.ZoomExtents();
		m_DrawingWindow.RedrawWindow();
	}
}

BOOL CTestDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(drawing.isOpen())
	{
		if(zDelta>0)
			drawing.SetZoomLevel( drawing.GetZoomLevel() * 1.5 );
		else
			drawing.SetZoomLevel( drawing.GetZoomLevel() / 1.5 );
		m_DrawingWindow.RedrawWindow();
	}
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

