## 前言
---
为了更好的开发Elastos5.0系统，使得系统结构更加的合理、简洁，我们将对现有的一些规则作出调整，并增加一些新的规则。现将这些变化说明如下。

## 使用Object为基类
---
当编写一个C++类或者CAR类时，请将Object类作为其基类。你可以直接继承Object，或者间接继承Object。

Object类实现了IObject、ISynchronize和IWeakReferenceSource接口，原则上，Object的子类需实现GetClassID方法，并可以按需重载ToString、GetHashCode和Equals方法。

为当前的类实现好IObject接口中的ToString和GetClassID方法对程序调试非常有用。一般的，ToString方法可以返回当前类的类名，而GetClassID可以返回当前类的ClassID。对于CAR类，Object基类的ToString方法可以通过反射获取具体的类名，因此，如无特定的实现，CAR类可以直接继承使用Object基类的ToString方法。对于C++类，请**一定重载**ToString方法，若无有特定的实现，须在其中返回该类的类名。另外，对于C++类，编译器并不会自动生成其ClassID，所以，请**一定使用UUID生成器**（如visual studio的guid生成器或Ubuntu下的<code>uuidgen</code> 命令）生成一个128位的UUID给该C++类，并在其重载的GetClassID方法中返回。

Object是所有C++类或者CAR类的父类，所以，所有CAR类都隐含实现了IObject、ISynchronize和IWeakReferenceSource接口。但是，在car文件中声明CAR类时，不用显式的声明实现这些接口，它们将由CAR编译器自动的加入到CAR类实现的接口中。

相应的示例可以参考Thread的实现（位于Thread.h和Thread.cpp中）。

## 请使用Object的锁功能，不要使用Mutex
---
当前Elastos类库中的Mutex是对pthread_mutex_t的包装，并且被初始化为重入检查锁。重入检查锁不是递归锁，虽然它支持同一个线程多次调用lock而不会锁死，但是不论调用几次lock，只要调用一次unlock就会解锁。所以，它并不适用于递归调用的情况。而我们不用递归锁的原因是其效率比较底下。

现在，基类Object实现了ISynchronize接口，具有锁的功能，支持递归调用，并且执行效率要比递归锁高。C++类和CAR类继承Object以后，本身都具有了锁的功能，可以像Java代码那样去使用它。

基类 Object 实现了宏 synchronized：
``` cpp
#define synchronized(obj)  for(Elastos::Core::Object::AutoLock obj##_lock(obj); obj##_lock; obj##_lock.SetUnlock())
```
通过该宏，我们可以像 Java 关键字 synchronized 那样使用。

## CAR接口支持方法重载
---
之前，CAR接口方法不支持重载，必须给同名的方法加“Ex”、“Ex2”等后缀。这种处理方式除了对编程不友好以外，还有一个比较大的弊端。在运行Java程序时，在兼容层需要根据CAR的方法转调同名的Java方法。对于加了后缀的CAR方法，我们不太方便得到相应的Java方法（人为的去掉后缀在某些情况下可能行不通）。

现在，CAR编译器已能支持方法重载，因此，我们在声明接口中的方法时不需要再添加任何后缀，完全可以与Java中的方法同名。

相应的示例可以参考ISynchronize接口的声明。

## CAR反射获取方法增加方法签名参数
CAR支持接口方法重载，通过反射获取接口或者类中的方法需要同时指定方法名和方法类型签名。因此，CAR反射API中的GetMethodInfo方法增加了方法签名参数。

CAR方法类型签名为**\"(\"** ***所有参数类型签名*** **\")\"** ***返回类型签名***

CAR数据类型的签名规则如下表:

| Types                          | Signature                  |
|:------------------------------:|:--------------------------:|
| Char8                          | C8                         |
| Char16                         | C16                        |
| Boolean                        | Z                          |
| Byte                           | B                          |
| Int8                           | I8                         |
| Int16                          | I16                        |
| Int32                          | I32                        |
| Int64                          | I64                        |
| UInt16                         | U16                        |
| UInt32                         | U32                        |
| UInt64                         | U64                        |
| Float                          | F                          |
| Double                         | D                          |
| ECode                          | E                          |
| EMuid                          | IID                        |
| EGuid                          | CID                        |
| String                         | LElastos/String;           |
| ArrayOf<Int32\>                | [I32                       |
| ArrayOf<String\>               | [LElastos/String;          |
| Enum Elastos.Core.ThreadState  | LElastos/Core/ThreadState; |
| interface Elastos.Core.IThread | LElastos/Core/IThread;     |

指针类型"**Type \***"的类型签名为"***Type的签名* \***"
指针类型"**Type \*\***"的类型签名为"***Type的签名* \*\***"

举例如下：

- 方法"CARAPI SetValue(String)"的类型签名是"(LElastos/String;)E"
- 方法"CARAPI GetValue(String\*)"的类型签名是"(LElastos/String;\*)E"
- 方法"CARAPI SetValue(Int32, String, Int64)"的类型签名是"(I32LElastos/String;I64)E"
- 方法"CARAPI GetValue(Int32\*, String\*, Int64\*)"的类型签名是"(I32\*LElastos/String;\*I64\*)E"

相应的示例可以参考Libcore/tests/Reflection下的测试程序。

## CAR反射方法GetClassInfo、GetInterfaceInfo以及GetEnumInfo需使用包含命名空间的全名
目前，通过反射中IModuleInfo的GetClassInfo、GetInterfaceInfo、GetEnumInfo方法以及IClassInfo的GetInterfaceInfo方法获取相应的反射接口时，指定名称时须使用包含命名空间的全名。

类、接口以及Enum类型的完整名称即是它们的类型签名。例如，interface Elastos.Core.IThread的包含命名空间的全名是LElastos/Core/IThread，因此，需用该全名作为调用GetInterfaceInfo时的参数。

相应的示例可以参考Libcore/tests/Reflection下的测试程序。

## 取消接口间的继承
---
当前，我们是按照Java中类的继承关系来定义Elastos中接口间的继承。比如，IActivity接口继承IContextThemeWrapper接口。

接口继承的好处是编程方便，通过IActivity接口指针就可以调用IContextThemeWrapper接口中的方法，类似Java代码。

但接口继承也有比较大的弊端，一个弊端是若Java代码中，一个类实现了多个接口，那么Elastos中应该继承哪个接口并没有相应的标准。

另一个比较大的弊端是会导致代码的冗余。比如，实现IActivity接口的类可以继承实现IContextThemeWrapper的类，但即使前者没有重新定义IContextThemeWrapper接口中的任何方法，仍然需要将该接口中的所有方法都包一下，转调基类的相应方法。

因此，我们打算取消接口间的继承关系。不过，这会给编程带来一些小麻烦，不能再像以前那样一个接口支持所有方法，而是要Probe出相应的接口再调用。比如，当IActivity接口不再继承IContextThemeWrapper接口后，我们就不能通过IActivity接口调用IContextThemeWrapper里的方法了，而是要从IActivity接口Probe出IContextThemeWrapper接口再调用。

由于取消了接口间的继承，因而，实现某个接口往往伴随着要实现别的接口。比如，实现IActivity的类应该也实现IContextThemeWrapper接口，而实现IContextThemeWrapper接口也应该实现IContextWrapper接口，而实现IContextWrapper接口也应该实现IContext接口。这些要同时实现的接口，在IActivity接口的声明中都无法体现出来。因此，我们需要在其声明的头部加上一定的注释，注明这些要同时实现的接口。注释的形式如下：
``` cpp
/**
  * @Involve
  * interface IContext;
  * interface IContextWrapper;
  * interface IContextThemeWrapper;
  */

interface IActivity {
   ...
}
```

## 让基类继承接口
---
当前，被继承的C++类不直接实现相应的接口，比如，Thread类不实现IThread接口。这样处理，是因为CThread之类本身会通过_CThread继承IThread接口，所以，在CThread类里必须重新定义IThread接口中的所有方法。

现在，我们修正了CAR编译器生成的镜像代码，简化了CAR类的继承结构，已经可以支持让Thread类继承IThread接口，而CThread直接继承Thread即可。这样做也可以减小代码的冗余，让类的继承结构更加的简单、合理。

相应的示例可以参考Thread和CThread的实现。

## ICloneable 接口的实现
<code>ICloneable</code> 接口的实现需要特殊处理，由于<code>Java</code>代码中子类通常会调用父类的 Clone() 来克隆父类的成员．在<code>Elastos</code>代码中，我们可以使用如下机制来实现：

``` cpp
    // CLocale.h
    public:
    // 覆写 ICloneable 接口
    CARAPI Clone(
        /* [out] */ IInterface** newObj);

    protected:
    // 供自身以及子类调用
    CARAPI CloneImpl(
        /* [in] */ ILocale* locale);

    // CLocale.cpp
    ECode CLocale::Clone(
        /* [out] */ IInterface** newObj)
    {
        VALIDATE_NOT_NULL(newObj);
        // 调用适当的构造函数创建对象（通常是无参数的那一个）
        AutoPtr<ILocale> locale;
        CLocale::New((ILocale**)&locale);

        // 调用自身的 CloneImpl
        CloneImpl(locale);

        // 返回接口指针
        *newObj = locale->Probe(EIID_IInterface);
        REFCOUNT_ADD(*newObj);
        return NOERROR;
    }

    ECode CLocale::CloneImpl(
        /* [in] */ ILocale* locale)
    {
        assert(locale);

        // 若有父类，调用直接父类的 CloneImpl
        // CloneImpl(IParent::Probe(locale);

        CLocale* l = (CLocale*)locale;
        l->mCountryCode = mCountryCode;
        l->mLanguageCode = mLanguageCode;
        l->mVariantCode = mVariantCode;
        l->mScriptCode = mScriptCode;

        l->mUnicodeAttributes = mUnicodeAttributes;
        l->mUnicodeKeywords = mUnicodeKeywords;
        l->mExtensions = mExtensions;

        l->mHasValidatedFields = mHasValidatedFields;

        l->mCachedToStringResult = mCachedToStringResult;
        l->mCachedLanguageTag = mCachedLanguageTag;
        l->mCachedIcuLocaleId = mCachedIcuLocaleId;
        return NOERROR;
    }
```

## 导出
5.0 不再以 lib 形式导出，而是导出符号表。参考示例：<code>Elastos.CoreLibrary.def</code>。导出步骤如下：

### 编译
生成与目标 eco 同名的 def 文件，如 <code>Elastos.CoreLibrary.eco</code> 对应的 def 文件为：<code>Elastos.CoreLibrary.def</code>，并放置在同一目录下，该 def 文件头示例如下：

``` cpp
LIBRARY Elastos.CoreLibrary.eco

EXPORTS
    ;;elastos/core/Object
    _ZN7Elastos4Core6Object10GetClassIDEPNS_6_EGuidE
    _ZN7Elastos4Core6Object11GetHashCodeEPi
    .....
```

编译要导出符号所对应的文件。

### 确定要导出的符号
编译完成后，使用 <code>pd m</code> 进入 mirror 目录，然后进入到目标文件所在 mirror 目录。如 Object 类对应目录为：<code>ElastosRDK5_0/Targets/obj/rdk/arm.gnu.android.dbg/mirror/Elastos/LibCore/src/elastos/core</code>

使用 ls 查看是否生成了相应的目标文件，如示例： Object.o 是否存在。若存在，使用如下命令查看其符号表:

``` shell
arm-linux-androideabi-nm Object.o
```

确定那一些符号需要导出，通常前缀为 T(成员函数)，B，或 R（常量） 的 public 符号都需要导出。

### 写入导出的符号
将需要导出的符号去除前缀后拷贝到 def 文件中。示例如上 <code>Elastos.CoreLibrary.def</code>。


## 其它
---
### 一些宏
* 宏<code>INTERFACE_ADDREF/ARRAYOF_ADDREF</code>改名为<code>REFCOUNT_ADD</code>；宏<code>INTERFACE_RELEASE/ARRAYOF_RELEASE</code>改名为<code>REFCOUNT_RELEASE</code>，定义在elrefbase.h中

* 所有的非单例 CAR 类都需要使用 <code>CAR_OBJECT_DECL/CAR_OBJECT_IMPL</code>
* 所有的单例 CAR 类都需要使用 <code>CAR_SINGLETON_DECL/CAR_SINGLETON_IMPL</code>
* 所有在头文件中继承 CAR 接口的类（ C++ 类或 CAR 类）都需要使用 <code>CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL</code>
* 请参考下面的范例 <code>HelloCar</code> 了解如何使用这些宏。

### 夹壁墙类文件名包含命名空间前缀
后台生成的夹壁墙类文件名包含命名空间前缀，如 CBoolean 类对应的夹壁墙类文件名为：<code>_Elastos_Core_CBoolean.h</code>

## 范例
---
### HelloCar

HelloCar 演示了如何编写带有继承结构的 CAR 类以及如何编写单例类，其代码路径在：
\- <code>ElastosRDK5_0/Sources/Elastos/LibCore/tests/HelloCar</code>：

    + HelloCar
            + eco
                - Animal.cpp
                - Animal.h
                - CAnimalHelper.cpp
                - CAnimalHelper.h
                - CCat.cpp
                - CCat.h
                - CDog.cpp
                - CDog.h
                - Elastos.HelloCar.car
                - IAnimal.car
                - IDlog.car
                - sources
            - dirs
            - main.cpp
            - sources

HelloCar 包括两个模块：生成 Elastos.HelloCar.eco 的模块和使用这个 eco 的模块，请阅读代码与注释了解细节，下面将讲解一下升级5.0做出的一些改变与约定：

* HelloCar/eco/IDog.car
    ``` cpp
    module
    {

        namespace Elastos {
        namespace HelloCar {

            // 基类注释说明 IDog extends IAnimal
            /**
             * @Involve
             * interface IAnimal;
             */
            interface IDog {
                Bark();
            }

        } // HelloCar
        } // Elastos
    }
    ```
* C++基类：HelloCar/eco/Animal.h

    ``` cpp
    #ifndef __HELLOCAR_ANIMAL_H__
    #define __HELLOCAR_ANIMAL_H__

    #include <Elastos.HelloCar_server.h>    // include 模块头文件，文件名格式：模块名称_server.h
    #include <elastos/core/Object.h>        // include Object 基类

    using Elastos::Core::Object;

    namespace Elastos {
    namespace HelloCar {

    // C++ 类 Animal 作为 CCat，CDog 的基类，实现了 IAnimal 接口，并继承自基类 Object
    class Animal
        : public Object
        , public IAnimal
    {
    public:
        CAR_INTERFACE_DECL()    // 实现某个 CAR 接口的类都需要使用 CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL

        Animal();               // 若有成员需要初始化，则需显式声明构造函数，以便使用初始化列表进行初始化

        CARAPI constructor();   // 用于被子类继承的 CAR 构造函数

        CARAPI constructor(     // 用于被子类继承的 CAR 构造函数
            /* [in] */ Int32 age,
            /* [in] */ const String& name);

        // IAnimal 接口函数
        //
        CARAPI SetName(
            /* [in] */ const String& name);

    protected:
        // 成员变量
        Int32 mAge;
        String mName;
    };

    } // HelloCar
    } // Elastos

    #endif //__HELLOCAR_ANIMAL_H__
    ```

* 只实现基类接口的子类：HelloCar/eco/Cat.h

    ``` cpp
    #ifndef __HELLOCAR_CAT_H__
    #define __HELLOCAR_CAT_H__

    #include "Animal.h"                     // include 基类

    namespace Elastos {
    namespace HelloCar {

    class Cat
        : public Animal
    {
    public:
        CARAPI CanFly(                      // 覆写基类方法
            /* [out] */ Boolean* canFly);
    };

    } // HelloCar
    } // Elastos

    #endif //__HELLOCAR_CAT_H__
    ```

* 继承 C++ 类 的Car类：HelloCar/eco/CCat.h

    ``` cpp
    #ifndef __HELLOCAR_CCAT_H__
    #define __HELLOCAR_CCAT_H__

    #include "_Elastos_HelloCar_CCat.h"     // include 编译器生成的夹壁墙头文件，文件名格式：_命名空间_CAR类名称.h
    #include "Cat.h"                     // include 基类

    namespace Elastos {
    namespace HelloCar {

    CarClass(CCat)
        , public Cat
    {
    public:
        CAR_OBJECT_DECL()   // 非单例 CAR 类需要使用宏 CAR_OBJECT_DECL/CAR_OBJECT_IMPL
    };

    } // HelloCar
    } // Elastos

    #endif //__HELLOCAR_CAT_H__
    ```

* 还实现了其它接口的子类：HelloCar/eco/Dog.h

    ``` cpp
    #ifndef __HELLOCAR_DOG_H__
    #define __HELLOCAR_DOG_H__

    #include "Animal.h"                     // include 基类

    namespace Elastos {
    namespace HelloCar {

    class Dog
        : public Animal
        , public IDog           // 实现　CAR 类　IDog
    {
    public:
        CAR_INTERFACE_DECL()    // 实现某个 CAR 接口的类都需要使用 CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL

        CARAPI Bark();
    };

    } // HelloCar
    } // Elastos

    #endif //__HELLOCAR_DOG_H__
    ```

* 继承 C++ 类的Car类：HelloCar/eco/CDog.h

    ``` cpp
    #ifndef __HELLOCAR_CDOG_H__
    #define __HELLOCAR_CDOG_H__

    #include "_Elastos_HelloCar_CDog.h"     // include 编译器生成的夹壁墙头文件，文件名格式：_命名空间_CAR类名称.h
    #include "Dog.h"                     // include 基类

    namespace Elastos {
    namespace HelloCar {

    CarClass(CDog)
        , public Dog
    {
    public:
        CAR_OBJECT_DECL()       // 非单例 CAR 类需要使用宏 CAR_OBJECT_DECL/CAR_OBJECT_IMPL
    };

    } // HelloCar
    } // Elastos

    #endif //__HELLOCAR_CDOG_H__
    ```

* 单例 CAR 类：HelloCar/eco/CAnimalHelper.h

    ``` cpp
    #ifndef __HELLOCAR_CANIMALHELPER_H__
    #define __HELLOCAR_CANIMALHELPER_H__

    #include "_Elastos_HelloCar_CAnimalHelper.h"    // include 编译器生成的夹壁墙头文件，文件名格式：_命名空间_CAR类名称.h
    #include <elastos/core/Singleton.h>             // include 单例基类

    using Elastos::Core::Singleton;

    namespace Elastos {
    namespace HelloCar {

    // CAnimalHelper 是单例 CAR 类，单例 CAR 类需要继承 Singleton，并使用宏 CAR_SINGLETON_DECL/CAR_SINGLETON_IMPL
    CarClass(CAnimalHelper)
        , public Singleton      // 单例 CAR 类需要继承 Singleton
        , public IAnimalHelper  // 实现　CAR 类IAnimalHelper
    {
    public:
        CAR_INTERFACE_DECL()    // 实现某个 CAR 接口的类都需要使用 CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL

        CAR_SINGLETON_DECL()    // 单例 CAR 类需要使用宏 CAR_SINGLETON_DECL/CAR_SINGLETON_IMPL

        CARAPI CanFly(
            /* [in] */ IAnimal* animal,
            /* [out] */ Boolean* canFly);

    };

    } // HelloCar
    } // Elastos

    #endif //__HELLOCAR_CANIMALHELPER_H__
    ```

* sources 文件的变化
    sources 文件中请加入如下 flag:

    ```
    CAR_FLAGS += -n -u
    LUBE_FLAGS += -n -u
    ```

### LibCore 子模块的编译
LibCore 模块将编译成 <code>Elastos.CoreLibrary.eco</code>，并将所有导出符号放到该 eco 中，而不是生成 lib 导出．LibCore 下的子模块都将生成对应的 lib 并合并到该 eco 中．

下面以 core 目录为例讲述如何编译其中的子模块．

1. 加入编译路径：在<code>ElastosRDK5_0/Sources/Elastos/LibCore/inc/elastos/dirs</code>中加入要编译的子目录，如<code>core</code>：

    ```
    DIRS = core
    #DIRS += math
    ```

2. 在子目录中添加<code>sources</code>文件，以设置编译参数与编译文件．如：<code>ElastosRDK5_0/Sources/Elastos/LibCore/src/elastos/core/soruces</code>

    ```
    TARGET_NAME= core
    TARGET_TYPE= lib

    C_DEFINES += -DELASTOS_CORELIBRARY

    include $(MAKEDIR)/../../sources.inc

    INCLUDES += $(PREBUILD_INC)/sys;
    INCLUDES += ../
    INCLUDES += $(MAKEDIR)/../../../inc/elastos;
    INCLUDES += $(MAKEDIR)/../../../inc/elastos/core;

    SOURCES += Thread.cpp
    SOURCES += NativeThread.cpp
    SOURCES += Object.cpp
    SOURCES += Singleton.cpp
    ```

3. 将子模块编译结果合并到模块 eco 中：在<code>ElastosRDK5_0/Sources/Elastos/LibCore/src/soruces</code>中添加如下两行：

    ```
    INCLUDES += $(MAKEDIR)/../../inc/elastos/core
    ...
    LIBRARIES += core/core.lib
    ```

4. 添加子模块 CAR 类声明文件：生成<code>ElastosRDK5_0/Sources/Elastos/LibCore/car/core.car</code>，并将其合并到<code>ElastosRDK5_0/Sources/Elastos/LibCore/src/elastos/Elastos.CoreLibrary.car</code>中：

    ```
    merge("../../car/elastos/core.car");
    ```