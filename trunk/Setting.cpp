#include "stdafx.h"
#include "Setting.h"

#include "DataViewerCommon.h"

DataSetting1D::DataSetting1D()
{
	dataType = DataType_Int;
	viewAuto = true;
	_tcscpy(countFormula, _T(""));
	_tcscpy(viewMinFormula, _T(""));
	_tcscpy(viewMaxFormula, _T(""));
}

std::map<DataSetting1D::DataType, const std::type_info*>	typeInfoMap;
std::map<DataSetting1D::DataType, size_t>					typeSizeMap;

void InitMap()
{
	typeInfoMap[DataSetting1D::DataType_Char]				= &typeid(char);
	typeInfoMap[DataSetting1D::DataType_UnsignedChar]		= &typeid(unsigned char);
	typeInfoMap[DataSetting1D::DataType_Short]				= &typeid(short);
	typeInfoMap[DataSetting1D::DataType_UnsignedShort]		= &typeid(unsigned short);
	typeInfoMap[DataSetting1D::DataType_Int]				= &typeid(int);
	typeInfoMap[DataSetting1D::DataType_UnsignedInt]		= &typeid(unsigned int);
	typeInfoMap[DataSetting1D::DataType_LongLong]			= &typeid(long long);
	typeInfoMap[DataSetting1D::DataType_UnsignedLongLong]	= &typeid(unsigned long long);
	typeInfoMap[DataSetting1D::DataType_Float]				= &typeid(float);
	typeInfoMap[DataSetting1D::DataType_Double]				= &typeid(double);
	
	typeSizeMap[DataSetting1D::DataType_Char]				= sizeof(char);
	typeSizeMap[DataSetting1D::DataType_UnsignedChar]		= sizeof(unsigned char);
	typeSizeMap[DataSetting1D::DataType_Short]				= sizeof(short);
	typeSizeMap[DataSetting1D::DataType_UnsignedShort]		= sizeof(unsigned short);
	typeSizeMap[DataSetting1D::DataType_Int]				= sizeof(int);
	typeSizeMap[DataSetting1D::DataType_UnsignedInt]		= sizeof(unsigned int);
	typeSizeMap[DataSetting1D::DataType_LongLong]			= sizeof(long long);
	typeSizeMap[DataSetting1D::DataType_UnsignedLongLong]	= sizeof(unsigned long long);
	typeSizeMap[DataSetting1D::DataType_Float]				= sizeof(float);
	typeSizeMap[DataSetting1D::DataType_Double]				= sizeof(double);
}

struct Dummy
{
	Dummy()
	{
		InitMap();
	}
};
Dummy d;

size_t DataSetting1D::GetTotalBytes() const
{
	return GetElementSize() * (int) EvalFormula(countFormula);
}

size_t DataSetting1D::GetElementSize() const
{
	return typeSizeMap[dataType];
}

const std::type_info& DataSetting1D::GetTypeInfo() const
{
	return *typeInfoMap[dataType];
}

size_t DataSetting2D::GetTotalBytes() const
{
	int width = abs(EvalFormula(widthFormula));
	int height = abs(EvalFormula(heightFormula));
	int lineStride = bUseLineStride ? EvalFormula(lineStrideFormula) : (width * GetByteSize(colorFormat));
	return height * abs(lineStride);
}

int DataSetting2D::GetAddressOffset() const
{
	if (!bUseLineStride) {
		return 0;
	}
	int lineStride = EvalFormula(lineStrideFormula);
	int height = EvalFormula(heightFormula);
	if (addressedLine == AddressedLine_First) {
		if (lineStride < 0) {
			return lineStride * (height - 1);
		}else {
			return 0;
		}
	}else {
		return -1 * lineStride * (height - 1);
	}
}


size_t DataSettingTEXT::GetTotalBytes() const
{
	return EvalFormula(bytesFormula);
}

int DataSettingTEXT::GetAddressOffset() const
{
	return 0;
}

ProcessSetting::ProcessSetting()
{
	dataSourceKeyType = DataSourceKeyType_ImageName;
	_tcscpy(imageName, _T(""));
	_tcscpy(addressBaseFormula, _T(""));
	_tcscpy(addressOffsetFormula, _T(""));
	pid = 0;
	addressOffsetMultiplier = 1;
}

size_t GetByteSize(ColorFormatType t)
{
	switch (t) {
	case ColorFormatType_B5G6R5:
		return 2;
	case ColorFormatType_B8G8R8:
		return 3;
	case ColorFormatType_B8G8R8A8:
	case ColorFormatType_F32:
		return 4;
	case ColorFormatType_F64:
		return 8;
	case ColorFormatType_1:
		return 0;
		break;
	case ColorFormatType_U8:
	case ColorFormatType_S8:
		return 1;
	case ColorFormatType_U16:
	case ColorFormatType_S16:
		return 2;
	case ColorFormatType_U32:
	case ColorFormatType_S32:
		return 4;
	default:
		return 0;
	}
}

bool IsSingleComponent(ColorFormatType t)
{
	switch (t) {
	case ColorFormatType_B5G6R5:
	case ColorFormatType_B8G8R8:
	case ColorFormatType_B8G8R8A8:
		return false;
	case ColorFormatType_F32:
	case ColorFormatType_F64:
	case ColorFormatType_1:
	case ColorFormatType_U8:
	case ColorFormatType_U16:
	case ColorFormatType_U32:
	case ColorFormatType_S8:
	case ColorFormatType_S16:
	case ColorFormatType_S32:
		return true;
	default:
		return false;
	}
}