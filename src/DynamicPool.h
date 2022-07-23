#pragma once

//
// Dynamic instance pool
//

template<typename T>
class DynamicPool
{
public:
	DynamicPool()
	{
		_initCapacity = 0;
		_usedCapacity = 0;
	}

	virtual ~DynamicPool()
	{
#ifdef _DEBUG
		printf("\nDestroying pool with size: %zu\n", _vec_Data.size());
#endif
		assert(_vec_Data.empty());
	}

	void Create(size_t nCapacity)
	{
		_initCapacity = nCapacity;

		_vec_Data.reserve(nCapacity);
		_vec_Free.reserve(nCapacity);
	}

	void Destroy()
	{
		std::for_each(_vec_Data.begin(), _vec_Data.end(), Delete);

		_vec_Data.clear();
		_vec_Free.clear();
	}

	void Clear()
	{
		Destroy();
	}

	T* Allocate()
	{
		if (_vec_Free.empty())
		{
			T* pNewData = new T;
			_vec_Data.push_back(pNewData);
			++_usedCapacity;
#ifdef _DEBUG
			printf("\nDynamic Pool New Alloc\n Current UsedCapacity: %zu New Alloc Ptr: %p\n", _usedCapacity, pNewData);
#endif
			return pNewData;
		}

		T* pFreeData = _vec_Free.back();
		_vec_Free.pop_back();
		return pFreeData;
	}

	void Free(T* pkData)
	{
		_vec_Free.push_back(pkData);
	}

	void FreeAll()
	{
		_vec_Free = _vec_Data;
	}

	size_t GetCapacity()
	{
		return _vec_Data.size();
	}

protected:
	static void Delete(T* pData)
	{
		delete pData;
	}

protected:
	std::vector<T*> _vec_Data;
	std::vector<T*> _vec_Free;

	size_t _initCapacity;
	size_t _usedCapacity;
};