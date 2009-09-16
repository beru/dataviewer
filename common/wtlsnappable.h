
#ifndef __WTL_SNAPPABLE_H_
#define __WTL_SNAPPABLE_H_

/////////////////////////////////////////////////////////////////////
// Tool windows that can either float or snap to the edges of another
// window. A snapped window can auto hide to show only it's title bar.
//
// The size of the view (child window being snapped or floated) is
// kept independent of snapping side or if floating.
//
// Written by Jens Nilsson, jnilsson@icebreaker.com
//
// The code and information is provided "as-is" without warranty
// of any kind, either expressed or implied.
//
//
// Thanks goes to Bjarke Viksoe for his WTL docking framwork which
// provided the inspiration to get me started.
// See http://www.viksoe.dk
/////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __cplusplus
   #error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
   #error wtlsnappable.h requires atlapp.h to be included first
#endif

/////////////////////////////////////////////////////////////////////
// Context of the snappable window
//
struct SNAPCONTEXT
{
	HWND  hWndSnapped; // Snapped window
	HWND  hWndFloated; // Floating window
	HWND  hWndView;    // View
	HWND  hWndRoot;    // Snap window manager
	DWORD dwFlags;	   // Position and state flags
};

//
// Position combinations allowed are
//
// snapFloat
// snapLeft, snapLeft | snapTop, snapLeft | snapBottom
// snapRight, snapRight | snapTop, snapRight | snapBottom
// snapTop
// snapBottom
//
// snapPinned and snapMinibar has no effect when floating
//

//
// Enumeration which will enforce allowed position combination for the flags
//
enum SnapPosition
{
	snapFloat		= 0x00000000,
	snapLeft		= 0x00000001,
	snapTop			= 0x00000002,
	snapRight		= 0x00000004,
	snapBottom		= 0x00000008,
	// Convenience flags
	snapTopLeft		= 0x00000003, // snapLeft  | snapTop
	snapTopRight	= 0x00000006, // snapRight | snapTop
	snapBottomLeft	= 0x00000009, // snapLeft  | snapBottom
	snapBottomRight	= 0x0000000C, // snapRight | snapBottom
	// Hidden (unsnapped or unfloated)
	snapHidden		= 0x00000010,
};

//
// Additional flags and masking values to manage the dwFlags attribute
//
enum SnapFlags
{
	// State
	snapPinned		= 0x00000100,
	snapMinibar		= 0x00000200,

	// Masks
	snapPosition	= 0x000000FF,
	snapState		= 0x00FFFF00,
	snapReserved	= 0xFF000000,
};


/////////////////////////////////////////////////////////////////////
// Helper functions
inline bool IsFloating(DWORD dwFlags)
{
	return ((dwFlags & snapPosition) == snapFloat);
};
inline bool IsFloating(SNAPCONTEXT* pCtx)
{
	return IsFloating(pCtx->dwFlags);
};

inline bool IsHidden(DWORD dwFlags)
{
	return ((dwFlags & snapPosition) == snapHidden);
};
inline bool IsHidden(SNAPCONTEXT* pCtx)
{
	return IsHidden(pCtx->dwFlags);
};

inline bool HasVerticalCaption(DWORD dwFlags)
{
	return ((dwFlags & snapLeft)!=0 || (dwFlags & snapRight)!=0);
};
inline bool HasVerticalCaption(SNAPCONTEXT* pCtx)
{
	return HasVerticalCaption(pCtx->dwFlags);
};


/////////////////////////////////////////////////////////////////////
// Collection of snap context windows
//
typedef CSimpleValArray<SNAPCONTEXT *> CSnapContainer;


/////////////////////////////////////////////////////////////////////
// Information structure
//
struct SNAPWINDOWINFO
{
	SnapPosition	snapPosition;
	POINT			ptPosition;
	int				cxy;
	DWORD			dwFlags;
};

/////////////////////////////////////////////////////////////////////
// Default view windows style
//
#ifndef SNAP_DEFAULT_VIEW_STYLE
#define SNAP_DEFAULT_VIEW_STYLE WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS
#endif

/////////////////////////////////////////////////////////////////////
// Snappable framework custom messages
//
#ifndef SNAP_MSGBASE
#define SNAP_MSGBASE				WM_USER+860
#endif

#define WM_SNAP_FLOAT				SNAP_MSGBASE
#define WM_SNAP_SNAP				SNAP_MSGBASE + 1
#define WM_SNAP_HIDE				SNAP_MSGBASE + 2
#define WM_SNAP_QUERYRECT			SNAP_MSGBASE + 3
#define WM_SNAP_MOVEDONE			SNAP_MSGBASE + 4
#define WM_SNAP_REPOSITION			SNAP_MSGBASE + 5
#define WM_SNAP_UPDATELAYOUT		SNAP_MSGBASE + 6
#define WM_SNAP_QUERYSIZE			SNAP_MSGBASE + 7

/////////////////////////////////////////////////////////////////////
// Helper classes
//
class CSnapWindowInfo : public SNAPWINDOWINFO
{
public:
	CSnapWindowInfo()
	{
		memset(this, 0, sizeof(SNAPWINDOWINFO));
	}
};

// Offset reference point table depending on snap position
//
// SnapPosition			Window reference corner		Flags
// ------------------	-----------------------		------------------
// snapFloat			Top - Left					snapFloat
// snapLeft				Top - Left					snapLeft
// snapTop				Top - Left					snapTop
// snaptTopLeft			Top - Left					snapTop | snapLeft 
// snapTopRight			Top - Right					snapTop | snapRight
// snapRight			Top - Right					snapRight
// snapBottomRight		Bottom - Right				snapBottom | snapRight
// snapBottom			Bottom - Left				snapBottom
// snapBottomLeft		Bottom - Left				snapBottom | snapLeft
//

class CSnapTrackInfo
{
public:

	enum ReferencePosition
	{
		rpTopLeft		= 0,
		rpTopRight		= 1,
		rpBottomRight	= 2,
		rpBottomLeft	= 3,
	};

	static ReferencePosition ConvertFromSnapPosition(DWORD dwFlags)
	{
		switch (dwFlags & snapPosition)
		{
			case snapBottomRight:
				return rpBottomRight;
			case snapBottom:
			case snapBottomLeft:
				return rpBottomLeft;
			case snapTopRight:
			case snapRight:
				return rpTopRight;
		}
		return rpTopLeft;
	}

	CSnapTrackInfo()
	{
		m_refPos = rpTopLeft;
		m_offset.cx = 0;
		m_offset.cy = 0;
	}

	CSnapTrackInfo(const RECT& rcWin, const POINT& ptCursor, DWORD dwPos)
	{
		SetTrackInfo(rcWin, ptCursor, dwPos);
	}

	~CSnapTrackInfo() {}

	void SetTrackInfo(const RECT& rcWin, const POINT& ptCursor, DWORD dwPos)
	{
		m_size.cx = rcWin.right - rcWin.left;
		m_size.cy = rcWin.bottom - rcWin.top;

		m_refPos = ConvertFromSnapPosition(dwPos);

		m_offset.cx = ptCursor.x - rcWin.left;
		m_offset.cy = ptCursor.y - rcWin.top;

		if ( m_refPos == rpTopRight || m_refPos == rpBottomRight )
			m_offset.cx -= m_size.cx;
		if ( m_refPos == rpBottomRight || m_refPos == rpBottomLeft )
			m_offset.cy -= m_size.cy;
	}

	void MoveReferencePosition(const SIZE& size, DWORD dwTo)
	{
		ReferencePosition refPos = ConvertFromSnapPosition(dwTo);

		if ( m_refPos == refPos )
		{
			m_size = size;
			return;
		}

		if ( refPos == rpTopLeft)
			m_size = size;

		if ( m_refPos == rpTopRight || m_refPos == rpBottomRight )
			m_offset.cx += m_size.cx;
		if ( m_refPos == rpBottomRight || m_refPos == rpBottomLeft )
			m_offset.cy += m_size.cy;

		if ( refPos == rpTopRight || refPos == rpBottomRight )
			m_offset.cx -= m_size.cx;
		if ( refPos == rpBottomRight || refPos == rpBottomLeft )
			m_offset.cy -= m_size.cy;

		if ( refPos != rpTopLeft)
			m_size = size;

		m_refPos = refPos;
	}

	void GetReferencePoint(const POINT& ptCursor, POINT& ptRef)
	{
		ptRef.x = ptCursor.x - m_offset.cx;
		ptRef.y = ptCursor.y - m_offset.cy;
	}

	void GetRectangleFromReference(const POINT& ptRef, RECT& rc)
	{
		GetRectangleFromReference(ptRef, m_size, rc);
	}

	void GetRectangleFromReference(const POINT& ptRef, const SIZE& size, RECT& rc)
	{
		rc.left = ptRef.x;
		rc.top = ptRef.y;

		if ( m_refPos == rpTopRight || m_refPos == rpBottomRight )
			rc.left -= size.cx;
		if ( m_refPos == rpBottomRight || m_refPos == rpBottomLeft )
			rc.top -= size.cy;

		rc.right = rc.left + size.cx;
		rc.bottom = rc.top + size.cy;
	}

	void GetRectangle(const POINT& ptCursor, RECT& rc)
	{
		POINT ptRef;
		GetReferencePoint(ptCursor, ptRef);
		GetRectangleFromReference(ptRef, m_size, rc);
	}

	SIZE  m_size;
	SIZE  m_offset;
	ReferencePosition m_refPos;
};

/////////////////////////////////////////////////////////////////////
// CSnapWindowMover
//
// A helper class to move the snapped or docked window. Synchronizes the
// switch between floating and snapped position during the move operation.
//
// When window is within a specific distance from the layout border it will
// snap to the border. When draged out it will float again.
//
template <class T>
class ATL_NO_VTABLE CSnapWindowMover
{
public:
	// Member data
	SIZE  m_sizeDrag;			// The drag rectangle for snapping to the frame
	POINT m_ptStartCursorDrag;	// Cursor point at start of drag operation

	RECT  m_rcStartRect;		// Window rectangle at start of drag operation
	DWORD m_dwStartPos;			// Snap flags at start of drag operation

	//
	// Method used to manage dragging of snappable windows 
	//
	bool TrackMove(SNAPCONTEXT* pCtx)
	{
		T* pT = static_cast<T*>(this);

		// Store cursor starting point
		::GetCursorPos(&m_ptStartCursorDrag);

		// Don't start move until we are dragging the window
		if (! ::DragDetect(pT->m_hWnd, m_ptStartCursorDrag) )
			return false;

		CSnapTrackInfo snapTrackInfo;
		pT->StartTracking(pCtx, snapTrackInfo);

		// Get messages until capture lost or until Move is done
		while( pT->HasCapture(pCtx) )
		{
			MSG msg;
			if( !::GetMessage(&msg, NULL, 0, 0) ) 
			{
				::PostQuitMessage(msg.wParam);
				break;
			}

			switch( msg.message ) 
			{
				case WM_MOUSEMOVE:
					pT->OnMove(pCtx, msg.wParam, msg.pt, snapTrackInfo); 
					break;

				case WM_LBUTTONUP:
					pT->OnEndMove(pCtx); 
					pT->CancelTracking(pCtx);
					return true;

				case WM_RBUTTONDOWN:
					pT->CancelTracking(pCtx);
					return false;  

				case WM_KEYUP:
					pT->OnKey(pCtx, (int)msg.wParam, false);
					break;

				case WM_KEYDOWN:
					pT->OnKey(pCtx, (int)msg.wParam, true);
					if( msg.wParam == VK_ESCAPE ) 
					{
						pT->CancelTracking(pCtx);
						return false;
					}
					break;
	
				default:
					::DispatchMessage(&msg);
					break;
			}
		}

		ATLTRACE(_T("CSnapWindowMover::TrackMove - Lost capture\n"));

		pT->CancelTracking(pCtx);
		return false;
	}

	//
	// Helper methods
	//
	void StartTracking(SNAPCONTEXT* pCtx, CSnapTrackInfo& snapTrackInfo)
	{
		ATLTRACE(_T("CSnapWindowMover::StartTracking\n"));

		// Store starting data
		m_dwStartPos = pCtx->dwFlags;
		
		if ( IsFloating(pCtx->dwFlags) )
		{
			::SetCapture(pCtx->hWndFloated);
			::GetWindowRect(pCtx->hWndFloated, &m_rcStartRect);
		}
		else
		{
			::SetCapture(pCtx->hWndSnapped);
			::GetWindowRect(pCtx->hWndSnapped, &m_rcStartRect);
		}

		snapTrackInfo.SetTrackInfo(m_rcStartRect, m_ptStartCursorDrag, m_dwStartPos);

		// Set drag rectangle for snapping to layout
		m_sizeDrag.cx = ::GetSystemMetrics(SM_CXDRAG) * 6;
		m_sizeDrag.cy = ::GetSystemMetrics(SM_CYDRAG) * 6;
		return;
	}

	void CancelTracking(SNAPCONTEXT* pCtx)
	{
		if ( HasCapture(pCtx) )
			::ReleaseCapture();
		ATLTRACE(_T("CSnapWindowMover::CancelTracking\n"));
		return;
	}

	inline void GetLayoutRect(SNAPCONTEXT* pCtx, RECT* prc)
	{
		::SendMessage(pCtx->hWndRoot, WM_SNAP_QUERYRECT, 0, (LPARAM)prc);
		return;
	}

	// Check capture belongs to this context 
	bool HasCapture(SNAPCONTEXT* pCtx)
	{
		HWND hWnd = ::GetCapture();
		return (hWnd == pCtx->hWndFloated || hWnd == pCtx->hWndSnapped);
	}

	// Returns position part of snapping flags
	DWORD CheckSnapping(SNAPCONTEXT* pCtx, const RECT& rcLayout, const RECT& rect)
	{
		DWORD dwNewPos = (pCtx->dwFlags & snapPosition);

		// Window needs to be fully inside to snap to frame
		RECT rcTest = rcLayout;
		::InflateRect(&rcTest,m_sizeDrag.cx,m_sizeDrag.cy);
		RECT rcIntersect;
		::IntersectRect(&rcIntersect, &rcTest, &rect);
		if ( ::EqualRect(&rect, &rcIntersect) )
		{
			if ( (rect.left - rcLayout.left < m_sizeDrag.cx) && (rect.left - rcLayout.left > -m_sizeDrag.cx) )
				dwNewPos |= snapLeft;
			else
				dwNewPos &= ~snapLeft;

			if ( !(pCtx->dwFlags & snapLeft) && (rect.right - rcLayout.right < m_sizeDrag.cx && rect.right - rcLayout.right > -m_sizeDrag.cx) )
				dwNewPos |= snapRight;
			else
				dwNewPos &= ~snapRight;

			if ( (rect.top - rcLayout.top < m_sizeDrag.cy) && (rect.top - rcLayout.top > -m_sizeDrag.cy) )
				dwNewPos |= snapTop;
			else
				dwNewPos &= ~snapTop;

			if ( !(pCtx->dwFlags & snapTop) && (rect.bottom - rcLayout.bottom < m_sizeDrag.cy) && (rect.bottom - rcLayout.bottom > -m_sizeDrag.cy) )
				dwNewPos |= snapBottom;
			else
				dwNewPos &= ~snapBottom;
		}
		else
		{
				dwNewPos = snapFloat;
		}

		return dwNewPos;
	}

	void EnforceSnapPosition(SNAPCONTEXT* pCtx, const RECT& rcLayout, POINT& pos)
	{
		if (pCtx->dwFlags & snapLeft)
			pos.x = rcLayout.left;
		if (pCtx->dwFlags & snapTop)
			pos.y = rcLayout.top;
		if (pCtx->dwFlags & snapRight)
			pos.x = rcLayout.right;
		if (pCtx->dwFlags & snapBottom)
			pos.y = rcLayout.bottom;
	}

	bool FloatedReposition(SNAPCONTEXT* pCtx, const POINT& pt, DWORD dwNewPos, CSnapTrackInfo& snapTrackInfo)
	{
		if ( IsFloating(dwNewPos) )
		{
			POINT ptRef;
			snapTrackInfo.GetReferencePoint(pt, ptRef);
			::SetWindowPos(pCtx->hWndFloated, NULL, ptRef.x, ptRef.y, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
			return true;
		}

		RECT rcView;
		::GetWindowRect(pCtx->hWndView, &rcView);
		SIZE size = {rcView.right - rcView.left,  rcView.bottom - rcView.top};

		::SendMessage(pCtx->hWndSnapped,WM_SNAP_QUERYSIZE, dwNewPos, (LPARAM)&size);

		snapTrackInfo.MoveReferencePosition(size, dwNewPos);

		CSnapWindowInfo swi;
		swi.snapPosition = (SnapPosition) dwNewPos;
		
		T* pT = static_cast<T*>(this);
		RECT rcLayout, rcSnap;
		pT->GetLayoutRect(pCtx, &rcLayout);
		
		snapTrackInfo.GetRectangle(pt, rcSnap);
		
		if ((dwNewPos & snapLeft) || (dwNewPos & snapRight))
			swi.cxy = rcSnap.top - rcLayout.top;
		else
			swi.cxy = rcSnap.left - rcLayout.left;
		
		::SendMessage(pCtx->hWndRoot, WM_SNAP_SNAP, (WPARAM)&swi, (LPARAM)pCtx);
		::SetCapture(pCtx->hWndSnapped);

		return false;
	}

	bool SnappedReposition(SNAPCONTEXT* pCtx, const POINT& pt, DWORD dwNewPos, CSnapTrackInfo& snapTrackInfo)
	{
		// Retrieve size of view
		RECT rcView;
		::GetWindowRect(pCtx->hWndView, &rcView);
		SIZE size = {rcView.right - rcView.left,  rcView.bottom - rcView.top};

		if ( ! IsFloating(dwNewPos) )
		{
			::SendMessage(pCtx->hWndSnapped,WM_SNAP_QUERYSIZE, dwNewPos, (LPARAM)&size);

			snapTrackInfo.MoveReferencePosition(size, dwNewPos);

			// Update position part of flag
			pCtx->dwFlags &= ~snapPosition;
			pCtx->dwFlags |= dwNewPos;


			T* pT = static_cast<T*>(this);
			RECT rcLayout;
			pT->GetLayoutRect(pCtx, &rcLayout);

			POINT ptRef;
			snapTrackInfo.GetReferencePoint(pt, ptRef);

			// Enforce snapped positioning  (moving window to the snapped edge)
			pT->EnforceSnapPosition(pCtx, rcLayout, ptRef);

			RECT rcSnap;
			snapTrackInfo.GetRectangleFromReference(ptRef, size, rcSnap);

			POINT pos = { rcSnap.left, rcSnap.top };

			// Pos should be relative parent client rectangle
			::ScreenToClient(::GetParent(pCtx->hWndSnapped), &pos);

			::SetWindowPos(pCtx->hWndSnapped, NULL, pos.x, pos.y, size.cx, size.cy, SWP_NOZORDER);
			::SendMessage(pCtx->hWndSnapped,WM_SNAP_UPDATELAYOUT,0,0);

			return true;
		}

		::SendMessage(pCtx->hWndFloated,WM_SNAP_QUERYSIZE, dwNewPos, (LPARAM)&size);

		snapTrackInfo.MoveReferencePosition(size, dwNewPos);

		CSnapWindowInfo swi;
		swi.snapPosition = (SnapPosition) dwNewPos;
		
		snapTrackInfo.GetReferencePoint(pt, swi.ptPosition);

		::SendMessage(pCtx->hWndRoot, WM_SNAP_FLOAT, (WPARAM)&swi, (LPARAM)pCtx);
		::SetCapture(pCtx->hWndFloated);

		return false;
	}

	// Screen coordinates
	void OnMove(SNAPCONTEXT* pCtx, DWORD dwMouseKeys, POINT& ptCursor, CSnapTrackInfo& snapTrackInfo)
	{
		T* pT = static_cast<T*>(this);

		RECT rcLayout;
		pT->GetLayoutRect(pCtx, &rcLayout);

		RECT rc;
		snapTrackInfo.GetRectangle(ptCursor, rc);

		DWORD dwNewPos = pT->CheckSnapping(pCtx, rcLayout, rc);

		if ( IsFloating(pCtx)  )
		{
			pT->FloatedReposition(pCtx, ptCursor, dwNewPos, snapTrackInfo);
		}
		else
		{
			pT->SnappedReposition(pCtx, ptCursor, dwNewPos, snapTrackInfo);
		}
	}

	void OnEndMove(SNAPCONTEXT* pCtx)
	{
		ATLTRACE(_T("CSnapWindowMover::OnEndMove\n"));
	}
	void OnKey(SNAPCONTEXT* pCtx, int nKeyCode, bool bKeyDown) {}

};

/////////////////////////////////////////////////////////////////////
// CSnapFloatingWindow and CSnapFloatingWindowImpl
//
typedef CWinTraits<WS_POPUPWINDOW|WS_CLIPSIBLINGS|WS_OVERLAPPED|WS_THICKFRAME|WS_DLGFRAME, WS_EX_TOOLWINDOW|WS_EX_WINDOWEDGE> CSnapFloatWinTraits;

template <class T, class TBase = CWindow, class TWinTraits = CSnapFloatWinTraits>
class ATL_NO_VTABLE CSnapFloatingWindowImpl : 
		public CWindowImpl< T, TBase, TWinTraits >,
		public CSnapWindowMover<T>
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS, NULL)

	SNAPCONTEXT* m_pCtx;

	typedef CSnapFloatingWindowImpl< T , TBase, TWinTraits > thisClass;

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_NCLBUTTONDOWN, OnNCLeftButtonDown)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingsChange)
		MESSAGE_HANDLER(WM_SNAP_QUERYSIZE, OnSnapQuerySize)
	END_MSG_MAP()

	CSnapFloatingWindowImpl(SNAPCONTEXT* pCtx) : m_pCtx(pCtx)
	{
		BOOL bHandled;
		OnSettingsChange(0,(WPARAM)0, (LPARAM)0, bHandled);
	}

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
				DWORD dwStyle = 0, DWORD dwExStyle = 0,
				UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		return CWindowImpl< T, TBase, TWinTraits >::Create(hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
	}

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		delete this;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		DefWindowProc();

		// Change system menu
		HMENU hSysMenu = GetSystemMenu(FALSE);
		if (::IsMenu(hSysMenu))
		{
			int count = ::GetMenuItemCount(hSysMenu);
			while (count > 0)
			{
				UINT iPos = --count;
				UINT wID = ::GetMenuItemID(hSysMenu, iPos);
				if ( wID == SC_CLOSE )
				{
					MENUITEMINFO mii;
					mii.cbSize = sizeof(MENUITEMINFO);
					mii.fMask = MIIM_TYPE;
					mii.fType = MFT_STRING;
					mii.dwTypeData = _T("&Hide\tAlt+F4");
					mii.cch = 12;
#if(WINVER >= 0x0500)
					mii.fMask |= MIIM_BITMAP;
					mii.hbmpItem = HBMMENU_MBAR_CLOSE;
#endif
					::SetMenuItemInfo(hSysMenu, iPos, TRUE, &mii);
				}
				else if ( wID != SC_SIZE && wID != SC_MOVE)
				{
					::RemoveMenu(hSysMenu, iPos, MF_BYPOSITION);
				}
			}
		}
		return 0;
	}

	LRESULT OnSettingsChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE; //Give other handlers a chance
		return 0;
	}

	LRESULT OnSysCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		switch( wParam & 0xFFF0 )
		{
			case SC_CLOSE: 
				::SendMessage(m_pCtx->hWndRoot, WM_SNAP_HIDE, 0, (LPARAM)m_pCtx);
				return 0;
		}
		DefWindowProc();
		return 0;
	}

	LRESULT OnNCLeftButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SetFocus();
		if( wParam==HTCAPTION ) 
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			T* pT = static_cast<T*>(this);
			pT->TrackMove(m_pCtx);
			if (!IsFloating(m_pCtx))
				::SendMessage(m_pCtx->hWndSnapped,WM_SNAP_MOVEDONE,0,0);
			return 0;
		}
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1; // handled, no background painting needed
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if ( ! IsFloating(m_pCtx) )
			return 1;

		if (::IsWindow(m_pCtx->hWndView))
		{
			RECT rc;
			GetClientRect(&rc);
			::SetWindowPos(m_pCtx->hWndView, NULL, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		return 0;
	}

	LRESULT OnSnapQuerySize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LPSIZE lpSize = (LPSIZE) lParam;
		RECT rcFloat = { 0, 0, lpSize->cx, lpSize->cy };

		if ( IsFloating(wParam) )
		{
			// Adjust the size for floating window to keep the size of the view
			::AdjustWindowRectEx(&rcFloat, GetWndStyle(0), FALSE, GetWndExStyle(0));
			lpSize->cx = rcFloat.right - rcFloat.left;
			lpSize->cy = rcFloat.bottom - rcFloat.top;
		}
		else
		{
			ATLASSERT(FALSE);
			// Should be a floating window
			return FALSE;
		}

		return TRUE;
	}
};

class CSnapFloatingWindow : public CSnapFloatingWindowImpl<CSnapFloatingWindow>
{
public:
	DECLARE_WND_CLASS_EX(_T("WTL_SnapFloatingWindow"), CS_DBLCLKS, NULL)

	CSnapFloatingWindow(SNAPCONTEXT* pCtx) : CSnapFloatingWindowImpl<CSnapFloatingWindow>(pCtx)
	{ 
	}
};



/////////////////////////////////////////////////////////////////////
// CSnapAutoHideWindow and CSnapAutoHideWindowImpl
//
typedef CWinTraits<WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_THICKFRAME, WS_EX_WINDOWEDGE> CSnapAutoHideWinTraits;

template <class T, class TBase = CWindow, class TWinTraits = CSnapAutoHideWinTraits>
class ATL_NO_VTABLE CSnapAutoHideWindowImpl : 
		public CWindowImpl< T, TBase, TWinTraits >,
		public CSnapWindowMover<T> 
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, NULL)

	// Timer id and interval used for auto hide
	enum { IDT_AUTOHIDE = 1234, IDT_INTERVAL = 500 };

	// These could probably be static
	CFont m_hFontSmCaption;	// Horizontal font
	CFont m_vFontSmCaption; // Vertical font
	SIZE  m_sizeSmCapBtn;	// Size of caption buttons
	int   m_cxyCaption;		// Height of caption bar
	int   m_cxyBorder;		// Width of border

	SNAPCONTEXT* m_pCtx;

	SIZE  m_size; // Size of expanded window

	// State during mouse button down / up
	bool  m_bCloseDown, m_bPushpinDown;

	// These are set by UpdateLayout
	RECT m_rcChild;
	RECT m_rcCaption;
	RECT m_rcCloseBtn;
	RECT m_rcPushpinBtn;

	typedef CSnapAutoHideWindowImpl< T , TBase, TWinTraits > thisClass;

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingsChange)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNCHitTest)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLeftButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLeftButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_SNAP_MOVEDONE, OnSnapMoveDone)
		MESSAGE_HANDLER(WM_SNAP_REPOSITION, OnSnapReposition)
		MESSAGE_HANDLER(WM_SNAP_UPDATELAYOUT, OnSnapUpdateLayout)
		MESSAGE_HANDLER(WM_SNAP_QUERYSIZE, OnSnapQuerySize)
	END_MSG_MAP()

	CSnapAutoHideWindowImpl(SNAPCONTEXT* pCtx) : m_pCtx(pCtx)
	{
		m_bCloseDown = m_bPushpinDown = false;
		BOOL bHandled;
		OnSettingsChange(0,(WPARAM)0, (LPARAM)0, bHandled);
	}

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
				DWORD dwStyle = 0, DWORD dwExStyle = 0,
				UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		return CWindowImpl< T, TBase, TWinTraits >::Create(hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
	}

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		delete this;
	}

	// Message handlers

	LRESULT OnSettingsChange(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// Height of caption bar
		m_cxyCaption =  ::GetSystemMetrics(SM_CYSMCAPTION);

		// Width of border
		m_cxyBorder = ::GetSystemMetrics(SM_CXFRAME);

		// Small caption button
		m_sizeSmCapBtn.cx = ::GetSystemMetrics(SM_CXSMSIZE) - 2; // Even smaller
		m_sizeSmCapBtn.cy = ::GetSystemMetrics(SM_CYSMSIZE) - 4; // Even smaller

		// Caption font
		NONCLIENTMETRICS ncm = { 0 };
		ncm.cbSize = sizeof(ncm);
		::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
		if( !m_hFontSmCaption.IsNull() )
			m_hFontSmCaption.DeleteObject();
		// Don't use bold font
		ncm.lfSmCaptionFont.lfWeight = FW_NORMAL;
		m_hFontSmCaption.CreateFontIndirect(&ncm.lfSmCaptionFont); //Small caption font

		if( !m_vFontSmCaption.IsNull() )
			m_vFontSmCaption.DeleteObject();
		ncm.lfSmCaptionFont.lfEscapement = ncm.lfSmCaptionFont.lfOrientation  = 2700;
		m_vFontSmCaption.CreateFontIndirect(&ncm.lfSmCaptionFont); //Small caption font

		bHandled = FALSE; //Give other handlers a chance
		return 0;
	}

	LRESULT OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);

		if (m_pCtx->dwFlags & snapMinibar)
		{
			// If auto hidden as a mini bar, no sizing at all
			switch (lRet)
			{
				case HTLEFT:
				case HTTOP:
				case HTRIGHT:
				case HTBOTTOM:
				case HTTOPLEFT:
				case HTTOPRIGHT:
				case HTBOTTOMLEFT:
				case HTBORDER:
					return HTCLIENT;
				default:
					break;
			}
		}

		// Prohibit sizing of snapped side
		DWORD dwPos = (m_pCtx->dwFlags & snapPosition);

		switch (lRet)
		{
			case HTLEFT:
				if (dwPos & snapLeft)
					lRet = HTBORDER;
				break;

			case HTTOP:
				if (dwPos & snapTop)
					lRet = HTBORDER;
				break;

			case HTRIGHT:
				if (dwPos & snapRight)
					lRet = HTBORDER;
				break;

			case HTBOTTOM:
				if (dwPos & snapBottom)
					lRet = HTBORDER;
				break;

			case HTTOPLEFT:
				if (dwPos == (snapTop | snapLeft) )
					lRet = HTBORDER;
				else if (dwPos & snapTop)
					lRet = HTLEFT;
				else if (dwPos & snapLeft)
					lRet = HTTOP;
				break;

			case HTTOPRIGHT:
				if (dwPos == (snapTop | snapRight) )
					lRet = HTBORDER;
				else if (dwPos & snapTop)
					lRet = HTRIGHT;
				else if (dwPos & snapRight)
					lRet = HTTOP;
				break;

			case HTBOTTOMLEFT:
				if (dwPos == (snapBottom | snapLeft) )
					lRet = HTBORDER;
				else if (dwPos & snapBottom)
					lRet = HTLEFT;
				else if (dwPos & snapLeft)
					lRet = HTBOTTOM;
				break;

			case HTBOTTOMRIGHT:
				if (dwPos == (snapBottom | snapRight) )
					lRet = HTBORDER;
				else if (dwPos & snapBottom)
					lRet = HTRIGHT;
				else if (dwPos & snapRight)
					lRet = HTBOTTOM;
				break;
		}
		return lRet;
	}

	LRESULT OnSysCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		switch( wParam & 0xFFF0 )
		{
			case SC_CLOSE: 
				::SendMessage(m_pCtx->hWndRoot, WM_SNAP_HIDE, 0, (LPARAM)m_pCtx);
				return 0;
		}
		DefWindowProc();
		return 0;
	}


	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1; // handled, no background painting needed
	}


	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);
		RECT rc;
		GetClientRect(&rc);

		dc.ExcludeClipRect(&m_rcChild);
		dc.IntersectClipRect(&rc);
		dc.FillRect(&rc, (HBRUSH)LongToPtr(COLOR_3DFACE + 1));

		bool bVertical = HasVerticalCaption(m_pCtx);

		T* pT = static_cast<T*>(this);
		pT->DrawCaption(dc.m_hDC, bVertical, m_rcCaption);

		return 0;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if ( IsFloating(m_pCtx) || IsHidden(m_pCtx) )
			return 1;

		if (::IsWindow(m_pCtx->hWndView))
		{
			T* pT = static_cast<T*>(this);
			pT->UpdateLayout();
			::SetWindowPos(m_pCtx->hWndView, NULL, m_rcChild.left, m_rcChild.top, m_rcChild.right-m_rcChild.left, m_rcChild.bottom-m_rcChild.top, SWP_NOZORDER|SWP_NOACTIVATE);
		}
		return 0;
	}

	LRESULT OnLeftButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		if (::PtInRect(&m_rcCloseBtn,pt))
		{
			m_bCloseDown = true;
			InvalidateRect(&m_rcCloseBtn,FALSE);
			return 1;
		}
		if (::PtInRect(&m_rcPushpinBtn,pt))
		{
			m_bPushpinDown = true;
			InvalidateRect(&m_rcPushpinBtn,FALSE);
			return 2;
		}

		m_bCloseDown = m_bPushpinDown = false;

		if (m_pCtx->dwFlags & snapMinibar)
			return 3;

		// Kill timer during Move operation
		T* pT = static_cast<T*>(this);
		::KillTimer(m_hWnd, pT->IDT_AUTOHIDE);

		ClientToScreen(&pt);
		pT->TrackMove(m_pCtx);

		// If the timer is needed it will be set here
		pT->CheckAutoHide();

		SetWindowPos(HWND_TOP,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);

		return 0;
	}

	LRESULT OnLeftButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		if (m_bCloseDown && ::PtInRect(&m_rcCloseBtn,pt))
		{
			m_bPushpinDown = m_bCloseDown = false;
			ShowWindow(SW_HIDE);
			return 0;
		}
		else if (m_bPushpinDown && ::PtInRect(&m_rcPushpinBtn,pt))
		{
			if (m_pCtx->dwFlags & snapPinned)
			{
				m_pCtx->dwFlags &= ~snapPinned;
			}
			else
			{
				m_pCtx->dwFlags |= snapPinned;
			}
			m_bPushpinDown = false;
			InvalidateRect(&m_rcPushpinBtn, FALSE);
		}

		m_bPushpinDown = m_bCloseDown = false;

		T* pT = static_cast<T*>(this);
		if (m_pCtx->dwFlags & snapMinibar)
		{
			pT->ExpandWindow();
		}
		// Will set timer if needed
		pT->CheckAutoHide();

		return 0;
	}

	LRESULT OnSnapReposition(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		RECT* pRect = reinterpret_cast<RECT*>(lParam);
		RECT rc;
		GetWindowRect(&rc);
		POINT pt = { rc.left, rc.top };
		if (m_pCtx->dwFlags & snapLeft)
			pt.x = pRect->left;
		if (m_pCtx->dwFlags & snapTop)
			pt.y = pRect->top;
		if (m_pCtx->dwFlags & snapRight)
			pt.x = pRect->right - rc.right + rc.left;
		if (m_pCtx->dwFlags & snapBottom)
			pt.y = pRect->bottom - rc.bottom + rc.top;
		::ScreenToClient(GetParent(), (LPPOINT)&pt);
		::SetWindowPos(m_hWnd,NULL,pt.x,pt.y,0,0,SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);

		T* pT = static_cast<T*>(this);
		pT->UpdateLayout();
		return 0;
	}

	LRESULT OnSnapMoveDone(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		// Will set timer if needed
		pT->CheckAutoHide();
		return 0;
	}

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if ((m_pCtx->dwFlags & snapPosition) == snapFloat)
			return 1;

		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		if (wParam & MK_LBUTTON )
		{
			bool bCloseDown = (::PtInRect(&m_rcCloseBtn,pt) == TRUE);
			bool bPushpinDown = (::PtInRect(&m_rcPushpinBtn,pt) == TRUE);

			if (m_bCloseDown != bCloseDown)
				InvalidateRect(&m_rcCloseBtn, FALSE);
			m_bCloseDown = bCloseDown;

			if (m_bPushpinDown != bPushpinDown)
				InvalidateRect(&m_rcPushpinBtn, FALSE);
			m_bPushpinDown = bPushpinDown;

			return 1;
		}

		if (m_pCtx->dwFlags & snapMinibar)
		{
			TRACKMOUSEEVENT tme = { 0 };
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = m_hWnd;
			tme.dwFlags = TME_HOVER;
			tme.dwHoverTime = HOVER_DEFAULT;
			_TrackMouseEvent(&tme);
		}
		else
		{
			if (::PtInRect(&m_rcCloseBtn,pt) == FALSE && ::PtInRect(&m_rcPushpinBtn,pt) == FALSE)
				::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
		}
		return 0;
	}

	LRESULT OnSnapQuerySize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LPSIZE lpSize = (LPSIZE) lParam;
		RECT rcSnap = { 0, 0, lpSize->cx, lpSize->cy };

		// Adjust the size for snapping window to keep the size of the view
		::AdjustWindowRectEx(&rcSnap, GetWndStyle(0), FALSE, GetWndExStyle(0));
		lpSize->cx = rcSnap.right - rcSnap.left;
		lpSize->cy = rcSnap.bottom - rcSnap.top;

		// Add size for caption
		if ((wParam & snapLeft) || (wParam & snapRight))
		{	
			lpSize->cx += m_cxyCaption;
		}
		else if ((wParam & snapTop) || (wParam & snapBottom))
		{	
			lpSize->cy += m_cxyCaption;
		}
		else
		{
			ATLASSERT(FALSE);
			// Should be snapped to at least one side
			return FALSE;
		}

		return TRUE;
	}

	LRESULT OnSnapUpdateLayout(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->UpdateLayout();
		::SetWindowPos(m_pCtx->hWndView, NULL, m_rcChild.left, m_rcChild.top, m_rcChild.right-m_rcChild.left, m_rcChild.bottom-m_rcChild.top, SWP_NOZORDER | SWP_NOACTIVATE);

		// Extra flags
		if (wParam & snapPinned)
			m_pCtx->dwFlags |= snapPinned;

		if ( (wParam & snapMinibar) && (m_pCtx->dwFlags & snapPinned) == 0 )
			pT->ShrinkWindow();
		else
			InvalidateRect(&m_rcCaption, TRUE);

		return 0;
	}

	LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if (wParam & MK_LBUTTON)
			return 0;

		T* pT = static_cast<T*>(this);
		pT->ExpandWindow();
		// Will set timer if needed
		pT->CheckAutoHide();
		return 0;
	}

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		if ( wParam == pT->IDT_AUTOHIDE)
		{
			POINT ptCursor;
			GetCursorPos(&ptCursor);
			RECT rcWindow;
			GetWindowRect(&rcWindow);

			// Make little bigger so we are not accidently hiding the window
			::InflateRect(&rcWindow, ::GetSystemMetrics(SM_CXDRAG) * 6, ::GetSystemMetrics(SM_CYDRAG) * 6);

			if (! ::PtInRect(&rcWindow, ptCursor) )
			{
				::KillTimer(m_hWnd, pT->IDT_AUTOHIDE);
				pT->ShrinkWindow();
			}
			return 0;
		}

		bHandled = FALSE;
		return 1;
	}

	//
	// Overridable methods
	//

	void UpdateLayout()
	{
		// Updates child and caption rectangles
		GetClientRect(&m_rcChild);
		m_rcCaption = m_rcChild;

		const int dxy = (m_cxyCaption - m_sizeSmCapBtn.cx) / 2;

		if (m_pCtx->dwFlags & snapLeft)
		{	
			m_rcChild.right -= m_cxyCaption;
			m_rcCaption.left = m_rcChild.right;
			::SetRect(&m_rcCloseBtn,m_rcCaption.left+dxy,m_rcCaption.top+2,m_rcCaption.left+dxy+m_sizeSmCapBtn.cx, m_rcCaption.top+2+m_sizeSmCapBtn.cy);
			::SetRect(&m_rcPushpinBtn,m_rcCloseBtn.left,m_rcCloseBtn.bottom+2,m_rcCloseBtn.right, m_rcCloseBtn.bottom+2+m_sizeSmCapBtn.cy);
		}
		else if (m_pCtx->dwFlags & snapRight)
		{	
			m_rcChild.left += m_cxyCaption;
			m_rcCaption.right = m_rcChild.left;
			::SetRect(&m_rcCloseBtn,m_rcCaption.left+dxy,m_rcCaption.top+2,m_rcCaption.left+dxy+m_sizeSmCapBtn.cx, m_rcCaption.top+2+m_sizeSmCapBtn.cy);
			::SetRect(&m_rcPushpinBtn,m_rcCloseBtn.left,m_rcCloseBtn.bottom+2,m_rcCloseBtn.right, m_rcCloseBtn.bottom+2+m_sizeSmCapBtn.cy);
		}
		else if (m_pCtx->dwFlags & snapTop)
		{	
			m_rcChild.bottom -= m_cxyCaption;
			m_rcCaption.top = m_rcChild.bottom;
			::SetRect(&m_rcCloseBtn,m_rcCaption.left+2,m_rcCaption.top + dxy,m_rcCaption.left+2+m_sizeSmCapBtn.cx, m_rcCaption.top+dxy+m_sizeSmCapBtn.cy);
			::SetRect(&m_rcPushpinBtn,m_rcCloseBtn.right+2,m_rcCloseBtn.top,m_rcCloseBtn.right+2+m_sizeSmCapBtn.cx, m_rcCloseBtn.bottom);
		}
		else if (m_pCtx->dwFlags & snapBottom)
		{	
			m_rcChild.top += m_cxyCaption;
			m_rcCaption.bottom = m_rcChild.top;
			::SetRect(&m_rcCloseBtn,m_rcCaption.left+2,m_rcCaption.top + dxy,m_rcCaption.left+2+m_sizeSmCapBtn.cx, m_rcCaption.top+dxy+m_sizeSmCapBtn.cy);
			::SetRect(&m_rcPushpinBtn,m_rcCloseBtn.right+2,m_rcCloseBtn.top,m_rcCloseBtn.right+2+m_sizeSmCapBtn.cx, m_rcCloseBtn.bottom);
		}
	}

	void DrawCaption(CDCHandle dc, bool bVertical, RECT &rc)
	{      
		if( ::IsRectEmpty(&rc) )
			return;

		T* pT = static_cast<T*>(this);

		RECT rcText = rc;
		if (bVertical)
			rcText.top = m_rcPushpinBtn.bottom + 6;
		else
			rcText.left = m_rcPushpinBtn.right + 6;
		pT->DrawCaptionText(dc, bVertical, rcText);
		pT->DrawCloseButton(dc, bVertical, m_rcCloseBtn, m_bCloseDown);
		bool bPushpinDowned = (m_pCtx->dwFlags & snapPinned) != 0;
		if (m_bPushpinDown)
			bPushpinDowned = !bPushpinDowned; //reverse
		pT->DrawPushpinButton(dc, bVertical, m_rcPushpinBtn, bPushpinDowned);
	}

	void DrawCloseButton(CDCHandle dc, bool /*bVertical*/, const RECT &rc, bool bPushed)
	{
		if( ::IsRectEmpty(&rc) )
			return;

		RECT rcClose = rc;
		dc.DrawFrameControl(&rcClose, DFC_CAPTION, bPushed ? DFCS_CAPTIONCLOSE|DFCS_PUSHED : DFCS_CAPTIONCLOSE);
	}

	void DrawPushpinButton(CDCHandle dc, bool /*bVertical*/, const RECT &rc, bool bPushed)
	{
		if( ::IsRectEmpty(&rc) )
			return;

		RECT rcPushpin = rc;
		dc.DrawFrameControl(&rcPushpin, DFC_BUTTON, DFCS_ADJUSTRECT|DFCS_BUTTONPUSH | (bPushed ? DFCS_PUSHED : 0));

		POINT ptPin = { rcPushpin.left - 1, rcPushpin.top - 1 + (rcPushpin.bottom - rcPushpin.top) / 2};
		if (bPushed)
		{
			ptPin.x += 1;
			ptPin.y += 1;
		}

		POINT ptsPushpin[] = {	{ptPin.x, ptPin.y},
								{ptPin.x + 2, ptPin.y},
								{ptPin.x + 2, ptPin.y - 3},
								{ptPin.x + 4, ptPin.y - 1},
								{ptPin.x + 5, ptPin.y - 2},
								{ptPin.x + 6, ptPin.y - 2},
								{ptPin.x + 6, ptPin.y + 2},
								{ptPin.x + 5, ptPin.y + 2},
								{ptPin.x + 4, ptPin.y + 1},
								{ptPin.x + 2, ptPin.y + 3},
								{ptPin.x + 2, ptPin.y} };

		dc.Polyline(&ptsPushpin[0], 11);
	}

	void DrawCaptionText(CDCHandle dc, bool bVertical, RECT &rc)
	{
		TCHAR szCaption[80];
		::GetWindowText(m_pCtx->hWndView, szCaption, sizeof(szCaption)/sizeof(TCHAR));
		dc.SetBkMode(TRANSPARENT);

		HFONT fontOld = dc.SelectFont(bVertical ? m_vFontSmCaption : m_hFontSmCaption);
		
		if (bVertical)
		{
			dc.TextOut(rc.right, rc.top, szCaption);
		}
		else
		{
			dc.DrawText(szCaption, ::lstrlen(szCaption), &rc, DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX);
		}
		dc.SelectFont(fontOld);
	}

	void CheckAutoHide()
	{
		T* pT = static_cast<T*>(this);
		if ( IsFloating(m_pCtx) || (m_pCtx->dwFlags & snapPinned) != 0 || (m_pCtx->dwFlags & snapMinibar) != 0)
		{
			::KillTimer(m_hWnd, pT->IDT_AUTOHIDE);
		}
		else
		{
			::SetTimer(m_hWnd, pT->IDT_AUTOHIDE, pT->IDT_INTERVAL, NULL);
		}
	}

	void ExpandWindow()
	{
		if ( (m_pCtx->dwFlags & snapMinibar) == 0 )
			return; // Already expanded

		// Remove minibar flag (expanded state)
		m_pCtx->dwFlags &= ~snapMinibar;

		int cx = m_size.cx;
		int cy = m_size.cy;
		
		RECT rcWin;
		GetWindowRect(&rcWin);

		DWORD dwPos = m_pCtx->dwFlags & snapPosition;

		if (dwPos & snapLeft)
		{
			rcWin.right = rcWin.left + cx;
		}
		else if (dwPos & snapRight)
		{
			rcWin.left = rcWin.right - cx;
		}
		else if (dwPos & snapTop)
		{
			rcWin.bottom = rcWin.top + cy;
		}
		else if (dwPos & snapBottom)
		{
			rcWin.top = rcWin.bottom - cy;
		}

		::MapWindowPoints(HWND_DESKTOP, GetParent(), (LPPOINT)&rcWin, sizeof(RECT)/sizeof(POINT));
		SetWindowPos(HWND_TOP,rcWin.left,rcWin.top,rcWin.right-rcWin.left,rcWin.bottom-rcWin.top,SWP_NOACTIVATE); //|SWP_NOZORDER
	}

	void ShrinkWindow()
	{
		if ( m_pCtx->dwFlags & snapMinibar )
			return; // Already minimized

		// Set minibar flag (minimized state)
		m_pCtx->dwFlags |= snapMinibar;

		// Save window size
		RECT rcWindow;
		GetWindowRect(&rcWindow);
		m_size.cx = rcWindow.right-rcWindow.left;
		m_size.cy = rcWindow.bottom-rcWindow.top;

		// Autohide according to snapped sides
		RECT rcMinibar = rcWindow;
		::MapWindowPoints(HWND_DESKTOP, GetParent(), (LPPOINT)&rcMinibar, sizeof(RECT)/sizeof(POINT));

		DWORD dwPos = m_pCtx->dwFlags & snapPosition;

		if (dwPos & snapLeft)
		{
			rcMinibar.right = rcMinibar.left + m_cxyCaption + m_cxyBorder;
		}
		else if (dwPos & snapRight)
		{
			rcMinibar.left = rcMinibar.right - m_cxyCaption - m_cxyBorder;
		}
		else if (dwPos & snapTop)
		{
			rcMinibar.bottom = rcMinibar.top + m_cxyCaption + m_cxyBorder;
		}
		else if (dwPos & snapBottom)
		{
			rcMinibar.top = rcMinibar.bottom - m_cxyCaption - m_cxyBorder;
		}

		SetWindowPos(NULL,rcMinibar.left,rcMinibar.top,rcMinibar.right-rcMinibar.left,rcMinibar.bottom-rcMinibar.top,SWP_NOACTIVATE|SWP_NOZORDER);
		return;
	}
};

class CSnapAutoHideWindow : public CSnapAutoHideWindowImpl<CSnapAutoHideWindow>
{
public:
	DECLARE_WND_CLASS_EX(_T("WTL_SnapAutoHideWindow"), CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, NULL)

	CSnapAutoHideWindow(SNAPCONTEXT* pCtx) : CSnapAutoHideWindowImpl<CSnapAutoHideWindow>(pCtx)
	{ 
	}
};


/////////////////////////////////////////////////////////////////////
// CSnappingWindow and CSnappingWindowImpl
//
template <class T, 
          class TSnappedWindow = CSnapAutoHideWindow,
          class TFloatingWindow = CSnapFloatingWindow,
          class TBase = CWindow, 
          class TWinTraits = CControlWinTraits>
class ATL_NO_VTABLE CSnappingWindowImpl : public CWindowImpl< T, TBase, TWinTraits >
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, NULL)

	typedef CSnappingWindowImpl< T, TSnappedWindow, TFloatingWindow, TBase, TWinTraits > thisClass;
	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SNAP_SNAP,   OnSnapWindow)
		MESSAGE_HANDLER(WM_SNAP_FLOAT, OnFloatWindow)
		MESSAGE_HANDLER(WM_SNAP_HIDE, OnHideWindow)
		MESSAGE_HANDLER(WM_SNAP_QUERYRECT, OnSnapQueryRect)
	END_MSG_MAP()

	void SetClient(HWND hWndClient)
	{
		m_hWndClient = hWndClient;
	}

	inline HWND GetClient()
	{
		return m_hWndClient;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		for( int i=0; i<m_snappableWindows.GetSize(); i++ ) 
		{
			if (::IsWindow(m_snappableWindows[i]->hWndSnapped))
				::DestroyWindow(m_snappableWindows[i]->hWndSnapped);
			if (::IsWindow(m_snappableWindows[i]->hWndFloated))
				::DestroyWindow(m_snappableWindows[i]->hWndFloated);
			
			delete m_snappableWindows[i];
		}
		m_snappableWindows.RemoveAll();
		m_snappedWindows.RemoveAll();
		m_floatedWindows.RemoveAll();
		return 1;
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		RECT rect;
		GetClientRect(&rect);
		T* pT = static_cast<T*>(this);

		::MapWindowPoints(m_hWnd, ::GetParent( pT->GetClient() ), (LPPOINT)&rect, sizeof(RECT)/sizeof(POINT));
		::SetWindowPos( pT->GetClient() , HWND_BOTTOM, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0);

		pT->QueryRect(rect);

		for( int i=0; i<m_snappedWindows.GetSize(); i++ ) 
		{
			::SendMessage(m_snappedWindows[i]->hWndSnapped, WM_SNAP_REPOSITION, 0, (LPARAM)&rect);
		}
		return 0;
	}

	LRESULT OnSnapQueryRect(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		RECT* pRect = reinterpret_cast<RECT*>(lParam);
		ATLASSERT(pRect != NULL);
		if (!pRect)
			return -1;
		pT->QueryRect(*pRect);
		return 0;
	}

	LRESULT OnSnapWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		SNAPWINDOWINFO* pswi= (SNAPWINDOWINFO*) wParam;
		SNAPCONTEXT* pCtx = (SNAPCONTEXT*) lParam;
		ATLASSERT(pswi != NULL);
		ATLASSERT(pCtx != NULL);
		if (!pswi || !pCtx)
			return -1;
		T* pT = static_cast<T*>(this);
		if (!IsHidden(pCtx->dwFlags))
			pT->UnfloatWindow(pCtx);
		pT->SnapWindow(pCtx, pswi->snapPosition, pswi->cxy, pswi->dwFlags);
		return 0;
	}
	LRESULT OnFloatWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		SNAPWINDOWINFO* pswi= (SNAPWINDOWINFO*) wParam;
		SNAPCONTEXT* pCtx = (SNAPCONTEXT*) lParam;
		ATLASSERT(pswi != NULL);
		ATLASSERT(pCtx != NULL);
		if (!pswi || !pCtx)
			return -1;
		T* pT = static_cast<T*>(this);
		if (!IsHidden(pCtx->dwFlags))
			pT->UnsnapWindow(pCtx);
		pT->FloatWindow(pCtx, pswi->ptPosition, pswi->dwFlags);
		return 0;
	}
	LRESULT OnHideWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		SNAPCONTEXT* pCtx = (SNAPCONTEXT*) lParam;
		ATLASSERT(pCtx != NULL);
		if (!pCtx)
			return -1;
		T* pT = static_cast<T*>(this);
		pT->HideWindow(pCtx);
		return 0;
	}

	void QueryRect(RECT& rect)
	{
		// Typically override this method to calculate your client layout
		T* pT = static_cast<T*>(this);
		HWND hWndClient = pT->GetClient();
		::GetWindowRect(hWndClient ,&rect);
		LONG style = ::GetWindowLong(hWndClient,GWL_STYLE);
		if (style & WS_VSCROLL)
		{
			rect.right -= ::GetSystemMetrics(SM_CXVSCROLL);
		}
		if (style & WS_HSCROLL)
		{
			rect.bottom -= ::GetSystemMetrics(SM_CYHSCROLL);
		}

		// Compensate for 3D edge of client window
		LONG styleEx = ::GetWindowLong(hWndClient,GWL_EXSTYLE);
		if (styleEx & WS_EX_CLIENTEDGE)
			::InflateRect(&rect, -2, -2);
	}

	SNAPCONTEXT* AddSnappableWindow(HWND hWndView)
	{
		ATLASSERT( ::IsWindow(hWndView) );
		if (!::IsWindow(hWndView))
			return NULL;

		// Initialize context
		SNAPCONTEXT* pCtx = new SNAPCONTEXT;
		::ZeroMemory(pCtx, sizeof(SNAPCONTEXT));
		pCtx->hWndView = hWndView;
		pCtx->hWndRoot = m_hWnd;
		pCtx->dwFlags = snapHidden; // Is in hidden state

		// Create snapping window
		TSnappedWindow* wndSnapped = new TSnappedWindow(pCtx);
		ATLASSERT(wndSnapped);
		wndSnapped->Create(m_hWnd, rcDefault, NULL);
		ATLASSERT(::IsWindow(wndSnapped->m_hWnd));
		pCtx->hWndSnapped = *wndSnapped;

		// Create floating window
		TFloatingWindow* wndFloating = new TFloatingWindow(pCtx);
		ATLASSERT(wndFloating);
		TCHAR szCaption[128];    // max text length is 127 for floating caption
		::GetWindowText(hWndView, szCaption, sizeof(szCaption)/sizeof(TCHAR));
		wndFloating->Create(m_hWnd, rcDefault, szCaption);
		ATLASSERT(::IsWindow(wndFloating->m_hWnd));
		pCtx->hWndFloated = *wndFloating;

		// Store context pointer in the container (used for lookup and for memory mgmnt)
		m_snappableWindows.Add(pCtx);

		return pCtx;
	}

	void UnfloatWindow(SNAPCONTEXT* pCtx)
	{
		ATLASSERT( IsFloating(pCtx) );
		pCtx->dwFlags &= ~snapPosition;
		pCtx->dwFlags |= snapHidden;
		for (int i=0; i < m_floatedWindows.GetSize(); ++i)
		{
			if ( m_floatedWindows[i] == pCtx )
			{
				m_floatedWindows.RemoveAt(i);
			}
		}
		::ShowWindow(pCtx->hWndFloated, SW_HIDE);
	}

	void UnsnapWindow(SNAPCONTEXT* pCtx)
	{
		ATLASSERT( ! IsFloating(pCtx) );
		ATLASSERT( ! IsHidden(pCtx) );
		pCtx->dwFlags &= ~snapPosition;
		pCtx->dwFlags |= snapHidden;
		for (int i=0; i < m_snappedWindows.GetSize(); ++i)
		{
			if ( m_snappedWindows[i] == pCtx )
			{
				m_snappedWindows.RemoveAt(i);
			}
		}
		::ShowWindow(pCtx->hWndSnapped, SW_HIDE);
	}

	SNAPCONTEXT* GetSnapContext(HWND hWndView)
	{
		for( int i=0; i<m_snappableWindows.GetSize(); i++ ) 
		{
			if ( m_snappableWindows[i]->hWndView == hWndView)
				return m_snappableWindows[i];
		}
		return NULL;
	}

	void FloatWindow(HWND hWndView, const POINT& ptPos, DWORD dwFlags = 0)
	{
		T* pT = static_cast<T*>(this);
		SNAPCONTEXT* pCtx = pT->GetSnapContext(hWndView);
		if (pCtx)
		{
			pT->FloatWindow(pCtx, ptPos, dwFlags);
		}
	}
	void SnapWindow(HWND hWndView, SnapPosition spPos, int cxy = 0, DWORD dwFlags = snapMinibar)
	{
		T* pT = static_cast<T*>(this);
		SNAPCONTEXT* pCtx = pT->GetSnapContext(hWndView);
		if (pCtx)
		{
			pT->SnapWindow(pCtx, spPos, cxy, dwFlags);
		}
	}
	void HideWindow(HWND hWndView)
	{
		T* pT = static_cast<T*>(this);
		SNAPCONTEXT* pCtx = pT->GetSnapContext(hWndView);
		if (pCtx)
		{
			pT->HideWindow(pCtx);
		}
	}

	void FloatWindow(SNAPCONTEXT* pCtx, const POINT& ptPos, DWORD dwFlags)
	{
		ATLASSERT( IsHidden(pCtx) );
		pCtx->dwFlags &= ~snapHidden;
		pCtx->dwFlags |= snapFloat;
		m_floatedWindows.Add(pCtx);

		// Retrieve size of view
		RECT rcView;
		::GetWindowRect(pCtx->hWndView, &rcView);
		// Adjust the size for the floating window
		SIZE size = {rcView.right - rcView.left,  rcView.bottom - rcView.top};
		::SendMessage(pCtx->hWndFloated,WM_SNAP_QUERYSIZE, pCtx->dwFlags, (LPARAM)&size);

		// Changed parent of view
		::SetParent(pCtx->hWndView, pCtx->hWndFloated);
		::SetWindowPos(pCtx->hWndFloated, NULL, ptPos.x, ptPos.y, size.cx, size.cy, SWP_NOZORDER);

		// Reposition the view
		RECT rc;
		::GetClientRect(pCtx->hWndFloated, &rc);
		::SetWindowPos(pCtx->hWndView, NULL, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, SWP_NOZORDER|SWP_NOACTIVATE);
		::ShowWindow(pCtx->hWndFloated,SW_SHOWNORMAL);
		::ShowWindow(pCtx->hWndView,SW_SHOWNOACTIVATE);
	}


	void SnapWindow(SNAPCONTEXT* pCtx, SnapPosition spPos, int cxy, DWORD dwFlags)
	{
		ATLASSERT( IsHidden(pCtx) );
		pCtx->dwFlags &= ~snapHidden;
		pCtx->dwFlags |= spPos;
		m_snappedWindows.Add(pCtx);

		// Retrieve size of view
		RECT rcView;
		::GetWindowRect(pCtx->hWndView, &rcView);
		// Adjust the size for the snapped window
		SIZE size = {rcView.right - rcView.left,  rcView.bottom - rcView.top};
		::SendMessage(pCtx->hWndSnapped,WM_SNAP_QUERYSIZE, pCtx->dwFlags, (LPARAM)&size);

		T* pT = static_cast<T*>(this);
		RECT rcLayout;
		pT->QueryRect(rcLayout);

		POINT pos = {rcLayout.left + cxy, rcLayout.top + cxy};

		if (pCtx->dwFlags & snapLeft)
			pos.x = rcLayout.left;
		if (pCtx->dwFlags & snapTop)
			pos.y = rcLayout.top;
		if (pCtx->dwFlags & snapRight)
			pos.x = rcLayout.right - size.cx;
		if (pCtx->dwFlags & snapBottom)
			pos.y = rcLayout.bottom - size.cy;

		::SetParent(pCtx->hWndView, pCtx->hWndSnapped);
		::ScreenToClient(::GetParent(pCtx->hWndSnapped), &pos);
		::SetWindowPos(pCtx->hWndSnapped, NULL, pos.x, pos.y, size.cx, size.cy, SWP_NOZORDER);
		::SendMessage(pCtx->hWndSnapped, WM_SNAP_UPDATELAYOUT, dwFlags, 0);
		::ShowWindow(pCtx->hWndSnapped,SW_SHOWNORMAL);
		::ShowWindow(pCtx->hWndView,SW_SHOWNOACTIVATE);
	}

	void HideWindow(SNAPCONTEXT* pCtx)
	{
		T* pT = static_cast<T*>(this);
		if ( IsHidden(pCtx) )
			return;
		else if ( IsFloating(pCtx) )
			pT->UnfloatWindow(pCtx);
		else
			pT->UnsnapWindow(pCtx);
	}

	HWND           m_hWndClient;
	CSnapContainer m_snappableWindows;
	CSnapContainer m_snappedWindows;
	CSnapContainer m_floatedWindows;
};


class CSnappingWindow : public CSnappingWindowImpl<CSnappingWindow>
{
public:
	DECLARE_WND_CLASS_EX(_T("WTL_SnappingWindow"), CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, NULL)

	CSnappingWindow() : CSnappingWindowImpl<CSnappingWindow>()
	{ 
	}
};

#endif //__WTL_SNAPPABLE_H_