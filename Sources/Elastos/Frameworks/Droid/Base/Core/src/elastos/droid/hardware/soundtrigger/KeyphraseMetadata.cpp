
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/hardware/soundtrigger/KeyphraseMetadata.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CAR_INTERFACE_IMPL(KeyphraseMetadata, Object, IKeyphraseMetadata)

KeyphraseMetadata::KeyphraseMetadata()
    : mId(0)
    , mKeyphrase(NULL)
    , mSupportedLocales(NULL)
    , mRecognitionModeFlags(0)
{
}

KeyphraseMetadata::~KeyphraseMetadata()
{
}

ECode KeyphraseMetadata::constructor(
    /* [in] */ Int32 id,
    /* [in] */ const String& keyphrase,
    /* [in] */ IArraySet* supportedLocales,
    /* [in] */ Int32 recognitionModeFlags)
{
    mId = id;
    mKeyphrase = keyphrase;
    mSupportedLocales = supportedLocales;
    mRecognitionModeFlags = recognitionModeFlags;
    return NOERROR;
}

ECode KeyphraseMetadata::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "id=";
    sb += mId;
    sb += ", keyphrase=";
    sb += mKeyphrase;
    sb += ", supported-locales=";
    String tmp;
    (IObject::Probe(mSupportedLocales))->ToString(&tmp);
    sb += tmp;
    sb += ", recognition-modes=";
    sb += mRecognitionModeFlags;
    return sb.ToString(str);
}

ECode KeyphraseMetadata::SupportsPhrase(
    /* [in] */ const String& phrase,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mKeyphrase.IsEmpty() || mKeyphrase.EqualsIgnoreCase(phrase);
    return NOERROR;
}

ECode KeyphraseMetadata::SupportsLocale(
    /* [in] */ ILocale* locale,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean result1, result2;
    FAIL_RETURN(ICollection::Probe(mSupportedLocales)->IsEmpty(&result1))
    FAIL_RETURN(ICollection::Probe(mSupportedLocales)->Contains(TO_IINTERFACE(locale), &result2))
    *result = (result1 || result2);
    return NOERROR;
}

} // namespace Soundtrigger
} // namespace Hardware
} // namespace Droid
} // namespace Elastos