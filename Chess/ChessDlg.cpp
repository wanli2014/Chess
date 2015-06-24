// chessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Chess.h"
#include "ChessDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
#define BORDERWIDTH 13 //棋盘(左右)边缘的宽度
#define BORDERHEIGHT 15//棋盘(上下)边缘的高度
#define GRILLEWIDTH 35  //棋盘上每个格子的高度
#define GRILLEHEIGHT 35 //棋盘上每个格子的宽度

#define RedTime 1
#define BlkTime 2
/////////////////////////////////////////////////////////////////////////////////

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
// CChessDlg dialog

CChessDlg::CChessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChessDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChessDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	////添加的
	m_SelectMoveFrom = NOMOVE;
	m_SelectMoveTo = NOMOVE;
}

void CChessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);


	////添加的
	//{{AFX_DATA_MAP(CChessDlg)
	DDX_Control(pDX, IDC_RED_TIMEPASS, m_RedTimePass_Ctr);
	DDX_Control(pDX, IDC_RED_TIMELEFT, m_RedTimeLeft_Ctr);
	DDX_Control(pDX, IDC_BLK_TIMEPASS, m_BlkTimePass_Ctr);
	DDX_Control(pDX, IDC_BLK_TIMELEFT, m_BlkTimeLeft_Ctr);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_ButExit);
	DDX_Control(pDX, IDC_BUT_BEGIN, m_ButBegin);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChessDlg, CDialog)
	//{{AFX_MSG_MAP(CChessDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUT_BEGIN, OnButBegin)

	//添加的
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(IDM_LET_COMPUTERTHINK, OnLetComputerThink)//建立消息及其响应函数间的映射
	ON_WM_TIMER()

	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	//}}AFX_MSG_MAP
	//ON_BN_CLICKED(IDC_RADIOBLKFIR, &CChessDlg::OnBnClickedRadioblkfir)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChessDlg message handlers

BOOL CChessDlg::OnInitDialog()
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
	BITMAP BitMap;
	m_BoardBmp.LoadBitmap(IDB_CHESSBOARD);//加载棋盘
	m_BoardBmp.GetBitmap(&BitMap);
	m_nBoardWidth =  BitMap.bmWidth; 
	m_nBoardHeight = BitMap.bmHeight;
	m_BoardBmp.DeleteObject();

	m_Chessman.Create(IDB_CHESSMAN, 31, 15, RGB(0,128,128)); 
		//初始化图像列表并将它附加到 CImageList选件类 对象。 
		//31,cx定义图像的宽度，单位为象素；15,nGrow用来确定图像列表可控制的图像数量。
		//RGB(0,128,128)，crMask表示颜色屏蔽位

	rectBoard.left = BORDERWIDTH;
	rectBoard.right = BORDERWIDTH + GRILLEWIDTH*9;
	rectBoard.top = BORDERHEIGHT;
	rectBoard.bottom = BORDERHEIGHT + GRILLEHEIGHT*10;

	m_BlkTimeLeft_Ctr.SetWindowText(L"");
	m_BlkTimePass_Ctr.SetWindowText(L"");
	m_RedTimeLeft_Ctr.SetWindowText(L"");
	m_RedTimePass_Ctr.SetWindowText(L"");


	InitData();
	m_TotalTime = CTimeSpan(0,0,30,0);
	m_BlkTimer = 0;
	m_RedTimer = 0;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChessDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CChessDlg::OnPaint() 
{
	CPaintDC dc(this);
	CDC MemDC;
	POINT pt;
	CBitmap *pOldBmp;
	int z;
	
	MemDC.CreateCompatibleDC( &dc );
	m_BoardBmp.LoadBitmap(IDB_CHESSBOARD);		//加载棋盘

	//加载棋子图标
	pOldBmp = MemDC.SelectObject(&m_BoardBmp);
	for (short i=0; i<90; i++)
	{
		if (m_interface[i] == 0)
		{
			if(i == m_SelectMoveFrom)	//m_SelectMoveFrom为9*10类型
			{
				pt.x = (i % 9)*GRILLEHEIGHT + BORDERWIDTH ;
				pt.y = (i / 9)*GRILLEWIDTH + BORDERHEIGHT;
				m_Chessman.Draw(&MemDC, 14, pt, ILD_TRANSPARENT);
				//CImageList类的操作，CImageList::Draw  绘制在拖放操作过程中拖动的图像。
				//ILD_TRANSPARENT	使用掩码，无论背景色，透明地绘制图像。 
				//14,nImage 绘制图形的从零开始的索引
			}
				continue;
		}
		pt.x = (i % 9)*GRILLEHEIGHT + BORDERWIDTH ;
		pt.y = (i / 9)*GRILLEWIDTH + BORDERHEIGHT;
		
		z = IntToSubscript(m_interface[i]);
		
		m_Chessman.Draw(&MemDC,z , pt, ILD_TRANSPARENT);
		if(i == m_SelectMoveFrom)
			m_Chessman.Draw(&MemDC, 14, pt, ILD_TRANSPARENT);
		if(i == m_SelectMoveTo)
			m_Chessman.Draw(&MemDC, 14, pt, ILD_TRANSPARENT);
	}

	dc.BitBlt(0, 0, m_nBoardWidth, m_nBoardHeight, &MemDC, 0, 0, SRCCOPY);
			//BitBlt(),该函数对指定的源设备环境区域中的像素进行位块（bit_block）转换，以传送到目标设备环境。
			//SRCCOPY：将源矩形区域直接拷贝到目标矩形区域。
	MemDC.SelectObject(&pOldBmp);
	MemDC.DeleteDC();
	m_BoardBmp.DeleteObject();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.

//未变化
HCURSOR CChessDlg::OnQueryDragIcon()		//未变化
{
	return (HCURSOR) m_hIcon;
}

//初始化棋局，InitData(),ClearBoard()，StringToArray（）
void CChessDlg::OnButBegin() 
{
	// TODO: Add your control notification handler code here
	InitData();	//初始化数组board[]和m_interface[]
	m_SelectMoveFrom = NOMOVE;
	m_SelectMoveTo = NOMOVE;
	InvalidateRect(&rectBoard, false);//函数向指定的窗体更新区域添加一个矩形，然后窗口客户区域的这一部分将被重新绘制。
										//false);重画时不擦除背景
	UpdateWindow();//如果窗口更新的区域不为空，UpdateWindow函数通过发送一个WM_PAINT消息来更新指定窗口的客户区。
					//函数绕过应用程序的消息队列，直接发送WM_PAINT消息给指定窗口的窗口过程，如果更新区域为空，则不发送消息。
	
	m_Board.ClearBoard();//board[]置为0
	m_Board.StringToArray("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w");
						//转换为board[]数组，初始局面
	m_tsBlkTimeLeft = m_TotalTime;
	m_BlkTimeLeft_Ctr.SetWindowText(m_tsBlkTimeLeft.Format("%H:%M:%S"));//SetWindowText 改变控件的文本内容
			//Format是CString类的一个成员函数，它通过格式操作使任意类型的数据转换成一个字符串
	m_tsBlkTimePass = m_TotalTime - m_tsBlkTimeLeft;
	m_BlkTimePass_Ctr.SetWindowText(m_tsBlkTimePass.Format("%H:%M:%S"));
	m_tsRedTimeLeft = m_tsBlkTimeLeft;
	m_RedTimeLeft_Ctr.SetWindowText(m_tsRedTimeLeft.Format("%H:%M:%S"));
	m_tsRedTimePass = m_TotalTime - m_tsRedTimeLeft;
	m_RedTimePass_Ctr.SetWindowText(m_tsRedTimePass.Format("%H:%M:%S"));

	 m_RedTimer = SetTimer(1,1000,NULL);	//创建或设置一个定时器
	 m_gameState = REDTHINKING;
	

}

//鼠标点是否在棋盘内，为OnLButtonDown（）调用
BOOL CChessDlg::IsPtInBoard(CPoint point)
{

	return rectBoard.PtInRect(point);//判断参数中给出的点是否在矩形区域rectBoard内
}

//红方（棋手）走棋，响应红方的棋盘鼠标操作
void CChessDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (!IsPtInBoard(point) || m_gameState!=REDTHINKING)
		return;

	int SideTag = 16 + m_HumanSide * 16;

	short dest,from;
	int num;
	
	//清空高亮显示
	from = m_SelectMoveFrom;
	dest = m_SelectMoveTo;
	m_SelectMoveTo = NOMOVE;
	m_SelectMoveFrom = NOMOVE;
	if(from != NOMOVE)
		RequireDrawCell(from);//重绘棋盘上的一点from
	if(dest != NOMOVE)
		RequireDrawCell(dest);//重绘棋盘上的一点dest

	dest = GetPiecePos(point);//鼠标点对应的棋盘位置

	BYTE piece = m_interface[dest];

	if (piece & SideTag) //选手选中本方棋子
	{
		if (from != NOMOVE) {	//开始已经选中有本方其他棋子,则取消高亮显示原来选中的棋子
			m_SelectMoveFrom = NOMOVE;
			RequireDrawCell(from);
		}
		m_SelectMoveFrom = dest;	//高亮显示新选中的棋子
		RequireDrawCell(dest);
	}
	else //棋子落在空处或者他方的棋子上
	if (from != NOMOVE)	//如果已经先选中本方棋子
	{
		move mv;
		mv.from = ((from/9 +3) *16 + from%9 +3);	//将10*9的棋盘位置转换成16*16的棋盘位置
		mv.to = ((dest/9 +3) *16 + dest%9 +3);

		if (m_Board.LegalMove(mv)) //走法合理性检验，源位置z，目的位置k
		{
			m_Board.MakeMove(mv); //piece、board、movstack
			m_interface[dest] = m_interface[from];
			m_interface[from] = 0;

			m_SelectMoveTo = dest;
			RequireDrawCell(dest);

			m_SelectMoveFrom = from;	//着重显示走法起始点
			RequireDrawCell(from);		//将源点及目的点重新显示

			Beep(200,300);//Beep函数，使扬声器发出简单声音

			num = m_Board.HasLegalMove(); //判断胜负
			if (!num) {
				KillTimer(m_RedTimer);//杀害从的 nIDEvent 确定的事件之前调用对 SetTimer的计时器。 
				m_gameState = GAMEOVER;
				MessageBox(L"红方获胜", L"系统消息");
				return;
			}

			KillTimer(m_RedTimer);//杀害从的 nIDEvent 确定的事件之前调用对 SetTimer的计时器。 
			m_gameState = BLACKTHINKING;

			PostMessage(WM_COMMAND, IDM_LET_COMPUTERTHINK);//向消息窗口发送一条消息并立即返回。
		}
	}
	
	CDialog::OnLButtonDown(nFlags, point);
}

//鼠标点pt对应的棋盘位置，为CChessDlg::OnLButtonDown()调用
short CChessDlg::GetPiecePos(POINT pt)
{
	if (!rectBoard.PtInRect(pt)) return -1;
	short x = (pt.x-rectBoard.left) / GRILLEWIDTH;
	short y = (pt.y-rectBoard.top)  / GRILLEHEIGHT;

	return x + y * 9;// 返回仅有效棋盘上的下标用于m_interface[] 9*10棋盘

}

//重绘pos位置,pos为【9*10】形式,为OnLButtonDown，OnLetComputerThink调用
void CChessDlg::RequireDrawCell(short pos)
{
	CRect rect = GetPieceRect(pos);
	InvalidateRect(&rect, false);
	UpdateWindow();
}

//获取以xy为左上点的棋盘方格区域，为RequireDrawCell（）调用
CRect CChessDlg::GetPieceRect(short pos)
{
	short x = BORDERWIDTH + (pos % 9)*GRILLEWIDTH;
	short y = BORDERHEIGHT + (pos / 9)*GRILLEHEIGHT;

	CRect rect(x, y, x+GRILLEWIDTH, y+GRILLEHEIGHT);
	return rect;	

}

//黑方（电脑）走棋
void CChessDlg::OnLetComputerThink()
{
	if (m_gameState==GAMEOVER)
		return;
	CTime t1 = CTime::GetCurrentTime();

	m_Board.ComputerThink();

	m_tsBlkTimePass = m_tsBlkTimePass + (CTime::GetCurrentTime() - t1);//更新pass time
	if(m_tsBlkTimePass > m_TotalTime)//如果超时
	{
		m_gameState = GAMEOVER;

		if(m_RedTimer)
			KillTimer(m_RedTimer);//

		MessageBox(L"黑方超时判负", L"系统提示");
		return;
	}

	//如果未超时,却没有最佳走法
	short z,k;
	z = m_Board.BestMove.from;	//bestMove是全局变量
	k = m_Board.BestMove.to;

	if(z == 0)		//???????????????
	{
		m_gameState = GAMEOVER;
		if(m_RedTimer)
			KillTimer(m_RedTimer);
		MessageBox(L"黑方认输，红方获胜", L"系统提示");
		return;
	}
	//未超时，有最佳走法
	int num;
	m_Board.MakeMove(m_Board.BestMove);
		
	short zz,kk;

	//清空高亮显示
	zz = m_SelectMoveFrom;
	kk = m_SelectMoveTo;
	m_SelectMoveFrom = NOMOVE;
	m_SelectMoveTo = NOMOVE;
	RequireDrawCell(zz); 
	RequireDrawCell(kk);

	zz = ((z/16 -3) *9 + z%16 -3);	//将16*16的棋盘位置转换成10*9的棋盘位置，由board[]转化为m_interface[]
	kk = ((k/16 -3) *9 + k%16 -3);

	m_interface[kk] = m_interface[zz];
	m_interface[zz] = 0;

	//高亮显示电脑走法
	m_SelectMoveFrom = zz;
	m_SelectMoveTo = kk; 
	RequireDrawCell(zz); 
	RequireDrawCell(kk);
	Beep(500,300);

	m_tsBlkTimeLeft = m_TotalTime - m_tsBlkTimePass;	//黑方（电脑）时间剩余
	m_BlkTimeLeft_Ctr.SetWindowText(m_tsBlkTimeLeft.Format("%H:%M:%S"));
	m_BlkTimePass_Ctr.SetWindowText(m_tsBlkTimePass.Format("%H:%M:%S"));

	num = m_Board.HasLegalMove(); //判断当前棋局(对于红方)有无合法走法
	if (!num) {// num为0，黑方胜
		KillTimer(m_RedTimer);
		m_gameState = GAMEOVER;
		Beep(700,1000);
		MessageBox(L"黑方获胜", L"系统消息");
		return;
	}
	//红方有合理走法
	m_gameState = REDTHINKING;
	m_RedTimer = SetTimer(1,1000,NULL);
}

//显示动态倒计时
void CChessDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case 2:		//黑方计时器
		m_tsBlkTimePass =  m_tsBlkTimePass + CTimeSpan(0,0,0,1);//CTimeSpan( LONG，int, int, int )天时分秒
		m_tsBlkTimeLeft = m_TotalTime - m_tsBlkTimePass;
		m_BlkTimeLeft_Ctr.SetWindowText(m_tsBlkTimeLeft.Format("%H:%M:%S"));
		m_BlkTimePass_Ctr.SetWindowText(m_tsBlkTimePass.Format("%H:%M:%S"));
		break;
	case 1:		//红方计时器
		m_tsRedTimePass =  m_tsRedTimePass + CTimeSpan(0,0,0,1);
		if(m_tsRedTimePass > m_TotalTime)
		{
			m_gameState = GAMEOVER;
			KillTimer(m_RedTimer);
			MessageBox(L"红方超时判负", L"系统提示");
		}
		else
		{
			m_tsRedTimeLeft = m_TotalTime - m_tsRedTimePass;
			m_RedTimeLeft_Ctr.SetWindowText(m_tsRedTimeLeft.Format("%H:%M:%S"));
			m_RedTimePass_Ctr.SetWindowText(m_tsRedTimePass.Format("%H:%M:%S"));
		}
		break;
	default:
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

//初始化m_interface[90]、m_ComputerSide、m_HumanSide、m_gameState
void CChessDlg::InitData()
{

	static BYTE board[BOARD_SIZE]= {  //局部变量
		39, 37, 35, 33, 32, 34, 36, 38, 40,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 41, 0, 0, 0, 0, 0, 42, 0,
		43, 0, 44, 0, 45, 0, 46, 0, 47,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		27, 0, 28, 0, 29, 0, 30, 0, 31,
		0, 25, 0, 0, 0, 0, 0, 26, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		23, 21, 19, 17, 16, 18, 20, 22, 24
	};

	for (int i=0; i<BOARD_SIZE; i++) 
	{
		m_interface[i] = board[i];
	}

	m_ComputerSide = 1; 
	m_HumanSide = 0;
		
	m_gameState = GAMEOVER;
}

//将棋子编码转化为棋子种类编码0-6、7-13
int CChessDlg::IntToSubscript(int a)
{
	if(a<16 && a>=48)
		return 14;
	
	switch(a)
	{
		//红方棋子
	case 16:	return 0;
	case 17:
	case 18:	return 1;
	case 19:
	case 20:	return 2;
	case 21:
	case 22:	return 3;
	case 23:
	case 24:	return 4;
	case 25:
	case 26:	return 5;
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:	return 6;

		//黑方棋子
	case 32:	return 7;
	case 33:
	case 34:	return 8;
	case 35:
	case 36:	return 9;
	case 37:
	case 38:	return 10;
	case 39:
	case 40:	return 11;
	case 41:
	case 42:	return 12;
	case 43:
	case 44:
	case 45:
	case 46:
	case 47:	return 13;

	default:	return 14;
	}
}

//关闭对话框
void CChessDlg::OnButtonClose() 
{
	// TODO: Add your control notification handler code here
	if(MessageBox(L"真的要退出吗？",L"系统提示",MB_OKCANCEL)==IDOK)
	{
		DestroyWindow();
		delete this;
	}
}
