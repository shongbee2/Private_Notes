# set Debug callback

1.在创建 Instance的时候，需要设置他的layer和extension
layer需要添加 g_AppData.m_LayerNames.push_back( "VK_LAYER_LUNARG_standard_validation" );
extension 需要添加 g_AppData.m_ExtensionNames.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
在device的时候 设置 VK_EXT_DEBUG_MARKER_EXTENSION_NAME
2.设置回调函数，他的错误就会回调了，回调函数可以设置多次。
3.注意，如果使用了renderdoc，这个就不能用了。错误应该都给renderdoc了。

```
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

```
3.退出的时候销毁回调的注册。
这个都是参考Nintendo的vksimple示例。

4.Vulkan还有其他的记得Layer validation 的。
VK_LAYER_LUNARG_core_validation  
VK_LAYER_LUNARG_standard_validation  
VK_LAYER_LUNARG_parameter_validation  
VK_LAYER_LUNARG_object_tracker  
等等，这样在遇到错误时，会有更多的信息。
