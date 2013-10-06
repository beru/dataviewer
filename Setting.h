#pragma once

#pragma pack(push,1)

struct SourceSetting
{
	virtual ~SourceSetting() {}
	TCHAR	addressBaseFormula[128];
	TCHAR	addressOffsetFormula[128];
	int		addressOffsetMultiplier;
};

enum DataSourceKeyType
{
	DataSourceKeyType_ImageName,
	DataSourceKeyType_PID,
};

struct ProcessSetting : public SourceSetting
{
	ProcessSetting();
	
	DataSourceKeyType	dataSourceKeyType;
	TCHAR				imageName[128];
	DWORD				pid;
};

struct FileSetting : public SourceSetting
{
	TCHAR	filePath[1024];
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
	TCHAR		countFormula[128];
	bool		viewAuto;
	TCHAR		viewMinFormula[128];
	TCHAR		viewMaxFormula[128];

	size_t GetTotalBytes() const;
	size_t GetElementSize() const;
	int GetAddressOffset() const { return 0; }
};

enum ColorFormatType
{
	ColorFormatType_B5G6R5,
	ColorFormatType_B8G8R8,
	ColorFormatType_B8G8R8A8,
//		ColorFormatType_16F,
	ColorFormatType_F32,
	ColorFormatType_F64,
	ColorFormatType_1,
	ColorFormatType_U8,
	ColorFormatType_U16,
	ColorFormatType_U32,
	ColorFormatType_S8,
	ColorFormatType_S16,
	ColorFormatType_S32,

	ColorFormatType_Begin = 0,
	ColorFormatType_End = ColorFormatType_S32,
};

size_t GetByteSize(ColorFormatType t);
bool IsSingleComponent(ColorFormatType t);

struct DataSetting2D : public IDataSetting
{
	enum AddressedLine
	{
		AddressedLine_First,
		AddressedLine_Last,
	};
	
	TCHAR	widthFormula[128];
	TCHAR	heightFormula[128];
	bool	bUsePixelStride;
	TCHAR	pixelStrideFormula[128];
	bool	bUseLineStride;
	TCHAR	lineStrideFormula[128];
	AddressedLine	addressedLine;
	ColorFormatType	colorFormat;
	TCHAR	minimumFormula[128];
	TCHAR	maximumFormula[128];

	size_t GetTotalBytes() const;
	int GetAddressOffset() const;
};

struct DataSettingTEXT : public IDataSetting
{
	TCHAR	bytesFormula[128];
	size_t GetTotalBytes() const;
	int GetAddressOffset() const;
};

#pragma pack(pop)
