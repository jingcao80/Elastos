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

#include "org/alljoyn/bus/NativeTranslator.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {


NativeTranslator::NativeTranslator(
    /* [in] */ ITranslator* translator)
{
    IWeakReferenceSource::Probe(translator)->GetWeakReference((IWeakReference**)&mTranslator);
}

NativeTranslator::~NativeTranslator()
{
}

size_t NativeTranslator::NumTargetLanguages()
{
    AutoPtr<ITranslator> obj;
    mTranslator->Resolve(EIID_ITranslator, (IInterface**)&obj);
    if (obj == NULL) {
        Logger::E("NativeTranslator", "NumTargetLanguages(): Can't get new local reference to Translator");
        return 0;
    }

    Int32 ret;
    if (FAILED(obj->NumTargetLanguages(&ret))) {
        Logger::E("NativeTranslator", "NumTargetLanguages(): Exception");
        return 0;
    }

    return ret;
}

void NativeTranslator::GetTargetLanguage(
    /* [in] */ size_t index,
    /* [in] */ qcc::String& ret)
{
    AutoPtr<ITranslator> obj;
    mTranslator->Resolve(EIID_ITranslator, (IInterface**)&obj);
    if (obj == NULL) {
        Logger::E("NativeTranslator", "GetTargetLanguage(): Can't get new local reference to Translator");
        return;
    }

    String res;
    if (FAILED(obj->GetTargetLanguage((Int32)index, &res))) {
        Logger::E("NativeTranslator", "GetTargetLanguage(): Exception");
        return ;
    }

    if (res == NULL) {
        return;
    }

    ret.assign(res.string());
}

const char* NativeTranslator::Translate(
    /* [in] */ const char* sourceLanguage,
    /* [in] */ const char* targetLanguage,
    /* [in] */ const char* source,
    /* [in] */ qcc::String& buffer)
{
    AutoPtr<ITranslator> obj;
    mTranslator->Resolve(EIID_ITranslator, (IInterface**)&obj);
    if (obj == NULL) {
        Logger::E("NativeTranslator", "Translate(): Can't get new local reference to Translator");
        return NULL;
    }

    String res;
    if (FAILED(obj->Translate(String(sourceLanguage), String(targetLanguage), String(source), &res))) {
        Logger::E("NativeTranslator", "Translate(): Exception");
        return NULL;
    }

    if (res == NULL) {
        return NULL;
    }
    buffer.assign(res.string());

    return buffer.c_str();
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
