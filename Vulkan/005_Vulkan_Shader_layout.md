# Shader Layout

- [Shader Layout](#shader-layout)
  - [check list](#check-list)
  - [VkDescriptorSetLayout](#vkdescriptorsetlayout)
  - [VkDescriptorSet](#vkdescriptorset)
  - [VkPipelineLayout](#vkpipelinelayout)
  - [PipelineLayout VS Shader Layout VS SetLayout VS sets](#pipelinelayout-vs-shader-layout-vs-setlayout-vs-sets)
  - [Code and Shader](#code-and-shader)
  - [paramter type](#paramter-type)
  - [Vulkan Errors](#vulkan-errors)

## check list

- [ ] VkDescriptorSetLayout


## VkDescriptorSetLayout

shader描述的布局，表示shader的哪个set的哪个位置绑定了什么，例如 第一个set的第一个位置绑定的是constantbuffer,第二个位置绑定是texture等。他只能确定一个set，如果有多个set，需要创建多个 setlayout。set的对应关系就是pipelinelayout的sets数组索引关系。
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
需要注意的：  
binding的内容必须要跟Shader的内容完全一致，否则就会创建pipeline 的时候失败。
bindging的个数和类型，都有硬件数量限制，这些限制可以通过API查询到。绑定的数量不能超出限制。  
VkDescriptorSetLayoutCreateInfo 结构体是创建的内容，注意pNext，flag的使用都需要硬件支持, 通过api可以获取这些信息。  
VkDescriptorSetLayoutBinding 结构体中，只有count，type，binding。 binding对应set里面的起始binding，count是指同样的类型的数组，他会暂用参数个数，但是不能代替binding。
例如：  
```
VkDescriptorSetLayoutBinding bindings[10];
VkDescriptorSetLayoutBinding binding;        
binding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;        
binding.pImmutableSamplers = NULL; 

binding.binding = 2;
binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
binding.descriptorCount = 3;        
bindings[0] = binding;

binding.binding = 3;
binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
binding.descriptorCount = 2;
bindings[1] = binding;

这样也，如果没有binding=3这个，也会提示solot 0.3没有binding东西。所以我认为descriptorCount只是一个数组。并不能指定layout的bingding的值。
如果他的值超过12个，switch也会提示uniformbuffer超出限制。他这些count是绑定在什么地方的呢？怎么去更新他呢？
```

通过API可以获取这些信息，这些API是什么呢？参考[xxx？](还没有写)  
switch 对 VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER 只能支持12个。  
switch就不支持这些flag，例如 VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT。  

## VkDescriptorSet

这个对象是保存set的参数信息，通过绑定他来指定Shader的参数具体使用哪个buffer，哪个texture等。  
通过 vkUpdateDescriptorSets 更新数据。

```cpp
// Create a single pool to contain data for our static sampler descriptor set
VkDescriptorPoolSize poolSize[3];
poolSize[0].type = VK_DESCRIPTOR_TYPE_SAMPLER;
poolSize[0].descriptorCount = 9;
poolSize[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
poolSize[1].descriptorCount = 12;
poolSize[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
poolSize[2].descriptorCount = 5;

VkDescriptorPoolCreateInfo poolInfo = {};
poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
poolInfo.pNext = NULL;
poolInfo.maxSets = 3;
poolInfo.poolSizeCount = 3;
poolInfo.pPoolSizes = &poolSize;
poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

VkDescriptorPool m_DescriptorSetPool;
vkCreateDescriptorPool(m_Device, &poolInfo, NULL, &m_DescriptorSetPool);

VkDescriptorSetLayout m_DescriptorSetLayouts[3];//ensure it create successed.
VkDescriptorSetAllocateInfo allocInfo;
allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
allocInfo.pNext = NULL;
allocInfo.descriptorPool = m_DescriptorSetPool;
allocInfo.descriptorSetCount = 3;
allocInfo.pSetLayouts = m_DescriptorSetLayouts;
// Populate descriptor sets
VkDescriptorSet m_DescriptorSets[3];
vkAllocateDescriptorSets(m_Device, &allocInfo, m_DescriptorSet);
```
需要注意的：  
VkDescriptorSetAllocateInfo 可以一次从pool里面创建多个set，当然pool也要支持这些数据量。  
VkDescriptorPoolCreateInfo 可以指定最多创建多少个set，PoolSize指定了最多支持多少个数据类型和种类。他确定之后，set不能超过这个值。他一次只能创建一个pool。  
VkDescriptorPoolSize 表示这个Pool里面可以存放的数据情况，他是一个数组，每个元素保存了数据的类型和个数，表示这个pool允许创建的最大数量。创建set的时候，set中定义对应类型和数量不能超过poolsize。直接点这个就是pool容器的大小。  

layout确定set的布局情况，确定了绑定的对应关系，set用这个对应关系去提供对应正确的数据。一旦set提供的数据，类型等更layout不一致。就会提示错误。  
set需要调用 vkCmdBindDescriptorSets去绑定让render使用，这个函数需要指定一个pipelinelayout，同时需要指定这些set在pipelinelayout中对应是哪个索引。这里虽然没有明确规定说创建set的setlayout必须和创建pipelineLayout的setlayout一致，也没有明确说明这个pipelinelayout需要和创建GraphicsPipeline的layout需要一致。但是实际上他们就需要完全一致，可以是不同的对象，但是里面的布局必须要完全一致。哪怕冗余数据都需要完全一致，否则就会提示错误。

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

## PipelineLayout VS Shader Layout VS SetLayout VS sets

- [x] Layout 和 Shader 是否必须完全匹配 ？ 对应的set，binding是必须完全匹配，但是Layout可以多于Shader需要的数量。
- [x] 一个SetLayout不能在一个binding绑定不同类型，报错 duplicated binding number in VkDescriptorSetLayoutBinding.  
- [x] 多个setLayout 独自绑定相同的binding不同类型 是可以的。因为他们本来就对应shader中不同的Set。
- [x] 多个setLayout 独自绑定相同的binding不同类型 是怎么工作的？每个set都对应shader的set，所以本来就是独立的。
- [x] Set 更新 Shader 不需要的冗余数据是可以的，对结果没有影响。
- [x] Layout 跟 Shader 匹配， 但是没有更新必要的数据，GPU 使用会报错。
- [x] 错误提示: Shader uses descriptor slot 0.1 中俄slot 0.1是什么意思？是shader的set=0,binding=1的意思。
- [x] 怎么看SPIR-V 格式的 Layout 信息 ？通过Vulkan的工具spriv-dis.exe可以查看
- [x] setLayout Binding 的顺序无关，可以先binding 0,5, 4, 3, 10,6等乱序。同时允许中间有绑定的。
- [x] vkUpdateDescriptorSets 更新set数据需要和set的layout一致。否则报错 : descriptorType must match the type of dstBinding within dstSet.
- [x] layout 数量有限制么？限制是多少？ 有限制，可以通过硬件函数去查。uniform buffer switch是12个
- [x] vkCmdBindDescriptorSets,vkBindPipeline他们分别指定不同的Layout 会发生生么？没有强行指定需要一个对象，但需要完全匹配，否则报错。
- [x] 在绘制的时候，需要绑定pipeline，sets，这里Pipeline创建的时候使用的layout需要和sets需要的layout一致么？没有强行指定需要一个对象，但需要完全匹配，否则报错。
- [x] vkCmdBindDescriptorSets 指定的layout和Pipeline不一致会怎么样？报不兼容的错误。
- [x] vkCmdBindDescriptorSets 是否可以只绑定部分sets？ 可以只绑定Shader需要的sets，冗余set可以不绑定。
- [x] 我怎么让第二个sets工作起来？shader的版本用460就可以了，参考下面的代码。
- [x] 在绑定的这些数据中，并没有指定Vs，Ps等他们的各自参数，他们是统一在使用么？在创建的时候明明有flag指定的。
- [x] 在Shader的不同阶段,vs,ps中，可以在相同的binding中绑定不同的数据么？创建是没有问题，但是更新有问题，更新的时候没有指定stage的参数。所以相当于不能用。
- [x] 怎么去验证Buffer是一个有效的Buffer？抓帧验证？没有办法验证他是否有效，因为他的数据本来就是可以随便放的。
- [ ] VkDescriptorSetLayoutBinding.descriptorCount 属性的理解

在渲染的时候，pipelineLayout是创建Pipeline的一个关键数据，创建可变的binding数据的sets也是通过setlayout来创建的。 sets又是绑定可变数据给渲染管线使用的关键数据。他们之间是怎么合作的呢？  
首先我们知道Pipeline创建之后就不能修改了，能动态修改的就是通过调用 vkUpdateDescriptorSets 来更新texutre,buffer等。同时在渲染之前也会有两个关键的API vkCmdBindPipeline,vkCmdBindDescriptorSets 来确定使用那个pipeline和哪些sets。 这些绑定有一些细节要求，他们表现在:  
  
Shader 决定了我们需要最小布局，需要的最小参数，如果创建的layout和绑定的数据 和 shader需要的数据没有匹配，就会直接报错。  
PipelineLayout,DescriptorSets等都可以作为超级，更新绑定符合自己规则的冗余数据。可以作为多个shader的超集存在。例如：  
```
Shader A 需要的layout是 :
layout(std140, set=0, binding=1) uniform ParameterS {vec4 g_param1;};
layout(std430, set=1, binding=1) buffer ParameterS2 { vec4 g_Color1;};

Shader B 需要的layout是 :
layout(std140, set=0, binding=1) uniform ParameterS {vec4 g_param1;};
layout(std430, set=1, binding=1) buffer ParameterS2 { vec4 g_Color1;};
layout(std430, set=2, binding=1) buffer ParameterS3 { vec4 g_Color1;};

Shader C 需要的layout是 :
layout(std140, set=0, binding=1) uniform ParameterS {vec4 g_param1;};
layout(std430, set=2, binding=1) buffer ParameterS3 { vec4 g_Color1;};

如果我创建一个Layout，同时包含3个Set，并且每个set都提供上面Shader需要的正确数据。那么我这个Layout是可以同时适用于这3个Shader的。尽管这个layout对每个Shader都有冗余数据的存在，但是他不影响实际运行。Vulkan是允许这样做的。
```
setlayout和shader中的set对应关系为，一个setlayout对应shader的一个set，至于对应哪个set就是他绑定在layout的索引值。例如：  
```cpp
shader:
#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout(std140, set=0, binding=1) uniform ParameterS {vec4 g_param1;};
layout(std430, set=1, binding=1) buffer ParameterS2 { vec4 g_Color1;};
这个就表示Shader需要两个Set(0,1)，第0个binding=1是一个uniform，第1个的binding=1是一个storage buffer。那么在创建Layout的时候就需要创建两个setlayout。绑定数据也需要两个set来绑定。
同时要注意version 460， 如果version过低，是不支持这些语法的，就会报错。
```
同是set如果一次数据都没有绑定过，会提示:"Object: 0x208c097f40 (Type = 23) | Descriptor Set 0x208c097f40 bound but it was never updated. You may want to either update it or not bind it."  
如果Shader需要的set没有绑定，会提示: "Object: 0x208c097668 (Type = 6) | VkPipeline 0x208c9e69d8 uses set #1 but that set is not bound."  
如果绑定的set和他对应layout不匹配，会提示错误：Object: 0x208c097e00 (Type = 23) | descriptorSet #0 being bound is not compatible with overlapping descriptorSetLayout at index 0 of pipelineLayout 0x208fedd000 due to: Binding 1 for DescriptorSetLayout 0x208d6996c0 is type 'VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER' but binding 1 for DescriptorSetLayout 0x208d6997e0, which comes from pipelineLayout, is type 'VK_DESCRIPTOR_TYPE_STORAGE_BUFFER'. The spec valid usage text states 'Each element of pDescriptorSets must have been allocated with a VkDescriptorSetLayout that matches (is the same as, or identically defined as) the VkDescriptorSetLayout at set n in layout, where n is the sum of firstSet and the index into pDescriptorSets' (https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#VUID-vkCmdBindDescriptorSets-pDescriptorSets-00358)
注意这个提示是跟Shader的layout有关，其实那个layout参数就是摆设，他必须和Shader创建pipeline的layout一致。如果不一致，就提示这个set没有绑定，一致，如果set里面的数据不一致，也提示不匹配。所以这个layout就必须和创建pipeline的layout完全一致。  

再啰嗦一下，虽然Layout可以作为超集存在，但是在更新数据的时候，也必须用这个超集的Layout去更新，一旦不完全匹配，就会报错，哪怕不匹配是完全不使用的冗余绑定关系。例如：
```
Shader:
layout(std140, set=0, binding=1) uniform ParameterS {vec4 g_param1;};
layout(std430, set=1, binding=1) buffer ParameterS2 { vec4 g_Color1;};

Layout A
绑定了slot0.1, slot1.1, slot1.2 
这里slot1.2是冗余的，
layout B
绑定了 slot0.1,slot1.1
如果我使用了 Layout A 去创建的pipeline， 通过 Layout B 创建的 set 去更新，是会出错的。
同时如果我使用 LayoutB 创建的Pipline，通过Layout A创建的set去更新，哪怕只更新了slot1.1，也是会报错的。
他们需要完全的匹配，哪怕不匹配的地方完全没有使用。
```

## Code and Shader
 
 这个是根据Nintendo的示例VkSimple修改的，打开也需要使用这个工程，然后替换里面的文件和Shader。
 [VkSimple.cpp](./assets/VkSimple.cpp)  [fragment_shader.frag](./assets/fragment_shader.frag)  
 里面有一些冗余代码，不一定能直接运行，可以通过查找关键字去看这些函数是怎么调用的。

## paramter type

VkDescriptorType 

 VK_DESCRIPTOR_TYPE_SAMPLER = 0,
    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
    VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
    VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
    VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
    VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,

ptr to const uniform image          |   VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
ptr to const uniform sampler        |   VK_DESCRIPTOR_TYPE_SAMPLER
ptr to uniform struct               |   VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
ptr to uniform struct of (oddtype)  |   VK_DESCRIPTOR_TYPE_STORAGE_BUFFER

## Vulkan Errors

vkUpdateDescriptorSets() failed write update validation for Descriptor Set 0x212aadf140 with error: DescriptorSet 0x212aadf140 does not have binding 1. The spec valid usage text states 'dstBinding must be less than or equal to the maximum value of binding of all VkDescriptorSetLayoutBinding structures specified when dstSet's descriptor set layout was created' (https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#VUID-VkWriteDescriptorSet-dstBinding-00315)"
这个错误是说定义里面并没有binding 1，但是尝试去更新binding 1的数据。所以报错。  


Shader expects at least 1024 descriptors for binding 0.0 (used as type `ptr to const uniform arr[1024] of image(dim=1, sampled=1)`) but only 1 provided"
就是说bingding 0.1期望的是一个1024的数组，但是我们只绑定了一个。
