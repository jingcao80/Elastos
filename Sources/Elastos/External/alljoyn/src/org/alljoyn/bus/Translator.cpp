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

#include "org/alljoyn/bus/Translator.h"
#include "org/alljoyn/bus/NativeTranslator.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(Translator, Object, ITranslator)

Translator::Translator()
    : mHandle(0)
{}

Translator::~Translator()
{
    Destroy();
}

ECode Translator::constructor()
{
    Create();
    return NOERROR;
}

void Translator::Create()
{
    mHandle = reinterpret_cast<Int64>(new NativeTranslator((ITranslator*)this));
}

void Translator::Destroy()
{
    if (mHandle != 0) {
        NativeTranslator* t = reinterpret_cast<NativeTranslator*>(mHandle);
        delete t;
        mHandle = 0;
    }
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

