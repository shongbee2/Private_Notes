# Unity头发Shader

## 参考资料

### 我在网上找了一些参考资料  

[Hair Rendering and Shading头发渲染和着色](https://www.cnblogs.com/jaffhan/p/7382106.html)  
[Scheuermann_HairSketchSlides.pdf](http://amd-dev.wpengine.netdna-cdn.com/wordpress/media/2012/10/Scheuermann_HairSketchSlides.pdf)  
[Scheuermann_HairRendering](http://developer.amd.com/wordpress/media/2012/10/Scheuermann_HairRendering.pdf)
[hair-sg03final.pdf](http://www.graphics.stanford.edu/papers/hair/hair-sg03final.pdf)  
[RENERING FUR WITH THREE DIMENSIONAL TEXTURES](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.127.5564&rep=rep1&type=pdf)  
[Light Scattering from Human Hair Fibers](http://59.80.44.45/graphics.stanford.edu/papers/hair/hair-sg03final.pdf)  
[unity3d Hair real time rendering 真实头发实时渲染](https://blog.csdn.net/wolf96/article/details/45823761)
[游戏中各向异性头发的渲染](https://blog.csdn.net/z18636930051/article/details/78436922)
[unity-shader-头发渲染-各向异性](https://blog.csdn.net/yangxuan0261/article/details/89027809)

### 主要的两个论文

* KAJIYA, J. and KAY, T. 1989. Rendering Fur With Three Dimensional Textures. InComputer Graphics (Proceedings of ACM SIGGRAPH 89),23, 3, ACM, 271-280.  
* MARSCHNER, S.R., JENSEN, H.W., CAMMARANO, M., WORLEY, S. and HANRAHAN, P. 2003. Light Scattering from Human Hair Fibers.ACM Transactions on Graphics,22, 3, 780-791.

### Unity相关的资源

[THE BLACKSMITH hair](https://assetstore.unity.com/packages/essentials/the-blacksmith-hair-shader-39945)  
[Genesis Hair Shader](http://standsurestudio.com/genesishair/)
[UnityHairShader](https://github.com/AdamFrisby/UnityHairShader)
[Marschner-Hair-Unity](https://github.com/maajor/Marschner-Hair-Unity)
[CG Shaders - Kajiya-Kay](http://www.artisticexperiments.com/cg-shaders/cg-shaders-kajiya-kay)
[ati-hair-实在找不到了](www.google.com)  

## 理论基础

这个先不写，资料里面很多

## THE BLACKSMITH hair Shader

### 重要参数

```Code
重要的参数：
_KKFlowMap("KK FlowMap", 2D) = "white" {}
_KKReflectionSmoothness("KK Reflective Smoothness", Range(0.0, 1.0)) = 0.5
_KKReflectionGrayScale("KK Reflective Gray Scale", Range(0.0, 48.0)) = 5.0
_KKPrimarySpecularColor("KK Primary Specular Color", Color) = (1.0, 1.0, 1.0)
_KKPrimarySpecularExponent("KK Primary Exponent", Range(1.0, 192.0)) = 64.0
_KKPrimaryRootShift("KK Primary Root Shift", Range(-1.0, 1.0)) = 0.275
_KKSecondarySpecularColor("KK Secondary Specular Color", Color) = (1.0, 1.0, 1.0)
_KKSecondarySpecularExponent("KK Secondary Exponent", Range(1.0, 192.0)) = 48.0
_KKSecondaryRootShift("KK Secondary Root Shift", Range(-1.0, 1.0)) = -0.040
_KKSpecularMixDirectFactors("KK Spec Mix Direct Factors", Vector) = (0.15, 0.10, 0.05, 0)
_KKSpecularMixIndirectFactors("KK Spec Mix Indirect Factors", Vector) = (0.75, 0.60, 0.15, 0)

```

### 渲染流程

首先他是一个透明材质，而且会写入深度和深度检查，以及有cutoff。
他使用3个Pass来渲染，Tags { "LightMode" = "ForwardBase" } 表示主光源（最亮的方向的光照），Tags { "LightMode" = "ForwardAdd" }次要光源的光照， Tags { "LightMode" = "ShadowCaster" } 产生阴影的阶段。

#### 主光源光照阶段

* 标准的SrcBlend，DstBlend的标准混合方式
* 写入深度，并且深度检查
* 开启Cull
* 可以支持透明测试alphaTest,透明混合alphaBlend,自发光EmissionMap,高光Specglossmap,细节图Detail_MULX2,
* 但是不支持视差贴图 Parallaxmap
* 顶点初始函数 vertForwardBase
* 像素处理函数 fragForwardBase
* 没有其他的Computer等



#### 次光源光照阶段
#### 阴影阶段

### fragForwardBase函数

Input格式

```shaderlab
struct VertexInput
{
    float4 vertex   : POSITION;  
    half3 normal    : NORMAL;
    float2 uv0      : TEXCOORD0;
    float2 uv1      : TEXCOORD1;
#if defined(DYNAMICLIGHTMAP_ON) || defined(UNITY_PASS_META)
    float2 uv2      : TEXCOORD2;
#endif
#ifdef _TANGENT_TO_WORLD
    half4 tangent    : TANGENT;
#endif

//begin-volund-hair: Bind vertex color attribute
    float4 color    : COLOR;
//end-volund-hair:
};
```

顶点处理阶段，并没有做一些特殊的事情，里面有EyeVec，tangenttoWorld等操作。

fragForwardBase像素处理阶段

half3 tanFlow = tex2D(_KKFlowMap, i.tex.xy).xyz * 2.f - 1.f; 这个是去FlowMap，并转换到-1，1范围

```code
half3 BRDF_Unity_KK_ish(half3 baseColor, //基础颜色  
                        half3 specColor, //高光颜色
                        half reflectivity, //反射率  暂时没用这个参数
                        half roughness, //粗糙度 没用这个参数
                        half3 normal, //法线
                        half3 normalVertex, // 没用用这个参数
                        half3 viewDir, //观察方向，世界坐标系
                        UnityLight light,// 光信息
                        UnityIndirect indirect, //间接方向
                        half3 specGI, //全局高光
                        half3 tanDir1, //切线方向1
                        half3 tanDir2, //切线方向2
                        half occlusion, //闭塞值
                        half atten) //阴影衰减，是否在阴影里面。
{
half3 halfDir = normalize (light.dir + viewDir); //观察方向和光照方向的半角
half nl = light.ndotl; //法线和光方向的点积，漫反射系数
half nh = BlinnTerm (normal, halfDir);//Blinn 模型的反射系数
half sp = RoughnessToSpecPower (roughness); //粗糙度转换成高光强度

half diffuseTerm = nl;  //漫反射系数
half specularTerm = pow(nh, sp); //blinn模型的高光反射系数

// Poor man's KK. Not physically correct.
half th1 = dot(tanDir1, halfDir); //切线和半角的点积？这个的意思就是他的高光不是跟着法线的，是跟着tandir的。
half th2 = dot(tanDir2, halfDir); //同上

//kk主要反射公式使用的是 sqrt(1.f - th1 * th1) 这个表示如果跟切线点积越大，这个值就越小，对应的指数值也就越小kkSpecTermPrimary就越小
half3 kkSpecTermPrimary = pow(sqrt(1.f - th1 * th1), _KKPrimarySpecularExponent) * _KKPrimarySpecularColor.rgb;
half3 kkSpecTermSecondary = pow(sqrt(1.f - th2 * th2), _KKSecondarySpecularExponent) * _KKSecondarySpecularColor.rgb;
half3 kkSpecTermBlinn = specularTerm * specColor;  //原来的高光颜色

half kkDirectFactor = min(1.f, Luminance(indirect.diffuse) + nl * atten); //漫反射的情况，通过中间亮度+漫反射系数*衰减
_KKSpecularMixDirectFactors *= kkDirectFactor; //混合的情况，这个如果说漫反射越大，这个值就越接近_KKSpecularMixDirectFactors

//就是把三个方向的高光加起来。
half3 kkSpecTermDirect = kkSpecTermPrimary * _KKSpecularMixDirectFactors.x //切线高光1
    + kkSpecTermSecondary * _KKSpecularMixDirectFactors.y //切线高光2
    + kkSpecTermBlinn * _KKSpecularMixDirectFactors.z; //法线高光
kkSpecTermDirect *= light.color; //高光乘以光的颜色。得到最终的高光颜色。

//间接的高光，
half3 kkSpecTermIndirect = kkSpecTermPrimary * _KKSpecularMixIndirectFactors.x
    + kkSpecTermSecondary * _KKSpecularMixIndirectFactors.y
    + kkSpecTermBlinn * _KKSpecularMixIndirectFactors.z;
kkSpecTermIndirect *= specGI; //间接高光乘以GI的高光颜色确定了最终的间接高光值。。

#ifdef DBG_LIGHTING
baseColor = 0.5f;
#endif
#ifdef DBG_SPECULAR
baseColor = 0;
#endif

half3 diffuseColor = baseColor;
half3 color = half3(0.f, 0.f, 0.f);
color += baseColor * (indirect.diffuse + light.color * diffuseTerm);
color += (kkSpecTermIndirect + kkSpecTermDirect) * occlusion; //最后的高光颜色还要乘以闭塞值。如果被遮挡，就没有高光。但是有diffuse  
return color;
}

/**
这个流程就是：使用blinn光照模型得到了直接的漫反射系数。
然后漫反射 = 基础颜色 * （间接漫反射颜色 + 光颜色*漫反射系数）
高光反射 分为六部分 主高光颜色和间接高光颜色
主高光颜色 = 主Blinn模式的高光 + 主Tangent高光 + 次tangent高光
间接高光颜色 = 间接Blinn模式的高光 + 间接Tangent高光 + 间接tangent高光
高光反射 = （主高光颜色+间接高光颜色）* 闭塞

indirect.diffuse 间接漫反射的颜色
tanDir1 主切线方向
**/
```

```shaderlab

half4 fragForwardBase (VertexOutputForwardBase i) : SV_Target
{
    FRAGMENT_SETUP(s) //构建一个commondata s,
    UnityLight mainLight = MainLight (s.normalWorld); //主光源信息
    half atten = SHADOW_ATTENUATION(i); //阴影衰减信息

    half occlusion = Occlusion(i.tex.xy); //通过纹理坐标去得到闭塞值
    UnityGI gi = FragmentGI (
        s.posWorld, occlusion, i.ambientOrLightmapUV, atten, s.oneMinusRoughness, s.normalWorld, s.eyeVec, mainLight);
        //得到GI的结果。通过世界坐标，闭塞，lightmapUV，阴影衰减，粗糙度，法线，viewdir，主光源。

    //begin-volund-hair: Hair shading
    half vtxOcclusion = i.vcolor.b; //不知道？
    half grayMask = i.vcolor.a; //？？
    half3 tanFlow = tex2D(_KKFlowMap, i.tex.xy).xyz * 2.f - 1.f; //采样头发的flowMap？不知道做什么的

    half4 c = ShadeHair(tanFlow, //头发的走势
                        grayMask, //灰色遮罩
                        vtxOcclusion, //？
                        atten,//阴影
                        WorldNormal(i.tangentToWorldAndParallax), //?
                        half3x3(i.tangentToWorldAndParallax[0].xyz, i.tangentToWorldAndParallax[1].xyz, i.tangentToWorldAndParallax[2].xyz), //这是个矩阵啊
                        gi.light, //主光源
                        gi.indirect, //间接光源
                        s); //公共数据

    #ifdef DBG_OCCLUSION
    return half4(vtxOcclusion, vtxOcclusion, vtxOcclusion, s.alpha);
    #endif
    #ifdef DBG_GRAYMASK
    return half4(grayMask, grayMask, grayMask, s.alpha);
    #endif
    #ifdef DBG_FLOW
    return half4(half3(tanFlow.x, tanFlow.y, -1.f) * 0.5 + 0.5, s.alpha);
    #endif
    #ifdef DBG_MASKEDALBEDO
    return half4(s.diffColor.rgb, s.alpha);
    #endif
    #ifdef DBG_SPECULAR
    return half4(c.rgb, s.alpha);
    #endif
    //end-volund-hair:

    c.rgb += Emission(i.tex.xy); //添加自发光

    UNITY_APPLY_FOG(i.fogCoord, c.rgb); //雾效果
    return OutputForward (c, s.alpha); //就是把c.rgb = c.rgb,c.a = s.alpha然后返回c
}
```

ShadeHair 函数

```shaderlab

//begin-volund-hair: Shared helper
half4 ShadeHair(const half3 tanFlow, \\头发的tan方向走向
                const half grayMask, \\灰色的遮罩
                const half vtxOcclusion,\\ 顶点的闭塞
                const half atten, \\阴影值
                const half3 normalWorldVertex, \\world法线
                const half3x3 tangentToWorldMatrix, \\world tangent 矩阵，tangent转换到世界坐标的矩阵
                UnityLight light, \\光源
                UnityIndirect indirect, \\间接光照
                inout FragmentCommonData s) {\\公共数据
    half3 worldTangent = mul(tangentToWorldMatrix, tanFlow);//切线空间转换到世界空间。
    half3 tanDir = normalize(worldTangent + normalWorldVertex.xyz * _KKPrimaryRootShift); //主切线，主要是他的_KKPrimaryRootShift 偏移，
    half3 tanDir2 = normalize(worldTangent + normalWorldVertex.xyz * _KKSecondaryRootShift);

    #ifndef ADDITIVE_PASS
    half3 reflectVector = reflect(s.eyeVec, normalWorldVertex);

    indirect.specular = Unity_GlossyEnvironment(UNITY_PASS_TEXCUBE(unity_SpecCube0), unity_SpecCube0_HDR, reflectVector, _KKReflectionSmoothness);

    UNITY_BRANCH
    if (unity_SpecCube0_BoxMin.w < 0.99999f) {
        indirect.specular = lerp(
            Unity_GlossyEnvironment (UNITY_PASS_TEXCUBE_SAMPLER(unity_SpecCube1,unity_SpecCube0), unity_SpecCube1_HDR, reflectVector, _KKReflectionSmoothness),
            indirect.specular,
            unity_SpecCube0_BoxMin.w
        );
    }
    #endif

    s.diffColor *= grayMask; //diffsue颜色*灰色Mask
    half specOcclusion = (1.f - min(1.f, _KKReflectionGrayScale * Luminance(s.diffColor))) * min(1.f, vtxOcclusion * 2.f);
    //？？为什么这样可以计算出高光的遮罩
    s.diffColor *= vtxOcclusion; //还要乘以这个值？这个是什么值？

    half3 c = BRDF_Unity_KK_ish(s.diffColor, //diffsue 颜色
                                s.specColor, //高光颜色
                                1.f - s.oneMinusReflectivity, //反射率，没用
                                1.f - s.oneMinusRoughness, //没用用
                                s.normalWorld, //法线，世界坐标系
                                normalWorldVertex, //没用用
                                -s.eyeVec, //观察方向
                                light, //光源
                                indirect, //间接光
                                indirect.specular,//间接高光，全局光高光
                                tanDir,//切线1，根据切线图和发现偏移获得
                                tanDir2,//切线2
                                specOcclusion,//高光的遮罩
                                atten);
    return float4(c, 1.f);
}
//end-volund-hair:

```

half vtxOcclusion = i.vcolor.b; //不知道？
normalWorldVertex这个是什么