#ifndef EXPHASHMAP_H
#define EXPHASHMAP_H

// ExpandableHashMap.h
// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.
#include <list>
#include <vector>
#include <iterator> // for iterators 
#include <functional>
//#include <iostream>

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
	ExpandableHashMap(double maximumLoadFactor = 0.5);
	~ExpandableHashMap();
	void reset();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	ExpandableHashMap(const ExpandableHashMap&) = delete;
	ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
	struct Pair
	{
		Pair(KeyType k, ValueType v)
		{
			s_key = k; s_value = v;
		}
		KeyType s_key;
		ValueType s_value;
	};


	unsigned int MapIndexGenerator(const KeyType& key) const;
	bool checkRehash();
	double calcLoad();
	double m_MaxLoadfac;
	int m_nItemsAdded;
	std::vector<std::list<Pair>> m_HT;
	
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType,ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
	m_MaxLoadfac = maximumLoadFactor;
	m_nItemsAdded = 0;
	for (int i = 0; i < 8; i++)
	{
		m_HT.push_back(std::list<Pair>());
	}
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType, ValueType>::~ExpandableHashMap()
{
	//for (int i = 0; i < m_HT.size(); i++)
	//{
	//	if (m_HT[i].empty() == false)
	//	{
	//		delete m_HT[i];
	//		std::
	//	}
	//}
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
	for (int i = 0; i < m_HT.size(); i++)
	{
		if (m_HT[i].empty() == false)
		{
			m_HT[i].clear();
		}
	}
	m_HT.clear();
	for (int i = 0; i < 8; i++)
	{
		m_HT.push_back(std::list<Pair>());
	}
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
	return m_HT.size();
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	int index = MapIndexGenerator(key);
	if (m_HT[index].empty() == false )
	{
		typename std::list<Pair>::iterator myIT;
		for (myIT = m_HT[index].begin(); myIT != m_HT[index].end(); myIT++)
		{
			if ((*myIT).s_key == key)
			{
				(*myIT).s_value = value;
				return;
			}
		}
	}
	m_HT[index].push_back(Pair(key, value));
	m_nItemsAdded++;
	checkRehash();

}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
	int index = MapIndexGenerator(key);
	if (m_HT[index].empty() == true)
	{
		return nullptr;
	}
	//int count = 0;
	typename std::list<Pair>::const_iterator myIT = m_HT[index].begin();
	for (; myIT != m_HT[index].end(); myIT++)
	{
		//count++;
		if ((*myIT).s_key == key)
		{
			break;
		}
	}

	if (myIT != m_HT[index].end())
	{
		//std::cout << count;
		return &(*myIT).s_value;
	}
	return nullptr;
}

template<typename KeyType, typename ValueType>
unsigned int ExpandableHashMap<KeyType, ValueType>::MapIndexGenerator(const KeyType& key) const
{
	unsigned int hasher(const KeyType& k);
	unsigned int h = hasher(key);
	return h % m_HT.size();

}

template<typename KeyType, typename ValueType>
double ExpandableHashMap<KeyType, ValueType>::calcLoad()
{
	return (m_nItemsAdded * 1.0 / m_HT.size());
}

template<typename KeyType, typename ValueType>
bool ExpandableHashMap<KeyType, ValueType>::checkRehash()
{
	if (calcLoad() > m_MaxLoadfac)
	{
		int ogsize = m_HT.size();
		std::vector<Pair> temp;
		typename std::list<Pair>::iterator myIT;
		for (int i = 0; i < m_HT.size(); i++)
		{
			if (m_HT[i].empty() == true)
			{
				continue;
			}

			myIT = m_HT[i].begin();
			for (; myIT != m_HT[i].end(); myIT++)
			{
				temp.push_back(*myIT);
			}

		}
		m_HT.clear();
		int nsize = 2 * ogsize;
		for (int i = 0; i < nsize; i++)
		{
			m_HT.push_back(std::list<Pair>());
		}
		for (int i = 0; i < temp.size(); i++)
		{
			int index = MapIndexGenerator(temp[i].s_key);
			m_HT[index].push_back(temp[i]);
		}
		return true;
	}
	return false;
} 

#endif // !EXPHASHMAP.H

