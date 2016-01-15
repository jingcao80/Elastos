
#ifndef __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_KEYPHRASEMETADATA_H__
#define __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_KEYPHRASEMETADATA_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IArraySet;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

class KeyphraseMetadata
    : public Object
    , public IKeyphraseMetadata
{
public:
    CAR_INTERFACE_DECL()

    KeyphraseMetadata();

    virtual ~KeyphraseMetadata();

    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ const String& keyphrase,
        /* [in] */ IArraySet* supportedLocales,
        /* [in] */ Int32 recognitionModeFlags);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * @return Indicates if we support the given phrase.
     */
    CARAPI SupportsPhrase(
        /* [in] */ const String& phrase,
        /* [out] */ Boolean* result);

    /**
     * @return Indicates if we support the given locale.
     */
    CARAPI SupportsLocale(
        /* [in] */ ILocale* locale,
        /* [out] */ Boolean* result);

public:
    Int32 mId;
    String mKeyphrase;
    AutoPtr<IArraySet> mSupportedLocales;
    Int32 mRecognitionModeFlags;
};

} // namespace Soundtrigger
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_KEYPHRASEMETADATA_H__
