# vulkan Demo
- [ ] 为什么项目里面的vulkan说硬件不支持
学习他的demo，

vkCmdDrawIndexedIndirectCountKHR
https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/vkCmdDrawIndexedIndirectCountKHR.html

我对这个API又一个简单的认识了。
oid vkCmdDrawIndexedIndirectCountKHR(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride);
他又AMD和khr两个版本，一般nv的卡都是用的KHR的。
他的意思是他会调用几次drawcall，然后参数是在这些buffer中的，
buffer 表示渲染的参数，一个sub drawcall使用这个参数，里面的布局是5个int，表示index cout，instance count， first index，vertex offset，first instance
countBuffer 就是又多少个绘制，但是实际的绘制情况根据是 min(countBuffer,maxDrawCount)决定的。同时，如果buffer参数对应的数据也是0的化，也不会触发绘制的。如果
countBuffer = 0， 那么绘制的个数就是 maxDrawCount

还有不懂的就是他是怎么组织VS,PS数据的呢？
首先，我看到的vertex buffer 只有一个，没有看到instance的数据，难道这些都是instance数据么？
看到的indexbuffer是很大的一个buffer又155998674.
由于这个vertex buffer绑定的input是instance，我们的instance=1，所以看到的数据都是instance的值。（这个instance是不是材质呢？）
然后他的顶点等是怎么算的呢？



