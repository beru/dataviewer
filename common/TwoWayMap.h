#pragma once

#include <map>

template <typename AT, typename BT>
struct TwoWayMap
{
public:
	BT& operator [] (const AT& a)
	{
		std::map<AT, BT>::iterator it = ab_.find(a);
		if (it == ab_.end())
			return outB_;
		else
			return it->second;
	}

	AT& operator [] (const BT& b)
	{
		std::map<BT, AT>::iterator it = ba_.find(b);
		if (it == ba_.end())
			return outA_;
		else
			return it->second;
	}
protected:
	void map(const AT& a, const BT& b)
	{
		ab_[a] = b;
		ba_[b] = a;
	}

	void setOutOfMap(const AT& a, const BT& b)
	{
		outA_ = a;
		outB_ = b;
	}
	
	std::map<AT, BT> ab_;
	std::map<BT, AT> ba_;
	AT outA_;
	BT outB_;
};

