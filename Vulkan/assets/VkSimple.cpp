/*--------------------------------------------------------------------------------*
  Copyright (C)Nintendo All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *--------------------------------------------------------------------------------*/

/**
 * @examplesource{VkSimple.cpp,PageSampleVkSimple}
 *
 * @brief
 *  Basic example of initializing and using Vulkan
 */

/**
 * @page PageSampleVkSimple VkSimple
 * @tableofcontents
 *
 * @brief
 *  This sample demonstrates basic initialization of Vulkan and
 *  renders a single triangle.
 *
 * @section PageSampleVkSimple_SectionBrief Overview
 *  This sample demonstrates basic initialization of Vulkan and
 *  renders a single triangle.  Due to the usage of SPIR-V for
 *  precompiled shaders in Vulkan, a bat file (GLSL_to_SPIRV.bat)
 *  is included that uses glslangValidator.exe from the VulkanSDK
 *  to take the two included shaders (vertex/fragment_shader.vert/frag)
 *  and recompile them into the intermediary SPIR-V format to allow
 *  easy modification of the shaders.  The VulkanSDK must be installed
 *  with the VULKAN_SDK environment variable set.
 *
 * @subsection PageVkSimple_SectionExpectedOutput Expected Output
 * @image html VkSimple.png
 *
 * @section VkSimple_SectionFileStructure File Structure
 *  The main sample file and Visual Studio solutions can be found at
 *  @link ../../../Samples/Sources/Applications/VkSimple Samples/Sources/Applications/VkSimple @endlink
 *
 * @section PageSampleVkSimple_SectionNecessaryEnvironment System Requirements
 *  No extra system requirements.
 *
 * @section PageSampleVkSimple_SectionHowToOperate Operation Procedure
 *  This sample runs on its own without any additional input. Touching
 *  the touch screen on NX will exit the program.
 *
 * @section PageSampleVkSimple_SectionPrecaution Precautions
 *  None.
 *
 * @section PageSampleVkSimple_SectionHowToExecute Execution Procedure
 *  Build the Visual Solution in the desired configuration and run it.
 *
 * @section PageSampleVkSimple_SectionDetail Description
 *
 * @subsection PageSampleVkSimple_SectionSampleProgram Sample Program
 *  Below is the source code for the main tutorial file for this sample.
 *
 *  VkSimple.cpp
 *  @includelineno VkSimple.cpp
 *
 * @subsection PageSampleVkSimple_SectionSampleDetail Sample Program Description
 *  This application renders a checkerboard patterened rainbow triangle
 */

#ifndef VK_USE_PLATFORM_VI_NN
#define VK_USE_PLATFORM_VI_NN
#endif

#include <nn/nn_Assert.h>
#include <nn/nn_Log.h>

#include <nn/nn_Result.h>
#include <nn/fs.h>
#include <nn/os.h>
#include <nn/init.h>
#include <nn/vi.h>
#include <nn/hid.h>

#include <nv/nv_MemoryManagement.h>

#include <nn/mem.h>

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>
#include <cstdlib>

const int        g_MaxSwaps = 3;
const int        g_NumDescriptorSets = 5;

const size_t     g_GraphicsSystemMemorySize = 32 * 1024 * 1024;

nn::vi::Display* g_pDisplay;
nn::vi::Layer*   g_pLayer;

typedef struct _MyAppData
{
    VkInstance                          m_Instance;
    VkPhysicalDevice                    m_PhysicalDevice;
    VkPhysicalDeviceProperties          m_PhysicalDeviceProperties;
    VkPhysicalDeviceFeatures            m_PhysicalDeviceFeatures;
    VkPhysicalDeviceMemoryProperties    m_PhysicalDeviceMemoryProperties;
    std::vector<VkLayerProperties>      m_LayerProperties;
    std::vector<VkExtensionProperties>  m_ExtensionProperties;
    std::vector<const char*>            m_LayerNames;
    std::vector<const char*>            m_ExtensionNames;
    VkQueueFamilyProperties             m_QueueFamilyProperties;
    VkDebugReportCallbackEXT            m_DebugCallbackEXT;
    VkDevice                            m_Device;
    VkQueue                             m_Queue;
    VkCommandPool                       m_CommandPool;
    VkCommandBuffer                     m_CommandBuffer;

    VkSurfaceKHR                        m_SurfaceKhr;
    VkFormat                            m_Format;
    VkSwapchainKHR                      m_SwapChainKhr;
    VkExtent2D                          m_SwapChainExtent;
    uint32_t                            m_SwapchainImageCount;
    uint32_t                            m_CurrentBuffer;
    VkImage*                            m_pSwapChainImages;
    VkImageView*                        m_pColorImageView;
    VkImage                             m_DepthBufferImage;
    VkFormat                            m_DepthBufferFormat;
    VkImageView                         m_DepthBufferImageView;
    VkDeviceMemory                      m_DepthBufferMemory;

    VkRenderPass                        m_RenderPass;
    VkFramebuffer                       m_pFramebuffers[g_MaxSwaps];
    VkFence                             m_Fence;
    VkSemaphore                         m_SemaphoreSignal;
    VkSemaphore                         m_PresentSemaphore;
    VkPipeline                          m_Pipeline;
    VkPipelineCache                     m_PipelineCache;
    VkPipelineLayout                    m_PipelineLayout;
    VkPipelineLayout                    m_PipelineLayout2;
    VkDescriptorPool                    m_DescriptorPool;
    VkDescriptorSetLayout               m_DescriptorSetLayout[3];
    VkDescriptorSet                     m_DescriptorSet[3];
    VkDescriptorSet                     m_DescriptorSet2;
    VkBuffer                            m_VertexBuffer;
    VkDeviceMemory                      m_VertexBufferMem;
    VkViewport                          m_Viewport;
    VkRect2D                            m_Scissor;
    uint32_t                            m_VertexCount;
    VkPipelineShaderStageCreateInfo     m_pShaderStages[3];

    VkImage                             m_TextureImage;
    VkImageView                         m_TextureImageView;
    VkDeviceMemory                      m_TextureMemory;
    VkSampler                           m_TextureSampler;

    VkBuffer                            m_uniformBuffer;
    VkDeviceMemory                      m_uniformBufferMemory;
    VkDescriptorBufferInfo              m_uniformBufferInfo;

    VkFence                             m_ComputeFence;
    VkCommandBuffer                     m_ComputeCommandBuffer;
    VkPipeline                          m_ComputePipeline;
    VkDescriptorSet                     m_ComputeDescriptorSet;
    VkPipelineLayout                    m_ComputePipelineLayout;
    VkDescriptorPool                    m_ComputeDescriptorPool;
    VkImage                             m_ComputeTextureImage;
    VkImageView                         m_ComputeTextureImageView;
    VkDeviceMemory                      m_ComputeTextureMemory;

    VkImage                             m_ComputeTextureImage2;
    VkImageView                         m_ComputeTextureImageView2;
    VkDeviceMemory                      m_ComputeTextureMemory2;
} MyAppData;

MyAppData g_AppData = { 0 };

bool SelectMemoryType(uint32_t typeBits, uint32_t requirements_mask, uint32_t* pMemoryTypeIndex)
{
    bool pass = false;
    for (uint32_t i = 0; i < 32; i++)
    {
        if ((typeBits & 1) == 1)
        {
            if ((g_AppData.m_PhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask)
            {
                *pMemoryTypeIndex = i;
                pass = true;
                break;
            }
        }
        typeBits >>= 1;
    }
    return pass;
}

void InitializeLayer()
{
    nn::Result result = nn::vi::OpenDefaultDisplay(&g_pDisplay);
    NN_ASSERT(result.IsSuccess());
    NN_UNUSED(result);

    result = nn::vi::CreateLayer(&g_pLayer, g_pDisplay);
    NN_ASSERT(result.IsSuccess());
}

bool InitializeExtensions()
{
    VkResult result;
    uint32_t layerCount;
    VkLayerProperties* pLayers = NULL;

    g_AppData.m_LayerProperties.clear();
    g_AppData.m_ExtensionProperties.clear();
    g_AppData.m_LayerNames.clear();
    g_AppData.m_ExtensionNames.clear();

    do
    {
        result = vkEnumerateInstanceLayerProperties(&layerCount, NULL);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

        if (layerCount == 0)
        {
            return true;
        }
        g_AppData.m_LayerProperties.resize(layerCount);
        pLayers = g_AppData.m_LayerProperties.data();

        result = vkEnumerateInstanceLayerProperties(&layerCount, pLayers);
    } while (result == VK_INCOMPLETE);

    uint32_t totalExtensions = 0;
    {
        uint32_t extensionCount = 0;
        VkExtensionProperties* pExtensions = NULL;
        result = vkEnumerateInstanceExtensionProperties( NULL, &extensionCount, NULL );
        NN_ASSERT_EQUAL( VK_SUCCESS, result );

        if ( extensionCount != 0 )
        {
            totalExtensions += extensionCount;

            g_AppData.m_ExtensionProperties.resize(totalExtensions);
            pExtensions = g_AppData.m_ExtensionProperties.data();
            result = vkEnumerateInstanceExtensionProperties( NULL, &extensionCount, pExtensions );
        }
    }
    for (uint32_t i = 0; i < layerCount; i++)
    {
        VkExtensionProperties* pExtensions = NULL;
        uint32_t extensionCount = 0;
        do
        {
            result = vkEnumerateInstanceExtensionProperties(pLayers[i].layerName, &extensionCount, NULL);
            NN_ASSERT_EQUAL(result, VK_SUCCESS);

            if (extensionCount == 0)
            {
                break;
            }

            g_AppData.m_ExtensionProperties.resize(totalExtensions + extensionCount);
            pExtensions = g_AppData.m_ExtensionProperties.data();
            pExtensions += totalExtensions;
            totalExtensions += extensionCount;

            result = vkEnumerateInstanceExtensionProperties(pLayers[i].layerName, &extensionCount, pExtensions);

        } while (result == VK_INCOMPLETE);
    }

    return true;
}

bool InitializeDeviceExtensions()
{
    VkResult result;
    uint32_t extensionCount;
    uint32_t totalExtensions = 0;
    VkExtensionProperties* pExtensions = NULL;

    do
    {
        result = vkEnumerateDeviceExtensionProperties( g_AppData.m_PhysicalDevice, NULL, &extensionCount, NULL );
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

        if (extensionCount == 0)
        {
            break;
        }

        totalExtensions = static_cast< uint32_t >( g_AppData.m_ExtensionProperties.size() );
        g_AppData.m_ExtensionProperties.resize(totalExtensions + extensionCount);
        pExtensions = g_AppData.m_ExtensionProperties.data();
        pExtensions += totalExtensions;
        result = vkEnumerateDeviceExtensionProperties( g_AppData.m_PhysicalDevice, NULL, &extensionCount, pExtensions );
        totalExtensions += extensionCount;

    } while (result == VK_INCOMPLETE);

    uint32_t layerCount = static_cast< uint32_t >( g_AppData.m_LayerProperties.size() );
    for ( uint32_t idxLayer = 0; idxLayer < layerCount; ++idxLayer )
    {
        do
        {
            result = vkEnumerateDeviceExtensionProperties( g_AppData.m_PhysicalDevice, g_AppData.m_LayerProperties[ idxLayer ].layerName, &extensionCount, NULL );
            NN_ASSERT_EQUAL( VK_SUCCESS, result );

            if ( extensionCount == 0 )
            {
                break;
            }

            g_AppData.m_ExtensionProperties.resize(totalExtensions + extensionCount);
            pExtensions = g_AppData.m_ExtensionProperties.data();
            pExtensions += totalExtensions;
            totalExtensions += extensionCount;

            result = vkEnumerateDeviceExtensionProperties( g_AppData.m_PhysicalDevice, g_AppData.m_LayerProperties[ idxLayer ].layerName, &extensionCount, pExtensions );
        }
        while ( result == VK_INCOMPLETE );
    }

    return true;
}

VkBool32 VKAPI_CALL DebugMessageCallback(
    VkDebugReportFlagsEXT                       flags,
    VkDebugReportObjectTypeEXT                  objectType,
    uint64_t                                    object,
    size_t                                      location,
    int32_t                                     messageCode,
    const char*                                 pLayerPrefix,
    const char*                                 pMessage,
    void*                                       pUserData)
{
    NN_UNUSED(flags);
    NN_UNUSED(objectType);
    NN_UNUSED(object);
    NN_UNUSED(location);
    NN_UNUSED(messageCode);
    NN_UNUSED(pLayerPrefix);
    NN_UNUSED(pMessage);
    NN_UNUSED(pUserData);

    NN_LOG("%s\n", pMessage);
    return VK_TRUE;
}

PFN_vkCreateDebugReportCallbackEXT pVkCreateDebugReportCallbackEXT = NULL;
PFN_vkDestroyDebugReportCallbackEXT pVkDestroyDebugReportCallbackEXT = NULL;
PFN_vkDebugReportMessageEXT pVkDebugReportMessageEXT = NULL;

bool RegisterDebugCallback()
{
    VkResult result;
    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT;
    createInfo.pfnCallback = DebugMessageCallback;
    createInfo.pNext = NULL;
    createInfo.pUserData = NULL;

    pVkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(g_AppData.m_Instance, "vkCreateDebugReportCallbackEXT");
    pVkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(g_AppData.m_Instance, "vkDestroyDebugReportCallbackEXT");
    pVkDebugReportMessageEXT = (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(g_AppData.m_Instance, "vkDebugReportMessageEXT");

    result = pVkCreateDebugReportCallbackEXT(g_AppData.m_Instance, &createInfo, NULL, &g_AppData.m_DebugCallbackEXT);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    return true;
}

bool VerifyLayers()
{
    for (size_t i = 0; i < g_AppData.m_LayerNames.size(); i++)
    {
        bool foundLayer = false;
        for (size_t j = 0; j < g_AppData.m_LayerProperties.size(); j++)
        {
            if (!strcmp(g_AppData.m_LayerNames[i], g_AppData.m_LayerProperties[j].layerName))
            {
                foundLayer = true;
                break;
            }
        }
        if (!foundLayer)
        {
            return false;
        }
    }

    return true;
}

bool InitializeInstance()
{
    VkResult result;

    VkInstanceCreateInfo createInfo = {};
    VkApplicationInfo myAppInfo = {};

    g_AppData.m_ExtensionNames.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    g_AppData.m_ExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    g_AppData.m_ExtensionNames.push_back(VK_NN_VI_SURFACE_EXTENSION_NAME);
    g_AppData.m_LayerNames.push_back( "VK_LAYER_NN_vi_swapchain" );
    g_AppData.m_LayerNames.push_back( "VK_LAYER_LUNARG_standard_validation" );
    //g_AppData.m_LayerNames.push_back( "VK_LAYER_RENDERDOC_Capture" );
    g_AppData.m_LayerNames.push_back( "VK_LAYER_LUNARG_core_validation" );
    g_AppData.m_LayerNames.push_back( "VK_LAYER_LUNARG_parameter_validation" );
    g_AppData.m_LayerNames.push_back( "VK_LAYER_LUNARG_object_tracker" );

    if (!VerifyLayers())
    {
        return false;
    }

    myAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    myAppInfo.pNext = NULL;
    myAppInfo.pApplicationName = "VulkanTestBench";
    myAppInfo.applicationVersion = 1;
    myAppInfo.pEngineName = "VulkanTestBench";
    myAppInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &myAppInfo;
    createInfo.enabledLayerCount = (uint32_t)g_AppData.m_LayerNames.size();
    createInfo.ppEnabledLayerNames = g_AppData.m_LayerNames.data();
    createInfo.enabledExtensionCount = (uint32_t)g_AppData.m_ExtensionNames.size();
    createInfo.ppEnabledExtensionNames = g_AppData.m_ExtensionNames.data();

    result = vkCreateInstance(&createInfo, NULL, &g_AppData.m_Instance);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    return true;
}

bool InitializePhysicalDevice()
{
    VkResult result;
    uint32_t deviceCount = 0;

    result = vkEnumeratePhysicalDevices(g_AppData.m_Instance, &deviceCount, NULL);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    if (deviceCount < 1)
    {
        return false;
    }

        /* Grabs the first device available */
    deviceCount = 1;
    result = vkEnumeratePhysicalDevices(g_AppData.m_Instance, &deviceCount, &g_AppData.m_PhysicalDevice);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    vkGetPhysicalDeviceProperties(g_AppData.m_PhysicalDevice, &g_AppData.m_PhysicalDeviceProperties);
    vkGetPhysicalDeviceMemoryProperties(g_AppData.m_PhysicalDevice, &g_AppData.m_PhysicalDeviceMemoryProperties);
    vkGetPhysicalDeviceFeatures(g_AppData.m_PhysicalDevice, &g_AppData.m_PhysicalDeviceFeatures);

    NN_LOG("timestampPeriod: %g\n", g_AppData.m_PhysicalDeviceProperties.limits.timestampPeriod);
    return true;
}

VkResult CreateBuffer(VkBuffer* pBuffer, VkDeviceMemory* pBufferMemory, size_t size, void* data, VkBufferUsageFlags usageFlags)
{
    VkResult result;

    VkBufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.pNext = NULL;
    info.usage = usageFlags;
    info.queueFamilyIndexCount = 0;
    info.pQueueFamilyIndices = NULL;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.flags = 0;
    info.size = size;

    result = vkCreateBuffer(g_AppData.m_Device, &info, NULL, pBuffer);
    if (result != VK_SUCCESS)
    {
        return result;
    }

        /* Required size may be larger than our buffer */
    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(g_AppData.m_Device, *pBuffer, &memReqs);

    VkMemoryAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.memoryTypeIndex = 0;
    allocInfo.allocationSize = memReqs.size;

    if (!SelectMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &allocInfo.memoryTypeIndex))
    {
        NN_ASSERT(false, "Failed to select memory type\n");
    }

    result = vkAllocateMemory(g_AppData.m_Device, &allocInfo, NULL, pBufferMemory);
    if (result != VK_SUCCESS)
    {
        return result;
    }

    if (data != nullptr)
    {
        void* pData;
        result = vkMapMemory(g_AppData.m_Device, *pBufferMemory, 0, memReqs.size, 0, &pData);
        if (result != VK_SUCCESS)
        {
            return result;
        }

        memcpy(pData, data, size);
        vkUnmapMemory(g_AppData.m_Device, *pBufferMemory);
    }

    result = vkBindBufferMemory(g_AppData.m_Device, *pBuffer, *pBufferMemory, 0);
    return result;
}

bool InitializeLogicalDevice()
{
    VkResult result;

    const char* DeviceExtensions[] = {
        "VK_KHR_swapchain",
    };

    VkDeviceQueueCreateInfo queueInfo = {};
    float priority = 0.0;
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = NULL;
    queueInfo.flags = 0;
    queueInfo.queueFamilyIndex = 0;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &priority;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueInfo;
    createInfo.enabledLayerCount = (uint32_t)g_AppData.m_LayerNames.size();
    createInfo.ppEnabledLayerNames = g_AppData.m_LayerNames.data();
    createInfo.enabledExtensionCount = sizeof(DeviceExtensions) / sizeof(DeviceExtensions[0]);
    createInfo.ppEnabledExtensionNames = DeviceExtensions;
    createInfo.pEnabledFeatures = &g_AppData.m_PhysicalDeviceFeatures;

    result = vkCreateDevice(g_AppData.m_PhysicalDevice, &createInfo, NULL, &g_AppData.m_Device);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    vkGetDeviceQueue(g_AppData.m_Device, 0, 0, &g_AppData.m_Queue);

    return true;
}

bool InitializeQueue()
{
    uint32_t queueFamilyPropertyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(g_AppData.m_PhysicalDevice, &queueFamilyPropertyCount, NULL);

    if (queueFamilyPropertyCount > 0)
    {
        std::unique_ptr< VkQueueFamilyProperties > pQueueFamilyProperties(new VkQueueFamilyProperties[queueFamilyPropertyCount]);
        vkGetPhysicalDeviceQueueFamilyProperties(g_AppData.m_PhysicalDevice, &queueFamilyPropertyCount, pQueueFamilyProperties.get());
        memcpy(&g_AppData.m_QueueFamilyProperties, pQueueFamilyProperties.get(), sizeof(*pQueueFamilyProperties));
    }
    return true;
}

bool InitializeCommandBuffer()
{
    VkResult result;
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    info.pNext = NULL;
    info.queueFamilyIndex = 0;

    result = vkCreateCommandPool(g_AppData.m_Device, &info, NULL, &g_AppData.m_CommandPool);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = NULL;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = g_AppData.m_CommandPool;
    allocInfo.commandBufferCount = 1;
    result = vkAllocateCommandBuffers(g_AppData.m_Device, &allocInfo, &g_AppData.m_CommandBuffer);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    return true;
}

void SetImageLayoutTransition(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier imageMemBarrier = {};
    imageMemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemBarrier.pNext = NULL;
    imageMemBarrier.srcAccessMask = 0;
    imageMemBarrier.dstAccessMask = 0;
    imageMemBarrier.oldLayout = oldLayout;
    imageMemBarrier.newLayout = newLayout;
    imageMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemBarrier.image = image;
    imageMemBarrier.subresourceRange.aspectMask = aspectMask;
    imageMemBarrier.subresourceRange.baseMipLevel = 0;
    imageMemBarrier.subresourceRange.levelCount = 1;
    imageMemBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemBarrier.subresourceRange.layerCount = 1;

    if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        imageMemBarrier.srcAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        imageMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }

    if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        imageMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    }

    if (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
    {
        imageMemBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    }

    if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        imageMemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }

    if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED)
    {
        imageMemBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
    }

    if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        imageMemBarrier.srcAccessMask =
            VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        imageMemBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    }

    if (newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
    {
        imageMemBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    }

    if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        imageMemBarrier.dstAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        imageMemBarrier.dstAccessMask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }

    VkPipelineStageFlags src_stages = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    VkPipelineStageFlags dest_stages = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

    vkCmdPipelineBarrier(g_AppData.m_CommandBuffer, src_stages, dest_stages, 0, 0, NULL, 0, NULL,
        1, &imageMemBarrier);
}

bool InitializeSwapchain()
{
    VkResult result;

    VkViSurfaceCreateInfoNN createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_VI_SURFACE_CREATE_INFO_NN;
    createInfo.pNext = NULL;
    createInfo.flags = 0;

    nn::vi::NativeWindowHandle nativeWindow;
    nn::vi::GetNativeWindow(&nativeWindow, g_pLayer);
    createInfo.window = nativeWindow;

    result = vkCreateViSurfaceNN(g_AppData.m_Instance, &createInfo, NULL, &g_AppData.m_SurfaceKhr);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    VkBool32 queueSupportsPresent = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(g_AppData.m_PhysicalDevice, 0, g_AppData.m_SurfaceKhr, &queueSupportsPresent);

        /* Asserts if default queue doesn't support present */
    NN_ASSERT_NOT_EQUAL(queueSupportsPresent, static_cast< VkBool32 >(VK_FALSE));

    uint32_t formatCount = 0;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(g_AppData.m_PhysicalDevice, g_AppData.m_SurfaceKhr, &formatCount, NULL);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    VkSurfaceFormatKHR* pSurfaceFormatKHR = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(g_AppData.m_PhysicalDevice, g_AppData.m_SurfaceKhr, &formatCount, pSurfaceFormatKHR);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    if (formatCount == 1 && pSurfaceFormatKHR[0].format == VK_FORMAT_UNDEFINED)
    {
            /* No preference for format, use a default here */
        g_AppData.m_Format = VK_FORMAT_B8G8R8A8_UNORM;
    }
    else
    {
            /* Asserts if no VkSurfaceFormatKHRs found */
        NN_ASSERT_NOT_EQUAL(formatCount, 0U);
        g_AppData.m_Format = pSurfaceFormatKHR[0].format;
    }
    free(pSurfaceFormatKHR);

    VkSurfaceCapabilitiesKHR surfCapabilities;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(g_AppData.m_PhysicalDevice, g_AppData.m_SurfaceKhr, &surfCapabilities);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    uint32_t presentModeCount = 0;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(g_AppData.m_PhysicalDevice, g_AppData.m_SurfaceKhr, &presentModeCount, NULL);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    VkPresentModeKHR* pPresentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(g_AppData.m_PhysicalDevice, g_AppData.m_SurfaceKhr, &presentModeCount, pPresentModes);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    if (surfCapabilities.currentExtent.width == UINT32_MAX)
    {
        g_AppData.m_SwapChainExtent.width = 1280;
        g_AppData.m_SwapChainExtent.height = 720;
    }
    else
    {
        g_AppData.m_SwapChainExtent = surfCapabilities.currentExtent;
    }

    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (uint32_t i = 0; i < presentModeCount; i++)
    {
            /* This is the preferred and fastest mode */
        if (pPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            swapchainPresentMode = pPresentModes[i];
            break;
        }

            /* This mode is faster but tears */
        if (pPresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            swapchainPresentMode = pPresentModes[i];
        }
    }

    if (pPresentModes != NULL)
    {
        free(pPresentModes);
    }

    uint32_t desiredSwapImages = surfCapabilities.minImageCount + 1;
    if (surfCapabilities.maxImageCount > 0 && desiredSwapImages > surfCapabilities.maxImageCount)
    {
        desiredSwapImages = surfCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapChainInfo = {};
    swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainInfo.pNext = NULL;
    swapChainInfo.surface = g_AppData.m_SurfaceKhr;
    swapChainInfo.minImageCount = desiredSwapImages;
    swapChainInfo.imageFormat = g_AppData.m_Format;
    swapChainInfo.imageExtent = g_AppData.m_SwapChainExtent;
    swapChainInfo.preTransform = (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR : surfCapabilities.currentTransform;
    swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainInfo.imageArrayLayers = 1;
    swapChainInfo.presentMode = swapchainPresentMode;
    swapChainInfo.oldSwapchain = VK_NULL_HANDLE;
    swapChainInfo.clipped = true;
    swapChainInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainInfo.queueFamilyIndexCount = 0;
    swapChainInfo.pQueueFamilyIndices = NULL;

    result = vkCreateSwapchainKHR(g_AppData.m_Device, &swapChainInfo, NULL, &g_AppData.m_SwapChainKhr);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

        /* Query the total images */
    result = vkGetSwapchainImagesKHR(g_AppData.m_Device, g_AppData.m_SwapChainKhr, &g_AppData.m_SwapchainImageCount, NULL);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

        /* Grab the VkImage handles for the swap buffers */
    g_AppData.m_pSwapChainImages = (VkImage*)malloc(sizeof(VkImage) * g_AppData.m_SwapchainImageCount);
    result = vkGetSwapchainImagesKHR(g_AppData.m_Device, g_AppData.m_SwapChainKhr, &g_AppData.m_SwapchainImageCount, g_AppData.m_pSwapChainImages);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    g_AppData.m_pColorImageView = (VkImageView*)malloc(sizeof(VkImageView) * g_AppData.m_SwapchainImageCount);
    for (uint32_t i = 0; i < g_AppData.m_SwapchainImageCount; i++)
    {
        VkImageViewCreateInfo colorInfo = {};
        colorInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        colorInfo.pNext = NULL;
        colorInfo.format = g_AppData.m_Format;
        colorInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        colorInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        colorInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        colorInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        colorInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        colorInfo.subresourceRange.baseMipLevel = 0;
        colorInfo.subresourceRange.levelCount = 1;
        colorInfo.subresourceRange.baseArrayLayer = 0;
        colorInfo.subresourceRange.layerCount = 1;
        colorInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        colorInfo.flags = 0;
        colorInfo.image = g_AppData.m_pSwapChainImages[i];

        SetImageLayoutTransition(colorInfo.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        result = vkCreateImageView(g_AppData.m_Device, &colorInfo, NULL, &g_AppData.m_pColorImageView[i]);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);
    }

    g_AppData.m_CurrentBuffer = 0;

    return true;
}//NOLINT(impl/function_size)

bool InitializeDepthBuffer()
{
    VkResult result;

    {
        g_AppData.m_DepthBufferFormat = VK_FORMAT_D24_UNORM_S8_UINT;

        VkImageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.pNext = NULL;
        info.flags = 0;
        info.arrayLayers = 1;
        info.extent.width = g_AppData.m_SwapChainExtent.width;
        info.extent.height = g_AppData.m_SwapChainExtent.height;
        info.extent.depth = 1;
        info.format = g_AppData.m_DepthBufferFormat;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        info.mipLevels = 1;
        info.queueFamilyIndexCount = 0;
        info.pQueueFamilyIndices = NULL;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        result = vkCreateImage(g_AppData.m_Device, &info, NULL, &g_AppData.m_DepthBufferImage);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);
    }

    {
        VkMemoryAllocateInfo info = {};
        VkMemoryRequirements memReq;
        vkGetImageMemoryRequirements(g_AppData.m_Device, g_AppData.m_DepthBufferImage, &memReq);

        info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        info.pNext = NULL;
        info.allocationSize = memReq.size;

        if (!SelectMemoryType(memReq.memoryTypeBits, 0, &info.memoryTypeIndex))
        {
            NN_ASSERT(false, "Failed to select memory type\n");
        }

        result = vkAllocateMemory(g_AppData.m_Device, &info, NULL, &g_AppData.m_DepthBufferMemory);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

        result = vkBindImageMemory(g_AppData.m_Device, g_AppData.m_DepthBufferImage, g_AppData.m_DepthBufferMemory, 0);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);
    }

    {
        VkImageViewCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = NULL;
        info.image = g_AppData.m_DepthBufferImage;
        info.format = g_AppData.m_DepthBufferFormat;
        info.components.r = VK_COMPONENT_SWIZZLE_R;
        info.components.g = VK_COMPONENT_SWIZZLE_G;
        info.components.b = VK_COMPONENT_SWIZZLE_B;
        info.components.a = VK_COMPONENT_SWIZZLE_A;
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;
        info.subresourceRange.levelCount = 1;
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.flags = 0;

        result = vkCreateImageView(g_AppData.m_Device, &info, NULL, &g_AppData.m_DepthBufferImageView);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

       SetImageLayoutTransition(info.image, info.subresourceRange.aspectMask, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
       // SetImageLayoutTransition(info.image, info.subresourceRange.aspectMask, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    return true;
}

bool InitializeRenderPass()
{
    VkResult result;
    VkAttachmentDescription attachments[2];
    attachments[0].format = g_AppData.m_Format;
    attachments[0].flags = 0;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].format = g_AppData.m_DepthBufferFormat;
    attachments[1].flags = 0;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassInfo = {};
    subpassInfo.flags = 0;
    subpassInfo.inputAttachmentCount = 0;
    subpassInfo.pInputAttachments = NULL;
    subpassInfo.colorAttachmentCount = 1;
    subpassInfo.pColorAttachments = &colorReference;
    subpassInfo.pDepthStencilAttachment = &depthReference;
    subpassInfo.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassInfo.pPreserveAttachments = NULL;
    subpassInfo.preserveAttachmentCount = 0;
    subpassInfo.pResolveAttachments = NULL;

    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.flags = 0;
    createInfo.pNext = NULL;
    createInfo.attachmentCount = 2;
    createInfo.pAttachments = attachments;
    createInfo.dependencyCount = 0;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpassInfo;
    result = vkCreateRenderPass(g_AppData.m_Device, &createInfo, NULL, &g_AppData.m_RenderPass);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    return true;
}

bool InitializeFrameBuffer()
{
    VkResult result;

    VkImageView attachments[2];
    attachments[1] = g_AppData.m_DepthBufferImageView;

    VkFramebufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.pNext = NULL;
    info.renderPass = g_AppData.m_RenderPass;
    info.attachmentCount = 2;
    info.pAttachments = attachments;
    info.width = g_AppData.m_SwapChainExtent.width;
    info.height = g_AppData.m_SwapChainExtent.height;
    info.layers = 1;

    for (uint32_t i = 0; i < g_AppData.m_SwapchainImageCount; i++)
    {
        attachments[0] = g_AppData.m_pColorImageView[i];
        result = vkCreateFramebuffer(g_AppData.m_Device, &info, NULL, &g_AppData.m_pFramebuffers[i]);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);
    }

    return true;
}

enum DescriptorRangeAccess
{
    ReadOnly,   ///< Read only descriptors.
    ReadWrite,  ///< Read write descriptors.
};

enum DescriptorRangeType
{
    Texture,        ///< Texture descriptor range
    Buffer,         ///< Read write descriptors.
    StructuredBuffer,///< Read write descriptors.
    FormattedBuffer,///< Formatted buffer descriptor range
    ConstantBuffer, ///< Constant buffer descriptor range
    Sampler,        ///< Sampler descriptor range
};

struct DescriptorRange
{
    DescriptorRangeAccess Access;   ///< Range access type.
    DescriptorRangeType Type;       ///< Range type.
    int NumDescriptors;             ///< Number of descriptors. if unbound, need to pass NumDescriptor == FUSION_MAX_U32
    int BaseShaderRegister;
    int RegisterSpace;
};

VkDescriptorType GetDescriptorType(const DescriptorRangeType type, const DescriptorRangeAccess access)
{
    VkDescriptorType vulkanDescriptorType[2][6] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_SAMPLER }, // ReadOnly,   ///< Read only descriptors.
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_SAMPLER }  // ReadWrite,  ///< Read write descriptors.
    };

    return vulkanDescriptorType[int(access)][int(type)];
}

bool InitializeShaders()
{
    VkResult result;

    //VkDevice VkDeviceValue = g_AppData.m_Device;

    VkShaderModuleCreateInfo moduleCreateInfo;

    nn::Result openResult;
    nn::fs::FileHandle vertexSpirvFile;
    openResult = nn::fs::OpenFile(&vertexSpirvFile, "Contents:/vertex_shader.spv", nn::fs::OpenMode_Read);
    NN_ASSERT(openResult.IsSuccess());

    int64_t vertexSize = -1;
    openResult = nn::fs::GetFileSize(&vertexSize, vertexSpirvFile);
    NN_ASSERT(openResult.IsSuccess());

    std::unique_ptr< uint32_t > pVertexData(new uint32_t[static_cast<size_t>(vertexSize) / sizeof(uint32_t)]);
    nn::fs::ReadFile(vertexSpirvFile, 0, pVertexData.get(), static_cast<size_t>(vertexSize));
    nn::fs::CloseFile(vertexSpirvFile);

    g_AppData.m_pShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    g_AppData.m_pShaderStages[0].pNext = NULL;
    g_AppData.m_pShaderStages[0].pSpecializationInfo = NULL;
    g_AppData.m_pShaderStages[0].flags = 0;
    g_AppData.m_pShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    g_AppData.m_pShaderStages[0].pName = "main";

    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = NULL;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = static_cast<size_t>(vertexSize);
    moduleCreateInfo.pCode = pVertexData.get();
    result = vkCreateShaderModule(g_AppData.m_Device, &moduleCreateInfo, NULL, &g_AppData.m_pShaderStages[0].module);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    nn::fs::FileHandle fragmentSpirvFile;
    openResult = nn::fs::OpenFile(&fragmentSpirvFile, "Contents:/fragment_shader.spv", nn::fs::OpenMode_Read);
    NN_ASSERT(openResult.IsSuccess());

    int64_t fragmentSize = -1;
    openResult = nn::fs::GetFileSize(&fragmentSize, fragmentSpirvFile);
    NN_ASSERT(openResult.IsSuccess());

    std::unique_ptr< uint32_t > pFragmentData(new uint32_t[static_cast<size_t>(fragmentSize) / sizeof(uint32_t)]);
    nn::fs::ReadFile(fragmentSpirvFile, 0, pFragmentData.get(), static_cast<size_t>(fragmentSize));
    nn::fs::CloseFile(fragmentSpirvFile);

    g_AppData.m_pShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    g_AppData.m_pShaderStages[1].pNext = NULL;
    g_AppData.m_pShaderStages[1].pSpecializationInfo = NULL;
    g_AppData.m_pShaderStages[1].flags = 0;
    g_AppData.m_pShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    g_AppData.m_pShaderStages[1].pName = "main";

    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = NULL;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = static_cast<size_t>(fragmentSize);
    moduleCreateInfo.pCode = pFragmentData.get();
    result = vkCreateShaderModule(g_AppData.m_Device, &moduleCreateInfo, NULL, &g_AppData.m_pShaderStages[1].module);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    //laod compute shader
    nn::fs::FileHandle computeSpirvFile;
    openResult = nn::fs::OpenFile(&computeSpirvFile, "Contents:/compute_shader.spv", nn::fs::OpenMode_Read);
    NN_ASSERT(openResult.IsSuccess());

    int64_t computeSize = -1;
    openResult = nn::fs::GetFileSize(&computeSize, computeSpirvFile);
    NN_ASSERT(openResult.IsSuccess());

    std::unique_ptr< uint32_t > pcomputeData(new uint32_t[static_cast<size_t>(computeSize) / sizeof(uint32_t)]);
    nn::fs::ReadFile(computeSpirvFile, 0, pcomputeData.get(), static_cast<size_t>(computeSize));
    nn::fs::CloseFile(computeSpirvFile);

    g_AppData.m_pShaderStages[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    g_AppData.m_pShaderStages[2].pNext = NULL;
    g_AppData.m_pShaderStages[2].pSpecializationInfo = NULL;
    g_AppData.m_pShaderStages[2].flags = 0;
    g_AppData.m_pShaderStages[2].stage = VK_SHADER_STAGE_COMPUTE_BIT;
    g_AppData.m_pShaderStages[2].pName = "CSMain";

    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = NULL;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = static_cast<size_t>(computeSize);
    moduleCreateInfo.pCode = pcomputeData.get();
    result = vkCreateShaderModule(g_AppData.m_Device, &moduleCreateInfo, NULL, &g_AppData.m_pShaderStages[2].module);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);       
    return true;
}

bool InitializeTexture()
{
    VkResult result;

    {
        VkImageCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.pNext = NULL;
        info.flags = 0;
        info.arrayLayers = 1;
        info.extent.width = 512;
        info.extent.height = 512;
        info.extent.depth = 1;
        info.format = VK_FORMAT_R8G8B8A8_UNORM;
        info.imageType = VK_IMAGE_TYPE_2D;
        info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
        info.mipLevels = 1;
        info.pQueueFamilyIndices = NULL;
        info.queueFamilyIndexCount = 0;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.tiling = VK_IMAGE_TILING_LINEAR;
        info.usage = VK_IMAGE_USAGE_SAMPLED_BIT;

        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(g_AppData.m_PhysicalDevice, info.format, &formatProperties);

            /* Asserts if physical device doesn't support linear textures of specified format */
        NN_ASSERT_EQUAL((formatProperties.linearTilingFeatures & (VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)),
            static_cast< VkFormatFeatureFlags >(VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT));

        result = vkCreateImage(g_AppData.m_Device, &info, NULL, &g_AppData.m_TextureImage);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

        VkImageSubresource subresource;
        subresource.arrayLayer = 0;
        subresource.mipLevel = 0;
        subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        VkSubresourceLayout layout;
        vkGetImageSubresourceLayout(g_AppData.m_Device, g_AppData.m_TextureImage, &subresource, &layout);

        VkMemoryRequirements memReq;
        vkGetImageMemoryRequirements(g_AppData.m_Device, g_AppData.m_TextureImage, &memReq);

        VkMemoryAllocateInfo allocInfo;
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = NULL;
        allocInfo.memoryTypeIndex = 0;
        allocInfo.allocationSize = memReq.size;

        if (!SelectMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &allocInfo.memoryTypeIndex))
        {
            NN_ASSERT(false, "Failed to select memory type\n");
        }

        result = vkAllocateMemory(g_AppData.m_Device, &allocInfo, NULL, &g_AppData.m_TextureMemory);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

        uint8_t* pData = NULL;

        result = vkMapMemory(g_AppData.m_Device, g_AppData.m_TextureMemory, 0, memReq.size, 0, (void**)&pData);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

        for (uint32_t yOff = 0; yOff < 512; yOff++)
        {
            for (uint32_t xOff = 0; xOff < 512; xOff++)
            {
                uint8_t value = ((xOff / 32) & 1) ^ ((yOff / 32) & 1) ? 0xFF : 0x80;
                pData[4 * xOff + 0] = value;
                pData[4 * xOff + 1] = value;
                pData[4 * xOff + 2] = value;
                pData[4 * xOff + 3] = 0xFF;
            }
            pData += layout.rowPitch;
        }

        vkUnmapMemory(g_AppData.m_Device, g_AppData.m_TextureMemory);

        result = vkBindImageMemory(g_AppData.m_Device, g_AppData.m_TextureImage, g_AppData.m_TextureMemory, 0);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

            /* Need an image barrier to transition the texture data */
        SetImageLayoutTransition(g_AppData.m_TextureImage, VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_GENERAL);

        VkImageViewCreateInfo viewInfo;
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.pNext = NULL;
        viewInfo.flags = 0;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        viewInfo.format = info.format;
        viewInfo.image = g_AppData.m_TextureImage;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

        result = vkCreateImageView(g_AppData.m_Device, &viewInfo, NULL, &g_AppData.m_TextureImageView);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);
    }

    {
        VkSamplerCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.anisotropyEnable = VK_FALSE;
        info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        info.compareEnable = VK_FALSE;
        info.compareOp = VK_COMPARE_OP_ALWAYS;
        info.flags = 0;
        info.magFilter = VK_FILTER_LINEAR;
        info.minFilter = VK_FILTER_LINEAR;
        info.maxAnisotropy = 1.0f;
        info.maxLod = 0;
        info.minLod = 0;
        info.mipLodBias = 0;
        info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        info.pNext = NULL;
        info.unnormalizedCoordinates = VK_FALSE;

        result = vkCreateSampler(g_AppData.m_Device, &info, NULL, &g_AppData.m_TextureSampler);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);
    }
    return true;
}//NOLINT(impl/function_size)

bool InitializeScene()
{
    VkResult result;

    if (!InitializeShaders())
    {
        return false;
    }

    if (!InitializeTexture())
    {
        return false;
    }
    //=======================
    {
            /*g_AppData.m_Device = device;
            g_AppData.m_PhysicalDevice = physicalDevice;
            vkGetPhysicalDeviceMemoryProperties(g_AppData.m_PhysicalDevice, &g_AppData.m_PhysicalDeviceMemoryProperties);
            g_AppData.m_CommandBuffer = vulkanDevice->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);*/
            VkResult result;            
            //VkPipelineShaderStageCreateInfo computeShaderStageInfo;
            /*std::string fileName = getAssetPath() + "shaders/computeshader/emboss.comp.spv";            
            computeShaderStageInfo = loadShader(fileName, VK_SHADER_STAGE_COMPUTE_BIT);
            g_AppData.m_pShaderStages[2] = computeShaderStageInfo;*/
            //Create Compute Pipeline
            
            VkDescriptorSetLayout m_computeSetlayout;
            {
                VkDescriptorSetLayoutBinding computebinding{};
                computebinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                computebinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
                computebinding.binding = 0;
                computebinding.descriptorCount = 1;
                VkDescriptorSetLayoutBinding computeSetLayoutBindings[5];
                computeSetLayoutBindings[0] = computebinding;
                computebinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                computebinding.binding = 6;
                computeSetLayoutBindings[1] = computebinding;
                computebinding.binding = 7;
                computeSetLayoutBindings[2] = computebinding;
                computebinding.binding = 12;
                computeSetLayoutBindings[3] = computebinding;
                computebinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                computebinding.binding = 1;
                computeSetLayoutBindings[4] = computebinding;

                VkDescriptorSetLayoutCreateInfo computeSetLayoutCreateInfo{};
                computeSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                computeSetLayoutCreateInfo.flags = 0;
                computeSetLayoutCreateInfo.pNext = nullptr;
                computeSetLayoutCreateInfo.bindingCount = 5;
                computeSetLayoutCreateInfo.pBindings = computeSetLayoutBindings;
                vkCreateDescriptorSetLayout(g_AppData.m_Device, &computeSetLayoutCreateInfo, nullptr, &m_computeSetlayout);
            }                                                
    
            VkPipelineLayout m_computePipelineLayout;
            {
                VkPipelineLayoutCreateInfo computeLayoutCreateInfo{};
                computeLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                computeLayoutCreateInfo.setLayoutCount = 1;
                computeLayoutCreateInfo.pSetLayouts = &m_computeSetlayout;
                vkCreatePipelineLayout(g_AppData.m_Device, &computeLayoutCreateInfo, nullptr, &m_computePipelineLayout);
            }
           
            VkPipeline m_computePipeline;
            {
                VkComputePipelineCreateInfo computeCreateInfo{};
                computeCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
                computeCreateInfo.layout = m_computePipelineLayout;
                computeCreateInfo.flags = 0;
                computeCreateInfo.stage = g_AppData.m_pShaderStages[2];

                VkPipelineCache pipelineCache;
                VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
                pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
                vkCreatePipelineCache(g_AppData.m_Device, &pipelineCacheCreateInfo, nullptr, &pipelineCache);

                result = vkCreateComputePipelines(g_AppData.m_Device, pipelineCache, 1, &computeCreateInfo, nullptr, &m_computePipeline);
            }                 
            g_AppData.m_ComputePipeline = m_computePipeline;
            g_AppData.m_ComputePipelineLayout = m_computePipelineLayout;

            //create Image, for SRV
            {
                VkImageCreateInfo info;
                info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                info.pNext = NULL;
                info.flags = 0;
                info.arrayLayers = 1;
                info.extent.width = 512;
                info.extent.height = 512;
                info.extent.depth = 1;
                info.format = VK_FORMAT_R8G8B8A8_UNORM;
                info.imageType = VK_IMAGE_TYPE_2D;
                info.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
                //info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                info.mipLevels = 1;
                info.pQueueFamilyIndices = NULL;
                info.queueFamilyIndexCount = 0;
                info.samples = VK_SAMPLE_COUNT_1_BIT;
                info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                info.tiling = VK_IMAGE_TILING_OPTIMAL;
                info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

                VkFormatProperties formatProperties;
                vkGetPhysicalDeviceFormatProperties(g_AppData.m_PhysicalDevice, info.format, &formatProperties);
        
                /* Asserts if physical device doesn't support linear textures of specified format */
               
                VkImageFormatProperties xxx2;
                vkGetPhysicalDeviceImageFormatProperties(g_AppData.m_PhysicalDevice, info.format, info.imageType, info.tiling, info.usage, info.flags, &xxx2);
                result = vkCreateImage(g_AppData.m_Device, &info, NULL, &g_AppData.m_ComputeTextureImage);
               
        
                VkImageSubresource subresource;
                subresource.arrayLayer = 0;
                subresource.mipLevel = 0;
                subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

                /* VkSubresourceLayout layout;
                 vkGetImageSubresourceLayout(g_AppData.m_Device, g_AppData.m_ComputeTextureImage, &subresource, &layout);*/

                VkMemoryRequirements memReq;
                vkGetImageMemoryRequirements(g_AppData.m_Device, g_AppData.m_ComputeTextureImage, &memReq);

                VkMemoryAllocateInfo allocInfo;
                allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocInfo.pNext = NULL;
                allocInfo.memoryTypeIndex = 0;
                allocInfo.allocationSize = memReq.size;
                allocInfo.memoryTypeIndex = 0;
                if (!SelectMemoryType(memReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &allocInfo.memoryTypeIndex))
                {
                    NN_ASSERT(false, "Failed to select memory type\n");
                }

                result = vkAllocateMemory(g_AppData.m_Device, &allocInfo, NULL, &g_AppData.m_ComputeTextureMemory);
                

                uint8_t* pData = NULL;

                /*result = vkMapMemory(g_AppData.m_Device, g_AppData.m_ComputeTextureMemory, 0, memReq.size, 0, (void**)&pData);
                NN_ASSERT_EQUAL(result, VK_SUCCESS);*/

                /* for (uint32_t yOff = 0; yOff < 512; yOff++)
                 {
                     for (uint32_t xOff = 0; xOff < 512; xOff++)
                     {
                         uint8_t value = ((xOff / 32) & 1) ^ ((yOff / 32) & 1) ? 0xFF : 0x80;
                         pData[4 * xOff + 0] = value;
                         pData[4 * xOff + 1] = value;
                         pData[4 * xOff + 2] = value;
                         pData[4 * xOff + 3] = 0xFF;
                     }
                     pData += layout.rowPitch;
                 }*/

               // vkUnmapMemory(g_AppData.m_Device, g_AppData.m_ComputeTextureMemory);

                result = vkBindImageMemory(g_AppData.m_Device, g_AppData.m_ComputeTextureImage, g_AppData.m_ComputeTextureMemory, 0);
                

                /* Need an image barrier to transition the texture data */
                SetImageLayoutTransition(g_AppData.m_ComputeTextureImage, VK_IMAGE_ASPECT_COLOR_BIT,
                    VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_GENERAL);


                {
                    /*VkImageMemoryBarrier imageMemBarrier = {};
                    imageMemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    imageMemBarrier.pNext = NULL;
                    imageMemBarrier.srcAccessMask = 0;
                    imageMemBarrier.dstAccessMask = 0;
                    imageMemBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                    imageMemBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imageMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    imageMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    imageMemBarrier.image = image;
                    imageMemBarrier.subresourceRange.aspectMask = aspectMask;
                    imageMemBarrier.subresourceRange.baseMipLevel = 0;
                    imageMemBarrier.subresourceRange.levelCount = 1;
                    imageMemBarrier.subresourceRange.baseArrayLayer = 0;
                    imageMemBarrier.subresourceRange.layerCount = 1;
                    imageMemBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                    if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                    {
                        imageMemBarrier.srcAccessMask =
                            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    }

                    if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
                    {
                        imageMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    }

                    if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
                    {
                        imageMemBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    }

                    if (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
                    {
                        imageMemBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                    }

                    if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
                    {
                        
                    }

                    if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED)
                    {
                        imageMemBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
                    }

                    if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                    {
                        imageMemBarrier.srcAccessMask =
                            VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                        imageMemBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                    }

                    if (newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
                    {
                        imageMemBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                    }

                    if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                    {
                        imageMemBarrier.dstAccessMask =
                            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    }

                    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                    {
                        imageMemBarrier.dstAccessMask =
                            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    }

                    VkPipelineStageFlags src_stages = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
                    VkPipelineStageFlags dest_stages = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

                    vkCmdPipelineBarrier(g_AppData.m_CommandBuffer, src_stages, dest_stages, 0, 0, NULL, 0, NULL,
                        1, &imageMemBarrier);*/
                }

               // SetImageLayoutTransition(g_AppData.m_ComputeTextureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
               // SetImageLayoutTransition(g_AppData.m_ComputeTextureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
              //  SetImageLayoutTransition(g_AppData.m_ComputeTextureImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

                VkImageViewCreateInfo viewInfo;
                viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.pNext = NULL;
                viewInfo.flags = 0;
                viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
                viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
                viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
                viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
                viewInfo.format = info.format;
                viewInfo.image = g_AppData.m_ComputeTextureImage;
                viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                viewInfo.subresourceRange.baseArrayLayer = 0;
                viewInfo.subresourceRange.baseMipLevel = 0;
                viewInfo.subresourceRange.layerCount = 1;
                viewInfo.subresourceRange.levelCount = 1;
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

                result = vkCreateImageView(g_AppData.m_Device, &viewInfo, NULL, &g_AppData.m_ComputeTextureImageView);
                //NN_ASSERT_EQUAL(result, VK_SUCCESS);

                result = vkCreateImage(g_AppData.m_Device, &info, NULL, &g_AppData.m_ComputeTextureImage2);
                //NN_ASSERT_EQUAL(result, VK_SUCCESS);

                result = vkAllocateMemory(g_AppData.m_Device, &allocInfo, NULL, &g_AppData.m_ComputeTextureMemory2);
                //NN_ASSERT_EQUAL(result, VK_SUCCESS);

                VkMemoryRequirements memReq2;
                vkGetImageMemoryRequirements(g_AppData.m_Device, g_AppData.m_ComputeTextureImage2, &memReq2);
                result = vkBindImageMemory(g_AppData.m_Device, g_AppData.m_ComputeTextureImage2, g_AppData.m_ComputeTextureMemory2, 0);
                //NN_ASSERT_EQUAL(result, VK_SUCCESS);

                /* Need an image barrier to transition the texture data */
                SetImageLayoutTransition(g_AppData.m_ComputeTextureImage2, VK_IMAGE_ASPECT_COLOR_BIT,
                    VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_GENERAL);
                viewInfo.image = g_AppData.m_ComputeTextureImage2;
                result = vkCreateImageView(g_AppData.m_Device, &viewInfo, NULL, &g_AppData.m_ComputeTextureImageView2);
                //NN_ASSERT_EQUAL(result, VK_SUCCESS);
            }
           // vulkanDevice->flushCommandBuffer(g_AppData.m_CommandBuffer, queue);
         //   g_AppData.m_CommandBuffer = 0;
            //create Sampler, image info
            VkSampler computeImgeSampler;
            {
    
            VkSamplerCreateInfo sampler = {};
            sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            sampler.maxAnisotropy = 1.0f;
            sampler.magFilter = VK_FILTER_LINEAR;
            sampler.minFilter = VK_FILTER_LINEAR;
            sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            sampler.addressModeV = sampler.addressModeU;
            sampler.addressModeW = sampler.addressModeU;
            sampler.mipLodBias = 0.0f;
            sampler.maxAnisotropy = 1.0f;
            sampler.compareOp = VK_COMPARE_OP_NEVER;
            sampler.minLod = 0.0f;
            sampler.maxLod = 1;
            sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            vkCreateSampler(g_AppData.m_Device, &sampler, nullptr, &computeImgeSampler);
            }

            //create Uniform Buffer
            VkDescriptorBufferInfo uniformBufferInfo;
            VkDescriptorBufferInfo storageBufferInfo;
            {
                int32_t data[8] = { 1,0,3,4,5,6,7,8 };
                void* pData;
                int dataSize = sizeof(data);

                VkBuffer uniformBuffer;
                VkBufferCreateInfo bufferCreateInfo;
                bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferCreateInfo.pNext = nullptr;
                bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                bufferCreateInfo.queueFamilyIndexCount = 0;
                bufferCreateInfo.pQueueFamilyIndices = NULL;
                bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                bufferCreateInfo.flags = 0;
                bufferCreateInfo.size = dataSize;
                result = vkCreateBuffer(g_AppData.m_Device, &bufferCreateInfo, nullptr, &uniformBuffer);
                NN_ASSERT_EQUAL(result, VK_SUCCESS);

                VkMemoryRequirements memReqs;
                vkGetBufferMemoryRequirements(g_AppData.m_Device, uniformBuffer, &memReqs);

                VkMemoryAllocateInfo allocInfo;
                allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocInfo.pNext = NULL;
                allocInfo.memoryTypeIndex = 0;
                allocInfo.allocationSize = memReqs.size;

                if (!SelectMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &allocInfo.memoryTypeIndex))
                {
                    NN_ASSERT(false, "Failed to select memory type\n");
                }


                VkDeviceMemory uniformBufferMemory;
                result = vkAllocateMemory(g_AppData.m_Device, &allocInfo, NULL, &uniformBufferMemory);
                NN_ASSERT_EQUAL(result, VK_SUCCESS);
               
                result = vkMapMemory(g_AppData.m_Device, uniformBufferMemory, 0, memReqs.size, 0, &pData);
                memcpy(pData, data, dataSize);
                vkUnmapMemory(g_AppData.m_Device, uniformBufferMemory);
                result = vkBindBufferMemory(g_AppData.m_Device, uniformBuffer, uniformBufferMemory, 0);
                NN_ASSERT_EQUAL(result, VK_SUCCESS);

                //VkDescriptorBufferInfo uniformBufferInfo;
                uniformBufferInfo.buffer = uniformBuffer;
                uniformBufferInfo.offset = 0;
                uniformBufferInfo.range = dataSize;              

                dataSize = 16;
                VkBuffer storageBuffer;
               // VkBufferCreateInfo bufferCreateInfo;
                bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferCreateInfo.pNext = nullptr;
                bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
                bufferCreateInfo.queueFamilyIndexCount = 0;
                bufferCreateInfo.pQueueFamilyIndices = NULL;
                bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                bufferCreateInfo.flags = 0;
                bufferCreateInfo.size = dataSize;
                result = vkCreateBuffer(g_AppData.m_Device, &bufferCreateInfo, nullptr, &storageBuffer);
                NN_ASSERT_EQUAL(result, VK_SUCCESS);

                //VkMemoryRequirements memReqs;
                vkGetBufferMemoryRequirements(g_AppData.m_Device, storageBuffer, &memReqs);

             //   VkMemoryAllocateInfo allocInfo;
                allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocInfo.pNext = NULL;
                allocInfo.memoryTypeIndex = 0;
                allocInfo.allocationSize = memReqs.size;

                if (!SelectMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &allocInfo.memoryTypeIndex))
                {
                    NN_ASSERT(false, "Failed to select memory type\n");
                }


                VkDeviceMemory storageBufferMemory;
                result = vkAllocateMemory(g_AppData.m_Device, &allocInfo, NULL, &storageBufferMemory);
                NN_ASSERT_EQUAL(result, VK_SUCCESS);

                result = vkMapMemory(g_AppData.m_Device, storageBufferMemory, 0, memReqs.size, 0, &pData);
                //memcpy(pData, data, dataSize);
                memset(pData, 0, dataSize);
                vkUnmapMemory(g_AppData.m_Device, storageBufferMemory);
                result = vkBindBufferMemory(g_AppData.m_Device, storageBuffer, storageBufferMemory, 0);
                NN_ASSERT_EQUAL(result, VK_SUCCESS);

                storageBufferInfo.buffer = storageBuffer;                
                storageBufferInfo.offset = 0;
                storageBufferInfo.range = dataSize;
            }
            //create descriptor set
            {
                VkDescriptorPoolSize typeCount[3];
                typeCount[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                typeCount[0].descriptorCount = 1;
                typeCount[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                typeCount[1].descriptorCount = 2;
                typeCount[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                typeCount[2].descriptorCount = 2;

                VkDescriptorPoolCreateInfo info;
                info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                info.pNext = NULL;
                info.flags = 0;
                info.maxSets = 1;
                info.poolSizeCount = sizeof(typeCount) / sizeof(typeCount[0]);
                info.pPoolSizes = typeCount;
                result = vkCreateDescriptorPool(g_AppData.m_Device, &info, NULL, &g_AppData.m_ComputeDescriptorPool);
               // NN_ASSERT_EQUAL(result, VK_SUCCESS);


                VkDescriptorSetAllocateInfo m_ComputeDescriptorSetAllocInfo {};
                m_ComputeDescriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                m_ComputeDescriptorSetAllocInfo.pNext = NULL;
                m_ComputeDescriptorSetAllocInfo.descriptorPool = g_AppData.m_ComputeDescriptorPool;
                m_ComputeDescriptorSetAllocInfo.pSetLayouts = &m_computeSetlayout;
                m_ComputeDescriptorSetAllocInfo.descriptorSetCount = 1;      
                vkAllocateDescriptorSets(g_AppData.m_Device, &m_ComputeDescriptorSetAllocInfo, &g_AppData.m_ComputeDescriptorSet);
        
                VkDescriptorImageInfo computeBufferInfo;
                computeBufferInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                computeBufferInfo.imageView = g_AppData.m_ComputeTextureImageView;
                computeBufferInfo.sampler = computeImgeSampler;

                VkDescriptorImageInfo computeBufferInfo2;
                computeBufferInfo2.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
                computeBufferInfo2.imageView = g_AppData.m_ComputeTextureImageView2;
                computeBufferInfo2.sampler = computeImgeSampler;

                VkWriteDescriptorSet writeDescriptorSet {};
                writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writeDescriptorSet.dstSet = g_AppData.m_ComputeDescriptorSet;
                writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                writeDescriptorSet.dstBinding = 0;
                writeDescriptorSet.pBufferInfo = &uniformBufferInfo;
                //writeDescriptorSet.pImageInfo = &computeBufferInfo;
                writeDescriptorSet.descriptorCount = 1;

                VkWriteDescriptorSet computeWriteDescriptorSets[4];
                computeWriteDescriptorSets[0] = writeDescriptorSet;

       
                writeDescriptorSet.dstBinding = 6;
                writeDescriptorSet.pBufferInfo = &storageBufferInfo;
                writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
               // writeDescriptorSet.pImageInfo = &computeBufferInfo2;
                computeWriteDescriptorSets[1] = writeDescriptorSet;

                writeDescriptorSet.dstBinding = 7;
                computeWriteDescriptorSets[2] = writeDescriptorSet;
                writeDescriptorSet.dstBinding = 12;
                computeWriteDescriptorSets[3] = writeDescriptorSet;
            
                vkUpdateDescriptorSets(g_AppData.m_Device, 4, computeWriteDescriptorSets, 0, NULL);
            }




            //Create Compute CommandBuffer
            VkFenceCreateInfo fenceInfo = {};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.pNext = NULL;
            fenceInfo.flags = 0;
            result = vkCreateFence(g_AppData.m_Device, &fenceInfo, NULL, &g_AppData.m_ComputeFence);
           // NN_ASSERT_EQUAL(result, VK_SUCCESS);
            vkResetFences(g_AppData.m_Device, 1, &g_AppData.m_ComputeFence);
            //VkResult result;
            VkCommandPoolCreateInfo computeCommandPoolInfo = {};
            computeCommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            computeCommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            computeCommandPoolInfo.pNext = NULL;
            computeCommandPoolInfo.queueFamilyIndex = 0;
            VkCommandPool m_ComputeCommandPool;
            result = vkCreateCommandPool(g_AppData.m_Device, &computeCommandPoolInfo, NULL, &m_ComputeCommandPool);
            //NN_ASSERT_EQUAL(result, VK_SUCCESS);

            VkCommandBufferAllocateInfo allocInfo = {};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.pNext = NULL;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandPool = m_ComputeCommandPool;
            allocInfo.commandBufferCount = 1;
            result = vkAllocateCommandBuffers(g_AppData.m_Device, &allocInfo, &g_AppData.m_ComputeCommandBuffer);
           // NN_ASSERT_EQUAL(result, VK_SUCCESS);

        }


    //===========================================
    {
        VkPipelineCacheCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        info.pNext = NULL;
        info.initialDataSize = 0;
        info.pInitialData = NULL;
        info.flags = 0;

        result = vkCreatePipelineCache(g_AppData.m_Device, &info, NULL, &g_AppData.m_PipelineCache);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);
    }

    {
        VkDescriptorPoolSize typeCount[3];
        typeCount[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        typeCount[0].descriptorCount = 5;
        typeCount[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        typeCount[1].descriptorCount = 5;
        typeCount[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        typeCount[2].descriptorCount = 5;

        VkDescriptorPoolCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.pNext = NULL;
        info.flags = 0;
        info.maxSets = g_NumDescriptorSets;
        info.poolSizeCount = sizeof(typeCount) / sizeof(typeCount[0]);
        info.pPoolSizes = typeCount;
        result = vkCreateDescriptorPool(g_AppData.m_Device, &info, NULL, &g_AppData.m_DescriptorPool);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);
    }

    VkDescriptorSetLayout setlayouts[3];
    {
        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
        binding.descriptorCount = 1;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        binding.pImmutableSamplers = NULL; /* We will set samplers dynamically */
        binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutBinding bindings[5];


        bindings[0] = binding;
        binding.binding = 1;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        bindings[1] = binding;

        VkDescriptorSetLayoutCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.pNext = NULL;
        info.flags = 0;
        info.bindingCount = 2;
        info.pBindings = bindings;
        result = vkCreateDescriptorSetLayout(g_AppData.m_Device, &info, NULL, &g_AppData.m_DescriptorSetLayout[0]);
        setlayouts[0] = g_AppData.m_DescriptorSetLayout[0];
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

                
        info.bindingCount = 2;
        info.pBindings =bindings;
        result = vkCreateDescriptorSetLayout(g_AppData.m_Device, &info, NULL, &g_AppData.m_DescriptorSetLayout[1]);
        setlayouts[1] = g_AppData.m_DescriptorSetLayout[1];

        binding.binding = 1;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[1] = binding;

        result = vkCreateDescriptorSetLayout(g_AppData.m_Device, &info, NULL, &g_AppData.m_DescriptorSetLayout[2]);        
        setlayouts[2] = g_AppData.m_DescriptorSetLayout[2];
    }

    {
        VkDescriptorSetAllocateInfo info;
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        info.pNext = NULL;
        info.descriptorPool = g_AppData.m_DescriptorPool;
        info.descriptorSetCount = 3;
        info.pSetLayouts = g_AppData.m_DescriptorSetLayout;

        result = vkAllocateDescriptorSets(g_AppData.m_Device, &info, g_AppData.m_DescriptorSet);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

        VkDescriptorImageInfo imageDesc;
        imageDesc.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        imageDesc.imageView = g_AppData.m_TextureImageView;
        imageDesc.sampler = g_AppData.m_TextureSampler;

        VkWriteDescriptorSet descWrite;
        descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descWrite.pNext = NULL;
        descWrite.dstSet = g_AppData.m_DescriptorSet[0];
        descWrite.dstBinding = 0;
        descWrite.dstArrayElement = 0;
        descWrite.descriptorCount = 1;
        descWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descWrite.pBufferInfo = NULL;
        descWrite.pTexelBufferView = NULL;
        descWrite.pImageInfo = &imageDesc;
        vkUpdateDescriptorSets(g_AppData.m_Device, 1, &descWrite, 0, NULL);

        descWrite.dstSet = g_AppData.m_DescriptorSet[2];
        vkUpdateDescriptorSets(g_AppData.m_Device, 1, &descWrite, 0, NULL);
    }
    {
        VkPipelineLayoutCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.pNext = NULL;
        info.flags = 0;
        info.pushConstantRangeCount = 0;
        info.pPushConstantRanges = NULL;
        info.setLayoutCount = 2;
        info.pSetLayouts = setlayouts;

        result = vkCreatePipelineLayout(g_AppData.m_Device, &info, NULL, &g_AppData.m_PipelineLayout);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);


        info.setLayoutCount = 1;
        info.pSetLayouts = &setlayouts[0];
        result = vkCreatePipelineLayout(g_AppData.m_Device, &info, NULL, &g_AppData.m_PipelineLayout2);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);
    }

    {
        VkPipelineColorBlendAttachmentState attachments[1];
        attachments[0].colorWriteMask = 0xf;
        attachments[0].blendEnable = VK_FALSE;
        attachments[0].alphaBlendOp = VK_BLEND_OP_ADD;
        attachments[0].colorBlendOp = VK_BLEND_OP_ADD;
        attachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        attachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        attachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        attachments[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

        VkPipelineColorBlendStateCreateInfo colorBlendStateInfo;
        colorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateInfo.pNext = NULL;
        colorBlendStateInfo.flags = 0;
        colorBlendStateInfo.attachmentCount = 1;
        colorBlendStateInfo.pAttachments = attachments;
        colorBlendStateInfo.logicOpEnable = VK_FALSE;
        colorBlendStateInfo.logicOp = VK_LOGIC_OP_NO_OP;
        colorBlendStateInfo.blendConstants[0] = 1.0f;
        colorBlendStateInfo.blendConstants[1] = 1.0f;
        colorBlendStateInfo.blendConstants[2] = 1.0f;
        colorBlendStateInfo.blendConstants[3] = 1.0f;

        VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo;
        depthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilStateInfo.pNext = NULL;
        depthStencilStateInfo.flags = 0;
        depthStencilStateInfo.depthTestEnable = VK_TRUE;
        depthStencilStateInfo.depthWriteEnable = VK_TRUE;
        depthStencilStateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilStateInfo.stencilTestEnable = VK_FALSE;
        depthStencilStateInfo.back.failOp = VK_STENCIL_OP_KEEP;
        depthStencilStateInfo.back.passOp = VK_STENCIL_OP_KEEP;
        depthStencilStateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
        depthStencilStateInfo.back.compareMask = 0;
        depthStencilStateInfo.back.reference = 0;
        depthStencilStateInfo.back.depthFailOp = VK_STENCIL_OP_KEEP;
        depthStencilStateInfo.back.writeMask = 0;
        depthStencilStateInfo.minDepthBounds = 0;
        depthStencilStateInfo.maxDepthBounds = 0;
        depthStencilStateInfo.front = depthStencilStateInfo.back;

        VkDynamicState dynamicStates[VK_DYNAMIC_STATE_RANGE_SIZE];
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.pNext = NULL;
        dynamicStateInfo.flags = 0;
        dynamicStateInfo.dynamicStateCount = 0;
        dynamicStateInfo.pDynamicStates = dynamicStates;
        dynamicStates[dynamicStateInfo.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
        dynamicStates[dynamicStateInfo.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo;
        inputAssemblyStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyStateInfo.pNext = NULL;
        inputAssemblyStateInfo.flags = 0;
        inputAssemblyStateInfo.primitiveRestartEnable = VK_FALSE;
        inputAssemblyStateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        VkVertexInputBindingDescription bufferBindings[1];
        bufferBindings[0].binding = 0;
        bufferBindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bufferBindings[0].stride = sizeof(float) * 9; /* Buffer Stride */

        VkVertexInputAttributeDescription attribDescs[3];
        attribDescs[0].binding = 0; /* Buffer Binding */
        attribDescs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribDescs[0].location = 0; /* Shader location */
        attribDescs[0].offset = 0;
        attribDescs[1].binding = 0; /* Buffer Binding */
        attribDescs[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attribDescs[1].location = 1; /* Shader location */
        attribDescs[1].offset = sizeof(float) * 3;
        attribDescs[2].binding = 0; /* Buffer Binding */
        attribDescs[2].format = VK_FORMAT_R32G32_SFLOAT;
        attribDescs[2].location = 2; /* Shader location */
        attribDescs[2].offset = sizeof(float) * 7;


        VkPipelineVertexInputStateCreateInfo vertexInputStateInfo;
        memset(&vertexInputStateInfo, 0, sizeof(VkPipelineVertexInputStateCreateInfo));
        vertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateInfo.pNext = NULL;
        vertexInputStateInfo.flags = 0;
        vertexInputStateInfo.vertexBindingDescriptionCount = sizeof(bufferBindings) / sizeof(bufferBindings[0]);
        vertexInputStateInfo.pVertexBindingDescriptions = bufferBindings;
        vertexInputStateInfo.vertexAttributeDescriptionCount = sizeof(attribDescs) / sizeof(attribDescs[0]);
        vertexInputStateInfo.pVertexAttributeDescriptions = attribDescs;

        VkPipelineMultisampleStateCreateInfo multisampleStateInfo;
        multisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateInfo.pNext = NULL;
        multisampleStateInfo.flags = 0;
        multisampleStateInfo.pSampleMask = NULL;
        multisampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleStateInfo.sampleShadingEnable = VK_FALSE;
        multisampleStateInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleStateInfo.alphaToOneEnable = VK_FALSE;
        multisampleStateInfo.minSampleShading = 0.0;

        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo;
        rasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateInfo.pNext = NULL;
        rasterizationStateInfo.flags = 0;
        rasterizationStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationStateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationStateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateInfo.depthBiasEnable = VK_FALSE;
        rasterizationStateInfo.depthClampEnable = VK_TRUE;
        rasterizationStateInfo.lineWidth = 1.0f;

        VkPipelineViewportStateCreateInfo viewportStateInfo;
        viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateInfo.pNext = NULL;
        viewportStateInfo.flags = 0;
        viewportStateInfo.viewportCount = 1;
        viewportStateInfo.scissorCount = 1;
            /* These are part of the dynamic state */
        viewportStateInfo.pViewports = NULL;
        viewportStateInfo.pScissors = NULL;

        VkGraphicsPipelineCreateInfo info;
        info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        info.pNext = NULL;
        info.layout = g_AppData.m_PipelineLayout;
        info.basePipelineHandle = VK_NULL_HANDLE;
        info.basePipelineIndex = 0;
        info.flags = 0;
        info.pColorBlendState = &colorBlendStateInfo;
        info.pDepthStencilState = &depthStencilStateInfo;
        info.pDynamicState = &dynamicStateInfo;
        info.pInputAssemblyState = &inputAssemblyStateInfo;
        info.pMultisampleState = &multisampleStateInfo;
        info.pRasterizationState = &rasterizationStateInfo;
        info.stageCount = 2;
        info.pStages = g_AppData.m_pShaderStages;
        info.pTessellationState = NULL;
        info.pVertexInputState = &vertexInputStateInfo;
        info.pViewportState = &viewportStateInfo;
        info.renderPass = g_AppData.m_RenderPass;
        info.subpass = 0;

        result = vkCreateGraphicsPipelines(g_AppData.m_Device, g_AppData.m_PipelineCache, 1, &info, NULL, &g_AppData.m_Pipeline);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);
    }

        /* Setup Viewport */
    g_AppData.m_Viewport.x = 0;
    g_AppData.m_Viewport.y = 0;
    g_AppData.m_Viewport.width = static_cast< float >(g_AppData.m_SwapChainExtent.width);
    g_AppData.m_Viewport.height = static_cast< float >(g_AppData.m_SwapChainExtent.height);
    g_AppData.m_Viewport.minDepth = 0.0f;
    g_AppData.m_Viewport.maxDepth = 1.0f;

        /* Setup Scissor */
    g_AppData.m_Scissor.offset.x = 0;
    g_AppData.m_Scissor.offset.y = 0;
    g_AppData.m_Scissor.extent.width = g_AppData.m_SwapChainExtent.width;
    g_AppData.m_Scissor.extent.height = g_AppData.m_SwapChainExtent.height;

        /* Setup Vertex Buffer */
    g_AppData.m_VertexCount = 3;
    {
        float vertexData[] = {
            -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 1.0f,
            1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        };

        result = CreateBuffer(&g_AppData.m_VertexBuffer, &g_AppData.m_VertexBufferMem, sizeof(vertexData),
            vertexData, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    }

    {
        VkSemaphoreCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        info.pNext = NULL;
        info.flags = 0;
        result = vkCreateSemaphore(g_AppData.m_Device, &info, NULL, &g_AppData.m_PresentSemaphore);
    }

    {
        VkBuffer uniformBuffer;
        VkBufferCreateInfo bufferCreateInfo;
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.pNext =nullptr;   
        bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        bufferCreateInfo.queueFamilyIndexCount = 0;
        bufferCreateInfo.pQueueFamilyIndices = NULL;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCreateInfo.flags = 0;
        bufferCreateInfo.size = sizeof(float)*4;
        result = vkCreateBuffer(g_AppData.m_Device, &bufferCreateInfo, nullptr, &uniformBuffer);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

        VkMemoryRequirements memReqs;
        vkGetBufferMemoryRequirements(g_AppData.m_Device, uniformBuffer, &memReqs);    

        VkMemoryAllocateInfo allocInfo;
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = NULL;
        allocInfo.memoryTypeIndex = 0;
        allocInfo.allocationSize = memReqs.size;

        if (!SelectMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &allocInfo.memoryTypeIndex))
        {
            NN_ASSERT(false, "Failed to select memory type\n");
        }


        VkDeviceMemory uniformBufferMemory;
        result = vkAllocateMemory(g_AppData.m_Device, &allocInfo, NULL, &uniformBufferMemory);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);
        float data[4] = {1.0, 0.0, 0, 1};
        void* pData;
        result = vkMapMemory(g_AppData.m_Device, uniformBufferMemory, 0, memReqs.size, 0, &pData);
        memcpy(pData, data, sizeof(float)*4);
        vkUnmapMemory(g_AppData.m_Device, uniformBufferMemory);   
        result = vkBindBufferMemory(g_AppData.m_Device, uniformBuffer, uniformBufferMemory, 0);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

        VkDescriptorBufferInfo uniformBufferInfo;
        uniformBufferInfo.buffer = uniformBuffer;
        uniformBufferInfo.offset = 0;
        uniformBufferInfo.range = sizeof(float)*4;

        g_AppData.m_uniformBuffer = uniformBuffer;
        g_AppData.m_uniformBufferMemory = uniformBufferMemory;
        g_AppData.m_uniformBufferInfo = uniformBufferInfo;
    }
    
    return true;
}//NOLINT(impl/function_size)

bool InitializeVulkan()
{
    VkResult result;

    if (!InitializeExtensions())
    {
        return false;
    }
    if (!InitializeInstance())
    {
        return false;
    }
    if (!InitializePhysicalDevice())
    {
        return false;
    }
    if (!InitializeDeviceExtensions())
    {
        return false;
    }
    if (!InitializeQueue())
    {
        return false;
    }
    if (!InitializeLogicalDevice())
    {
        return false;
    }
    if (!RegisterDebugCallback())
    {
        return false;
    }
    if (!InitializeCommandBuffer())
    {
        return false;
    }

    {
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.pNext = NULL;
        info.flags = 0;
        info.pInheritanceInfo = NULL;
        result = vkBeginCommandBuffer(g_AppData.m_CommandBuffer, &info);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);
    }

    if (!InitializeSwapchain())
    {
        return false;
    }
    if (!InitializeDepthBuffer())
    {
        return false;
    }
    if (!InitializeRenderPass())
    {
        return false;
    }
    if (!InitializeFrameBuffer())
    {
        return false;
    }
    if (!InitializeScene())
    {
        return false;
    }

        /* Close off any commands and send */
    result = vkEndCommandBuffer(g_AppData.m_CommandBuffer);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    {
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = NULL;
        fenceInfo.flags = 0;
        result = vkCreateFence(g_AppData.m_Device, &fenceInfo, NULL, &g_AppData.m_Fence);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCreateInfo.pNext = nullptr;
        semaphoreCreateInfo.flags = 0;
        vkCreateSemaphore(g_AppData.m_Device, &semaphoreCreateInfo, nullptr, &g_AppData.m_SemaphoreSignal);
        VkSubmitInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.pNext = NULL;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &g_AppData.m_CommandBuffer;
        info.signalSemaphoreCount = 0;
        info.waitSemaphoreCount = 0;
        result = vkQueueSubmit(g_AppData.m_Queue, 1, &info, g_AppData.m_Fence);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

        result = vkQueueWaitIdle(g_AppData.m_Queue);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

            /* A fence is necessary to guarantee the command buffer is done */
        result = vkWaitForFences(g_AppData.m_Device, 1, &g_AppData.m_Fence, VK_TRUE, 10000000000ull);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);

        result = vkResetFences(g_AppData.m_Device, 1, &g_AppData.m_Fence);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);
    }

    return true;
}

void UpdateScene()
{
}

void DrawScene()
{
    VkResult result;

    VkWriteDescriptorSet descWrite;
    descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descWrite.pNext = NULL;
    descWrite.dstSet = g_AppData.m_DescriptorSet[1];
    descWrite.dstBinding = 1;
    descWrite.dstArrayElement = 0;
    descWrite.descriptorCount = 1;
    descWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descWrite.pBufferInfo = &g_AppData.m_uniformBufferInfo;
    descWrite.pTexelBufferView = NULL;
    descWrite.pImageInfo = nullptr;
    vkUpdateDescriptorSets(g_AppData.m_Device, 1, &descWrite, 0, NULL);

        /*  Assume we've submitted and synced and reset Command Buffers */
    result = vkResetCommandBuffer(g_AppData.m_CommandBuffer, 0);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    result = vkAcquireNextImageKHR(g_AppData.m_Device, g_AppData.m_SwapChainKhr, UINT64_MAX, g_AppData.m_PresentSemaphore, VK_NULL_HANDLE, &g_AppData.m_CurrentBuffer);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = NULL;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        beginInfo.pInheritanceInfo = NULL; /* Unused by Primary CBs */
        result = vkBeginCommandBuffer(g_AppData.m_CommandBuffer, &beginInfo);
        NN_ASSERT_EQUAL(result, VK_SUCCESS);
    }

    SetImageLayoutTransition(g_AppData.m_pSwapChainImages[g_AppData.m_CurrentBuffer], VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    {
        VkClearValue clearValues[2];
        clearValues[0].color.float32[0] = 0.2f;
        clearValues[0].color.float32[1] = 0.2f;
        clearValues[0].color.float32[2] = 0.2f;
        clearValues[0].color.float32[3] = 0.2f;
        clearValues[1].depthStencil.depth = 1.0f;
        clearValues[1].depthStencil.stencil = 0;

        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.pNext = NULL;
        info.renderPass = g_AppData.m_RenderPass;
        info.framebuffer = g_AppData.m_pFramebuffers[g_AppData.m_CurrentBuffer];
        info.renderArea.offset.x = 0;
        info.renderArea.offset.y = 0;
        info.renderArea.extent = g_AppData.m_SwapChainExtent;
        info.clearValueCount = 2;
        info.pClearValues = clearValues;

        vkCmdBeginRenderPass(g_AppData.m_CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    vkCmdBindPipeline(g_AppData.m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_AppData.m_Pipeline);
    vkCmdBindDescriptorSets(g_AppData.m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_AppData.m_PipelineLayout2, 0, 1, &g_AppData.m_DescriptorSet[0], 0, NULL);
    vkCmdBindDescriptorSets(g_AppData.m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_AppData.m_PipelineLayout, 1, 1, &g_AppData.m_DescriptorSet[1], 0, NULL);
    const VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(g_AppData.m_CommandBuffer, 0, 1, &g_AppData.m_VertexBuffer, offsets);
    vkCmdSetViewport(g_AppData.m_CommandBuffer, 0, 1, &g_AppData.m_Viewport);
    vkCmdSetScissor(g_AppData.m_CommandBuffer, 0, 1, &g_AppData.m_Scissor);
    
    vkCmdDraw(g_AppData.m_CommandBuffer, g_AppData.m_VertexCount, 1, 0, 0);

    vkCmdEndRenderPass(g_AppData.m_CommandBuffer);

        /* Transition the swapchain image so it can be presented */
    SetImageLayoutTransition(g_AppData.m_pSwapChainImages[g_AppData.m_CurrentBuffer], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    {
            /* Flush query/timestamp writes */
        VkMemoryBarrier memoryBarrier;
        memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        memoryBarrier.pNext = nullptr;
        memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        memoryBarrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT;
        vkCmdPipelineBarrier(g_AppData.m_CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_HOST_BIT, 0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
    }

    result = vkEndCommandBuffer(g_AppData.m_CommandBuffer);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    VkSemaphore waiteSemaphore[2];
    waiteSemaphore[0] = g_AppData.m_PresentSemaphore;
    waiteSemaphore[1] = g_AppData.m_SemaphoreSignal; 
        /* Pipelilne flags are required if we have a semaphore. */
    VkPipelineStageFlags pipelineStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = NULL;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &g_AppData.m_PresentSemaphore;
    submitInfo.pWaitDstStageMask = &pipelineStageMask;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &g_AppData.m_CommandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &g_AppData.m_SemaphoreSignal;
    result = vkQueueSubmit(g_AppData.m_Queue, 1, &submitInfo, g_AppData.m_Fence);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = NULL;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &g_AppData.m_SwapChainKhr;
    presentInfo.pImageIndices = &g_AppData.m_CurrentBuffer;
    presentInfo.waitSemaphoreCount = 0;
    presentInfo.pWaitSemaphores = NULL;
    presentInfo.pResults = NULL;

        /* Use the draw fence to sync before presenting. */
    result = vkWaitForFences(g_AppData.m_Device, 1, &g_AppData.m_Fence, VK_TRUE, 10000000000ull);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

        /* Reset the fence */
    result = vkResetFences(g_AppData.m_Device, 1, &g_AppData.m_Fence);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    result = vkQueuePresentKHR(g_AppData.m_Queue, &presentInfo);
    NN_ASSERT_EQUAL(result, VK_SUCCESS);

    //===
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &g_AppData.m_SemaphoreSignal;
    //submitInfo.pWaitDstStageMask = &pipelineStageMask;
    submitInfo.commandBufferCount = 0;
    submitInfo.pCommandBuffers = NULL;//&g_AppData.m_CommandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = NULL;
    vkQueueSubmit(g_AppData.m_Queue, 1, &submitInfo, VkFence());

    {
      //  g_AppData.m_Queue = queue;
        vkResetCommandBuffer(g_AppData.m_ComputeCommandBuffer, 0);
        {
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.pNext = NULL;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            beginInfo.pInheritanceInfo = NULL; /* Unused by Primary CBs */
            VkResult result = vkBeginCommandBuffer(g_AppData.m_ComputeCommandBuffer, &beginInfo);
            // NN_ASSERT_EQUAL(result, VK_SUCCESS);
        }

        vkCmdBindPipeline(g_AppData.m_ComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, g_AppData.m_ComputePipeline);
        vkCmdBindDescriptorSets(g_AppData.m_ComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, g_AppData.m_ComputePipelineLayout, 0, 1, &g_AppData.m_ComputeDescriptorSet, 0, 0);

        vkCmdDispatch(g_AppData.m_ComputeCommandBuffer, 1, 1, 1);

        vkEndCommandBuffer(g_AppData.m_ComputeCommandBuffer);



        // Submit compute commands
            // Use a fence to ensure that compute command buffer has finished executin before using it again
        //vkWaitForFences(g_AppData.m_Device, 1, &g_AppData.m_ComputeFence, VK_TRUE, UINT64_MAX);
        vkResetFences(g_AppData.m_Device, 1, &g_AppData.m_ComputeFence);

        VkSubmitInfo computeSubmitInfo{};
        computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        computeSubmitInfo.pNext = NULL;
        //computeSubmitInfo.waitSemaphoreCount = 0;
        //computeSubmitInfo.pWaitSemaphores = &g_AppData.m_PresentSemaphore;
        //computeSubmitInfo.pWaitDstStageMask = &pipelineStageMask;
        computeSubmitInfo.commandBufferCount = 1;
        computeSubmitInfo.pCommandBuffers = &g_AppData.m_ComputeCommandBuffer;
        computeSubmitInfo.signalSemaphoreCount = 0;
        computeSubmitInfo.pSignalSemaphores = NULL;

        vkQueueSubmit(g_AppData.m_Queue, 1, &computeSubmitInfo, g_AppData.m_ComputeFence);

    }
}

void FinalizeVulkan()
{
    for (int i = 0; i < g_MaxSwaps; ++i)
    {
        vkDestroyFramebuffer(g_AppData.m_Device, g_AppData.m_pFramebuffers[i], NULL);
    }

    vkDestroyFence(g_AppData.m_Device, g_AppData.m_Fence, NULL);
    vkDestroySemaphore(g_AppData.m_Device, g_AppData.m_PresentSemaphore, NULL);

    vkDestroyDescriptorPool(g_AppData.m_Device, g_AppData.m_DescriptorPool, NULL);
    vkDestroyDescriptorSetLayout(g_AppData.m_Device, g_AppData.m_DescriptorSetLayout[0], NULL);
    vkDestroyDescriptorSetLayout(g_AppData.m_Device, g_AppData.m_DescriptorSetLayout[1], NULL);

    vkFreeCommandBuffers(g_AppData.m_Device, g_AppData.m_CommandPool, 1, &g_AppData.m_CommandBuffer);
    vkDestroyCommandPool(g_AppData.m_Device, g_AppData.m_CommandPool, NULL);

    vkDestroyPipeline(g_AppData.m_Device, g_AppData.m_Pipeline, NULL);
    vkDestroyPipelineCache(g_AppData.m_Device, g_AppData.m_PipelineCache, NULL);
    vkDestroyPipelineLayout(g_AppData.m_Device, g_AppData.m_PipelineLayout, NULL);

    vkDestroyRenderPass(g_AppData.m_Device, g_AppData.m_RenderPass, NULL);

    vkDestroyBuffer(g_AppData.m_Device, g_AppData.m_VertexBuffer, NULL);
    vkFreeMemory(g_AppData.m_Device, g_AppData.m_VertexBufferMem, NULL);

    for (uint32_t i = 0; i < g_AppData.m_SwapchainImageCount; i++)
    {
        vkDestroyImageView(g_AppData.m_Device, g_AppData.m_pColorImageView[i], NULL);
    }

    free(g_AppData.m_pSwapChainImages);

    vkDestroyImageView(g_AppData.m_Device, g_AppData.m_TextureImageView, NULL);
    vkDestroyImage(g_AppData.m_Device, g_AppData.m_TextureImage, NULL);
    vkFreeMemory(g_AppData.m_Device, g_AppData.m_TextureMemory, NULL);
    vkDestroySampler(g_AppData.m_Device, g_AppData.m_TextureSampler, NULL);

    vkDestroyImageView(g_AppData.m_Device, g_AppData.m_DepthBufferImageView, NULL);
    vkDestroyImage(g_AppData.m_Device, g_AppData.m_DepthBufferImage, NULL);
    vkFreeMemory(g_AppData.m_Device, g_AppData.m_DepthBufferMemory, NULL);

    for ( size_t i = 0; i < sizeof(g_AppData.m_pShaderStages) / sizeof(g_AppData.m_pShaderStages[0]); ++i)
    {
        vkDestroyShaderModule(g_AppData.m_Device, g_AppData.m_pShaderStages[i].module, NULL);
    }

    vkDestroySwapchainKHR(g_AppData.m_Device, g_AppData.m_SwapChainKhr, NULL);

    vkDestroyDevice(g_AppData.m_Device, NULL);
    pVkDestroyDebugReportCallbackEXT(g_AppData.m_Instance, g_AppData.m_DebugCallbackEXT, NULL);
    vkDestroySurfaceKHR(g_AppData.m_Instance, g_AppData.m_SurfaceKhr, NULL);

    vkDestroyInstance(g_AppData.m_Instance, NULL);
}

namespace
{
    void* Allocate(size_t size, size_t alignment, void*)
    {
        return aligned_alloc(alignment, nn::util::align_up(size, alignment));
    }
    void Free(void* addr, void*)
    {
        free(addr);
    }
    void* Reallocate(void* addr, size_t newSize, void*)
    {
        return realloc(addr, newSize);
    }

}; // end anonymous namespace

extern "C" void nnMain()
{
    nn::Result result;

    nv::SetGraphicsAllocator(Allocate, Free, Reallocate, NULL);

    nv::SetGraphicsDevtoolsAllocator(Allocate, Free, Reallocate, NULL);

    nv::InitializeGraphics(malloc(g_GraphicsSystemMemorySize), g_GraphicsSystemMemorySize);

    size_t cacheSize = 0;
    result = nn::fs::QueryMountRomCacheSize(&cacheSize);
    NN_ASSERT(result.IsSuccess());

    void* mountRomCacheBuffer = malloc(cacheSize);
    NN_ASSERT_NOT_NULL(mountRomCacheBuffer);

    result = nn::fs::MountRom("Contents", mountRomCacheBuffer, cacheSize);
    NN_ASSERT(result.IsSuccess());

    nn::vi::Initialize();
    InitializeLayer();

    bool success = InitializeVulkan();
    NN_ASSERT(success, "InitializeVulkan Failed!\n");

        /* Initialize input for user exit */
    nn::hid::InitializeTouchScreen();
    nn::hid::TouchScreenState<1> touchScreenState;
    int touchCount = 0;

    NN_LOG("----------------------------------------------------------------------------------------\n");
    NN_LOG("Clicking the window on Windows or touching the Touch Screen on NX will exit the program.\n");
    NN_LOG("----------------------------------------------------------------------------------------\n");
    while (1)
    {
        UpdateScene();
        DrawScene();

        touchCount = nn::hid::GetTouchScreenStates(&touchScreenState, 1);
        if (touchScreenState.count > 0)
        {
            break;
        }
    }

    FinalizeVulkan();

    nn::vi::DestroyLayer(g_pLayer);
    nn::vi::CloseDisplay(g_pDisplay);
    nn::vi::Finalize();
}
