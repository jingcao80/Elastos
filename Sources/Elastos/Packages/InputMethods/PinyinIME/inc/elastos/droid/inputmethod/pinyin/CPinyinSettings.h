
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_CPINYINSETTINGS_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_CPINYINSETTINGS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Inputmethods_PinyinIME_CPinyinSettings.h"

using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

class PinyinSettings: public ElLightRefBase
{
public:
    PinyinSettings(
        /* [in] */ ISharedPreferences* pref);
};

CarClass(CPinyinSettings)
    , public Object
    , public ISettings
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI GetInstance(
        /* [in] */ ISharedPreferences* pref);

    CARAPI WriteBack();

    CARAPI ReleaseInstance();

    CARAPI GetKeySound(
        /* [out] */ Boolean* result);

    CARAPI SetKeySound(
        /* [in] */ Boolean v);

    CARAPI GetVibrate(
        /* [out] */ Boolean* result);

    CARAPI SetVibrate(
        /* [in] */ Boolean v);

    CARAPI GetPrediction(
        /* [out] */ Boolean* result);

    CARAPI SetPrediction(
        /* [in] */ Boolean v);

private:
    void InitConfs();

private:
    static String ANDPY_CONFS_KEYSOUND_KEY;
    static String ANDPY_CONFS_VIBRATE_KEY;
    static String ANDPY_CONFS_PREDICTION_KEY;

    static Boolean sKeySound;
    static Boolean sVibrate;
    static Boolean sPrediction;

    static AutoPtr<PinyinSettings> sInstance;
    static Int32 sRefCount;
    static AutoPtr<ISharedPreferences> sSharedPref;
    friend class PinyinSettings;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_CPINYINSETTINGS_H__
