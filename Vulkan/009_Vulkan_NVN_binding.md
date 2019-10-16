# VULKAN NVN binding

## The Limits：

| Resource              | Count | Binding API                             | Vulkan NX Count |
| :-------------------- | ----- | :-------------------------------------- | --------------- |
| uniform blocks        | 14    | nvn::CommandBuffer::BindUniformBuffer   | 12              |
| shader storage blocks | 16    | nvn::CommandBuffer::BindStorageBuffer   | 4096            |
| texture handles       | 32    | nvn::CommandBuffer::BindTexture         | 16384           |
| image handles         | 8     | nvn::CommandBuffer::BindImage           | 16384           |
| sampler-only handles  | 32    | nvn::CommandBuffer::BindSeparateSampler | 4000            |
| texture-only handles  | 128   | nvn::CommandBuffer::BindSeparateTexture | 16384           |

## The Binding diferences

| Binding Difference                     | VULKAN       | NVN                                        |
| -------------------------------------- | ------------ | ------------------------------------------ |
| binding Type                           | set, binding | resource type, binding                     |
| skip binding index (like 0,3,7)        | YES          | YES                                        |
| use both separate and combined texture | YES          | NO( we will used separate only)            |
| Compile Optimize bindings              | NO           | YES(used GLSLCoptionFlags::ignoreBindings) |


## Scimitar Resource Type mapping  


| scimitar ResourceType            | Scimitar Descriptor Type | Fusion Descriptor type | Fusion Descriptor Access | NVN descritor Type       | NVN Bind Array Index       | NVN bind Function                   |
| -------------------------------- | ------------------------ | ---------------------- | ------------------------ | ------------------------ | -------------------------- | ----------------------------------- |
| ResourceType_Invalid             | DT_Count                 | Count                  | ReadOnly                 | NvnInvalidType           | NumNvnDescriptorsBoundType | InvalidateType_NOFuncion            |
| ResourceType_Texture2D           | DT_Texture               | Texture                | ReadOnly                 | NvnSeparateTexture       | NvnBindTextures            | nvnCommandBufferBindSeparateTexture |
| ResourceType_Texture3D           | DT_Texture               | Texture                | ReadOnly                 | NvnSeparateTexture       | NvnBindTextures            | nvnCommandBufferBindSeparateTexture |
| ResourceType_TextureCube         | DT_Texture               | Texture                | ReadOnly                 | NvnSeparateTexture       | NvnBindTextures            | nvnCommandBufferBindSeparateTexture |
| ResourceType_Texture1D           | DT_Texture               | Texture                | ReadOnly                 | NvnSeparateTexture       | NvnBindTextures            | nvnCommandBufferBindSeparateTexture |
| ResourceType_TextureArray        | DT_Texture               | Texture                | ReadOnly                 | NvnSeparateTexture       | NvnBindTextures            | nvnCommandBufferBindSeparateTexture |
| ResourceType_TextureCubeArray    | DT_Texture               | Texture                | ReadOnly                 | NvnSeparateTexture       | NvnBindTextures            | nvnCommandBufferBindSeparateTexture |
| ResourceType_TextureStencil      | DT_Texture               | Texture                | ReadOnly                 | NvnSeparateTexture       | NvnBindTextures            | nvnCommandBufferBindSeparateTexture |
| ResourceType_Texture2DMS         | DT_Texture               | Texture                | ReadOnly                 | NvnSeparateTexture       | NvnBindTextures            | nvnCommandBufferBindSeparateTexture |
| ResourceType_RWTexture2D         | DT_TextureRW             | Texture                | ReadWrite                | NvnTextureImage          | NvnBindImage               | nvnCommandBufferBindImage           |
| ResourceType_RWTexture3D         | DT_TextureRW             | Texture                | ReadWrite                | NvnTextureImage          | NvnBindImage               | nvnCommandBufferBindImage           |
| ResourceType_RWTexture1D         | DT_TextureRW             | Texture                | ReadWrite                | NvnTextureImage          | NvnBindImage               | nvnCommandBufferBindImage           |
| ResourceType_RWTextureArray      | DT_TextureRW             | Texture                | ReadWrite                | NvnTextureImage          | NvnBindImage               | nvnCommandBufferBindImage           |
| ResourceType_Buffer              | DT_Buffer                | FormattedBuffer        | ReadOnly                 | NvnSeparateBufferTexture | NvnBindTextures            | nvnCommandBufferBindSeparateTexture |
| ResourceType_StructuredBuffer    | DT_StructuredBuffer      | StructuredBuffer       | ReadOnly                 | NvnStorageBuffer         | NvnBindStorageBuffer       | nvnCommandBufferBindStorageBuffer   |
| ResourceType_ByteAddressBuffer   | DT_ByteBuffer            | Buffer                 | ReadOnly                 | NvnStorageBuffer         | NvnBindStorageBuffer       | nvnCommandBufferBindStorageBuffer   |
| ResourceType_RWBuffer            | DT_BufferRW              | FormattedBuffer        | ReadWrite                | NvnBufferImage           | NvnBindImage               | nvnCommandBufferBindImage           |
| ResourceType_RWStructuredBuffer  | DT_StructuredBufferRW    | StructuredBuffer       | ReadWrite                | NvnStorageBuffer         | NvnBindStorageBuffer       | nvnCommandBufferBindStorageBuffer   |
| ResourceType_RWByteAddressBuffer | DT_ByteBufferRW          | Buffer                 | ReadWrite                | NvnStorageBuffer         | NvnBindStorageBuffer       | nvnCommandBufferBindStorageBuffer   |
| ResourceType_Sampler             | DT_Sampler               | Sampler                | ReadOnly                 | NvnSeparateSampler       | NvnBindSamplers            | nvnCommandBufferBindSeparateSampler |
| ResourceType_ConstantBuffer      | DT_ConstantBuffer        | ConstantBuffer         | ReadOnly                 | NvnUniformBuffer         | NvnBindUniformBuffer       | nvnCommandBufferBindUniformBuffer   |
