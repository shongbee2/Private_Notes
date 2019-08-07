# Vulkan Barrier

## check list

- [ ] 为什么Image barrier有些状态不能转换
- [ ] 为什么创建的Image需要调用 imageMemBarrier 才能使用。
- [ ] 为什么 Image的有些layout不能通过Barrier转换
- [ ] vkCmdPipelineBarrier 需要 Summbit么？
- [ ] vkCmdPipelineBarrier 在传入多个的Image的时候是怎么处理的？
- [ ] 当调用多次 barrier，对同一个Image是怎么处理的？
- [ ] 同时调用一次 barrier，对同一个Image是怎么处理的？

Object: 0x2144b47868 (Type = 6) | For image 0x212d6e2640 you cannot transition the layout of aspect=1 level=0 layer=0 from VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL when current layout is VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL. The spec valid usage text states 'oldLayout must be VK_IMAGE_LAYOUT_UNDEFINED or the current layout of the image subresources affected by the barrier' (https://www.khronos.org/registry/vulkan/specs/1.0/html/vkspec.html#VUID-VkImageMemoryBarrier-oldLayout-01197)
这个错误表示Image不能从 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL 转换到 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL。