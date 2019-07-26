# Shader Layout

- [Shader Layout](#shader-layout)
  - [VkDescriptorSetLayout](#vkdescriptorsetlayout)
  - [VkDescriptorSet](#vkdescriptorset)
  - [VkPipelineLayout](#vkpipelinelayout)
  

<span id="vkdescriptorsetLayout"/>  

## VkDescriptorSetLayout

shader描述的布局，表示shader的那个位置绑定了什么，例如 第一个位置绑定的是constantbuffer,第二个位置绑定是texture等。
创建他的函数是 vkCreateDescriptorSetLayout, 示例代码：  
```cpp
VkDescriptorSetLayout m_DescriptorSetLayout;
VkDescriptorSetLayoutBinding binding;
binding.binding = 0;
binding.descriptorCount = 1;
binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
binding.pImmutableSamplers = NULL; /* We will set samplers dynamically */
binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

VkDescriptorSetLayoutCreateInfo info;
info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
info.pNext = NULL;
info.flags = 0;
info.bindingCount = 1;
info.pBindings = &binding;
result = vkCreateDescriptorSetLayout(VkDeviceValue, &info, NULL, &m_DescriptorSetLayout);
NN_ASSERT_EQUAL(result, VK_SUCCESS);
```
这里要注意的是binding的内容必须要跟Shader的内容完全一致，否则就会创建pipeline 的时候失败。而且不同的硬件对某些类型有数量限制，switch 对 VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER 只能支持12个。  
VkDescriptorSetLayoutCreateInfo 结构体是创建的内容，注意用pNext，flag都需要硬件支持，如果硬件不支持，就会报错。switch就不支持这些flag，例如 VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT。
所有这些pNext的信息我也就一致是空的。具体怎么用我也不知道。
还有注意 VkDescriptorSetLayoutBinding 结构体中，只有cout，type，binding，并没有我直观的set,offset这样的属性，binding对应一个slot，cout我也不知道是连续绑定几个slot还是什么其他意思，我们都是使用的1。所以这里的uniform buffer最多只能bingd12次。多了就超出了，还有这个binding是layout的bingding，并不是uniform buffer的set。是两个不同的概念。我以前理解错了。
其他的就看官方文档了。

<span id="vkescriptorset" />  

## VkDescriptorSet

我实话，我还真不知道这个类做什么的，我只知道他很重要， vkUpdateDescriptorSets 的时候需要他，而这个就是更新layout的绑定状态的，例如切换贴图，例如切换buffer，都是通过他去更新的。 示例代码：

```cpp
VkDescriptorSetLayoutBinding* descriptorSetLayoutBindings = VK_NULL_HANDLE;

// Create a single pool to contain data for our static sampler descriptor set
VkDescriptorPoolSize poolSize = {};
poolSize.type = VK_DESCRIPTOR_TYPE_SAMPLER;
poolSize.descriptorCount = descriptorLayoutCreation.NumberStaticSamplers;

VkDescriptorPoolCreateInfo poolInfo = {};
poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
poolInfo.pNext = NULL;
poolInfo.maxSets = 1;
poolInfo.poolSizeCount = 1;
poolInfo.pPoolSizes = &poolSize;
poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

VkDescriptorSetLayout m_DescriptorSetPool;
FUSION_API_CALL(vkCreateDescriptorPool(m_Device, &poolInfo, NULL, &m_DescriptorSetPool));

VkDescriptorSetAllocateInfo allocInfo;
allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
allocInfo.pNext = NULL;
allocInfo.descriptorPool = m_DescriptorSetPool;
allocInfo.descriptorSetCount = 1;
allocInfo.pSetLayouts = &m_DescriptorSetLayout;
// Populate descriptor sets
FUSION_API_CALL(vkAllocateDescriptorSets(m_Device, &allocInfo, &m_DescriptorSet));
```

vkAllocateDescriptorSets 是可以创建多个的，有多少个 descriptorSetCount 就创建多少个 set，当然 layout, maxSets 也需要匹配。
vkCreateDescriptorPool 这个只能创建一个pool，里面指定了他最多容纳多少个set,以及每个pool里面的PoolSize。 poolSize我也是比较疑惑的，现在我认为是每个poolSize保存这一个类型和数量，可以有多个poolsize来保存不同的类型和数量，然后就表示这个pool的一个set最多可以容纳这些类型和数量了。然后layout确定set的具体样子。也就是说，layout可以是多个类型多个数量来创建一个set。
VkDescriptorSetLayout 这个就是set要使用的layout，根据我对setlayout的了解，他又60+个binding和不同的类型。

## VkPipelineLayout

这个就是用于创建pipelay的layout，跟Shader需要匹配。示例代码：

```cpp
VkPipelineLayout                    m_PipelineLayout;           
VkPipelineLayoutCreateInfo info;
info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
info.pNext = NULL;
info.flags = 0;
info.pushConstantRangeCount = 0;
info.pPushConstantRanges = NULL;
info.setLayoutCount = 1;
info.pSetLayouts = &m_DescriptorSetLayout;
result = vkCreatePipelineLayout(vkDeviceValue, &info, NULL, &m_PipelineLayout);                
```

他创建只需要SetLayout就可以了，只是我了解的情况是，一般这个setlayout会很多个，4，5个的样子，而且每个setlayout，他的binding都是用0开始的。而且也会有一些重叠把，例如sampler，uniformbuffer。这样我就只能理解为vulkan自动会把他展开。就不知道具体是什么情况。例如两个完全一样重叠的的setlayout，合在一起成成pipelinelayout会是什么情况。
还有就是如果shader的layout跟pipeline的layout对应位置是匹配的，但是shader的layout多几个或则pipelinelaout多几个会是怎么回事。
经过测试，setlayout多个的情况，包括一个layout里面binding有重复的情况，同一个bingding帮不同的类型.我测试的情况是，layout比shader多是没有问题的，多个setlayout也可以绑定同一个Binding，可能他是自己展开把。但是不能同一个set一个bingding绑定多次，否则会报错duplicated binding number in VkDescriptorSetLayoutBinding.  

经过测试，cpu端创建的layout可以多于shader，甚至可以调用 VkUpdateDescriptorSet 把数据传给GPU 端， 这样都没有错误。但是CPU端不能少layout和匹配错layout。而且如果有对应的uniformbuffer等也需要传过去，不然GPU使用没有这个也会报错。  

还有不懂的，我的layoutsets已经是两个了，而且在第二个的时候明显绑定了uniformbuffer的。确定binding正确，我绑定了5个。但是还是提示：  
Shader uses descriptor slot 0.1 (used as type `ptr to uniform struct of (vec4 of float32)`) but not declared in pipeline layout", 我不知道怎么写代码才能让第二个setlayout有效。