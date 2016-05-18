
#ifndef __ELASTOS_DROID_SPEECH_CSPEECHRECOGNIZERHELPER_H__
#define __ELASTOS_DROID_SPEECH_CSPEECHRECOGNIZERHELPER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>
#include "_Elastos_Droid_Speech_CSpeechRecognizerHelper.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Speech::ISpeechRecognizer;
using Elastos::Droid::Content::IComponentName;

namespace Elastos {
namespace Droid {
namespace Speech {

CarClass(CSpeechRecognizerHelper)
    , public Singleton
    , public ISpeechRecognizerHelper
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL()

    CARAPI IsRecognitionAvailable(
        /* [in] */ IContext * pContext,
        /* [out] */ Boolean * pRet);

    CARAPI CreateSpeechRecognizer(
        /* [in] */ IContext * pContext,
        /* [out] */ ISpeechRecognizer ** ppRet);

    CARAPI CreateSpeechRecognizer(
        /* [in] */ IContext * pContext,
        /* [in] */ IComponentName * pServiceComponent,
        /* [out] */ ISpeechRecognizer ** ppRet);

private:
};

} // namespace Speech
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_CSPEECHRECOGNIZERHELPER_H__

