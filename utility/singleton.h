#pragma once
#include <mutex>

template<typename T>
class singleton
{
protected:
	singleton(){}
	static T* mpinstance;
	static std::once_flag minsflag;

	friend class singleton<T>;

public:
	static T& instance(){
		std::call_once(minsflag, [&](){
			mpinstance = new T;
			assert(mpinstance);
		});

		return *mpinstance;
	}

	singleton(const singleton&) = delete;
	singleton(const singleton&&) = delete;
	singleton& operator=(const singleton&) = delete;
};

template<typename T>
T* singleton<T>::mpinstance = nullptr;

template<typename T>
std::once_flag singleton<T>::minsflag;
