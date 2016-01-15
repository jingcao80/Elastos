
#include "elastos/droid/speech/tts/CTtsEnginesHelper.h"
#include "elastos/droid/speech/tts/CTtsEngines.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

CAR_SINGLETON_IMPL(CTtsEnginesHelper)

CAR_INTERFACE_IMPL(CTtsEnginesHelper, Singleton, ITtsEnginesHelper)

ECode CTtsEnginesHelper::NormalizeTTSLocale(
    /* [in] */ ILocale* ttsLocale,
    /* [out] */ ILocale** outLocale)
{
    return TtsEngines::NormalizeTTSLocale(ttsLocale, outLocale);
}

} // namespace Tts
} // namespace Speech
} // namespace Droid
} // namespace Elastos

