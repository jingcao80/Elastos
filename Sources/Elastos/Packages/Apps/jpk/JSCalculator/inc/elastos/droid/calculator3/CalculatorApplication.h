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

#ifndef _ELASTOS_DROID_CALCULATOR3_CALCULATORAPPLICATION_H__
#define _ELASTOS_DROID_CALCULATOR3_CALCULATORAPPLICATION_H__

#include "_Elastos.Droid.Calculator3.h"

// #include "elastos/droid/calculator3/CalculatorEditText.h"
// #include "elastos/droid/calculator3/CalculatorExpressionEvaluator.h"
// #include "elastos/droid/calculator3/CalculatorExpressionTokenizer.h"
// #include <elastos/droid/animation/AnimatorListenerAdapter.h>
// #include <elastos/droid/app/Activity.h>
// #include <elastos/droid/ext/frameworkext.h>
// #include <Elastos.Droid.Text.h>
// #include <Elastos.Droid.Support.V4.View.h>
#include <elastos/core/Object.h>

#include <elastos/droid/app/Application.h>

using Elastos::Droid::App::Application;

namespace Elastos {
namespace Droid {
namespace Calculator3 {

class CalculatorApplication
    : public Application
    , public ICalculatorApplication
{
public:
    CalculatorApplication();

    CARAPI constructor();

    virtual ~CalculatorApplication();

    CAR_INTERFACE_DECL()

    CARAPI OnCreate();

    CARAPI OnTest();
};

} // namespace Calculator3
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_CALCULATOR3_CALCULATORAPPLICATION_H__