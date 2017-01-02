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

#ifndef __CANNOTATION_H__
#define __CANNOTATION_H__

#include "_CAnnotation.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

CarClass(CAnnotation)
    , public Object
    , public IAnnotation
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 data);

    CARAPI M1();

    CARAPI M2();

    CARAPI M3();

    CARAPI M4();

    CARAPI M5();

    CARAPI M6();

private:
    // TODO: Add your private member variables here.
};


#endif // __CANNOTATION_H__
