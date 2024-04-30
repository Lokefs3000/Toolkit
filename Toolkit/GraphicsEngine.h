#pragma once

#include <Volk/volk.h>

#include <vector>
#include <memory>
#include <optional>
#include <array>

const uint32_t s_MaxFramesInFlight = 2;

class Window;

struct WindowAssets
{
	Window* AssociatedWindow;
	VkSurfaceKHR Surface;
	VkSwapchainKHR Swapchain;

	VkFormat Format;
	VkColorSpaceKHR Colorspace;
	VkExtent2D Extent;

	~WindowAssets();
};

struct QueueFamily
{
	std::optional<uint32_t> GraphicsFamily;
	std::optional<uint32_t> PresentFamily;
};

struct CommandBuffer
{
	std::array<VkCommandBuffer, s_MaxFramesInFlight> CommandBuffers;

	~CommandBuffer();
};

class GraphicsEngine
{
private:
	VkInstance m_Instance = nullptr;
	VkDebugUtilsMessengerEXT m_DebugUtilsMessenger = nullptr;
	VkPhysicalDevice m_PhysicalDevice = nullptr;
	VkDevice m_LogicalDevice = nullptr;
	VkQueue m_GraphicsQueue = nullptr;
	VkQueue m_PresentQueue = nullptr;
	VkCommandPool m_CommandPool = nullptr;

	std::vector<QueueFamily> m_QueueFamilies;
	std::vector<std::unique_ptr<WindowAssets>> m_Windows;

	VkDebugUtilsMessengerCreateInfoEXT FillDebugMessengerCreateInfo();
	bool CheckValidationLayerSupport();
	std::vector<const char*> GetRequiredExtensions();

	friend struct WindowAssets;
	friend struct CommandBuffer;
public:
	GraphicsEngine(Window* window);
	~GraphicsEngine();

	void PushNewWindow(Window* window);

	CommandBuffer* AllocateNewCommandBuffer();

	static GraphicsEngine* GetInstance();
};