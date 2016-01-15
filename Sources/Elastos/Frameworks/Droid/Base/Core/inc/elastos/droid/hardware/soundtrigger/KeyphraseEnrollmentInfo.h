
#ifndef __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_KEYPHRASEENROLLMENTINFO_H__
#define __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_KEYPHRASEENROLLMENTINFO_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::ITypedArray;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

class KeyphraseEnrollmentInfo
    : public Object
    , public IKeyphraseEnrollmentInfo
{
public:
    CAR_INTERFACE_DECL()

    KeyphraseEnrollmentInfo();

    virtual ~KeyphraseEnrollmentInfo();

    CARAPI constructor(
        /* [in] */ IPackageManager* pm);

    CARAPI GetParseError(
        /* [out] */ String* error);

    /**
     * @return An array of available keyphrases that can be enrolled on the system.
     *         It may be null if no keyphrases can be enrolled.
     */
    CARAPI ListKeyphraseMetadata(
        /* [out, callee] */ ArrayOf<IKeyphraseMetadata*>** data);

    CARAPI GetManageKeyphraseIntent(
        /* [in] */ Int32 action,
        /* [in] */ const String& keyphrase,
        /* [in] */ ILocale* locale,
        /* [out] */ IIntent** intent);

    /**
     * Gets the {@link KeyphraseMetadata} for the given keyphrase and locale, null if any metadata
     * isn't available for the given combination.
     *
     * @param keyphrase The keyphrase that the user needs to be enrolled to.
     * @param locale The locale for which the enrollment needs to be performed.
     *        This is a Java locale, for example "en_US".
     * @return The metadata, if the enrollment client supports the given keyphrase
     *         and locale, null otherwise.
     */
    CARAPI GetKeyphraseMetadata(
        /* [in] */ const String& keyphrase,
        /* [in] */ ILocale* locale,
        /* [out] */ IKeyphraseMetadata** data);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI InitializeKeyphrasesFromTypedArray(
        /* [in] */ ITypedArray* array);

private:
    static const String TAG;
    /**
     * Name under which a Hotword enrollment component publishes information about itself.
     * This meta-data should reference an XML resource containing a
     * <code>&lt;{@link
     * android.R.styleable#VoiceEnrollmentApplication
     * voice-enrollment-application}&gt;</code> tag.
     */
    static const String VOICE_KEYPHRASE_META_DATA;

    AutoPtr<ArrayOf<IKeyphraseMetadata*> > mKeyphrases;
    String mEnrollmentPackage;
    String mParseError;
};

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_KEYPHRASEENROLLMENTINFO_H__
