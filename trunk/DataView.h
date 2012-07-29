#pragma once

namespace gl {
class IBuffer2D;
}

class CDataView : public CWindowImpl<CDataView>
{
public:
	CDataView();
	~CDataView();

	DECLARE_WND_CLASS(NULL)
	
	void ProcessData();
	
	BOOL PreTranslateMessage(MSG* pMsg);
	struct ProcessSetting& GetSetting() { return *m_pSetting; }
	void ReadData(const ProcessSetting& setting);
	void ProcessData(const ProcessSetting& setting);
	
	BEGIN_MSG_MAP(CDataView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_SIZE(OnSize)
		MSG_WM_HSCROLL(OnHScroll)
		MSG_WM_VSCROLL(OnVScroll)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_MBUTTONDOWN(OnMButtonDown)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
		MSG_WM_KEYDOWN(OnKeyDown)
	END_MSG_MAP()
	
private:
	boost::shared_ptr<struct ProcessSetting> m_pSetting;
	std::vector<char> m_data;
	std::vector<double> m_values;
	
	gl::IBuffer2D* m_pImage;
	size_t m_imgWidth;
	size_t m_imgHeight;
	
	HBITMAP m_hBMP;
	BITMAPINFO m_bmi;
	void* m_pBits;
	CDC m_memDC;
	int m_scale;
	
	void ProcessAs1D();
	void ProcessAs2D();
	void ProcessAsTEXT();
	
	void Render(CPaintDC& dc);
	void Render1D(CPaintDC& dc);
	void Render2D(CPaintDC& dc);
	void RenderTEXT(CPaintDC& dc);

	void setScrollInfo();
	void setScale(int newScale);
	
	// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT OnDestroy(void);
	LRESULT OnEraseBkgnd(HDC hdc);
	LRESULT OnSize(UINT state, CSize Size);
	LRESULT OnHScroll(int code, short pos, HWND hwndCtl);
	LRESULT OnVScroll(int code, short pos, HWND hwndCtl);

	LRESULT OnLButtonDown(UINT Flags, CPoint Pt);
	LRESULT OnMButtonDown(UINT Flags, CPoint Pt);
	LRESULT OnRButtonDown(UINT Flags, CPoint Pt);
	LRESULT OnMouseWheel(UINT ControlCodes, short Distance, CPoint Pt);
	LRESULT OnKeyDown(TCHAR vk, UINT cRepeat, UINT flags);
};
