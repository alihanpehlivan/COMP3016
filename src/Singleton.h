#pragma once

template <typename T> class Singleton
{
	static T* _singleton;

public:
	Singleton()
	{
		assert(!_singleton);
		UINT_PTR offset = (UINT_PTR)(T*)1 - (UINT_PTR)(Singleton <T>*) (T*) 1;
		_singleton = (T*)((UINT_PTR)this + offset);
	}

	virtual ~Singleton()
	{
		assert(_singleton);
		_singleton = 0;
	}

	__forceinline static T& Instance()
	{
		assert(_singleton);
		return (*_singleton);
	}

	__forceinline static T* InstancePtr()
	{
		return (_singleton);
	}
};

template <typename T> T* Singleton <T>::_singleton = 0;