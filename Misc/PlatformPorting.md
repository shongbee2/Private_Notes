# platform proting

记录平台移植中需要替换的东西


alignmnet:

```
#if defined(XXX_PLATFORM_WINDOWS) || defined(XXX_PLATFORM_XB1)
    #define XXX_ALIGN(alignment)     __declspec(align(alignment))
    #define XXX_ALIGNOF(type)        __alignof(type)
#elif defined(XXX_PLATFORM_LINUX) || defined(XXX_PLATFORM_PS4)
    #define XXX_ALIGN(alignment)     __aligned(alignment)
    #define XXX_ALIGNOF(type)        __alignof__(type)
#elif defined(XXX_PLATFORM_NX)
    #define XXX_ALIGN(alignment)     __attribute__((aligned(alignment)))
    #define XXX_ALIGNOF(type)        __alignof(type)
#else
    #error XXX: Platform alignment not defined
#endif
```

