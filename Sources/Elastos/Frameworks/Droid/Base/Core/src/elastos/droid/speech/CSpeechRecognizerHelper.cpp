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

#include "elastos/droid/speech/CSpeechRecognizerHelper.h"
#include "elastos/droid/speech/CSpeechRecognizer.h"

namespace Elastos {
namespace Droid {
namespace Speech {

CAR_SINGLETON_IMPL(CSpeechRecognizerHelper)

CAR_INTERFACE_IMPL(CSpeechRecognizerHelper, Singleton, ISpeechRecognizerHelper)

ECode CSpeechRecognizerHelper::IsRecognitionAvailable(
    /* [in] */ IContext * pContext,
    /* [out] */ Boolean * pRet)
{
    VALIDATE_NOT_NULL(pRet);
    *pRet = CSpeechRecognizer::IsRecognitionAvailable(pContext);
    return NOERROR;
}

ECode CSpeechRecognizerHelper::CreateSpeechRecognizer(
    /* [in] */ IContext * pContext,
    /* [out] */ ISpeechRecognizer ** ppRet)
{
    VALIDATE_NOT_NULL(ppRet);
    AutoPtr<ISpeechRecognizer> temp = CSpeechRecognizer::CreateSpeechRecognizer(pContext);
    *ppRet = temp;
    REFCOUNT_ADD(*ppRet);
    return NOERROR;
}

ECode CSpeechRecognizerHelper::CreateSpeechRecognizer(
    /* [in] */ IContext * pContext,
    /* [in] */ IComponentName * pServiceComponent,
    /* [out] */ ISpeechRecognizer ** ppRet)
{
    VALIDATE_NOT_NULL(ppRet);
    AutoPtr<ISpeechRecognizer> temp = CSpeechRecognizer::CreateSpeechRecognizer(pContext, pServiceComponent);
    *ppRet = temp;
    REFCOUNT_ADD(*ppRet);
    return NOERROR;
}

} // namespace Speech
} // namespace Droid
} // namespace Elastos

