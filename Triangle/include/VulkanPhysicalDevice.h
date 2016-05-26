#pragma once
#include <assert.h>
#include <vector>
#include <vulkan\vulkan.h>
#include <memory>

class VulkanPhysicalDevice 
{
public:
	VulkanPhysicalDevice(VkPhysicalDevice d) : vkPhysicalDevice(d) {}

	std::unique_ptr<std::vector<VkQueueFamilyProperties>> queueFamilyProperties()
	{
		uint32_t queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueCount, NULL);
		assert(queueCount >= 1);

		std::unique_ptr<std::vector<VkQueueFamilyProperties>> queueProperties = std::make_unique<std::vector<VkQueueFamilyProperties>>();
		queueProperties->resize(queueCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueCount, queueProperties->data());

		return queueProperties;
	}

	VkPhysicalDeviceMemoryProperties& memoryProperties()
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &memProperties);
		return memProperties;
	}

	VkFormatProperties& formatProperties(VkFormat format)
	{
		VkFormatProperties formProperties;
		vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice, format, &formProperties);
		return formProperties;
	}

	VkPhysicalDevice vkPhysicalDevice;
};
