
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

