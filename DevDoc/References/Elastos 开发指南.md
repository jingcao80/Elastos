# Elastos 开发指南

<span id="catalog"></span>
## 目录
* [前言](#preface)
* [编码规范](#codingguide)
    * [转译规则](#traslaterules)
* [Car](#car)
    * [Car 函数命名约束](#carfunction)
    * [Car 函数参数命名约束](#carparam)
    * [Car 常量命名约束](#carconstvar)
    * [Car 枚举命名约束](#carenum)
    * [构造函数](#carconstructor)
* [数据类型](#datatype)
    * [ArrayOf](#arrayof)
    * [ETL](#etl)
    * [类型转换](#typecast)
* [智能指针](#smartptr)
    * [AutoPtr](#autoptr)
    * [弱引用](#weakreference)
* [多线程](#thread)
* [编译](#compile)
* [调试](#debug)
    * [定位 crash](#crashlocation)
    * [定位内存泄漏](#memleaklocation)
* [异常](#exception)
* [其他](#others)
* [Q/A](#qa)


<span id="preface"></span>
## 前言
本文档记载有关 CAR 及常用类的使用与约束，以及开发过程中碰到的疑难杂症等等问题。Elastos 实现思路是用 C++/Car 改写 Android Java Framework 层，为了减少不必要的 Bug，在改写的过程中应当尽量保持与 Android 代码的一致。但由于资源或其他方面的考虑，改写过程中总有一些与 Android 代码不一致的地方，也在此一并做一些记录说明。

本文档使用 [Markdown](http://wowubuntu.com/markdown/) 编写而成。

<span id="codingguide"></span>
## 编码规范

<span id="traslaterules"></span>
### 转译规则
1. Java 函数返回 this 的情况  
    若 Java 函数 return this，则对应的 Car 函数不需用通过 out 参数返回 this 指针。由于 Car 对象不能实现函数连调(如:a->Foo()->Bar())，因此这个 out 参数可以省略，以简化调用方代码的编写。例如以下 Java 方法:

    ``` java
    public ViewPropertyAnimator alphaBy(float value) {
        animatePropertyBy(ALPHA, value);
        return this;
    }
    ```

    对应的 CAR 函数为:

    ``` cpp
    AlphaBy(
        [in] Float value);
    ```


<span id="car"></span>
## Car

<span id="carfunction"></span>
### Car 函数命名约束

1. Car 函数不支持重载，因此如果一个 Car 接口的既有函数（包括从父接口继承而来的函数）已有某个特定名称的函数，而又需要继续添加参数个数或类型不同的重载函数，那么：  
第二个重载函数要加后缀 Ex；  
第三个重载函数要加后缀 Ex2；  
第四个重载函数要加后缀 Ex3；  
... 依次类推。

    示例：

    ``` cpp
    // Elastos/FrameworkDroid/Base/Core/car/graphics/Canvas.car
    interface ICanvas {
        ClipRect(
            [in] IRectF* rect,
            [in] RegionOp op,
            [out] Boolean* isNonEmpty);

        ClipRectEx(
            [in] IRect* rect,
            [in] RegionOp op,
            [out] Boolean* isNonEmpty);

        ClipRectEx3(
            [in] IRect* rect,
            [out] Boolean* isNonEmpty);

        ClipRectEx4(
            [in] Float left,
            [in] Float top,
            [in] Float right,
            [in] Float bottom,
            [in] RegionOp op,
            [out] Boolean* isNonEmpty);
    }
    ```
2. 由于所有的 Car 类（形式为：CXXX）都继承自 IObject 接口，而该接口中定义了如下通用方法：

    ``` cpp
    // Elastos/Runtime/Library/inc/car/elaobj.h
    IObject : public IInterface
    {
        virtual CARAPI Equals(
            /* [in] */ IInterface *object,
            /* [out] */ _ELASTOS Boolean *equals) = 0;

        virtual CARAPI GetHashCode(
            /* [out] */ _ELASTOS Int32 *hashCode) = 0;

        virtual CARAPI ToString(
            /* [out] */ _ELASTOS String *info) = 0;
    };
    ```
    并提供了基于地址比较，返回地址作为 HashCode 以及 ToString 返回十六进制显示的地址的默认实现。因此如果你需要自己的 Car 接口中提供这些函数的不同实现，请在自己的 Car 接口中同时提供这三个函数，而且这三个函数都不需要添加后缀。如果需要提供不是比较 IInterface 的便利比较函数，这些便利函数的命名遵循前一条款中的命名约束。

    示例：  
    /Elastos/FrameworkDroid/Base/Core/car/graphics/Point.car

    ``` cpp
    interface IPoint {
        EqualsEx2(
            [in] Int32 x,
            [in] Int32 y,
            [out] Boolean* result);

        EqualsEx(
            [in] IPoint* p,
            [out] Boolean* result);

        Equals(
            [in] IInterface* obj,
            [out] Boolean* isEqual);

        GetHashCode(
            [out] Int32* hash);

        ToString(
            [out] String* info);
    }
    ```

<span id="carparam"></span>
### Car 函数参数命名约束

1. Car 关键字不能用作参数名。比如：__in，out，callback，callee__ 等等。

2. Car 接口中不能出现两个同名，参数个数，参数名称，参数顺序都一样，仅参数类型不一样的 Car 函数。这是为了在[命名重整（name mangling）](http://en.wikipedia.org/wiki/Name_mangling) 时能兼容没有类型概念的动态语言，如：Javascript。这一点尤其体现在 Car 构造函数 __constructor__ 上，因为 Car 构造函数必定是同名的，只可能是 __constructor__，此时，只能通过采用不同的参数名字来区分。

    示例：

    ``` cpp
    // Elastos/LibCore/src/Elastos/Security/Elastos.Security.car
    class CPKIXBuilderParameters {
        constructor();

        constructor(
            [in] ISet* trustAnchors,
            [in] ICertSelector* targetConstraints);

        constructor(
            [in] IKeyStore* keyStore,
            [in] ICertSelector* targetConstraints);

        interface IPKIXBuilderParameters;
    }
    ```

3. Widget 类的 Car 构造函数 __constructor__ 的参数命名有特殊的规则，为了支持从 xml 中解析出来 Widget 类名称创建对应的 Widget（即支持 Reflection），所有实现 IView/IActionProvider 接口的 Car 类的构造函数 __constructor__ 的第一个参数若为 IContext\* 类型则参数名称为 ctx；第二个参数若为 IAttributeSet\* 类型则参数名称为 attrs。

    示例：

    ``` cpp
    // Elastos/FrameworkDroid/Base/Core/car/widget.car
    class CView {
        constructor(
            [in] IContext* ctx);

        constructor(
            [in] IContext* ctx,
            [in] IAttributeSet* attrs);

        constructor(
            [in] IContext* ctx,
            [in] IAttributeSet* attrs,
            [in] Int32 defStyle);

        interface IView;
        interface IDrawableCallback;
        interface IKeyEventCallback;
        interface IAccessibilityEventSource;
    }
    ```

<span id="carconstvar"></span>
### Car 常量命名约束

1. Car 中可以定义 const Int32 和 const String 常量，这些常量名字请大写，若名字中包含多个单词，请用下划线连接起来。

2. 为了减少与 C++ 宏名字或全局变量名称的冲突，请不要使用那些简单过于抽象的名字，如：MAX，ID，IP 等。

<span id="carenum"></span>
### Car 枚举命名约束

1. 枚举成员的名字均为大写，并以该枚举的名字大写为前缀，若名字中包含多个单词，请用下划线连接起来。

2. 为了减少与 C++ 宏名字或全局变量名称的冲突，请不要使用那些简单过于抽象的名字，如：MAX，ID，IP 等。

<span id="carconstructor"></span>
### 构造函数

1. 对于一个 Car 类 CXXX 来说，它有两种构造函数：C++ 构造函数 CXXX() 和 Car 构造函数 constrctor()。这两种构造函数有些类似：

    * 如果没有提供自定义的实现则都会提供一个默认的无参的实现；
    * 都支持构造函数重载；

    但又有很大的不同：

    * C++ 构造函数支持初始化列表，而 Car 构造函数不支持；
    * 在 C++ 构造函数执行时，该对象还未就绪，而 Car 构造函数执行时，对象已经就绪；

    利用以上事实，我们可以利用 C++ 构造函数的初始化列表初始化成员变量。

2. 对于实现了 IParcelble 接口的 Car 类来说，它必须有一个可调用的无参的 Car 构造函数 constrctor（无论是默认提供的，还是显式提供的）。这是为了支持 unmarshall，即 unmarshall 时，先调用这个无参的 Car 构造函数创建对象，然后再转型为 IParcelble，调用 IParcelble::ReadFromParcel() 来设置对象的成员。因此统一规定，凡是实现了 IParcelable 的 Car 类都必须显式提供一个无参的 Car 构造函数。

    示例：

    ``` cpp
    // Elastos/FrameworkDroid/Base/Core/car/accounts.car
    class CAccount {
        constructor();

        constructor(
            [in] String name,
            [in] String type);

        interface IAccount;
        interface IParcelable;
    }

    ```

<span id="datatype"></span>
## 数据类型

<span id="arrayof"></span>
### ArrayOf

1. 简介  
    <code>ArrayOf</code>实现了类似数组的功能，即在一块连续的内存中分配若干个元素。同时 <code>ArrayOf</code>能对 Car 对象指针，继承自<code>ElRefBase/ElLightRefBase</code>类的对象指针以及 String 进行自动应用计数管理（前提是通过<code>Set(Int32 index, T const other)</code>设置元素。

    <code>ArrayOf</code>自身实现了引用计数，因此通常情况下，它都是与<code>AutoPtr</code>结合在一起使用的。

    <code>ArrayOf</code>源码定义在<code>Elastos/Runtime/Library/inc/eltypes/elquintet.h</code>中。

2. 引用计数说明  
    引起<code>ArrayOf</code>引用计数变化的成员方法可以分为三组:

    * 创建操作(<code>__Alloc/Clone__</code>)：  
    创建或克隆一个新对象，并且引用计数为 0（即还没有引入引用计数机制），返回的新对象需要加入<code>AutoPtr</code>自动释放或手动调用<code>Free/Release</code>释放；

    * 减少引用计数操作(<code>__Release/Free__</code>)：  
    <code>Release/Free</code>这两个接口完全等价，出于一致性考虑请使用<code>Release()</code>，或加入<code>AutoPtr</code>由<code>AutoPtr</code>在内部调用<code>Release()</code>自动减少引用计数，并在恰当的时候释放分配的内存；

    * 增加引用计数(<code>__AddRef__</code>)：  
    将一个<code>ArrayOf</code>指针赋值给一个<code>AutoPtr</code>变量就是调用<code>AddRef</code>来增加引用计数的。如果需要手动增加引用计数，通常使用宏<code>INTERFACE_ADDREF(array)</code>来实现。

3. 注意事项
    * 对于能放入其中的元素，请考虑是否需要对元素进行引用计数管理，若需要请让该元素能够支持引用计数，然后调用<code>Set(Int32 index, T const other)</code>设置元素。简单地通过下标索引器<code>[]</code>赋值并不会自动进行引用计数管理。

    * <code>ArrayOf</code>会调用<code>memset</code>将分配到的内存清零，因此<code>ArrayOf</code>只能存放基本数据类型，[POD 对象](http://zh.wikipedia.org/wiki/POD_(%E7%A8%8B%E5%BA%8F%E8%AE%BE%E8%AE%A1))以及指针，而无法存放带有虚函数或用自定义构造函数进行初始化的对象，因为它不会调用构造函数初始化其元素，而是简单地将内存清零。

    * <code>ArrayOf</code>支持分配 0 个元素，即支持<code>ArrayOf<T>::Alloc(0);</code>这种用法，这对减少<code>NULL</code>判断，简化代码大有好处。

    * 对于能够适用于<code>AutoPtr< T ></code>的类型 T，该类型的指针就能直接作为 <code>ArrayOf</code>的元素类型，即能够使用<code>ArrayOf< T *></code>的形式，而不需要使用<code>ArrayOf< AutoPtr< T > ></code>的形式。因此，不应该出现<code>ArrayOf< AutoPtr< T > ></code>形式的代码。

4. 典型使用场景
     * 一般场景

        ``` cpp
        // ThreadGroup.cpp
        AutoPtr<ArrayOf<IThread *> > newThreads;
        newThreads = ArrayOf<IThread *>::Alloc(mNumThreads * 2);
        newThreads->Copy(mChildrenThreads, 0, mNumThreads);
        newThreads->Set(mNumThreads++, thread);
        mChildrenThreads = newThreads;
        ```

     * 作为 Car 函数的<code>out, callee</code>参数  

        ``` cpp
        // IPackageInfo.car
        interface IPackageInfo {
            GetActivities(
                [out, callee] ArrayOf<IActivityInfo*>* activities);
        }

        // CPackageInfo.h
        CarClass(CPackageInfo) {
            CARAPI GetActivities(
                /* [out, callee] */ ArrayOf<IActivityInfo*>** activities);
        }

        // CPackageInfo.cpp
        ECode CPackageInfo::GetActivities(
            /* [out, callee] */ ArrayOf<IActivityInfo*>** activities) {
            VALIDATE_NOT_NULL(activities);
            *activities = mActivities;
            INTERFACE_ADDREF(*activities);
            return NOERROR;
        }

        // 调用方
        AutoPtr<IPackageInfo> info;
        ...
        AutoPtr< ArrayOf<IActivityInfo*> > activities;
        info->GetActivities((ArrayOf<IActivityInfo*> **)&activities);
        ```

    * 作为函数返回值

        ``` cpp
        // CHardwareCamera.h
        AutoPtr<ArrayOf<IInteger32*> > SplitInt(
            /* [in] */ const String& str);
        ```

5. 双重数组  
对于双重数组，请使用<code>AutoPtr\<ArrayOf\<AutoPtr\<ArrayOf\<T\> \> \> \></code>形式。为了简化代码的编写，有两种办法，这两种办法都实现在<code>elquintent.h</code>文件中：

    * 使用模板类。优点:通用性好。缺点:可读性不好。

        ``` cpp
        // 定义：elquintent.h
        template <typename T> class ArrayOf2 {
        public:
            typedef AutoPtr<ArrayOf<T> > ElementType;
            typedef ArrayOf<ElementType> Type;
        }
        ```

    * 使用预定义的<code>typedef</code>。优点:可读性好。缺点:不能通用。

        ``` cpp
        // 定义：elquintent.h
        typedef AutoPtr<ArrayOf<Byte> >     ByteArray;
        typedef AutoPtr<ArrayOf<Char32> >   Char32Array;
        typedef AutoPtr<ArrayOf<String> >   StringArray;
        typedef AutoPtr<ArrayOf<Int32> >    Int32Array;
        typedef AutoPtr<ArrayOf<Int64> >    Int64Array;
        typedef AutoPtr<ArrayOf<Float> >    FloatArray;
        typedef AutoPtr<ArrayOf<Double> >   DoubleArray;

        // 使用：View.h
        static const AutoPtr<ArrayOf<Int32Array> > VIEW_STATE_SETS;
        ```

<span id="etl"></span>
### ETL
1. 简介  
    ETL 是基于 SGI 实现的一套 C++ 模版库，它的使用方法与 STL 相似。

    在使用 ETL 之前需要预设一些宏，这些宏在<code>#include <cmdef.h>(Libcore 下)</code> 和<code>#include \<ext/frameworkdef.h> (Framework 下)</code>中有定义，所以在<code>include</code>任何 ETL 容器头文件之前，务必先<code>include</code>这两个头文件之一。

2. 引用计数说明  
    ETL 中的容器自身都添加了引用计数功能，因此可以使用 AutoPtr 包装它们，如：AutoPtr< List<Int32> >。但 ETL 容器并不会修改放置在它里面的元素的引用计数，这一点与 STL 的行为是一致的。

3. 遍历过程中如何修改迭代器  
    要特别留意在使用<code>Iterator/ReverseIterator</code>遍历的过程中修改迭代器的情况，常见的情形就是遍历过程中调用<code>Erase</code>修改迭代器使得当前迭代器失效（请参考[Effective STL](http://book.douban.com/subject/1243751/)）。下面将介绍各种容器分别在正向和反向遍历过程中<code>Erase</code>元素的正确使用方法。

    * 对于正向迭代器 Iterator 来说，List 和 HashMap 各有不同。

        List 的 Erase 返回下一个迭代器，所以使用<code>it = Erase(it)</code>即可；而 HashMap 的<code>Erase</code>返回<code>void</code>，所以需要生成当前迭代器的副本，然后递增当前迭代器，再<code>Erase</code>副本，这个顺序一定不能错。又因为 HashMap 重载的后缀++操作符是先创建副本，然后递增迭代器，然后返回副本，所以可以使用<code>map.Erase(mit++)</code>简化代码。  

        List 使用示例:

        ``` cpp
        List<Int32>::Iterator it;
        for (it = list.Begin(); it != list.End();) {
            if (0 == (*it)%2) {
                it = list.Erase(it);
            }
            else {
                ++it;
            }
        }
        ```

        HashMap 使用示例:  

        ``` cpp
        HashMap<Int32, Int32>::Iterator mit;
        for (mit = map.Begin(); mit != map.End();) {
            if (0 == mit->mFirst % 2) {
                map.Erase(mit++);
            }
            else {
                ++mit;
            }
        }
        ```

    * 对于反向迭代器，HashMap 没有反向迭代器，就只有 List 一种情形。由于反向迭代器是由后往前和正向迭代器相差一个元素，而<code>Erase</code>只接收正向迭代器，因此需要将反向迭代器转换为正向迭代器，情况更加复杂。首先要获得合适的正向迭代器，所以需要<code>--(rit.GetBase())</code>获得需要删除元素的正向迭代器，然后调用 Erase 删除该元素。然后将<code>Erase</code>返回的正向迭代器。

        List 使用示例:  

        ``` cpp
        List<Int32>::ReverseIterator rit;
        for (rit = list.RBegin(); rit != list.REnd();) {
            if(0 == (*rit) % 2) {
                rit = List<Int32>::ReverseIterator(list.Erase(--(rit.GetBase())));
            }
            else {
                ++rit;
            }
        }
        ```

    * 对 Erase 的用法 HashSet 与 HashMap 相同，Vector 用 Iterator 进行<code>Erase</code>与 List 相同，但 ReverseIterator 与 List 略有不同，因为 Vector 删除一个元素之后会进行内存拷贝，所以所删元素之后的迭代器均会失效，但 Vector 与 List 用 ReverseIterator 进行<code>Erase</code>可以统一到一个形式中去，所以统一下用法如下:

        * 使用 List/Vector 的正向迭代器 Iterator 遍历删除。  

            ``` cpp
            List<Int32>::Iterator lit;
            ...
            lit = list.Erase(lit);

            Vector<Int32>::Iterator vit;
            ...
            vit = vector.Erase(vit);
            ```

        * 使用 List/Vector 的反向迭代器 ReverseIterator 遍历删除。  

            ``` cpp
            List<Int32>::ReverseIterator lrit;
            ...
            lrit = List<Int32>::ReverseIterator(list.Erase(--(lrit.GetBase())));

            Vector<Int32>::ReverseIterator vrit;
            ...
            vrit = Vector<Int32>::ReverseIterator(vector.Erase(--(vrit.GetBase())));
            ```

       * 使用 HashMap/HashSet 的正向迭代器 Iterator 遍历删除。  

            ``` cpp
            HashMap<Int32, Int32>::Iterator mit;
            ...
            map.Erase(mit++);

            HashSet<Int32>::Iterator sit;
            ...
            set.Erase(sit++);
            ```

4. Car 对象作为 HashMap/HashSet 的 key  
    用 Car 对象作为 HashMap/HashSet 的 key 需要考虑是使用对象地址还是使用对象的 GetHashCode 返回的 hash 值作为 key 值。这需要特化<code>Hash/EqualTo</code>这两个类模版已满足 ETL 用作 key 的约束。在<code>frameworkhash.h</code>中针对 Car 接口提供了一些便利的宏，并且通常也在其中定义这些特化模版。

5. 错误用法  
    下面是用正向迭代器进行反向遍历的错误代码，请留意:

    ``` cpp
    List<Int32>::Iterator it = list->End() - 1;
    for (; it != list->Begin(); it--) {
        if (...) {
            list->Erase(it);
        }
    }
    ```

    这是一种错误的用法，请使用反向迭代器 ReverseIterator 进行遍历。

6. 其他容器  

    * 由于 ETL 不能作为参数用在 Car 函数中，所以需要有 Car 接口容器类，当前代码中使用到<code>IArray，IObjectContainer，IObjectInt32Map，IObjectStringMap</code>等几个主要的 Car 接口容器类。

        IObjectContainer 用法示例：

        ``` cpp
        AutoPtr<IProviderInfo> provider;
        ...

        AutoPtr<IObjectContainer> providers;
        CObjectContainer::New((IObjectContainer**)&providers);
        providers->Add(provider);
        ...

        Boolean hasNext = FALSE;
        AutoPtr<IObjectEnumerator> enumerator;
        providers->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        while(enumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            enumerator->Current((IInterface**)& obj);
            IProviderInfo * info = IProviderInfo::Probe(obj);
            ...
        }
        ```

        __注意：__  
        在上面的<code>while</code>循环中，<code>AutoPtr<IInterface> obj;</code>是声明在<code>while</code>循环体内的，而不是放在<code>while</code>循环体外。要小心 Car 指针变量作为 out 参数在循环中的多次使用，这很容易导致内存泄漏，在循环使用之前需要显式地调用<code>obj = NULL;</code>以清除上一次存储的内容。

    * <code>IList，IMap，ICollection，ISet</code>等 Car 容器类是与 Android 中的容器类一一对应的，将来会用这些容器类替代上面提到的<code>IObjectContainer，IObjectInt32Map，IObjectStringMap</code>等容器类。

<span id="typecast"></span>
### 类型转换

<span id="smartptr"></span>
## 智能指针

1. __AutoPtr__  <span id="autoptr"></span>  
    Elastos 中的<code>AutoPtr</code>类似于 STL 中的 shared_ptr 或 Android 中的 sp。

    * 使用<code>AutoPtr</code>作为返回值

        通常不推荐返回 C++ 裸指针，因为这需要额外多考察去判断由谁来释放这个 C++ 指针。一般都使用<code>AutoPtr</code>来包装要返回的指针。

        示例：

        ```
        // CSoftKeyboardView.h
        AutoPtr<ISoftKeyboard> GetSoftKeyboard();
        ```

        在调用返回<code>AutoPtr</code>的函数时，为避免返回指针所指的对象可能被提前释放。统一规定使用<code>AutoPtr</code>来接收返回值。

        推荐用法：

        ```
        // CSoftKeyboardView.cpp
        ECode CSoftKeyboardView::GetSoftKeyboard(
            /* [out] */ ISoftKeyboard** keyboard)
        {
            VALIDATE_NOT_NULL(keyboard);
            AutoPtr<ISoftKeyboard> temp = SoftKeyboardView::GetSoftKeyboard();
            *keyboard = temp;
            INTERFACE_ADDREF(*keyboard);
            return NOERROR;
        }
        ```

        不推荐：

        ```
        ECode CSoftKeyboardView::GetSoftKeyboard(
            /* [out] */ ISoftKeyboard** keyboard)
        {
            VALIDATE_NOT_NULL(keyboard);
            *keyboard = SoftKeyboardView::GetSoftKeyboard();
            // return ptr may be released already!
            INTERFACE_ADDREF(*keyboard);
            return NOERROR;
        }
        ```

    * 在循环中使用<code>AutoPtr</code>作为 out 参数  
    要小心 Car 指针变量作为 out 参数在循环中的多次使用，这很容易导致内存泄漏，在循环使用之前需要显式地调用<code>obj = NULL;</code> 以清除上一次存储的内容。或将变量的生命周期限定在循环体内。

        正确用法：

        ```
        while (...) {
            AutoPtr<IInterface> obj;
            GetObject((IInterface**)&obj);
        ...
            }

        // or
        AutoPtr<IInterface> obj;
        while (...) {
            obj = NULL;
            GetObject((IInterface**)&obj);
            ...
        }
        ```

        错误用法：

        ```
        // or
        AutoPtr<IInterface> obj;
        while (...) {
            GetObject((IInterface**)&obj); // memory leak!
            ...
        }
        ```

2. __弱引用__  <span id="weakreference"></span>  
    为了避免强引用导致的循环引用，Car 中引入了弱引用（IWeakReference/IWeakReferenceSource）的概念。Car 中的弱引用是基于 ElRefBase 实现的，详细代码情参考<code>elrefbase.h/elrefbase.cpp</code>。

    * Car 类实现弱引用  
        若 Car 类需要实现该功能，请在 sources 编译脚本中添加如下 flag 以开启 Car 编译器自动生成弱引用代码功能。

        ```
        CAR_FLAGS += -u
        LUBE_FLAGS += -n -u
        ```

        <code>Elastos.Droid.Core/Elastos.Droid.Server</code>已经开启了该功能。

    * C++ 类实现弱引用
        若某个 C++ 类需要提供弱引用功能，那么它需要继承 IWeakReferenceSource 接口，并实现其中的方法：

        ```
        CARAPI GetWeakReference(
            /* [out] */ IWeakReference** weakReference);
        ```

        示例：

        ```
        // app/LoadedPkg.h
        class LoadedPkg
            : public ElRefBase
            , public IWeakReferenceSource
        {
        public:
            CARAPI GetWeakReference(
            /* [out] */ IWeakReference** weakReference);
        };

        // app/LoadedPkg.cpp
        ECode LoadedPkg::GetWeakReference(
            /* [out] */ IWeakReference** weakReference)
        {
            VALIDATE_NOT_NULL(weakReference)
            *weakReference = new WeakReferenceImpl(this, CreateWeak(this));
            INTERFACE_ADDREF(*weakReference)
            return NOERROR;
        }

        // 使用
        AutoPtr<IWeakReference> mWeakRef;
        ...
        // initialize mWeakRef
        loadedPkg->GetWeakReference((IWeakReference**)&mWeakRef);
        ...

        // use mWeakRef
        AutoPtr<IInterface> strongObj;
        mWeakRef->Resolve(EIID_IInterface, (IInterface**)&strongObj);
        if (strongObj != NULL) {
            // strongObj is still alive!
            LoadedPkg* loadedPkg = (LoadedPkg*)strongObj.Get();
            ...
        }
        ```

<span id="thread"></span>
## 多线程
1. 继承 IThread，IRunnable，IHandler  
    IThread，IRunnable，IHandler 都有标准的父类，应该优先通过继承这些父类来实现子类的功能。

    * IThread 的父类为 <code>Elastos::Core::Threading::ThreadBase</code>
    * IRunnable 的父类为 <code>Elastos::Droid::Os::Runnable</code>
    * IHandler  的父类为 <code>Elastos::Droid::Os::HandlerBase</code>
    * 此外 <code>Elastos::Droid::Os::HandlerRunableBase</code> 同时实现了 IHandler 与 IRunnable 接口。

2. 锁  
    有两种方式来使用锁：<code>Elastos::Core::Threading::Metux</code>与<code>Elastos::Core::Threading::Object</code>，两者均提供了 Autolock 的便利设施。Object 还具备 Wait/Notify 功能。

3. 设置 IMessage 的 obj 参数  
    如果需要将自定义数据类型设置为 IMessage 的 obj 参数，那么该参数必须继承自 IInterface，通常我们会通过如下模式的代码来使用：

    ``` cpp
    // 参数类型
    class SomeArgs
        : public ElRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()
        ...
    };

    // 发送消息
    AutoPtr<SomeArgs> args = SomeArgs::Obtain();

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessageEx(MSG_RESIZED_REPORT, args, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);

    // 处理消息
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    ...
    case ViewRootImpl::MSG_RESIZED_REPORT: {
        AutoPtr<SomeArgs> args = (SomeArgs*)obj.Get();
        ...
        args->Recycle();
    }
    ```

    <code>Elastos::Droid::Os::SomeArgs</code>是能够存储多个 IInterface/Int32 类型的参数 wrapper 类，并提供了参数池机制，该类已经导出，可以在代码中自由使用。

    一些 Wigdet 接口提供了<code>SetTag/GetTag</code>函数，这个 tag 参数的设置也请参照上面 IMessage 的 obj 参数设置示例。

5. syn(this) 的翻译


<span id="compile"></span>
## 编译


<span id="debug"></span>
## 调试

<span id="crashlocation"></span>
### 定位 crash
Elastos 的 eco 支持根据 logcat 中程序 crash 的地址定位到源码中具体的位置。

主要步骤如下:

1. 在需要定位的 eco 的编译目录中的 sources 文件里加入<code>EXPORT_ALL_SYMBOLS = 1</code> (位置最好放到<code>TARGET_TYPE = eco</code>声明后面)。

2. 随便修改目录下的某个.cpp文件，或者 touch 一下某个.cpp文件(目的是使得接下来 emake 操作时能重新链接)，然后调用 emake。

3. 执行<code>pd @</code>，然后执行<code>elcopy xxx.eco /system/lib/</code>将 eco 拷贝到相应目录下(大多是<code>/system/lib</code>)，然后执行程序。

4. 当程序 crash 时，logcat 会 dump 出 crash 进程的信息。其中<code>backtrace:</code>栏目下会打出 crash 时的调用栈如下：

    ``` html
    backtrace:
    I/DEBUG   ( 1218):     #01  pc 013624a4  /system/lib/Elastos.Droid.Core.eco
    I/DEBUG   ( 1218):     #02  pc 0134e328  /system/lib/Elastos.Droid.Core.eco
    I/DEBUG   ( 1218):     #03  pc 0134e49c  /system/lib/Elastos.Droid.Core.eco
    ...
    ```
    其中，013624a4 是出错地址，0134e328 和 0134e49c 是调用者的函数栈展开。

5. 取得 crash 地址后，再<code>pd @</code>后的 target 目录下,然后<code>cd debug\_info</code>到下面的<code>dbg\_info</code>子目录下，然后执行：

    ```
    arm-linux-androideabi-addr2line –e XXX.eco crash-address
    ```
    就可以定位到源码中出错的具体位置。

    比如，针对例子中的地址，执行：

    ```
    arm-linux-androideabi-addr2line -e Elastos.Droid.Core.eco 013624a4
    ```
    会得到以下结果:

    ```
    /home/kesalin/ElastosRDK4_2_2/Targets/obj/rdk/arm.gnu.android.dbg/inc/elautoptr.h:112
    ```

方便调试的便利脚本：（感谢饶雪提供）

* 为了方便将多个地址信息批量转换为源码中的位置信息，饶雪写了一个便利脚本，内容如下：

    ``` shell
    #!/bin/bash
    cut $1 -d ' ' -f 13-15 > tempx8x9

    while read line
    do
        echo $line

    done < tempx8x9

    while read line
    do
        #echo $line
        NUM=`echo $line | cut -d ' ' -f 1`
        #echo $NUM
        F=`echo $line | cut -d ' ' -f 2 | cut -d '/' -f 4`
        #echo $F
        arm-linux-androideabi-addr2line -e ~/ElastosRDK4_2_2/Targets/rdk/arm.gnu.android.dbg/bin/dbg_info/$F $NUM

    done < tempx8x9
    rm tempx8x9
    ```

    注意：这个脚本只针对 dbg 版本，如果需要定位 rls 版本则需要修改脚本中的路径。

* 该脚本已经放入<code>ElastosRDK4_2_2/DevDoc/DbgTools/addr2line\_dbg.sh</code>中。使用时，进入到 <code>ElastosRDK4_2_2/DevDoc/DbgTools</code>目录下，如果将 crash 信息拷贝到 crashinfo 文档内，其格式如下：

    ```
    I/DEBUG   ( 1218):     #01  pc 013624a4  /system/lib/Elastos.Droid.Core.eco
    I/DEBUG   ( 1218):     #02  pc 0134e328  /system/lib/Elastos.Droid.Core.eco
    I/DEBUG   ( 1218):     #03  pc 0134e49c  /system/lib/Elastos.Droid.Core.eco
    ```

    然后执行如下命令<code>addr2line_dbg.sh crashinfo</code>，就能得到

    ```
    ~/ElastosRDK4_2_2/DevDoc/DbgTools$./addr2line_dbg.sh crashinfo
    013624a4 /system/lib/Elastos.Droid.Core.eco
    0134e328 /system/lib/Elastos.Droid.Core.eco
    0134e49c /system/lib/Elastos.Droid.Core.eco

    /home/kesalin/ElastosRDK4_2_2/Targets/obj/rdk/arm.gnu.android.dbg/inc/elautoptr.h:112
    /home/kesalin/ElastosRDK4_2_2/Sources/Elastos/Frameworks/Droid/Base/Core/src/view/CInputDevice.cpp:365
    /home/kesalin/ElastosRDK4_2_2/Sources/Elastos/Frameworks/Droid/Base/Core/src/view/CInputDevice.cpp:386
    ```

<span id="memleaklocation"></span>
### 定位内存泄漏
1. 将如下代码放到需要重复运行以便排查内存泄漏的地方：

    ``` cpp
    #include "os/CDebug.h"

    using Elastos::Droid::Os::IDebug;
    using Elastos::Droid::Os::CDebug;
    using Elastos::IO::IFile;
    using Elastos::IO::CFile;
    using Elastos::IO::IFileDescriptor;
    using Elastos::IO::CFileDescriptor;
    using Libcore::IO::IOsConstants;
    using Libcore::IO::COsConstants;
    using Libcore::IO::ILibcore;
    using Libcore::IO::CLibcore;
    using Libcore::IO::IOs;
    using Libcore::IO::IIoBridge;
    using Libcore::IO::CIoBridge;

    static void MyDumpMemery()
    {
        AutoPtr<IOsConstants> osConstans;
        COsConstants::AcquireSingleton((IOsConstants**)&osConstans);
        Int32 m1, m2, m3;
        osConstans->GetOsConstant(String("O_RDWR"), &m1);
        osConstans->GetOsConstant(String("O_CREAT"), &m2);
        osConstans->GetOsConstant(String("O_TRUNC"), &m3);

        AutoPtr<IFile> file;
        CFile::New(String("/data/debug"), (IFile**)&file);
        Boolean bval;
        file->Exists(&bval);
        if (!bval) {
            file->Mkdirs(&bval);
        }

        AutoPtr<IFileDescriptor> ifd;
        CFileDescriptor::New((IFileDescriptor**)&ifd);
        AutoPtr<IIoBridge> ioBridge;
        CIoBridge::AcquireSingleton((IIoBridge**)&ioBridge);
        Int32 fd;
        ioBridge->Open(String("/data/debug/1.txt"), m1 | m2 | m3, &fd);
        ifd->SetDescriptor(fd);

        AutoPtr<IDebug> dbg;
        CDebug::AcquireSingleton((IDebug**)&dbg);
        dbg->DumpHeap(ifd);
    }
    ```

2. 进入 shell 环境，赋予 /data/debug 写的权限： chmod 777 /data/debug，然后启用内存调试开关：

    ``` shell
    adb shell setprop libc.debug.malloc 1
    adb shell stop
    adb shell start
    ```

3. 杀死要调试的进程，让其自动重启。

4. 运行几次目标程序，将当前的内存信息保存下来，

    ``` shell
    adb pull /data/debug/1.txt 1.txt
    ```

5. 再次运行几次目标程序，将当前的内存信息保存下来

    ``` shell
    adb pull /data/debug/1.txt 1.txt
    ```

6. 使用文本比较工具，生成内存信息变动情况 diff.txt，将 diff.txt push 到 /data/debug 下。

7. 编译 DevSample 下面的 MemoryDummper，将 MemoryDummper push 到 /data /debug 下，运行 shell 命令<code>clear</code>清屏，然后运行<code>./MemoryDummper 要调试的进程 id</code>。

8. 将 shell 输出结果拷贝到 result.txt，运行 MemDumpFormatter.exe：<code> MemDumpFormatter result.txt addr2line.txt</code>，分析生成的 addr2line.txt 中的定位。

**为了调试方便，针对以上步骤制作了一个脚本: [loc_memleak](), 有以下注意事项**

 1. 脚本可以通过`./loc_memleak.sh -h` 查看帮助。
 2. 使用`./loc_memleak.sh -p` 开启内存调试开关
 3. 一般情况下，使用`./loc_memleak.sh -all`进行整体运行。在运行过程中，会出现用户交互。
    3.1. `Please run the target program, and then start the task(yes or no):` 表示执行脚本后，需要运行一个目标程序，然后输入`yes`
    3.2. `Please input the target program's NAME(such as: SystemUI PinyinIME):` 表示需要输入一个调试进程的名称，目的是获取该进程的id
    3.3. `Please run multiple times the target program, and then start the task(yes or no):` 再次运行几次目标程序，输入`yes`
    3.4. 脚本会将最后的结果输出到屏幕中，并且保存到一个文件中。
 4. 脚本生成的文件：
    4.1. `first.txt` 为首次运行目标程序生成的内存信息
    4.2. `second.txt` 为再次运行多次目标程序生成的内存信息
    4.3. `diff.txt` 为内存信息变动情况的文件
    4.4. `logcat.txt` 为开发板运行“MemoryDummper”生成的文件, 同时也是“MemDumpFormatter.exe”的输入文件
    4.5. `addr2line.txt` 为定位文件
    4.6. `result.txt` 为最终有可能内存泄露的源码位置信息

<span id="exception"></span>
## 异常
1. 关于 exceptions 的定义  
    翻译 framework 过程中，若碰到一些还没有在系统中定义的 Java exception，请统一在 ElastosRDK4\_2\_2/Sources/Elastos/Frameworks/Droid/Base/Core/car/Exceptions.car 中添加。

    例如，Droid Content 和 Droid App 模块的分别为 Elastos Content exception codes (Family: 0x50 - 0xD0)和 Elastos App exception codes (Family: 0x51 - 0xD1)。如需添加新的模块编号必须在 ElastosRDK4\_2\_2/Sources/Elastos/LibCore/car/Exception&ErrorCode.txt 中登记。

<span id="others"></span>
## 其他

<span id="qa"></span>
## Q/A
