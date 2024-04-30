#pragma once

#include <string_view>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <combaseapi.h>
#endif

#ifdef _DEBUG
#define ENABLE_TRACE
#endif

#ifdef _WIN32
typedef std::wstring_view string_view;
typedef std::wstring string;
typedef wchar_t chart;

static std::string WideToString(std::wstring w)
{
#ifdef _WIN32
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &w[0], w.size(), nullptr, 0, nullptr, nullptr);
	std::string str = std::string(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &w[0], (int)w.size(), &str[0], size_needed, NULL, NULL);
	return str;
#else
	return w;
#endif
}

static std::wstring StringToWide(std::string s)
{
#ifdef _WIN32
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &s[0], s.size(), nullptr, 0);
	std::wstring str = std::wstring(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &s[0], (int)s.size(), &str[0], size_needed);
	return str;
#else
	return s;
#endif
}
#else
typedef std::string_view string_view;
typedef std::string string;
typedef char chart;
#endif

class Marshal
{
public:
	static void* AllocHGlobal(size_t size)
	{
#ifdef _WIN32
		return LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, size);
#else
		return malloc(size);
#endif
	}

	static void FreeHGlobal(void* ptr)
	{
#ifdef _WIN32
		LocalFree(ptr);
#else
		free(ptr);
#endif
	}

	static chart* StringToCoTaskMemAuto(string_view str)
	{
		size_t length = str.size() + 1;
		size_t size = length * sizeof(chart);

#ifdef _WIN32
		chart* buffer = (chart*)CoTaskMemAlloc(size);

		if (buffer != nullptr)
		{
			memset(buffer, 0xCE, size);
			memcpy(buffer, str.data(), size - sizeof(chart));
			buffer[length - 1] = L'\0';
		}
#else
		chart* buffer = static_cast<chart*>(AllocHGlobal(size));

		if (buffer != nullptr)
		{
			memset(buffer, 0, size);
			memcpy(buffer, str.data(), size - sizeof(chart));
			buffer[length - 1] = '\0';
		}
#endif

		return buffer;
	}

	static void FreeCoTaskMem(void* mem)
	{
#ifdef _WIN32
		CoTaskMemFree(mem);
#else
		FreeHGlobal(mem);
#endif
	}
};

class NativeString
{
private:
	chart* _string = nullptr;
	uint32_t _disposed = 0;
public:
#ifdef _WIN32
	static NativeString New(const char* instring)
	{
		string expanded = StringToWide(instring);

		NativeString res;
		res.Assign(expanded);
		return res;
	}

	static NativeString New(std::string_view instring)
	{
		string expanded = StringToWide(instring.data());

		NativeString res;
		res.Assign(expanded);
		return res;
	}
#endif

	static NativeString New(const chart* instring)
	{
		NativeString res;
		res.Assign(instring);
		return res;
	}

	static NativeString New(string_view instring)
	{
		NativeString res;
		res.Assign(instring);
		return res;
	}

	static void Free(NativeString instring)
	{
		if (instring._string == nullptr)
			return;

		Marshal::FreeCoTaskMem(instring._string);
		instring._string = nullptr;
	}

	void Assign(string_view instring)
	{
		if (_string != nullptr)
			Marshal::FreeCoTaskMem(_string);

		_string = Marshal::StringToCoTaskMemAuto(instring.data());
	}

	operator std::string() const
	{
#ifdef _WIN32
		return WideToString(_string);
#else
		return std::string(_string);
#endif
	}

	bool operator==(const NativeString& other)
	{
		if (_string == other._string)
			return true;
		else if (_string == nullptr || other._string == nullptr)
			return false;

#ifdef _WIN32
		return wcscmp(_string, other._string) == 0;
#else
		return strcmp(_string, other._string) == 0;
#endif
	}

	bool operator==(std::string_view& other)
	{
#ifdef _WIN32
		std::wstring str = StringToWide(other.data());
		return wcscmp(_string, str.data()) == 0;
#else
		return strcmp(_string, other.data()) == 0;
#endif
	}

	chart* Data() { return _string; }
	const chart* Data() const { return _string; }
};

class ScopedNativeString
{
private:
	NativeString m_String;
public:
	ScopedNativeString(NativeString string)
		: m_String(string) {};

	ScopedNativeString& operator=(NativeString other)
	{
		NativeString::Free(m_String);
		m_String = other;
		return *this;
	}

	ScopedNativeString& operator=(const ScopedNativeString& other)
	{
		NativeString::Free(m_String);
		m_String = other.m_String;
		return *this;
	}

	~ScopedNativeString()
	{
		NativeString::Free(m_String);
	}

	operator std::string() const { return m_String; }
	operator NativeString() const { return m_String; };
};

struct NativeCall
{
	ScopedNativeString Name;
	ScopedNativeString Field;
	void* Method;

	NativeCall(ScopedNativeString name, ScopedNativeString field, void* method)
		: Name(std::move(name)), Field(std::move(field)), Method(method) {};
};

class Renderer;

class ScriptEngine
{
private:
	void* m_HostFXR;

	std::vector<NativeCall> m_AwaitingCalls;

	void LoadHostFXR();
	void LoadLibraries();
public:
	ScriptEngine();
	~ScriptEngine();

	void LoadAssembly(std::string_view context, std::string_view path);

	void* GetFunction(std::string_view dll, std::string_view type, std::string_view method);

	void PushCall(std::string_view name, std::string_view field, void* method);
	void PopCalls();

	void RegisterRenderer(Renderer* r);
};