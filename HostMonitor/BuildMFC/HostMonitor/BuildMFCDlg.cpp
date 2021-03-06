
// BuildMFCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BuildMFC.h"
#include "BuildMFCDlg.h"
#include "afxdialogex.h"
#include "UiBlock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CBuildMFCDlg dialog
CBuildMFCDlg* CBuildMFCDlg::m_the_dialog = NULL;
CBuildMFCDlg::CBuildMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BUILDMFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_the_dialog = this;
	m_is_main_ui_mode = false;
}

void CBuildMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBuildMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CBuildMFCDlg message handlers

BOOL CBuildMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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
	AfxBeginThread(CBuildMFCDlg::ThreadHostMonitor, NULL);
	AfxBeginThread(CBuildMFCDlg::ThreadRefreshUI, NULL);
	ShellExecute(NULL, L"open", L"sync_data.bat",
		L"60 98 30 120 80 100", NULL, SW_HIDE);//Ping cloud

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBuildMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CBuildMFCDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		updateUI(dc);
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBuildMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CBuildMFCDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CBuildMFCDlg::updateUI(CPaintDC& dc)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	if (m_is_main_ui_mode)
	{
		m_uiBlock[0]->setBlockSize(rcClient);
		m_uiBlock[0]->renderUI(0, 0, dc);
		return;
	}

	int width = rcClient.Width() / 2;
	int height = rcClient.Height() / 4;
	
	//left
	int y = 0;
	for (int i = 1; i < 5; i++)
	{
		m_uiBlock[i]->setBlockSize(CRect(CPoint(0, y), CSize(width, height)));
		m_uiBlock[i]->renderUI(0, y, dc);
		y += height;
	}

	//right
	y = 0;
	for (int i = 5; i < 9; i++)
	{
		m_uiBlock[i]->setBlockSize(CRect(CPoint(width, y), CSize(width, height)));
		m_uiBlock[i]->renderUI(width, y, dc);
		y += height;
	}
}

void CBuildMFCDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	for (int i = 0; i < UI_BLOCK_NUM; i++)
	{
		if (m_uiBlock[i])
		{
			m_uiBlock[i]->OnLButtonDown(nFlags, point);
		}
	}
}

void CBuildMFCDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	for (int i = 0; i < UI_BLOCK_NUM; i++)
	{
		if (m_uiBlock[i])
		{
			m_uiBlock[i]->OnLButtonUp(nFlags, point);
		}
	}
}

void CBuildMFCDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	for (int i = 0; i < UI_BLOCK_NUM; i++)
	{
		if (m_uiBlock[i])
		{
			m_uiBlock[i]->OnMouseMove(nFlags, point);
		}
	}
}

void CBuildMFCDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_is_main_ui_mode = !m_is_main_ui_mode;
}

UINT CBuildMFCDlg::ThreadHostMonitor(LPVOID pParam)
{
	for (int i = 0; i < UI_BLOCK_NUM; i++)
	{
		m_the_dialog->m_uiBlock[i] = new CUiBlock(i, COLOR_BYTES);
	}

	return startHostMonitor(1, 1024, 768, 8, 1024, 370, COLOR_BYTES);
}

UINT CBuildMFCDlg::ThreadRefreshUI(LPVOID pParam)
{
	while (true)
	{
		Sleep(30);
		m_the_dialog->Invalidate();
	}
}

///////////////// Implement functions for libs //////////////////////
void do_assert(const char* file, int line)
{	
#ifdef _DEBUG
	assert(false);
#endif
}

void log_out(const char* log)
{
	::OutputDebugStringA(log);
}
