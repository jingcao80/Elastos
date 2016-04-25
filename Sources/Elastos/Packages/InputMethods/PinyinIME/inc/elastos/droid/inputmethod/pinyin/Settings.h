
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_SETTINGS_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_SETTINGS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.InputMethod.Pinyin.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

class Settings : public Object
{
public:
    static CARAPI_(AutoPtr<Settings>) GetInstance(
        /* [in] */ ISharedPreferences* pref);

    static CARAPI_(void) WriteBack();

    static CARAPI_(void) ReleaseInstance();

    static CARAPI_(Boolean) GetKeySound();

    static CARAPI_(void) SetKeySound(
        /* [in] */ Boolean v);

    static CARAPI_(Boolean) GetVibrate();

    static CARAPI_(void) SetVibrate(
        /* [in] */ Boolean v);

    static CARAPI_(Boolean) GetPrediction();

    static CARAPI_(void) SetPrediction(
        /* [in] */ Boolean v);

protected:
    Settings(
        /* [in] */ ISharedPreferences* pref);

private:
    CARAPI_(void) InitConfs();

private:
    static String ANDPY_CONFS_KEYSOUND_KEY;
    static String ANDPY_CONFS_VIBRATE_KEY;
    static String ANDPY_CONFS_PREDICTION_KEY;

    static Boolean sKeySound;
    static Boolean sVibrate;
    static Boolean sPrediction;

    static AutoPtr<Settings> sInstance;

    static Int32 sRefCount;

    static AutoPtr<ISharedPreferences> sSharedPref;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_SETTINGS_H__
