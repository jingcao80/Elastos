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
