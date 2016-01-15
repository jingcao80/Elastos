
#include "elastos/droid/speech/srec/CRecognizerHelper.h"
#include "elastos/droid/speech/srec/Recognizer.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

CAR_SINGLETON_IMPL(CRecognizerHelper)

CAR_INTERFACE_IMPL(CRecognizerHelper, Singleton, IRecognizerHelper)

ECode CRecognizerHelper::GetConfigDir(
    /* [in] */ ILocale* locale,
    /* [out] */ String* ret)
{
    return Recognizer::GetConfigDir(locale, ret);
}

ECode CRecognizerHelper::EventToString(
    /* [in] */ Int32 event,
    /* [out] */ String* ret)
{
    return Recognizer::EventToString(event, ret);
}

} // namespace Srec
} // namespace Speech
} // namespace Droid
} // namespace Elastos
