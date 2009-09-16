#pragma once

enum DataSourceKeyType
{
	DataSourceKeyType_ImageName,
	DataSourceKeyType_PID,
};

struct IDataSetting
{
	virtual size_t GetTotalBytes() const = 0;
	virtual int GetAddressOffset() const = 0;
};

struct DataSetting1D : public IDataSetting
{
	enum DataType
	{
		DataType_Char,
		DataType_UnsignedChar,
		DataType_Short,
		DataType_UnsignedShort,
		DataType_Int,
		DataType_UnsignedInt,
		DataType_LongLong,
		DataType_UnsignedLongLong,
		DataType_Float,
		DataType_Double,
		
		DataType_First	= DataType_Char,
		DataType_Last	= DataType_Double,
	};
	
	const std::type_info& GetTypeInfo() const;
	
	DataSetting1D();
	
	DataType	dataType;
	CString		countFormula;
	bool		viewAuto;
	CString		viewMinFormula;
	CString		viewMaxFormula;

	size_t GetTotalBytes() const;
	size_t GetElementSize() const;
	int GetAddressOffset() const { return 0; }
};

struct DataSetting2D : IDataSetting
{
	enum TargetStructureType
	{
		TargetStructureType_Manual,
		TargetStructureType_BITMAPINFOHEADER,
	};
	
	enum AddressedLine
	{
		AddressedLine_First,
		AddressedLine_Last,
	};
	
	enum ColorFormatType
	{
		ColorFormatType_B8G8R8,
		ColorFormatType_B8G8R8A8,
//		ColorFormatType_16F,
		ColorFormatType_32F,
		ColorFormatType_64F,
		ColorFormatType_1,
		ColorFormatType_8,
		ColorFormatType_16,
		ColorFormatType_32,

		ColorFormatType_Begin = ColorFormatType_B8G8R8,
		ColorFormatType_End = ColorFormatType_32,
	};

	TargetStructureType	targetStructureType;
	CString	structureAddressFormula;
	
	CString	widthFormula;
	CString	heightFormula;
	CString	lineOffsetFormula;
	AddressedLine	addressedLine;
	ColorFormatType	colorFormat;

	size_t GetTotalBytes() const;
	int GetAddressOffset() const;
};

struct DataSettingTEXT : IDataSetting
{
	CString bytesFormula;
	size_t GetTotalBytes() const;
	int GetAddressOffset() const;
};

struct ProcessSetting
{
	ProcessSetting();
	
	DataSourceKeyType	dataSourceKeyType;
	CString				imageName;
	DWORD				pid;
	CString				addressBaseFormula;
	CString				addressOffsetFormula;
	int					addressOffsetMultiplier;
	
	boost::shared_ptr<IDataSetting>	pDataSetting;
};

