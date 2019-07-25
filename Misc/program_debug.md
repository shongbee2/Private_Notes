# debug

记录一些疑难杂症，这个都是超出我的能力范围的。

## judy array 算法出错  

这个算法在实现set的时候，在NX平台上会出错，插入的数据会丢失。最后找到的原因是端序问题。NX是小端，我们使用了大端。
因为这个算法采用数组去实现了一些数据结构，里面使用了内存的mask等。这样在端序不正确会导致问题出现。
例如CPU 寄存器 写入 0x01, 0x00,把他作为两部分写入内存，在读取的时候一起读入，然后再做mask & 0x11这种，如果是小端，则是做的0x00,如果是大端则是0x01。如果不匹配，就会出问题。
Big endian:内存中的高位，在CPU 中是低位，例如内存中的 0x00 0x01 0x02 0x03 放在CPU中就是 00010203
little endian ：内存中的高位，在cpu中是高位 例如内存中 0x00 0x01 0x02 0x03 放在CPU中就是 03020100

[judy array](https://en.wikipedia.org/wiki/Judy_array)
[Endianness](https://en.wikipedia.org/wiki/Endianness)

