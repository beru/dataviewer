#include "stdafx.h"
#include "resource.h"

#include "DataView.h"
#include "Setting.h"

#include "winutil.h"

#include "gl/fixed.h"
#include "gl/lineDrawer_Nelson.h"
#include "gl/colorBlender.h"
#include "gl/windows.h"
#include "gl/clipper.h"
#include "gl/bitBlockTransfer.h"
#include "gl/colorConverter.h"

typedef gl::fixed<8, unsigned char> Fixed8;
typedef gl::fixed<16, unsigned short int> Fixed16;
typedef gl::fixed<32, unsigned int> Fixed32;
typedef gl::Color4< gl::ColorBGRA<Fixed8> > ColorB8G8R8A8;
typedef gl::Color3< gl::ColorBGR<Fixed8> > ColorB8G8R8;

#define STATIC_ASSERT(expr) { char dummy[(expr) ? 1 : 0]; }

template <typename T, typename T2>
T convert(T2 v)
{
	return T(v);
}

#pragma pack(push, 1 )
struct ColorB5G6R5
{
	uint16_t b : 5;
	uint16_t g : 6;
	uint16_t r : 5;
};
#pragma pack(pop)

template <typename T>
ColorB8G8R8A8 convert(ColorB5G6R5 from)
{
	STATIC_ASSERT(sizeof(ColorB5G6R5) == 2);
	ColorB8G8R8A8 to;
	to.r.value = from.r << 3;
	to.g.value = from.g << 2;
	to.b.value = from.b << 3;
	return to;
}

template <typename T>
ColorB8G8R8A8 convert(double from)
{
	ColorB8G8R8A8 to;
	uint8_t v = (from * 255.0 + 0.5);
	to.r.value = v;
	to.g.value = v;
	to.b.value = v;
	return to;
}

#include "MathUtil.h"

#include <typeinfo.h>
#include "DataReader.h"

#include "DataViewerCommon.h"

// typedef gl::fixed<20> NumericT;
typedef double NumericT;

enum { SLOPE_CORR_TABLE_SIZE = 512+1 };
NumericT g_slopeCorrTable[SLOPE_CORR_TABLE_SIZE];

enum { FILTER_TABLE_SIZE = 64*2 };
NumericT g_filterTable[FILTER_TABLE_SIZE];

bool CDataView::isMouseClicked()
{
	HWND hWndCap = GetCapture();
	if (hWndCap == m_hWnd) {
		const int chkBtn = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
		SHORT state = GetAsyncKeyState(chkBtn);
		bool bClicked = state < 0;
		return bClicked;
	}
	return false;
}

CDataView::CDataView()
	:
	m_pImage(0),
	m_imgWidth(0),
	m_imgHeight(0)
{
}

CDataView::~CDataView()
{
	delete m_pImage;
}


LRESULT CDataView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	DataReader<NumericT>::Init();
	
	CWindow wnd(GetDesktopWindow());
	HDC hDC = wnd.GetDC();
	int width = GetDeviceCaps(hDC, HORZRES) + 10;
	int height = GetDeviceCaps(hDC, VERTRES) + 10;
	wnd.ReleaseDC(hDC);
	// TopDown形式
	m_hBMP = CreateDIB32(width, -height, m_bmi, m_pBits);
	m_pImage = gl::BuildBuffer2DFromBMP(m_bmi.bmiHeader, m_pBits);

	HDC dc = GetDC();
	m_memDC.CreateCompatibleDC(dc);
	m_memDC.SetMapMode(GetMapMode(dc));
	ReleaseDC(dc);
	m_memDC.SelectBitmap(m_hBMP);
	
	gl::SetupSlopeCorrectionTable(g_slopeCorrTable, SLOPE_CORR_TABLE_SIZE);
	gl::SetupFilterTable(g_filterTable, FILTER_TABLE_SIZE, 0.75);
	
	m_pSrcSetting = boost::shared_ptr<ProcessSetting>(new ProcessSetting);
	m_pDataSetting = boost::shared_ptr<DataSetting1D>(new DataSetting1D);
	m_scale = 1.0;

	return 0;
}

LRESULT CDataView::OnDestroy(void)
{
	//You should call SetMsgHandled(FALSE) or set bHandled = FALSE for the main window of your application
	if (m_hBMP)
		DeleteObject(m_hBMP);

	return 0;
}

LRESULT CDataView::OnEraseBkgnd(HDC hdc)
{
	return TRUE; // background is erased
}


BOOL CDataView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

void CDataView::drawZoomRect(CDC& dc, CPoint pt0, CPoint pt1)
{
	dc.SetROP2(R2_XORPEN);
	::SelectObject(dc, GetStockObject(WHITE_PEN));
	::SelectObject(dc, GetStockObject(NULL_BRUSH));
	dc.Rectangle(pt0.x, pt0.y, pt1.x, pt1.y);
	m_prevDrawPts[0] = pt0;
	m_prevDrawPts[1] = pt1;
}

LRESULT CDataView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CRect updateRect;
	if (!GetUpdateRect(updateRect)) {
		return 0;
	}
	CPaintDC dc(m_hWnd);
	Render(dc);

	switch (m_mode) {
	case App::Mode_Zoom:
		if (m_bClicked) {
			drawZoomRect(dc, getMouseDownPtScrolled(), m_prevMousePt);
		}
	}
	return 0;
}

void CDataView::Render(CPaintDC& dc)
{
	const type_info& ti = typeid(*m_pDataSetting);
	if (ti == typeid(DataSetting1D)) {
		Render1D(dc);
	}else if (ti == typeid(DataSetting2D)) {
		Render2D(dc);
	}else if (ti == typeid(DataSettingTEXT)) {
		RenderTEXT(dc);
	}
}

void CDataView::ProcessData()
{
	if (m_data.size() == 0)
		return;
	const type_info& ti = typeid(*m_pDataSetting);
	if (ti == typeid(DataSetting1D)) {
		ProcessAs1D();
	}else if (ti == typeid(DataSetting2D)) {
		ProcessAs2D();
	}else if (ti == typeid(DataSettingTEXT)) {
		ProcessAsTEXT();
	}else {
		assert(false);
	}
}

void CDataView::ProcessAs1D()
{
	size_t dataBytes = m_data.size();
	const char* pData = &m_data[0];
	const DataSetting1D& dataSetting = dynamic_cast<const DataSetting1D&>(*m_pDataSetting);
	DataReader<NumericT>::ReadDataAs(&dataSetting.GetTypeInfo(), m_values, pData, dataBytes);
}

void CDataView::ProcessAs2D()
{
	size_t dataBytes = m_data.size();
	const char* pData = &m_data[0];
	
	const DataSetting2D& setting = dynamic_cast<const DataSetting2D&>(*m_pDataSetting);
	int width = EvalFormula(setting.widthFormula);
	if (width <= 0)
		return;
	int height = EvalFormula(setting.heightFormula);
	if (height <= 0)
		return;
	
	int pixelStride;
	size_t colorFormatPixelSize = GetByteSize(setting.colorFormat);
	if (setting.bUsePixelStride) {
		pixelStride = EvalFormula(setting.pixelStrideFormula);
		if (pixelStride <= 0 || pixelStride < colorFormatPixelSize) {
			return;
		}
	}else {
		pixelStride = colorFormatPixelSize;
	}
	
	size_t lineBytes = 0;
	if (setting.colorFormat == ColorFormatType_1) {
		lineBytes = width / 8 + ((width % 8) ? 1 : 0);
	}else {
		lineBytes = width * pixelStride;
	}

	int lineStride = setting.bUseLineStride ? EvalFormula(setting.lineStrideFormula) : lineBytes;
	if (lineStride == 0)
		return;
	
	if (abs(lineStride) < lineBytes)
		return;
	if (abs(lineStride)*height != dataBytes)
		return;
	
	const char* pFirstLine = NULL;
	switch (setting.addressedLine) {
	case DataSetting2D::AddressedLine_Last:
	case DataSetting2D::AddressedLine_First:
		if (lineStride < 0) {
			pFirstLine = pData + -lineStride * (int)(height - 1);
		}else {
			pFirstLine = pData;
		}
		break;
	}
	if (m_pImage) {
		if (m_pImage->GetWidth() < width || m_pImage->GetHeight() < height) {
			DeleteObject(m_hBMP);
			delete m_pImage;
			m_pImage = 0;
		}
	}
	if (!m_pImage) {
		m_hBMP = CreateDIB32(width, -height, m_bmi, m_pBits);
		m_pImage = gl::BuildBuffer2DFromBMP(m_bmi.bmiHeader, m_pBits);
		m_memDC.SelectBitmap(m_hBMP);
	}
	m_imgWidth = width;
	m_imgHeight = height;
	gl::Buffer2D<ColorB8G8R8A8>& img = *(gl::Buffer2D<ColorB8G8R8A8>*)m_pImage;
	
	if (setting.colorFormat != ColorFormatType_1 && IsSingleComponent(setting.colorFormat)) {
		gl::ColorConverterMinMax<ColorB8G8R8A8> colorConverter(EvalFormula(setting.minimumFormula), EvalFormula(setting.maximumFormula));
		switch (setting.colorFormat) {
		case ColorFormatType_F32:
			gl::BitBlockTransfer(gl::Buffer2D<float>(width, height, lineStride, (void*)pFirstLine), pixelStride, img, 0,0,0,0,width,height, colorConverter);
			break;
		case ColorFormatType_F64:
			gl::BitBlockTransfer(gl::Buffer2D<double>(width, height, lineStride, (void*)pFirstLine), pixelStride, img, 0,0,0,0,width,height, colorConverter);
			break;
		case ColorFormatType_U8:
			gl::BitBlockTransfer(gl::Buffer2D<uint8_t>(width, height, lineStride, (void*)pFirstLine), pixelStride, img, 0,0,0,0,width,height, colorConverter);
			break;
		case ColorFormatType_U16:
			gl::BitBlockTransfer(gl::Buffer2D<uint16_t>(width, height, lineStride, (void*)pFirstLine), pixelStride, img, 0,0,0,0,width,height, colorConverter);
			break;
		case ColorFormatType_U32:
			gl::BitBlockTransfer(gl::Buffer2D<uint32_t>(width, height, lineStride, (void*)pFirstLine), pixelStride, img, 0,0,0,0,width,height, colorConverter);
			break;
		case ColorFormatType_S8:
			gl::BitBlockTransfer(gl::Buffer2D<int8_t>(width, height, lineStride, (void*)pFirstLine), pixelStride, img, 0,0,0,0,width,height, colorConverter);
			break;
		case ColorFormatType_S16:
			gl::BitBlockTransfer(gl::Buffer2D<int16_t>(width, height, lineStride, (void*)pFirstLine), pixelStride, img, 0,0,0,0,width,height, colorConverter);
			break;
		case ColorFormatType_S32:
			gl::BitBlockTransfer(gl::Buffer2D<int32_t>(width, height, lineStride, (void*)pFirstLine), pixelStride, img, 0,0,0,0,width,height, colorConverter);
			break;
		}
	}else {
		gl::ColorConverter<ColorB8G8R8A8> colorConverter;
		switch (setting.colorFormat) {
		case ColorFormatType_1:
			gl::BitBlockTransfer(gl::Buffer2D<bool>(width, height, lineStride, (void*)pFirstLine), img, 0,0,0,0,width,height, colorConverter);
			break;
		case ColorFormatType_B5G6R5:
			gl::BitBlockTransfer(gl::Buffer2D<ColorB5G6R5>(width, height, lineStride, (void*)pFirstLine), pixelStride, img, 0,0,0,0,width,height, colorConverter);
			break;
		case ColorFormatType_B8G8R8:
			gl::BitBlockTransfer(gl::Buffer2D<ColorB8G8R8>(width, height, lineStride, (void*)pFirstLine), pixelStride, img, 0,0,0,0,width,height, colorConverter);
			break;
		case ColorFormatType_B8G8R8A8:
			gl::BitBlockTransfer(gl::Buffer2D<ColorB8G8R8A8>(width, height, lineStride, (void*)pFirstLine), pixelStride, img, 0,0,0,0,width,height, colorConverter);
			break;
		}
	}

}

void CDataView::ProcessAsTEXT()
{
	// バイトそのままなので何もしない
}

void CDataView::Render1D(CPaintDC& dc)
{
	if (m_values.size() == 0) {
		return;
	}
	const std::vector<double>& values = m_values;
	const DataSetting1D& dataSetting = dynamic_cast<const DataSetting1D&>(*m_pDataSetting);
	size_t dataCount = dataSetting.GetTotalBytes() / dataSetting.GetElementSize();
	
	double min = 0;
	double max = 0;
	if (dataSetting.viewAuto) {
		min = *std::min_element(values.begin(), values.begin()+dataCount);
		max = *std::max_element(values.begin(), values.begin()+dataCount);
	}else {
		min = EvalFormula(dataSetting.viewMinFormula);
		max = EvalFormula(dataSetting.viewMaxFormula);
	}
	
	CRect rect;
	GetClientRect(rect);
	rect.DeflateRect(60, 20, 20, 60);
	if (rect.bottom < 2 || rect.left < 2)
		return;
	
	double xa, xb, ya, yb;
	SolveSimultaneousLinearEquations<double>(
		0, rect.left,
		dataCount, rect.right,
		xa, xb
	);
	SolveSimultaneousLinearEquations<double>(
		min, rect.bottom,
		max, rect.top,
		ya, yb
	);
	
	typedef gl::fixed<8, unsigned char> ComponentT;
	typedef gl::Color4< gl::ColorBGRA<ComponentT> > ColorT;
	typedef gl::AdditiveColorBlender<ColorT, NumericT> ColorBlenderT;
	typedef gl::LineDrawer_Nelson<NumericT, ColorT, ColorBlenderT, SLOPE_CORR_TABLE_SIZE, FILTER_TABLE_SIZE> LineDrawerT;


	CRect updateRect = dc.m_ps.rcPaint;
	CRect unionRect;
	::UnionRect(unionRect, rect, updateRect);

	gl::Clipper<NumericT> clipper;
	clipper.SetClipRegion(unionRect.left, unionRect.top, unionRect.right, unionRect.bottom);

	gl::Buffer2D<ColorT>* pBuffer = (gl::Buffer2D<ColorT>*) gl::BuildBuffer2DFromBMP(m_bmi.bmiHeader, m_pBits);
	std::auto_ptr<gl::Buffer2D<ColorT> > autoDeleter(pBuffer);

	gl::Buffer2D_Fill(*pBuffer, ColorT(0));

	LineDrawerT ld(pBuffer);
	ld.SetFilterTable(g_filterTable);
	ld.SetSlopeCorrectionTable(g_slopeCorrTable);

	ColorT col(0.9,0.9,0.9);

	// frame
	ld.DrawLine(col, rect.left, rect.top, rect.right, rect.top);
	ld.DrawLine(col, rect.left, rect.bottom, rect.right, rect.bottom);
	ld.DrawLine(col, rect.left, rect.top, rect.left, rect.bottom);
	ld.DrawLine(col, rect.right, rect.top, rect.right, rect.bottom);

	// axis
	if (rect.bottom <= yb && yb <= rect.top) {
		ld.DrawLine(col, xb, yb, xa*dataCount+xb, yb);
	}
	
	// grid

	// lines
	double x1 = xb;
	double y1 = ya*values[0]+yb;
	for (size_t i=1; i<dataCount; ++i) {
		double x = xa*i+xb;
		double y = ya*values[i]+yb;
		double tx1 = x1;
		double ty1 = y1;
		double tx = x;
		double ty = y;
		int clipped = clipper.ClipLinePoint(&tx1, &ty1, &tx, &ty);
		if (clipped != -1)
			ld.DrawLine(col, tx1, ty1, tx, ty);
		x1 = x;
		y1 = y;
	}
	
	dc.BitBlt(
		updateRect.left,
		updateRect.top,
		updateRect.Width(),
		updateRect.Height(),
		m_memDC,
		updateRect.left,
		updateRect.top,
		SRCCOPY
	);
	// ruler
}

void CDataView::Render2D(CPaintDC& dc)
{
	CRect updateRect = dc.m_ps.rcPaint;


	int w = updateRect.Width();
	int h = updateRect.Height();

	int tx = updateRect.left;
	int ty = updateRect.top;
	
	int sx = std::max(0, GetScrollPos(SB_HORZ)) + tx;
	int sy = std::max(0, GetScrollPos(SB_VERT)) + ty;

#if 1
	dc.StretchBlt(
		tx, ty, w, h,
		m_memDC, sx/m_scale, sy/m_scale, w/m_scale, h/m_scale,
		SRCCOPY
		);
#else
	dc.BitBlt(
		tx,
		ty,
		w,
		h,
		m_memDC,
		sx,
		sy,
		SRCCOPY
	);
#endif
}

void CDataView::RenderTEXT(CPaintDC& dc)
{
	if (m_values.size() == 0) {
		return;
	}
	const DataSettingTEXT& dataSetting = dynamic_cast<const DataSettingTEXT&>(*m_pDataSetting);
//	m_memDC.TextOut(10, 10, _T("test"), -1);
	size_t nBytes = dataSetting.GetTotalBytes();
	size_t nLines = nBytes / 16 + ((nBytes % 16) ? 1 : 0);
	m_memDC.FillSolidRect(0, 0, 600, nLines*30, RGB(0,0,0));
	TCHAR buff[32] = {0};
	for (size_t y=0; y<nLines; ++y) {
		size_t start = y * 16;
		size_t end = std::min(start+16, nBytes);
		size_t len = end - start;
		for (size_t x=0; x<len; ++x) {
			unsigned char byte = (unsigned char) m_data[start+x];
			_stprintf(buff, _T("%02X"), byte);
			m_memDC.TextOut(x*30, y*30, buff, -1);
		}
	}
}

void CDataView::setScrollInfo(int hPos, int vPos)
{
	if (typeid(*m_pDataSetting) == typeid(DataSetting2D) && m_pImage) {
		BOOL bRedraw = TRUE;
		CSize Size;
		CRect rec;
		GetClientRect(rec);
		Size.cx = rec.Width();
		Size.cy = rec.Height();

		SCROLLINFO si;
		si.fMask = SIF_RANGE|SIF_PAGE|SIF_POS;
		GetScrollInfo(SB_HORZ, &si);
		si.nPos = hPos;
		si.nPage = Size.cx;
		si.nMin = 0;
		si.nMax = m_imgWidth * m_scale;
		SetScrollInfo(SB_HORZ, &si, bRedraw);

		GetScrollInfo(SB_VERT, &si);
		si.nPos = vPos;
		si.nPage = Size.cy;
		si.nMin = 0;
		si.nMax = m_imgHeight * m_scale;
		SetScrollInfo(SB_VERT, &si, bRedraw);
		Invalidate();
	}

}

LRESULT CDataView::OnSize(UINT state, CSize Size)
{
	if (IsWindow() && Size.cx != 0 && Size.cy != 0) {
		CPoint pt = getScrollPt();
		setScrollInfo(pt.x, pt.y);
//		Invalidate();
//		UpdateWindow();
	}
	return 0;
}

static
bool fetchFileData(
	const FileSetting& fileSetting,
	size_t dataLength, int dataAddressOffset,
	void* buffer
	)
{
	FILE* f = _wfopen(fileSetting.filePath, L"rb");
	if (!f) {
		return false;
	}
	fseek(f, dataAddressOffset, SEEK_SET);
	fread(buffer, 1, dataLength, f);
	fclose(f);
	return true;
}

static
bool fetchProcessData(
	const ProcessSetting& processSetting,
	size_t dataLength, int address,
	void* buffer
	)
{
	LPCVOID pTarget = (LPVOID) address;
	bool ret = false;
	switch (processSetting.dataSourceKeyType) {
	case DataSourceKeyType_ImageName:
		if (_tcslen(processSetting.imageName) == 0) {
			return false;
		}
		ret = ReadProcessData(processSetting.imageName, pTarget, buffer, dataLength);
		break;
	case DataSourceKeyType_PID:
		ret = ReadProcessData(processSetting.pid, pTarget, buffer, dataLength);
		break;
	}
	return ret;
}

static
bool fetchData(
	const boost::shared_ptr<SourceSetting>& pSrcSetting,
	size_t dataLength,
	std::vector<char>& data
   )
{
	if (dataLength == 0)
		return false;
	data.resize(dataLength);
	void* buffer = &data[0];

	ptrdiff_t addressBase = AddressHexStrToNum(pSrcSetting->addressBaseFormula);
	ptrdiff_t addressOffset = EvalFormula(pSrcSetting->addressOffsetFormula) * pSrcSetting->addressOffsetMultiplier;
	ptrdiff_t address = addressBase + addressOffset;

	const type_info& ti = typeid(*pSrcSetting);
	if (ti == typeid(ProcessSetting)) {
		fetchProcessData(
			*((const ProcessSetting*)pSrcSetting.get()),
			dataLength,
			address,
			buffer
			);
	}else if (ti == typeid(FileSetting)) {
		fetchFileData(
			*((const FileSetting*)pSrcSetting.get()),
			dataLength,
			address,
			buffer
			);
	}else {
		assert(false);
	}


	return true;
}

void CDataView::ReadData(
	const boost::shared_ptr<SourceSetting>& pSrcSetting,
	boost::shared_ptr<IDataSetting>& pDataSetting)
{
	m_pSrcSetting = pSrcSetting;
	m_pDataSetting = pDataSetting;
	if (
		fetchData(
			pSrcSetting,
			pDataSetting->GetTotalBytes(),
			m_data
		)
	) {
		ProcessData();
		Invalidate();
		UpdateWindow();
	}
}

void CDataView::ProcessData(
	const boost::shared_ptr<SourceSetting>& pSrcSetting,
	boost::shared_ptr<IDataSetting>& pDataSetting)
{
	m_pSrcSetting = pSrcSetting;
	m_pDataSetting = pDataSetting;
	ProcessData();
	Invalidate();
	UpdateWindow();
}

void CDataView::ZoomIn()
{
	setScale(m_scale*2.0);
}

void CDataView::ZoomOut()
{
	setScale(m_scale*0.5);
}

void CDataView::ZoomReset()
{
	setScale(1.0);
}

LRESULT CDataView::OnHScroll(int code, short pos, HWND hwndCtl)
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS|SIF_PAGE;
	GetScrollInfo(SB_HORZ, &si);
	int newPos = -1;
	switch (code) {
	case SB_LEFT:
		newPos = 0;
		break;
	case SB_RIGHT:
		newPos = si.nMax;
		break;
	case SB_LINELEFT:
		newPos = si.nPos - 1;
		break;
	case SB_LINERIGHT:
		newPos = si.nPos + 1;
		break;
	case SB_PAGELEFT:
		newPos = si.nPos - si.nPage;
		break;
	case SB_PAGERIGHT:
		newPos = si.nPos + si.nPage;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		newPos = pos;
		break;
	}
	if (newPos != -1) {
		if (m_scale == 1.0) {
			ScrollWindowEx(si.nPos - newPos, 0, 0, 0, 0, 0, SW_INVALIDATE);
		}else {
			Invalidate();
		}
		si.nPos = newPos;
		SetScrollInfo(SB_HORZ, &si);
	}
	return 0;
}

LRESULT CDataView::OnVScroll(int code, short pos, HWND hwndCtl)
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS|SIF_PAGE;
	GetScrollInfo(SB_VERT, &si);
	int newPos = -1;
	switch (code) {
	case SB_LEFT:
		newPos = 0;
		break;
	case SB_RIGHT:
		newPos = si.nMax;
		break;
	case SB_LINELEFT:
		newPos = si.nPos - 1;
		break;
	case SB_LINERIGHT:
		newPos = si.nPos + 1;
		break;
	case SB_PAGELEFT:
		newPos = si.nPos - si.nPage;
		break;
	case SB_PAGERIGHT:
		newPos = si.nPos + si.nPage;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		newPos = pos;
		break;
	}
	if (newPos != -1) {
		if (m_scale == 1.0) {
			ScrollWindowEx(0, si.nPos - newPos, 0, 0, 0, 0, SW_INVALIDATE);
		}else {
			Invalidate();
		}
		si.nPos = newPos;
		SetScrollInfo(SB_VERT, &si);
	}
	return 0;
}

CPoint CDataView::getScrollPt()
{
	CPoint r;
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	GetScrollInfo(SB_HORZ, &si);
	r.x = si.nPos;
	GetScrollInfo(SB_VERT, &si);
	r.y = si.nPos;
	return r;
}

CPoint CDataView::getMouseDownPtScrolled()
{
	return m_mouseDownPt + (m_scrollPosAtMouseDown - getScrollPt());
}

LRESULT CDataView::OnLButtonDown(UINT Flags, CPoint Pt)
{
	SetFocus();
	
	m_mode = g_app.mode;
	if (GetKeyState(VK_SPACE) < 0) {
		m_mode = App::Mode_Hand;
	}

	switch (m_mode) {
	case App::Mode_Hand:
	case App::Mode_Zoom:
		m_bClicked = true;
		m_mouseDownPt = Pt;
		m_prevMousePt = Pt;
		m_scrollPosAtMouseDown = getScrollPt();
		SetCapture();
		break;
	}
	return 0;
}

LRESULT CDataView::OnLButtonUp(UINT Flags, CPoint Pt)
{
	switch (m_mode) {
	case App::Mode_Hand:
	case App::Mode_Zoom:
		ReleaseCapture();
		break;
	}
	return 0;
}

LRESULT CDataView::OnCaptureChanged(HWND NewCaptureOwner)
{
	if (m_bClicked) {
		switch (m_mode) {
		case App::Mode_Zoom:
			CClientDC dc(m_hWnd);
			CPoint startPt = getMouseDownPtScrolled();
			CPoint endPt = m_prevMousePt;
			drawZoomRect(dc, startPt, endPt);
			m_prevMousePt = m_mouseDownPt;

			CPoint diffPt = startPt - endPt;
			if (abs(diffPt.x) < 20 || abs(diffPt.y) < 20) {
				break;
			}

			startPt += getScrollPt();
			startPt.x /= m_scale;
			startPt.y /= m_scale;
			endPt += getScrollPt();
			endPt.x /= m_scale;
			endPt.y /= m_scale;

			if (startPt.x > endPt.x) {
				std::swap(startPt.x, endPt.x);
			}
			if (startPt.y > endPt.y) {
				std::swap(startPt.y, endPt.y);
			}

			CRect rec;
			GetClientRect(rec);
			// alway right & bottom ??
			rec.right -= GetSystemMetrics( SM_CXVSCROLL );
			rec.bottom -= GetSystemMetrics( SM_CYHSCROLL );
			
			diffPt = endPt - startPt;
			// 長い辺を基準にする
			if (endPt.x-startPt.x >= endPt.y-startPt.y) {
				setZoomWindow(rec.Width() / (double)diffPt.x, startPt.x, startPt.y);
			}else {
				setZoomWindow(rec.Height() / (double)diffPt.y, startPt.x, startPt.y);
			}
			break;
		}
		m_bClicked = false;
	}
	return 0;
}

LRESULT CDataView::OnMButtonDown(UINT Flags, CPoint Pt)
{
	SetFocus();
	return 0;
}

LRESULT CDataView::OnRButtonDown(UINT Flags, CPoint Pt)
{
	SetFocus();
	return 0;
}

LRESULT CDataView::OnMouseWheel(UINT ControlCodes, short Distance, CPoint Pt)
{
	if (GetKeyState(VK_CONTROL) < 0) {
		if (Distance > 0) {
			setScale(m_scale+1.0);
		}else {
			setScale(m_scale-1.0);
		}
	}else {
		SCROLLINFO si;
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS|SIF_PAGE;
		GetScrollInfo(SB_VERT, &si);
		int offset = (Distance < 0 ) ? 1 : -1;
		const int max = abs(Distance) / WHEEL_DELTA;
		si.nPos += offset*max*40;
		SetScrollInfo(SB_VERT, &si);
		Invalidate();
		UpdateWindow();
	}
	return 0;
}

LRESULT CDataView::OnMouseMove(UINT Flags, CPoint Pt)
{
	if (isMouseClicked()) {
		switch (m_mode) {
		case App::Mode_Hand:
			{
				CPoint diffPt = Pt - m_prevMousePt;
				SCROLLINFO si;
				si.cbSize = sizeof(si);
				si.fMask = SIF_POS;
				GetScrollInfo(SB_HORZ, &si);
				si.nPos -= diffPt.x;
				SetScrollInfo(SB_HORZ, &si);
				GetScrollInfo(SB_VERT, &si);
				si.nPos -= diffPt.y;
				SetScrollInfo(SB_VERT, &si);
				Invalidate();
				UpdateWindow();
			}
			break;
		case App::Mode_Zoom:
			CClientDC dc(m_hWnd);
			CPoint startPt = getMouseDownPtScrolled();
			drawZoomRect(dc, startPt, m_prevMousePt);
			drawZoomRect(dc, startPt, Pt);
//			dc.MoveTo(m_mouseDownPt);
//			dc.LineTo(Pt);
		}
	}
	m_prevMousePt = Pt;
	return 0;
}

inline
double fixZoomScale(double scale)
{
	return std::max(1.0, std::min(scale, 128.0));
}

void CDataView::setZoomWindow(double newScale, int hPos, int vPos)
{
	m_scale = fixZoomScale(newScale);
	setScrollInfo(hPos*m_scale, vPos*m_scale);
}

void CDataView::setScale(double newScale)
{
	newScale = fixZoomScale(newScale);
	CRect rec;
	GetClientRect(rec);
	CPoint pt = getScrollPt();
	double ratio;
	if (newScale < m_scale) {
		ratio = 1.0 - newScale/m_scale;
		CPoint offset(rec.Width()*ratio, rec.Height()*ratio);
		pt.x = pt.x / (m_scale/newScale) - offset.x/2;
		pt.y = pt.y / (m_scale/newScale) - offset.y/2;
	}else {
		ratio = 1.0 - m_scale/newScale;
		CPoint offset(rec.Width()*ratio, rec.Height()*ratio);
		pt.x = (pt.x + offset.x/2) / (m_scale/newScale);
		pt.y = (pt.y + offset.y/2) / (m_scale/newScale);
	}
//	pt += offset;
	m_scale = newScale;
	setScrollInfo(pt.x, pt.y);
}

LRESULT CDataView::OnKeyDown(TCHAR vk, UINT cRepeat, UINT flags)
{
	switch (vk) {
	case VK_ADD:
	case VK_OEM_PLUS:
		ZoomIn();
		break;
	case VK_SUBTRACT:
	case VK_OEM_MINUS:
		ZoomOut();
		break;
	}
	return 0;
}

void CDataView::CopyImage()
{
	if (!m_imgWidth || !m_imgHeight) {
		return;
	}

	if (!::OpenClipboard(NULL)) {
		return;
	}

	HGLOBAL hGlobal = GlobalAlloc(GHND | GMEM_SHARE,
            sizeof(BITMAPINFOHEADER) + m_imgWidth * m_imgHeight * sizeof(DWORD));

	BITMAPINFOHEADER* pBmpInfoHeader = (BITMAPINFOHEADER *)GlobalLock(hGlobal);
	ZeroMemory(pBmpInfoHeader, sizeof(BITMAPINFOHEADER));
	pBmpInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
	pBmpInfoHeader->biWidth = m_imgWidth;
	pBmpInfoHeader->biHeight = m_imgHeight;	// TopDown failed..
	pBmpInfoHeader->biPlanes = 1;
	pBmpInfoHeader->biBitCount = 32;
	pBmpInfoHeader->biCompression = BI_RGB;

	DWORD* pDst = (DWORD*) (pBmpInfoHeader + 1);
	DWORD* pSrc = (DWORD*) m_pImage->GetPixelVoidPtr(0, m_imgHeight-1);
	int srcLineStride = m_pImage->GetLineStride();
	for (size_t y=0; y<m_imgHeight; ++y) {
		memcpy(pDst, pSrc, m_imgWidth*4);
		pDst += m_imgWidth;
		OffsetPtr(pSrc, -srcLineStride);
	}
	
	GlobalUnlock(hGlobal);

	::EmptyClipboard();
	::SetClipboardData(CF_DIB, hGlobal);
	::CloseClipboard();
}
