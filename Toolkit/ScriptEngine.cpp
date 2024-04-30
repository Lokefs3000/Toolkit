#include "pch.h"
#include "ScriptEngine.h"
#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
static std::shared_ptr<spdlog::logger> s_Logger;

#include <coreclr/nethost.h>
#include <coreclr/coreclr_delegates.h>
#include <coreclr/hostfxr.h>

#include <utf8.h>
#include <utf8/cpp20.h>

#include <filesystem>

enum class MessageLevel
{
	Log,
	Warn,
	Error
};

struct CoreCLRFunctions
{
	hostfxr_initialize_for_runtime_config_fn InitForRuntimeConfig;
	hostfxr_close_fn Close;
	hostfxr_get_runtime_delegate_fn GetRuntimeDelegate;
	hostfxr_set_error_writer_fn SetErrorWriter;

	load_assembly_and_get_function_pointer_fn GetFunctionPointer;
	get_function_pointer_fn GetFunctionPointerPreloaded;
};

static CoreCLRFunctions s_CoreCLR;

typedef void (CORECLR_DELEGATE_CALLTYPE* managed_host_initialize_fn)(void(char_t*, MessageLevel));
typedef void (CORECLR_DELEGATE_CALLTYPE* assembly_loader_load_fn)(NativeString, NativeString);
typedef void (CORECLR_DELEGATE_CALLTYPE* assembly_loader_unload_all_fn)();
typedef void* (CORECLR_DELEGATE_CALLTYPE* assembly_loader_get_method_fn)(NativeString, NativeString, NativeString);
typedef void* (CORECLR_DELEGATE_CALLTYPE* call_system_push_calls_fn)(NativeCall*, int);

struct ToolkitCoreFunctions
{
	managed_host_initialize_fn ManagedHostInitialize;

	assembly_loader_load_fn AssemblyLoaderLoad;
	assembly_loader_unload_all_fn AssemblyLoaderUnloadAll;
	assembly_loader_get_method_fn AssemblyLoaderGetMethod;

	call_system_push_calls_fn CallSystemPushCalls;
};

static ToolkitCoreFunctions s_Toolkit;

void MessageCallback(char_t* message, MessageLevel level)
{
	std::string str = WideToString(message);

	switch (level)
	{
	case MessageLevel::Log:
		s_Logger->info(str);
		break;
	case MessageLevel::Warn:
		s_Logger->warn(str);
		break;
	case MessageLevel::Error:
		s_Logger->error(str);
		break;
	}
}

void ExceptionCallback(char_t* message)
{
	std::string str = WideToString(message);
	s_Logger->critical(str);
}

void CoreCLRErrorWriter(const char_t* message)
{
	MessageCallback((char_t*)message, MessageLevel::Error);
}

void ScriptEngine::LoadHostFXR()
{
	std::filesystem::path hostFXRPath;

	{
		get_hostfxr_parameters params{};
		params.size = sizeof(params);

		char_t buffer[MAX_PATH];
		size_t buffersize = sizeof(buffer) / sizeof(char_t);
		int rc = get_hostfxr_path(buffer, &buffersize, &params);
		if (rc != 0)
			exit(1);

		hostFXRPath = buffer;
	}

	//this should never get unloaded so let the OS cleanup itself.
	//plus the dotnet library doesnt clean its libraries either!
	void* libraryHandle = nullptr;

#ifdef _WIN32
	libraryHandle = LoadLibrary(hostFXRPath.c_str());
#else

#endif

	if (libraryHandle == nullptr)
		exit(2);

	s_CoreCLR.InitForRuntimeConfig = (hostfxr_initialize_for_runtime_config_fn)GetProcAddress((HMODULE)libraryHandle, "hostfxr_initialize_for_runtime_config");
	s_CoreCLR.Close = (hostfxr_close_fn)GetProcAddress((HMODULE)libraryHandle, "hostfxr_close");
	s_CoreCLR.GetRuntimeDelegate = (hostfxr_get_runtime_delegate_fn)GetProcAddress((HMODULE)libraryHandle, "hostfxr_get_runtime_delegate");
	s_CoreCLR.SetErrorWriter = (hostfxr_set_error_writer_fn)GetProcAddress((HMODULE)libraryHandle, "hostfxr_set_error_writer");

	int rc = s_CoreCLR.InitForRuntimeConfig(TEXT("script/Toolkit.Core.runtimeconfig.json"), nullptr, &m_HostFXR);
	if (rc != 0)
		exit(3);

	s_CoreCLR.SetErrorWriter(CoreCLRErrorWriter);

	rc = s_CoreCLR.GetRuntimeDelegate(m_HostFXR, hdt_load_assembly_and_get_function_pointer, (void**)&s_CoreCLR.GetFunctionPointer);
	if (rc != 0)
		exit(4);

	rc = s_CoreCLR.GetRuntimeDelegate(m_HostFXR, hdt_get_function_pointer, (void**)&s_CoreCLR.GetFunctionPointerPreloaded);
	if (rc != 0)
		exit(8);

	s_CoreCLR.Close(m_HostFXR);
}

void ScriptEngine::LoadLibraries()
{
	std::filesystem::path assemblyPath = "script/Toolkit.Core.dll";

	int rc = s_CoreCLR.GetFunctionPointer(assemblyPath.c_str(), TEXT("Toolkit.Core.Interop.ManagedHost, Toolkit.Core"), TEXT("Initialize"), UNMANAGEDCALLERSONLY_METHOD, nullptr, (void**)&s_Toolkit.ManagedHostInitialize);
	if (rc != 0)
		exit(5);

	rc = s_CoreCLR.GetFunctionPointer(assemblyPath.c_str(), TEXT("Toolkit.Core.Interop.AssemblyLoader, Toolkit.Core"), TEXT("Load"), UNMANAGEDCALLERSONLY_METHOD, nullptr, (void**)&s_Toolkit.AssemblyLoaderLoad);
	if (rc != 0)
		exit(6);

	rc = s_CoreCLR.GetFunctionPointer(assemblyPath.c_str(), TEXT("Toolkit.Core.Interop.AssemblyLoader, Toolkit.Core"), TEXT("UnloadAll"), UNMANAGEDCALLERSONLY_METHOD, nullptr, (void**)&s_Toolkit.AssemblyLoaderUnloadAll);
	if (rc != 0)
		exit(7);

	rc = s_CoreCLR.GetFunctionPointer(assemblyPath.c_str(), TEXT("Toolkit.Core.Interop.AssemblyLoader, Toolkit.Core"), TEXT("GetMethod"), UNMANAGEDCALLERSONLY_METHOD, nullptr, (void**)&s_Toolkit.AssemblyLoaderGetMethod);
	if (rc != 0)
		exit(9);

	rc = s_CoreCLR.GetFunctionPointer(assemblyPath.c_str(), TEXT("Toolkit.Core.Interop.CallSystem, Toolkit.Core"), TEXT("PushCalls"), UNMANAGEDCALLERSONLY_METHOD, nullptr, (void**)&s_Toolkit.CallSystemPushCalls);
	if (rc != 0)
		exit(10);
}

ScriptEngine::ScriptEngine()
{
#ifdef ENABLE_TRACE
#ifdef _WIN32
	//SetEnvironmentVariable(TEXT("COREHOST_TRACE"), TEXT("1"));
	//SetEnvironmentVariable(TEXT("COREHOST_TRACE_VERBOSITY"), TEXT("4"));
#endif
#endif

	s_Logger = spdlog::stdout_color_st("Script");

	LoadHostFXR();
	LoadLibraries();

	s_Toolkit.ManagedHostInitialize(MessageCallback);

	LoadAssembly("tk", "script/Toolkit.Graphics.dll");
}

ScriptEngine::~ScriptEngine()
{
	s_Toolkit.AssemblyLoaderUnloadAll();

	s_Logger = nullptr;
}

void ScriptEngine::LoadAssembly(std::string_view context, std::string_view path)
{
	std::filesystem::path absolute = std::filesystem::absolute(path);
	ScopedNativeString scoped = ScopedNativeString(NativeString::New(absolute.c_str()));
	ScopedNativeString contextsc = ScopedNativeString(NativeString::New(context.data()));
	s_Toolkit.AssemblyLoaderLoad(contextsc, scoped);
}

void* ScriptEngine::GetFunction(std::string_view dll, std::string_view type, std::string_view method)
{
	void* delegate = nullptr;

	ScopedNativeString dllw = ScopedNativeString(NativeString::New(dll));
	ScopedNativeString typew = ScopedNativeString(NativeString::New(type));
	ScopedNativeString methodw = ScopedNativeString(NativeString::New(method));
	delegate = s_Toolkit.AssemblyLoaderGetMethod(dllw, typew, methodw);

	return delegate;
}

void ScriptEngine::PushCall(std::string_view name, std::string_view field, void* method)
{
	NativeCall call = NativeCall(ScopedNativeString(NativeString::New(name)), ScopedNativeString(NativeString::New(field)), method);
	m_AwaitingCalls.push_back(std::move(call));
}

void ScriptEngine::PopCalls()
{
	s_Toolkit.CallSystemPushCalls(m_AwaitingCalls.data(), m_AwaitingCalls.size());
	m_AwaitingCalls.clear();
}

#include "Renderer.h"

static Renderer* s_ScriptEngine_Renderer = nullptr;

void SE_Renderer_SetClearColor(Color4* c) { s_ScriptEngine_Renderer->Config.ClearColor = *c; }

void ScriptEngine::RegisterRenderer(Renderer* r)
{
	s_ScriptEngine_Renderer = r;

	PushCall("Toolkit.Renderer.Graphics, Toolkit.Renderer", "_setClearColor", reinterpret_cast<void*>(SE_Renderer_SetClearColor));

	PopCalls();
}
