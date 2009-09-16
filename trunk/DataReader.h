#pragma once

#include <map>

template <typename NumericT, typename AsT>
struct DataReadFunc
{
	static void ReadDataAs(std::vector<NumericT>& values, const void* pData, size_t dataBytes)
	{
		AsT* pD = (AsT*) pData;
		size_t dataCnt = dataBytes / sizeof(AsT);
		values.resize(dataCnt);
		for (size_t i=0; i<dataCnt; ++i) {
			values[i] = *(pD + i);
		}
	}
};

// 配列のデータを指定した型の値として読み取る
template <typename NumericT>
struct DataReader
{
private:
	typedef fastdelegate::FastDelegate3<std::vector<NumericT>&, const void*, size_t> DataReadDelegate;
	typedef std::map<const std::type_info*, DataReadDelegate> DelegateMap;
	static DelegateMap delegateMap;

	template <typename T>
	static void InsertDelegate()
	{
		DataReadDelegate d;
		d.bind(&DataReadFunc<NumericT, T>::ReadDataAs);
		delegateMap.insert(
			std::make_pair(
				&typeid(T),
				d
			)
		);
	}

public:
	static void Init()
	{
		InsertDelegate<unsigned char>();
		InsertDelegate<unsigned short int>();
		InsertDelegate<unsigned int>();
		InsertDelegate<unsigned long long>();

		InsertDelegate<char>();
		InsertDelegate<short int>();
		InsertDelegate<int>();
		InsertDelegate<long long>();

		InsertDelegate<float>();
		InsertDelegate<double>();
	}

	static void ReadDataAs(const std::type_info* pTypeInfo, std::vector<NumericT>& values, const void* pData, size_t dataBytes)
	{
		DelegateMap::iterator it = delegateMap.find(pTypeInfo);
		if (it != delegateMap.end()) {
			DataReadDelegate& d = it->second;
			if (d)
				d(values, pData, dataBytes);
		}
	}
};

template <typename NumericT>
typename DataReader<NumericT>::DelegateMap DataReader<NumericT>::delegateMap;

