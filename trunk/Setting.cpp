#include "stdafx.h"
#include "Setting.h"

#include "DataViewerCommon.h"

DataSetting1D::DataSetting1D()
{
	dataType = DataType_Int;
	viewAuto = true;
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

ProcessSetting::ProcessSetting()
{
	dataSourceKeyType = DataSourceKeyType_ImageName;
	imageName = "";
	pid = 0;
	addressOffsetMultiplier = 1;
}


size_t DataSetting2D::GetTotalBytes() const
{
	int height = abs(EvalFormula(heightFormula));
	int lineOffset = EvalFormula(lineOffsetFormula);
	return height * abs(lineOffset);
}

int DataSetting2D::GetAddressOffset() const
{
	int lineOffset = EvalFormula(lineOffsetFormula);
	int height = EvalFormula(heightFormula);
	if (addressedLine == AddressedLine_First) {
		if (lineOffset < 0) {
			return lineOffset * (height - 1);
		}else {
			return 0;
		}
	}else {
		return -1 * lineOffset * (height - 1);
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
