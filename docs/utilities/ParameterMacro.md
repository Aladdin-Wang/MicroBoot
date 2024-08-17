# 变参函数和可变参数宏

---

# 一、变参函数的设计与实现
对于一个普通函数，我们在函数实现中，不用关心实参，只需要在函数体内对形参直接引用即可。当函数调用时，传递的实参和形参个数和格式是匹配的。

变参函数，顾名思义，跟 printf 函数一样：参数的个数、类型都不固定。我们在函数体内因为预先不知道传进来的参数类型和个数，所以实现起来会稍微麻烦一点。首先要解析传进来的实参，保存起来，然后才能接着像普通函数一样，对实参进行处理。
## 1.变参函数初体验
我们接下来，就定义一个变参函数，实现的功能很简单，即打印传进来的实参值。

```c
void print_num(int count, ...)
{
    int *args;
    args = &count + 1;
    for( int i = 0; i < count; i++)
    {
        printf("*args: %d\n", *args);
        args++;
    }
}
int main(void)
{
    print_num(5,1,2,3,4,5);
    return 0;
}
```
变参函数的参数存储其实跟 main 函数的参数存储很像，由一个连续的参数列表组成，列表里存放的是每个参数的地址。在上面的函数中，有一个固定的参数 count，这个固定参数的存储地址后面，就是一系列参数的指针。在 print_num 函数中，先获取 count 参数地址，然后使用 &count + 1 就可以获取下一个参数的指针地址，使用指针变量 args 保存这个地址，并依次访问下一个地址，就可以直接打印传进来的各个实参值了。程序运行结果如下：

```c
*args：1
*args：2
*args：3
*args：4
*args：5
```
## 2.变参函数改进版
上面的程序使用一个 int * 的指针变量依次去访问实参列表。我们接下来把程序改进一下，使用 char * 类型的指针来实现这个功能，使之兼容更多的参数类型。

```c
void print_num2(int count,...)
{
    char *args;
    args = (char *)&count + 4;
    for(int i = 0; i < count; i++)
    {
        printf("*args: %d\n", *(int *)args);
        args += 4;
    }
} 
int main(void)
{
    print_num2(5,1,2,3,4,5);
    return 0;
}
```
在这个程序中，我们使用char * 类型的指针。涉及到指针运算，一定要注意每一个参数的地址都是4字节大小，所以我们获取下一个参数的地址是： (char *)&count + 4; 。不同类型的指针加1操作，转换为实际的数值运算是不一样的。对于一个指向 int 类型的指针变量p，p+1表示 p + 1 * sizeof(int) ，对于一个指向 char 类型的指针变量，p + 1 表示 p + 1 * sizeof(char) 。两种不同类型的指针，其运算细节就体现在这里。当然，程序最后的运行结果跟上面的程序是一样的，如下所示：
```c
*args：1
*args：2
*args：3
*args：4
*args：5
```
## 3.变参函数 V3.0 版本
对于变参函数，编译器或计算机系统一般会提供一些宏给程序员使用，用来解析函数的参数。这样程序员就不用自己解析参数了，直接使用封装好的宏即可。编译器提供的宏有：
- va_list：定义在编译器头文件中 typedef char* va_list; 。
- va_start(args,fmt)：根据参数 fmt 的地址，获取 fmt 后面参数的地址，并保存在 args 指针变量中。
- va_end(args)：释放 args 指针，将其赋值为 NULL。
- va_arg(va_list ap, type)：该宏返回下一个额外的参数，是一个类型为 type 的表达式。

有了这些宏，我们的工作就简化了很多。我们就不用撸起袖子，自己解析了。

```c
void print_num3(int count,...)
{
    va_list args;
    va_start(args,count);
    for(int i = 0; i < count; i++)
    {
        printf("*args: %d\n", va_arg(args, int));
    } 
    va_end(args);
}
int main(void)
{
    print_num3(5,1,2,3,4,5);
    return 0;
}
```
## 3.变参函数 V4.0 版本
在 V3.0 版本中，我们使用编译器提供的三个宏，省去了解析参数的麻烦。但打印的时候，我们还必须自己实现。在 V4.0 版本中，我们继续改进，使用 vprintf 函数实现我们的打印功能。vprintf 函数的声明在 stdio.h 头文件中。

```c
CRTIMP int __cdecl __MINGW_NOTHROW \
    vprintf (const char*, __VALIST);
```
vprintf 函数有2个参数，一个是格式字符串指针，一个是变参列表。在下面的程序里，我们可以将，使用 va_start 解析后的变参列表，直接传递给 vprintf 函数，实现打印功能。

```c
void my_printf(char *fmt,...)
{
    va_list args;
    va_start(args,fmt);
    vprintf(fmt,args);
    va_end(args);
} 
int main(void)
{
    int num = 0;
    my_printf("I am litao, I have %d car\n", num);
    return 0;
}
```
运行结果如下：

```c
I am litao, I have 0 car
```
## 4.变参函数 V5.0 版本
上面的 my_printf() 函数，基本上实现了跟 printf() 函数相同的功能：支持变参，支持多种格式的数据打印。接下来，我们还需要对其添加 format 属性声明，让编译器在编译时，像检查 printf 一样，检查 my_printf() 函数的参数格式。
GNU 通过 attribute 扩展的 format 属性，用来指定变参函数的参数格式检查。
它的使用方法如下：

```c
__attribute__(( format (archetype, string-index, first-to-check)))
    void LOG(const char *fmt, ...) __attribute__((format(printf,1,2)));
```
属性 format(printf,1,2) 有三个参数。第一个参数 printf 是告诉编译器，按照 printf 函数的检查标准来检查；第2个参数表示在 LOG 函数所有的参数列表中，格式字符串的位置索引；第3个参数是告诉编译器要检查的参数的起始位置。

V5.0 版本如下：

```c
void __attribute__((format(printf,1,2))) my_printf(char *fmt,...)
{
    va_list args;
    va_start(args,fmt);
    vprintf(fmt,args);
    va_end(args);
} 
int main(void)
{
    int num = 0;
    my_printf("I am litao, I have %d car\n", num);
    return 0;
}
```

# 二、可变参数宏的设计与实现
## 1.什么是可变参数宏
在上面的教程中，我们学会了变参函数的定义和使用，基本套路就是使用 va_list 、 va_start 、 va_end 等宏，去解析那些可变参数列表我们找到这些参数的存储地址后，就可以对这些参数进行处理了：要么自己动手，自己处理；要么继续调用其它函来处理。
```c
void print_num(int count, ...)
{
    va_list args;
    va_start(args,count);
    for(int i = 0; i < count; i++)
    {
        printf("*args: %d\n", va_arg(args, int));
    }
} 
void __attribute__((format(printf,2,3))) LOG(int k,char *fmt,...)
{
    va_list args;
    va_start(args,fmt);
    vprintf(fmt,args);
    va_end(args);
}
```
可变参数宏的定义和使用。其实，C99 标准已经支持了这个特性，但是其它的编译器不太给力，对 C99 标准的支持
不是很好，只有 GNU C 支持这个功能，所以有时候我们也把这个可变参数宏看作是 GNU C 的一个语法扩展。 上面的 LOG 函数，如果我们想使用一个变参宏实现，就可以直接这样定义：

```c
#define LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define DEBUG(...) printf(__VA_ARGS__)
int main(void)
{
    LOG("Hello! I'm %s\n","Wanglitao");
    DEBUG("Hello! I'm %s\n","Wanglitao");
    return 0;
}
```
变参宏的实现形式其实跟变参函数差不多：用 ... 表示变参列表，变参列表由不确定的参数组成，各个参数之间用逗号隔开。可变参数宏使用 C99 标准新增加的一个 __VA_ARGS__ 预定义标识符来表示前面的变参列表，而不是像变参函数一样，使用 va_list 、va_start 、 va_end 这些宏去解析变参列表。预处理器在将宏展开时，会用变参列表替换掉宏定义中的所有 __VA_ARGS__ 标识符。

## 2.宏连接符##的作用
如果这个宏没有##
```c
#define LOG(fmt, ...) printf(fmt, __VA_ARGS__)
```
在这个宏定义中，有一个固定参数，通常为一个格式字符串，后面的变参用来打印各种格式的数据，跟前面的格式字符串相匹配。这种定义方式有一个漏洞，即当变参为空时，宏展开时就会产生一个语法错误。

```c
#define LOG(fmt,...) printf(fmt,__VA_ARGS__)
int main(void)
{
    LOG("hello\n");
    return 0;
}
```
上面这个程序编译时就会通不过，产生一个语法错误。这是因为，我们只给 LOG 宏传递了一个参数，而变参为空。当宏展开后，就变成了下面这个样子。

```c
printf("hello\n", );
```
宏展开后，在第一个字符串参数的后面还有一个逗号，所以就产生了一个语法错误。我们需要对这个宏进行改进，使用宏连接符##，来避免这个语法错误。

宏连接符 ## 的主要作用就是连接两个字符串，我们在宏定义中可以使用 ## 来连接两个字符。预处理器在预处理阶段对宏展开时，会将## 两边的字符合并，并删除 ## 这两个字符。

```c
#define CONNECT2(__A, __B)    __A##__B
int safe_atom_code(void)
{
    uint32_t CONNECT2(wTemp,__LINE__) = __disable_irq(); 
    /* do something here */
    __set_PRIMASK(CONNECT2(wTemp,__LINE__));
    return 0;
}
```
假设__LINE__所在的行号为123，那么这里定义的变量名字就为wTemp123。

知道了宏连接符 ## 的使用方法，我们接下来就可以就对 LOG 宏做一些分析。

```c
#define LOG(fmt,...) printf(fmt, ##__VA_ARGS__)
int main(void)
{
    LOG("hello\n");
    return 0;
}
```
我们在标识符 __ VA_ARGS __ 前面加上宏连接符 ##，这样做的好处是，当变参列表非空时，## 的作用是连接 fmt，和变参列表，各个参数之间用逗号隔开，宏可以正常使用；当变参列表为空时，## 还有一个特殊的用处，它会将固定参数 fmt 后面的逗号删除掉，这样宏也就可以正常使用了。

使用宏连接符 ##要注意一下两条结论：
- 第一条：任何使用到胶水运算“##”对形参进行粘合的参数宏，一定需要额外的再套一层
- 第二条：其余情况下，如果要用到胶水运算，一定要在内部借助参数宏来完成粘合过程

为了理解这一“结论”，我们不妨举一个例子：在前面的代码中，我们定义过一个用于自动关闭中断并在完成指定操作后自动恢复原来状态的safe_atom_code函数，现在我们把它改为宏来表示：
```c

#define SAFE_ATOM_CODE(...)                          \
  {                                                    \
      uint32_t wTemp##__LINE__ = __disable_irq();   \
      __VA_ARGS__;                                   \
      __set_PRIMASK(wTemp##__LINE__);               \
  }
```
如果这里不适用##连接符，连接一个行号会出现什么情况？
由于这里定义了一个变量wTemp，而如果用户插入的代码中也使用了同名的变量，就会产生很多问题：轻则编译错误（重复定义）；重则出现局部变量wTemp强行取代了用户自定义的静态变量的情况，从而直接导致系统运行出现随机性的故障（比如随机性的中断被关闭后不再恢复，或是原本应该被关闭的全局中断处于打开状态等等。

假设这里 SAFE_ATOM_CODE 所在行的行号是 123，那么我们期待的代码展开是这个样子的（我重新缩进过了）：

```c
  {                                                   
      uint32_t wTemp123 = __disable_irq();     
      __VA_ARGS__;                                    
      __set_PRIMASK(wTemp);                           
  }
```
然而，实际展开后的内容是这样的：

```c
  {                                                   
      uint32_t wTemp__LINE__ = __disable_irq();     
      __VA_ARGS__;                                    
      __set_PRIMASK(wTemp);                           
  }
```
这里，__LINE__似乎并没有被正确替换为123，而是以原样的形式与wTemp粘贴到了一起——这就是很多人经常抱怨的 __ LINE __ 宏不稳定的问题。实际上，这是因为上述宏的构建没有遵守前面所列举的两条结论导致的。

从内容上看，SAFE_ATOM_CODE() 要粘合的对象并不是形参，根据结论第二条，需要借助另外一个参数宏来帮忙完成这一过程。为此，我们需要引入一个专门的宏：

```c
#define CONNECT2(__A, __B)    __A##__B
```
注意到，这个参数宏要对形参进行胶水运算，根据结论第一条，需要在宏的外面再套一层，因此，修改代码得到：

```c

#define __CONNECT2(__A, __B)    __A##__B
#define CONNECT2(__A, __B)      __CONNECT2(__A, __B)

#define __CONNECT3(__A, __B, __C)    __A##__B##__C
#define CONNECT3(__A, __B, __C)      __CONNECT3(__A, __B, __C)
```
修改前面的定义得到：

```c
#define SAFE_ATOM_CODE(...)                           \
  {                                                   \
      uint32_t CONNECT2(wTemp,__LINE__) =              \
          __disable_irq();                            \
      __VA_ARGS__;                                    \
      __set_PRIMASK(CONNECT2(wTemp,__LINE__));          \
  }
```

## 3.可变参数宏的另一种写法
当我们定义一个变参宏时，除了使用预定义标识符 __ VA_ARGS __ 表示变参列表外，还可以使用下面这种写法。

```c
#define LOG(fmt,args...) printf(fmt, args)
```
使用预定义标识符 __VA_ARGS__ 来定义一个变参宏，是 C99 标准规定的写法。而上面这种格式是 GNU C 扩展的一个新写法。我们不再使用 __VA_ARGS__ ，而是直接使用 args... 来表示一个变参列表，然后在后面的宏定义中，直接使用 args 代表变参列表就可以了。
跟上面一样，为了避免变参列表为空时的语法错误，我们也需要添加一个连接符##。

```c
#define LOG(fmt,args...) printf(fmt,##args)
int main(void)
{
    LOG("hello\n");
    return 0;
}
```
使用这种方式，你会发现这种写法比使用 __ VA_ARGS __ 看起来更加直观和方便。


# 三、利用变参函数和可变参数宏实现自己的代码模块
## 1.实现函数重载
前边我们定义过CONNECT2， CONNECT3的宏，如果我们要粘连的字符串数量不同，比如，2个、4个、5个……n个，我们就要编写对应的版本：
```c

#define __CONNECT2(__0, __1)            __0##__1
#define __CONNECT3(__0, __1, __2)       __0##__1##__2
#define __CONNECT4(__0, __1, __2, __3)  __0##__1##__2##__3
...
#define __CONNECT8(__0, __1, __2, __3, __4, __5, __6, __7)      \
           __0##__1##__2##__3##__4##__5##__6##__7
#define __CONNECT9(__0, __1, __2, __3, __4, __5, __6, __7, __8) \
           __0##__1##__2##__3##__4##__5##__6##__7##__8
           
//! 安全“套”           
#define CONNECT2(__0, __1)             __CONNECT2(__0, __1)
#define CONNECT3(__0, __1, __2)        __CONNECT3(__0, __1, __2)
#define CONNECT4(__0, __1, __2, __3)   __CONNECT4(__0, __1, __2, __3)
...
#define CONNECT8(__0, __1, __2, __3, __4, __5, __6, __7)        \
    __CONNECT8(__0, __1, __2, __3, __4, __5, __6, __7)
#define CONNECT9(__0, __1, __2, __3, __4, __5, __6, __7, __8)   \
    __CONNECT9(__0, __1, __2, __3, __4, __5, __6, __7, __8)
```
所谓宏的重载是说：我们不必亲自去数要粘贴的字符串的数量而“手工选取正确的版本”，而直接让编译器自己替我们挑选。

比如，我们举一个组装16进制数字的例子：

```c
#define HEX_U8_VALUE(__B1, __B0)                         \
      CONNECT3(0x, __B1, __B0)

#define HEX_U16_VALUE(__B3, __B2, __B1, __B0)            \
      CONNECT5(0x, __B3, __B2, __B1, __B0)
            
#define HEX_U32_VALUE(__B7, __B6, __B4, __B4, __B3, __B2, __B1, __B0)\
      CONNECT9(0x, __B7, __B6, __B4, __B4, __B3, __B2, __B1, __B0)
```
在支持重载的情况下，我们希望这样使用：

```c
#define HEX_U8_VALUE(__B1, __B0)                         \
      CONNECT(0x, __B1, __B0)

#define HEX_U16_VALUE(__B3, __B2, __B1, __B0)            \
      CONNECT(0x, __B3, __B2, __B1, __B0)
            
#define HEX_U32_VALUE(__B7, __B6, __B4, __B4, __B3, __B2, __B1, __B0)\
      CONNECT(0x, __B7, __B6, __B4, __B4, __B3, __B2, __B1, __B0)
```

无论实际给出的参数是多少个，我们都可以使用同一个参数宏CONNECT()，而CONNCT() 会自动计算用户给出参数的个数，从而正确的替换为CONNETn()版本。假设这一切都是可能做到的，那么实际上我们还可以对上述宏定义进行简化：

```c
#define HEX_VALUE(...)          CONNECT(0x, __VA_ARGS__)
```

怎么实现宏的重载呢？为了简化这个问题，我们假设有一个“魔法宏”：它可以告诉我们用户实际传递了多少个参数，我们不妨叫它 VA_NUM_ARGS()：

```c
#define VA_NUM_ARGS(...)         /* 这里暂时先不管怎么实现 */
```
借助它，我们可以这样来编写宏 CONNECT():

```c
#define CONNECT(...)                                \
    CONNECT2(CONNECT, VA_NUM_ARGS(__VA_ARGS__))     /*part1*/\
        (__VA_ARGS__)                               /*part2*/
```
当用户使用CONNECT()时，VA_NUM_ARGS(__VA_ARGS__)会给出参数的数量；"part1" 中 CONNECT2() 的作用就是将 字符串“CONNCET”与这个数组组合起来变成一个新的“参数宏的名字”；而 "part2" 的作用则是给这个组装出来的参数宏传递参数。

假设用户想用 HEX_VALUE() 组装一个数字

uint16_t hwValue = HEX_VALUE(D, E, A, D);   //! 0xDEAD
它会被首先展开为：

```c
uint16_t hwValue = CONNECT(0x, D, E, A, D); 
```
进而：

```c

uint16_t hwValue = 
    CONNECT2(CONNECT, VA_NUM_ARGS(0x, D, E, A, D))
        (0x, D, E, A, D);
```
由于VA_NUM_ARGS() 告诉我们有5个参数，最终实际展开为：

```c

uint16_t hwValue = 
    CONNECT5(0x, D, E, A, D);
```
那么我们就来逆推这个问题：如何实现我们的魔法宏“VA_NUM_ARGS()” 呢？答案如下：

```c

#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,__N,...) __N
#define VA_NUM_ARGS(...)                                                \
            VA_NUM_ARGS_IMPL(__VA_ARGS__,9,8,7,6,5,4,3,2,1)
```
这里，首先构造了一个特殊的参数宏，VA_NUM_ARGS_IMPL()：
- 在涉及"..."之前，它要用用户至少传递10个参数；

- 这个宏的返回值就是第十个参数的内容；

- 多出来的部分会被"..."吸收掉，不会产生任何后果

VA_NUM_ARGS() 的巧妙在于，它把__VA_ARGS__放在了参数列表的最前面，并随后传递了 "9,8,7,6,5,4,3,2,1" 这样的序号：
1. 当__VA_ARGS__里有1个参数时，“1”对应第十个参数__N，所以返回值是1
2. 当__VA_ARGS__里有2个参数时，“2”对应第十个参数__N，所以返回值是2
...
3. 当__VA_ARGS__里有9个参数时，"9"对应第十个参数__N，所以返回值是9

如果觉得上述过程似懂非懂，我们不妨对前面的例子做一个展开：

```c
VA_NUM_ARGS(0x, D, E, A, D)
```

展开为：

```c
VA_NUM_ARGS_IMPL(0x, D, E, A, D,9,8,7,6,5,4,3,2,1)
```
从左往右数，第十个参数，正好是“5”。

宏的重载非常有用，可以极大的简化用户"选择困难"，你甚至可以将VA_NUM_ARGS() 与 函数名结合在一起，从而实现简单的函数重载（即，函数参数不同的时候，可以通过这种方法在编译阶段有预编译器根据用户输入参数的数量自动选择对应的函数），比如：

```c
extern device_write1(const char *pchString);
extern device_write2(uint8_t *pchStream, uint_fast16_t hwLength);
extern device_write3(uint_fast32_t wAddress, uint8_t *pchStream, uint_fast16_t hwLength);

#define device_write(...)                                       \
            CONNECT2(device_write, VA_NUM_ARGS(__VA_ARGS__))    \
                (__VA_ARGS__)
```
使用时：

```c
device_write("hello world");       //!< 发送字符串

extern uint8_t chBuffer[32];
device_write(chBuffer, 32);        //!< 发送缓冲

//! 向指定偏移量写数据
#define LCD_DISP_MEM_START      0x4000xxxx
extern uint16_t hwDisplayBuffer[320*240];
device_write(
    LCD_DISP_MEM_START, 
    (uint8_t *)hwDisplayBuffer, 
    sizeof(hwDisplayBuffer)
);
```
## 2.实现using结构
在C#中有一个类似的语法，叫做 using()，其典型的用法如下：

```c
using (StreamReader tReader = File.OpenText(m_InputTextFilePath))
{
    while (!tReader.EndOfStream)
    {
        ...
    }
}
```
以上述代码为例进行讲解：
- 在 using 圆括号内定义的变量，其生命周期仅覆盖 using 紧随其后的花括号内部；

- 当用于代码离开 using 结构的时候，using 会自动执行一个“扫尾工作”，而这个扫尾工作是对应的类事先定义好的。在上述例子中，所谓的扫尾工作就是关闭 与 类StreamReader的实例tReader 所关联的文件——简单说就是using会自动把文件关闭，而不必用户亲自动手。

要实现类似using的结构，首先要考虑如何构造一个"至执行一次"的for循环结构。要做到这一点，毫无难度：

```c
for (int i = 1; i > 0; i++) {
    ...
}
```
为实现using的效果，定义如下宏：
```c
#define using(__declare, __on_enter_expr, __on_leave_expr)   \
            for (__declare, *_ptr = NULL;                    \
                 _ptr++ == NULL ?                            \
                    ((__on_enter_expr),1) : 0;               \
                 __on_leave_expr                             \
                )
```
为了验证我们的结果，不妨写一个简单的代码：

```c
using(int a = 0,printf("========= On Enter =======\r\n"), 
                printf("========= On Leave =======\r\n")) 
{
    printf("\t In Body a=%d \r\n", ++a);
} 
```
这是对应的执行效果：

```c
========= On Enter =======
In Body a=1
========= On Leave =======
```
我们不妨将上述的宏进行展开，一个可能的结果是：

```c
for (int a = 0, *_ptr = NULL; 
     _ptr++ == NULL ? ((printf("========= On Enter =======\r\n")),1) : 0; 
     printf("========= On Leave =======\r\n") ) 
{
    printf("\t In Body a=%d \r\n", ++a);
}
```
接下来，为了提高宏的鲁棒性，我们可以继续做一些改良，比如给指针一个唯一的名字：

```c

#define using(__declare, __on_enter_expr, __on_leave_expr)                   \
            for (__declare, *CONNECT3(__using_, __LINE__,_ptr) = NULL;          \
                 CONNECT3(__using_, __LINE__,_ptr)++ == NULL ?                  \
                    ((__on_enter_expr),1) : 0;                                  \
                 __on_leave_expr                                                \
                )
```

更进一步，如果用户有不同的需求：比如想定义两个以上的局部变量，或是想省确 __on_enter_expr 或者是 __on_leave_expr ——我们完全可以定义多个不同版本的 using：
```c
#define __using1(__declare)                                                     \
            for (__declare, *CONNECT3(__using_, __LINE__,_ptr) = NULL;          \
                 CONNECT3(__using_, __LINE__,_ptr)++ == NULL;                   \
                )


#define __using2(__declare, __on_leave_expr)                                    \
            for (__declare, *CONNECT3(__using_, __LINE__,_ptr) = NULL;          \
                 CONNECT3(__using_, __LINE__,_ptr)++ == NULL;                   \
                 __on_leave_expr                                                \
                )


#define __using3(__declare, __on_enter_expr, __on_leave_expr)                   \
            for (__declare, *CONNECT3(__using_, __LINE__,_ptr) = NULL;          \
                 CONNECT3(__using_, __LINE__,_ptr)++ == NULL ?                  \
                    ((__on_enter_expr),1) : 0;                                  \
                 __on_leave_expr                                                \
                )

#define __using4(__dcl1, __dcl2, __on_enter_expr, __on_leave_expr)              \
            for (__dcl1, __dcl2, *CONNECT3(__using_, __LINE__,_ptr) = NULL;     \
                 CONNECT3(__using_, __LINE__,_ptr)++ == NULL ?                  \
                    ((__on_enter_expr),1) : 0;                                  \
                 __on_leave_expr                                                \
                )
```
借助宏的重载技术，我们可以根据用户输入的参数数量自动选择正确的版本：

```c
#define using(...)  \
    CONNECT2(__using, VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)
```
至此，我们完成了对 for 的改造，并提出了__using1, __using2, __using3 和 __using4 四个版本变体
## 3.实现一个原子操作宏
我们曾有意无意的提供过一个实现原子操作的封装：即在代码的开始阶段关闭全局中断并记录此前的中断状态；执行用户代码后，恢复关闭中断前的状态。其代码如下：

```c
#define SAFE_ATOM_CODE(...)                               \
{                                                         \
    uint32_t CONNECT2(temp, __LINE__) = __disable_irq();  \
    __VA_ARGS__                                           \
    __set_PRIMASK((CONNECT2(temp, __LINE__)));            \
}
```
唯一的问题是，这样的写法，在调试时完全没法在用户代码处添加断点（编译器会认为宏内所有的内容都写在了同一行），这是大多数人不喜欢使用宏来封装代码结构的最大原因。借助 __using2，我们可以轻松的解决这个问题：
```c
#define SAFE_NAME(__NAME)   CONNECT3(__,__NAME,__LINE__)
#   define safe_atom_code()                                                     \
            using(  uint32_t SAFE_NAME(temp) =                                  \
                        ({  uint32_t SAFE_NAME(temp2)=__get_PRIMASK();          \
                            __disable_irq();                                    \
                            SAFE_NAME(temp2);}),                                \
                        __set_PRIMASK(SAFE_NAME(temp)))
```

## 4.实现foreach结构
很多高级语言都有专门的 foreach 语句，用来实现对数组（或是链表）中的元素进行逐一访问。原生态C语言并没有这种奢侈，即便如此，Linux也定义了一个“野生”的 foreach 来实现类似的功能。为了演示如何使用 using 结构来构造 foreach，我们不妨来看一个例子：

```c
typedef struct example_lv0_t {
    uint32_t    wA;
    uint16_t    hwB;
    uint8_t     chC;
    uint8_t     chID;
} example_lv0_t;

example_lv0_t s_tItem[8] = {
    {.chID = 0},
    {.chID = 1},
    {.chID = 2},
    {.chID = 3},
    {.chID = 4},
    {.chID = 5},
    {.chID = 6},
    {.chID = 7},
};
```
我们希望实现一个函数，能通过 foreach 自动的访问数组 s_tItem 的所有成员，比如：

```c
foreach(example_lv0_t, s_tItem) {
    printf("Processing item with ID = %d\r\n", _.chID);
}
```
使用 "_" 表示当前循环下的元素。在这个例子中，为了使用 foreach，我们需要提供至少两个信息：目标数组元素的类型（example_lv0_t）和目标数组（s_tItem）。

这里的难点在于，如何定义一个局部的指针，并且它的作用范围仅仅只覆盖 foreach 的循环体。 __with1() 的功能就是允许用户定义一个局部变量，并覆盖由第三方所编写的、由 {} 包裹的区域：

```c
#define dimof(__array)          (sizeof(__array)/sizeof(__array[0]))

#define foreach(__type, __array)                                               \
            __using1(__type *_p = __array)                                         \
            for (   uint_fast32_t CONNECT2(count,__LINE__) = dimof(__array);    \
                    CONNECT2(count,__LINE__) > 0;                               \
                    _p++, CONNECT2(count,__LINE__)--                            \
                )
```
为了方便理解，我们不妨将前面的例子代码进行宏展开：

```c
for (example_lv0_t *_p = s_tItem, *__using_177_ptr = NULL; 
     __using_177_ptr++ == NULL ? ((_p = _p),1) : 0;
     ) 
     for ( uint_fast32_t count177 = (sizeof(s_tItem)/sizeof(s_tItem[0])); 
           count177 > 0; 
           _p = _p+1, count177-- ) 
     {
        printf("Processing item with ID = %d\r\n", (*_p).chID);
     }
```
允许用户再指定一个专门的局部变量，用于替代"_" 表示当前循环下的对象：

```c

#define foreach2(__type, __array)                                               \
            using(__type *_p = __array)                                         \
            for (   uint_fast32_t CONNECT2(count,__LINE__) = dimof(__array);    \
                    CONNECT2(count,__LINE__) > 0;                               \
                    _p++, CONNECT2(count,__LINE__)--                            \
                )

#define foreach3(__type, __array, __item)                                       \
            using(__type *_p = __array, *__item = _p, _p = _p, )                \
            for (   uint_fast32_t CONNECT2(count,__LINE__) = dimof(__array);    \
                    CONNECT2(count,__LINE__) > 0;                               \
                    _p++, __item = _p, CONNECT2(count,__LINE__)--               \
                )
```
这里的 foreach3 提供了3个参数，其中最后一个参数就是用来由用户“额外”指定新的指针的；与之相对，老版本的foreach我们称之为 foreach2，因为它只需要两个参数，只能使用"_"作为对象的指代。进一步的，我们可以使用宏的重载来简化用户的使用：

```c
#define foreach(...)        \
    CONNECT2(foreach, VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)
```
经过这样的改造，我们可以用下面的方法来为我们的循环指定一个叫做"ptItem"的指针：

```c
foreach(example_lv0_t, s_tItem, ptItem) {
    printf("Processing item with ID = %d\r\n", ptItem->chID);
}
```
展开后的形式如下：

```c
for (example_lv0_t *_p = s_tItem, ptItem = _p, *__using_177_ptr = NULL; 
     __using_177_ptr++ == NULL ? ((_p = _p),1) : 0;
     )
     for ( uint_fast32_t count177 = (sizeof(s_tItem)/sizeof(s_tItem[0])); 
           count177 > 0; 
           _p = _p+1, ptItem = _p, count177-- ) 
     {
           printf("Processing item with ID = %d\r\n", ptItem->chID);
     }
```