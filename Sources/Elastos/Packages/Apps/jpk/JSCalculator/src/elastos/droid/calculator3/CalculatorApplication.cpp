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

#include "elastos/droid/calculator3/CalculatorApplication.h"

#include <cutils/log.h>

//----------------------------------------------------------------
//           CalculatorApplication
//----------------------------------------------------------------

namespace Elastos {
namespace Droid {
namespace Calculator3 {

CAR_INTERFACE_IMPL(CalculatorApplication, Application, ICalculatorApplication)

CalculatorApplication::CalculatorApplication()
{
}

CalculatorApplication::~CalculatorApplication()
{}

ECode CalculatorApplication::constructor()
{
    return Application::constructor();
}

ECode CalculatorApplication::OnCreate()
{
    ALOGD("CalculatorApplication::OnCreate====begin====");
    return NOERROR;
}

ECode CalculatorApplication::OnTest()
{
    return NOERROR;
}

} // namespace Calculator3
} // namespace Droid
} // namespace Elastos
