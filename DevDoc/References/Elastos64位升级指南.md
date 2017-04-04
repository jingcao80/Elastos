# Elastos64位升级指南

<span id="content"></span>
## 目录
* [32位与64位简介](#32vs64)
    * [CPU](#cpu)
        * [AMD64](#amd64)
        * [Intel64](#intel64)
        * [ARM64](#arm64)
    * [Linux操作系统](#linux) 
    * [C/C++程序](#c/c++)
* [64位架构优缺点](#64-bitadvantage)
* [Elastos升级](#elastosupdate)
    * [CAR编译器升级](#carcompiler)
    * [CAR Runtime升级](#carruntime)
    * [Core Library升级](#corelibrary)
    * [Framework升级](#framework)
    * [应用升级](#apps)
* [gcc编译参数](#gcc)
* [其它](#others)

<span id="32vs64"></span>
## 32位与64位简介
我们分别从CPU、Linux操作系统以及C/C++程序三个层次介绍32位与64位的概念和差异。以下内容参考[linux内核学习之三：linux中的"32位"与"64位"](http://www.cnblogs.com/xiongyuanxiong/p/3528327.html)。

<span id="cpu"></span>
### CPU
早期的Intel处理器，如386、486、586、奔腾系列的CPU架构，都是32位的(32位的寄存器，32位的虚拟地址空间)，统称为x86架构。随着技术的发展，32位CPU已经无法满足需求，Intel与AMD采用不同的方法开发64位架构的CPU。Intel使用与x86完全不同的IA64架构，由于与原来的x86完全不兼容，因此在PC领域没有得到应用，只在服务器领域有一些使用。而AMD开发的架构称为AMD64(将寄存器扩充为64位，支持64位虚拟地址)，能够完全兼容x86指令集，不用任何修改就能运行老的操作系统和应用软件，在PC界得到广泛的应用，现在市场上几乎所有的PC用途的CPU都是支持AMD64指令集。Intel也很快开发出支持该指令集的CPU，不过Intel将其称为x64架构，而不是用AMD64这个名字，而在Linux中则称为x86_64，即x86架构的64位扩展。

可以通过如下方法在Linux中查看CPU是否是64位：运行<code>grep 'lm' /proc/cpuinfo</code>命令，如果<code>/proc/cpuinfo</code>内的flags字段中含有 __"lm"__，则表示使用的是64位CPU，反之则是32位CPU。

<span id="amd64"></span>
#### AMD64
以下内容参考wiki词条[x86-64](https://en.wikipedia.org/wiki/X86-64)。

AMD64架构的主要特点包括支持64位通用寄存器、64位整数以及逻辑运算，以及64位虚拟地址。详细描述如下：

- 支持64位长的整数
通用寄存器从32位扩展为64位，并且所有的算术和逻辑运算、从内存赋值到寄存器和从寄存器赋值到内存操作等等，都可以直接操作64位整数。可以一次压8个字节到栈中，或者从栈中弹出8个字节，并且指针的长度也是8个字节。

- 增加通用寄存器
除了增加通用寄存器的大小(从32位增加到64位)，通用寄存器的个数也从x86架构的8个(eax、ebx、ecx、edx、ebp、esp、esi、edi)增加到16个(rax、rbx、rcx、rdx、rbp、rsp、rsi、rdi、r8、r9、r10、r11、r12、r13、r14、r15)。因此，可以将更多的局部变量保存在寄存器中而不是栈中，并且让寄存器存储经常被访问的常量；子程序的参数也可以通过寄存器来传递。

- 增加XMM(SSE)寄存器
128位的XMM寄存器(用于SIMD指令)个数从8个增加到16个。

- 更大的虚拟地址空间
AMD64架构定义了64位虚拟地址格式，而目前的实现只用到了低48位。因此，当前AMD64架构的CPU的虚拟地址空间最大可以达到256TB。该架构允许将来扩展到真正的64位寻址，从而虚拟地址空间可以达到16EB。而32位的x86架构的虚拟地址空间只有4GB。

- 更大的物理地址空间
AMD64架构的初始实现支持40位物理地址寻址，因此最多支持1TB的RAM。而当前实现扩展到48位物理地址寻址，因此最多支持256TB的RAM。该架构允许将来可以再次扩展到52位物理地址寻址(受限于页表结构)，因此最多支持4PB的RAM。而32位x86处理器在PAE(Physical Address Extension)模式下最多支持64GB，在非PAE模式下最多支持4GB。

- 32位兼容模式下更大的物理地址空间
在32位兼容模式下，AMD64架构如同大多数x86处理器，支持PAE(Physical Address Extension)模式，但是AMD64将PAE模式的36位寻址扩展到52位寻址。

- 相对指令寄存器数据寻址
指令可以通过相对于指令寄存器(RIP)值的偏移来进行数据寻址。该功能可用于实现地址无关代码(PIC)。

- SSE指令集
(略)

- 不可执行位(No-Execute bit)
不可执行位或者是NX位(页表入口的第63位)允许操作系统用于设置虚拟地址空间中的哪些页包括可执行代码，而哪些页不包括可执行代码，从而是不可执行的。

- 去掉了一些旧特性
AMD64架构在长模式(64位模式，相对的另一种模式是32位兼容模式)下，去掉了(或限制性的保留了)x86架构中很少被现在操作系统使用到的特性。这些特性包括段寻址方式(保留FS和GS寄存器用于其它目的)，任务状态切换机制(the task state switch mechanism)，以及虚拟8086模式。而在32位兼容模式下，上述特性都得以保留，从而允许处理器能够兼容运行32位或者16位的操作系统。有一些指令因为被证实是无用指令，因此64位模式不再支持它们。这些指令包括存储段寄存器到栈中或从栈中恢复段寄存器，一次性存储/恢复所有寄存器(PUSHA/POPA)，十进制算术运算，BOUND指令和INTO指令，长跳转模式("far" jumps)以及带立即数的调用指令。

<span id="intel64"></span>
#### Intel64
以下内容参考wiki词条[x86-64](https://en.wikipedia.org/wiki/X86-64)。

Intel64架构与AMD64架构基本一致，但两者的指令集还是有差异的，一些较少用到的机器指令的语义是不同的。

<span id="arm"></span>
#### ARM64
以下内容参考wiki词条[ARM_architecture](https://en.wikipedia.org/wiki/ARM_architecture)。

官方称64位ARM架构为AArch64或A64，其主要特征如下：

- 新的指令集
    - 包括31个64位通用寄存器。
    - 包括一个零寄存器、一个栈指针寄存器(SP)。
    - 包括一个PC指令指针(PC)，其不再是一个能被直接访问的寄存器。
    - 指令长度任然是32位，并且大部分指令和A32架构的指令相同，但是舍弃了LDM/STM指令(采用配对的loads/stores指令代替)和大多数条件执行指令(保留跳转指令)。
    - 大部分指令可以带上32位或者是64位的参数。
    - 地址是64位。
    - 支持LPAE(Large Physical Address Extension)模式，目前支持最大48位的虚拟地址，将来可扩展到64位。
- 增强SIMD(NEON)指令 
    - 包含32个128位寄存器，支持双精度浮点。
    - 完整支持IEEE754标准。
    - AES加解密指令以及SHA-1/SHA-2哈希指令可使用这些128位的寄存器。
- 新的异常模式
    - 引入异常等级概念，共有从EL0到EL3四个异常等级，等级越高，权限越高。应用运行在EL0，操作系统运行在EL1，EL2和EL3留给虚拟化软件使用。 

<span id="linux"></span>
### Linux
系统同样可分为32位与64位。在x86_64架构的CPU出现之前，PC的操作系统都是32位的，例如Windows 2000，Windows xp，Unix，Linux等。在x86_64架构出现后，系统也就有了32位和64位两个版本，两者的主要区别在于与硬件体系结构相关的操作中。由于x86_64架构完全兼容x86架构，因此，32位系统完全可以在x86_64架构的CPU上运行，但是它只使用了x86部分，没有完全发挥硬件的功能。而64位系统专门针对64位的扩展功能而开发，能够最大限度地发挥硬件的能力，但它无法在x86架构的CPU上运行。另外，不论是Windows还是Linux，驱动程序都需要加载到内核空间中，因此它的类型(32位或64位）需要跟系统完全相同。

可以通过如下方法查看Linux系统是32位还是64位：执行shell命令<code>arch</code>，显示为 __x86_64__ 表示当前系统为64位，显示为 __i386__ , __i486__等表示系统是32位的。

<span id="c/c++"></span>
### C/C++程序
在系统上运行的程序同样有32位和64位之分。老的程序运行在32位CPU及32位系统中，属于32位程序。出现了64位系统之后，也出现了能够充分使用64位系统扩展功能的原生64位程序。对于C/C++而言，64位程序与32位程序的主要区别在于long以及指针类型的长度是64位的，可以使用64位的虚拟地址空间。另外，编绎器在编译64位C/C++程序时，会尽量使用CPU的寄存器，尽量避免访问内存，例如把函数的参数，及局部变量优先放在寄存器，而不是在用户栈中。当然，64位系统大都也支持32程序的运行(Windows和Linux都支持)，64位编绎器也可以选择编译成32位程序，例如，gcc通过指定 __-m32__ 编译参数可编译成32位程序。

在Linux中可以通过如下方法查看可执行文件是32位还是64位的：执行shell命令<code>file /usr/bin/xxx</code>, 如果是32位程序，将显示为 <code>ELF 32-bit ...</code>，如果是64位程序，则显示为 <code>ELF 64-bit...</code>。

分别在32位和64位Linux平台上通过sizeof检查出的C/C++基本数据类型长度如下：

- 32位Linux下结果。
<code>sizeof(char) = 1          sizeof(short) = 2</code>
<code>sizeof(float) = 4         sizeof(double) = 8</code>
<code>sizeof(int) = 4           sizeof(long) = 4</code>
<code>sizeof(long long) = 8     sizeof(void*) = 4</code>

- 64位Linux下结果。
<code>sizeof(char) = 1          sizeof(short) = 2</code>
<code>sizeof(float) = 4         sizeof(double) = 8</code>
<code>sizeof(int) = 4           sizeof(long) = 8</code>
<code>sizeof(long long) = 8     sizeof(void*) = 8</code>

因此，在64位C/C++程序中不要交换使用int类型和long类型，不要使用int类型存储指针，也不要用指针类型存储int类型的值。例如，posix api返回的是xxx_t类型(比如size_t)，这些类型有些是定义成long类型的，在64位系统下则不能用int类型变量接收这些api的返回值。

在printf的格式化输出中，%d用于int类型，%ld用于long类型，%lld用于long long类型。

如果只有long类型和指针类型为64位，那么称为LP64模式。如果long long类型和指针类型为64位，则称为LLP64模式。如果int类型、long类型和指针类型为64位，则称为ILP64模式。如果short类型、int类型、long类型和指针类型为64位，则称为SILP64模式。

Linux系统的编译器采用LP64模式，微软的VC++编译器采用LLP64模式。

<span id="64-bitadvantage"></span>
## 64位架构优缺点

64位架构的主要优点包括：

- 支持更大的地址空间，能够使用更多的内存。
- 支持将大文件(例如4GB以上的文件)全部映射到内存中，以提高文件访问效率。

64位架构的主要缺点包括：

- 由于数据类型大小以及内存对齐要求的差异，相同的数据会占用更多的内存空间，需要更多的RAM，且可能降低CPU缓存(cache)的命中率。

<span id="elastosupdate"></span>
## Elastos升级

Elastos的升级可能会涉及到以下模块的升级：

- CAR编译器
- CAR Runtime
- Core Library及实现的其它第三方库
- Framework
- 应用

Elastos采用C++语言开发，进行升级时需注意以下事项：

1. 基本数据类型的使用(特别是long类型)
除了CAR编译器和CAR Runtime外，Elastos其它模块程序大都采用CAR数据类型(定义在头文件 __elatypes.h__ 中)，而CAR数据类型严格区分32位类型和64位类型，没有使用long类型或用long定义的类型。因此，CAR里参数的类型是定义清晰的，升级时无需调整。

2. int类型与指针类型的相互赋值
Android的Framework会在Java程序中通过Java的long类型保存C++原生对象的指针，在Elastos Framework中使用的是Int64类型。因此，Elastos程序应该也不会出现使用32位int类型或Int32存储指针类型的情况。如果有，需要调整为Int64类型。

3. __volatile__ 关键字
之前我们基本上都略过了 __volatile__ 关键字，趁升级时可以过一遍该关键字的使用，有可能某些场合下是需要保留该关键字以告知编译器不要做相应的读取优化(特别是在64位架构通用寄存器增加的情况下)。

4. xxx_t类型的使用
Elastos的实现会调用posix api，一些api的返回类型是xxx_t类型(例如<code>read()</code>返回ssize_t，<code>lseek()</code>返回off_t)，这些类型有可能是用long类型来定义的。而Elastos代码中存在使用Int32类型或int类型来接收这些api返回值的情况。因此，当编译成64位程序时，64位的long类型值向32位int类型变量赋值时就可能发生值被截断的情况。需要逐个文件过的时候进行修正，修正办法是使用xxx_t类型变量来接收返回值。

5. 汇编程序
Elastos程序中存在少量的汇编程序，升级时需要重点检查这部分程序是否需要修正。

<span id="carcompiler"></span>
### CAR编译器升级

CAR编译器主要使用的是C/C++基本数据类型，较少使用CAR数据类型。升级CAR编译器时可能需主要关注事项4。

<span id="carruntime"></span>
### CAR Runtime升级

- 检查事项1、事项4、事项5。
- 检查<code>CMethodInfo::Invoke()</code>实现中从虚函数表中获取函数指针。
- 检查 __eldelegate.h__中获取函数指针的过程。
- car文件无需修改。

<span id="corelibrary"></span>
### Core Library升级

- car文件无需修改。
- 检查事项2、事项3、事项4。

<span id="framework"></span>
### Framework升级

- car文件无需修改。
- 检查事项2、事项3、事项4。

<span id="apps"></span>
### 应用升级

应用开发使用Elastos api，升级应该没有需要修改的。

<span id="gcc"></span>
## GCC编译参数

请参考以下几个链接：
- [3.17.15 Intel 386 and AMD x86-64 Options](https://gcc.gnu.org/onlinedocs/gcc-4.5.3/gcc/i386-and-x86_002d64-Options.html)
- [How to compile a C++ program as 64-bit on 64-bit machine?](http://stackoverflow.com/questions/11078915/how-to-compile-a-c-program-as-64-bit-on-64-bit-machine)
- [3.18.1 AArch64 Options](https://gcc.gnu.org/onlinedocs/gcc/AArch64-Options.html)
- [Generate 64 bit executable code for android using arm-linux-androideabi- toolchain](http://stackoverflow.com/questions/28251709/generate-64-bit-executable-code-for-android-using-arm-linux-androideabi-toolcha)

<span id="others"></span>
## 其它
(略)