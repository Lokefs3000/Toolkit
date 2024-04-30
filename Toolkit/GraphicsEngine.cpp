#include "pch.h"
#include "GraphicsEngine.h"

#include <cstdlib>
#include <vector>
#include <set>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
static std::shared_ptr<spdlog::logger> s_Logger;

#define VOLK_IMPLEMENTATION
#include <Volk/volk.h>

#include <SDL3/SDL_vulkan.h>

#include "Window.h"

#define ThrowIfFailed(res) \
	if (res != VK_SUCCESS) \
		std::abort()

#define VulkanVector(func, layers) \
	uint32_t layers##_COUNT; \
	ThrowIfFailed(func(&layers##_COUNT, nullptr)); \
	layers.resize(layers##_COUNT); \
	ThrowIfFailed(func(&layers##_COUNT, layers.data()))

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
	void* pUserData) {

	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		s_Logger->debug(pCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		s_Logger->info(pCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		s_Logger->warn(pCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		s_Logger->error(pCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
	default:
		break;
	}

	return VK_FALSE;
}

const std::vector<const char*> s_ValidationLayers =
{
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> s_DeviceExtensions =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef _DEBUG
const bool s_UseValidationLayers = true;
#else
const bool s_UseValidationLayers = false;
#endif

static GraphicsEngine* s_GraphicsEngine;

WindowAssets::~WindowAssets()
{
	vkDestroySwapchainKHR(s_GraphicsEngine->m_LogicalDevice, Swapchain, nullptr);
	vkDestroySurfaceKHR(s_GraphicsEngine->m_Instance, Surface, nullptr);
}

CommandBuffer::~CommandBuffer()
{
	vkFreeCommandBuffers(s_GraphicsEngine->m_LogicalDevice, s_GraphicsEngine->m_CommandPool, 2, CommandBuffers.data());
}

VkDebugUtilsMessengerCreateInfoEXT GraphicsEngine::FillDebugMessengerCreateInfo()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;

	return createInfo;
}

bool GraphicsEngine::CheckValidationLayerSupport()
{
	std::vector<VkLayerProperties> availableLayers;
	VulkanVector(vkEnumerateInstanceLayerProperties, availableLayers);

	for (const char* layerName : s_ValidationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

std::vector<const char*> GraphicsEngine::GetRequiredExtensions()
{
	uint32_t count = 0;
	char const* const* extensions = SDL_Vulkan_GetInstanceExtensions(&count);
	std::vector<const char*> extensionVector(extensions, extensions + count);

	if (extensions == nullptr)
		ThrowIfFailed(VK_ERROR_EXTENSION_NOT_PRESENT);

	if (s_UseValidationLayers)
		extensionVector.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	return extensionVector;
}

GraphicsEngine::GraphicsEngine(Window* window)
{
	s_Logger = spdlog::stdout_color_mt("Graphics");
	s_Logger->set_level(spdlog::level::debug);

	s_GraphicsEngine = this;

	ThrowIfFailed(volkInitialize());

	bool validationLayersSupported = (s_UseValidationLayers ? CheckValidationLayerSupport() : false);
	std::vector<const char*> requiredExtensions = GetRequiredExtensions();
	VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = FillDebugMessengerCreateInfo();

	//create instance
	{
		if (!validationLayersSupported && s_UseValidationLayers)
			s_Logger->warn("Validation layers requested but were not available!");

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "game";
		appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		appInfo.pEngineName = "Toolkit";
		appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 3, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = (validationLayersSupported ? s_ValidationLayers.size() : 0);
		createInfo.ppEnabledLayerNames = (validationLayersSupported ? s_ValidationLayers.data() : nullptr);
		createInfo.enabledExtensionCount = requiredExtensions.size();
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();
		createInfo.pNext = (validationLayersSupported ? &debugUtilsMessengerCreateInfo : nullptr);

		ThrowIfFailed(vkCreateInstance(&createInfo, nullptr, &m_Instance));
	}

	volkLoadInstance(m_Instance);

	//create debug messenger (optional)
	{
		if (validationLayersSupported)
			ThrowIfFailed(vkCreateDebugUtilsMessengerEXT(m_Instance, &debugUtilsMessengerCreateInfo, nullptr, &m_DebugUtilsMessenger));
	}

	//find a suitable physical device
	{
		std::vector<VkPhysicalDevice> devices;

		uint32_t count = 0;
		ThrowIfFailed(vkEnumeratePhysicalDevices(m_Instance, &count, nullptr));
		devices.resize(count);
		ThrowIfFailed(vkEnumeratePhysicalDevices(m_Instance, &count, devices.data()));

		std::multimap<uint32_t, VkPhysicalDevice> suitableDevices;
		for (VkPhysicalDevice& device : devices)
		{
			uint32_t score = 0;

			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(device, &properties);

			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				score += 100000;

			score += properties.limits.maxImageDimension2D;
			
			suitableDevices.insert(std::make_pair(score, device));
		}

		if (suitableDevices.empty())
		{
			s_Logger->error("Failed to find suitable GPU!");
			ThrowIfFailed(VK_ERROR_INITIALIZATION_FAILED);
		}
		else
			m_PhysicalDevice = suitableDevices.rbegin()->second;
	}

	//get device queues
	{
		VkSurfaceKHR temporarySurface = nullptr;
		if (!SDL_Vulkan_CreateSurface(window->m_Window, m_Instance, nullptr, &temporarySurface))
			ThrowIfFailed(VK_ERROR_UNKNOWN);

		std::vector<VkQueueFamilyProperties> families;

		uint32_t count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, nullptr);
		families.resize(count);
		vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, families.data());

		QueueFamily queueFamily;

		uint32_t i = 0;
		for (VkQueueFamilyProperties& family : families)
		{
			if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT && !queueFamily.GraphicsFamily.has_value())
			{
				queueFamily.GraphicsFamily = i++;
				continue;
			}

			VkBool32 supported = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, temporarySurface, &supported);

			if (supported == VK_TRUE)
				queueFamily.PresentFamily = i;

			if (queueFamily.GraphicsFamily.has_value() && queueFamily.PresentFamily.has_value())
				break;

			i++;
		}

		if (queueFamily.GraphicsFamily.has_value() && queueFamily.PresentFamily.has_value())
			m_QueueFamilies.push_back(queueFamily);

		if (m_QueueFamilies.empty())
		{
			s_Logger->error("Failed to find suitable queue indices on physical device!");
			ThrowIfFailed(VK_ERROR_UNKNOWN);
		}

		vkDestroySurfaceKHR(m_Instance, temporarySurface, nullptr);
	}

	//create logical device and get queues
	{
		std::vector<VkDeviceQueueCreateInfo> queues;
		
		float priority = 1.0f;

		VkDeviceQueueCreateInfo graphicsQueue{};
		graphicsQueue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		graphicsQueue.queueFamilyIndex = m_QueueFamilies.back().GraphicsFamily.value();
		graphicsQueue.queueCount = 1;
		graphicsQueue.pQueuePriorities = &priority;
		queues.push_back(graphicsQueue);

		VkDeviceQueueCreateInfo presentQueue{};
		presentQueue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		presentQueue.queueFamilyIndex = m_QueueFamilies.back().PresentFamily.value();
		presentQueue.queueCount = 1;
		presentQueue.pQueuePriorities = &priority;
		queues.push_back(presentQueue);

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = queues.size();
		createInfo.pQueueCreateInfos = queues.data();
		createInfo.enabledExtensionCount = s_DeviceExtensions.size();
		createInfo.ppEnabledExtensionNames = s_DeviceExtensions.data();

		ThrowIfFailed(vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice));

		vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilies.back().GraphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_LogicalDevice, m_QueueFamilies.back().PresentFamily.value(), 0, &m_PresentQueue);
	}

	volkLoadDevice(m_LogicalDevice);

	//create command pool
	{
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.queueFamilyIndex = m_QueueFamilies.back().GraphicsFamily.value();

		ThrowIfFailed(vkCreateCommandPool(m_LogicalDevice, &createInfo, nullptr, &m_CommandPool));
	}

	PushNewWindow(window);

	//print diagnostic information
	{
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);
		s_Logger->debug("GPU: {} (DRIVER: {}, VULKAN: {})", properties.deviceName, properties.driverVersion, properties.apiVersion);
	}
}

GraphicsEngine::~GraphicsEngine()
{
	vkDeviceWaitIdle(m_LogicalDevice);

	m_Windows.clear();
	vkDestroyDevice(m_LogicalDevice, nullptr);
	if (s_UseValidationLayers && m_DebugUtilsMessenger != nullptr)
		vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugUtilsMessenger, nullptr);
	vkDestroyInstance(m_Instance, nullptr);

	s_Logger = nullptr;
}

void GraphicsEngine::PushNewWindow(Window* window)
{
	std::unique_ptr<WindowAssets> assets = std::make_unique<WindowAssets>();
	assets->AssociatedWindow = window;

	//create surface
	{
		ThrowIfFailed(SDL_Vulkan_CreateSurface(window->m_Window, m_Instance, nullptr, &assets->Surface) == SDL_FALSE ? VK_ERROR_UNKNOWN : VK_SUCCESS);
	}

	//prepare swapchain
	{
		assets->Format = VK_FORMAT_R8G8B8A8_UNORM;
		assets->Colorspace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		SDL_GetWindowSizeInPixels(window->m_Window, (int*)&assets->Extent.width, (int*)&assets->Extent.height);
	}

	//create swapchain
	{
		QueueFamily& family = m_QueueFamilies.back();

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = assets->Surface;
		createInfo.minImageCount = 2;
		createInfo.imageFormat = assets->Format;
		createInfo.imageColorSpace = assets->Colorspace;
		createInfo.imageExtent = assets->Extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (family.GraphicsFamily != family.PresentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			uint32_t indices[] = { family.GraphicsFamily.value(), family.PresentFamily.value() };
			createInfo.pQueueFamilyIndices = indices;
		}
		else
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		createInfo.clipped = true;

		ThrowIfFailed(vkCreateSwapchainKHR(m_LogicalDevice, &createInfo, nullptr, &assets->Swapchain));
	}

	m_Windows.push_back(std::move(assets));
}

CommandBuffer* GraphicsEngine::AllocateNewCommandBuffer()
{
	CommandBuffer* buffer = new CommandBuffer();

	//allocate commandbuffers
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = s_MaxFramesInFlight;
		ThrowIfFailed(vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, buffer->CommandBuffers.data()));
	}

	return buffer;
}

GraphicsEngine* GraphicsEngine::GetInstance()
{
	return s_GraphicsEngine;
}
