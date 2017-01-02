//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __HELLOCAR_ANIMAL_H__
#define __HELLOCAR_ANIMAL_H__

#include <_Elastos.HelloCar.h>    // include 模块头文件，文件名格式：模块名称_server.h
#include <elastos/core/Object.h>        // include Object 基类

using Elastos::Core::IComparable;

namespace Elastos {
namespace HelloCar {

// C++ 类　Animal 作为 CCat，CDog 的基类，实现了 IAnimal 接口，并继承自基类 Object
class Animal
    : public Object
    , public IAnimal
    , public IComparable
{
public:
    CAR_INTERFACE_DECL()    // 实现某个 CAR 接口的类都需要使用 CAR_INTERFACE_DECL/CAR_INTERFACE_IMPL

    Animal();               // 若有成员需要初始化，则需显式声明构造函数，以便使用初始化列表进行初始化

    virtual ~Animal();

    CARAPI constructor();   // 用于被子类继承的 CAR 构造函数

    CARAPI constructor(     // 用于被子类继承的 CAR 构造函数
        /* [in] */ Int32 age,
        /* [in] */ const String& name);

    // IComparable 接口函数
    //
    CARAPI CompareTo(
        /* [in] */ IInterface* another,
        /* [out] */ Int32* result);

    // IAnimal 接口函数
    //
    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetAge(
        /* [in] */ Int32 age);

    CARAPI GetAge(
        /* [out] */ Int32* age);

    CARAPI CanFly(
        /* [out] */ Boolean* canFly);

protected:
    // 成员变量
    Int32 mAge;
    String mName;
};

} // HelloCar
} // Elastos

#endif //__HELLOCAR_ANIMAL_H__