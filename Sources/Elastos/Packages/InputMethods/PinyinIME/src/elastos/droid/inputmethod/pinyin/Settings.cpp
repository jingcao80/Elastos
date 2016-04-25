
#include "Elastos.Droid.Content.h"
#include "elastos/droid/inputmethod/pinyin/Settings.h"

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

String Settings::ANDPY_CONFS_KEYSOUND_KEY("Sound");
String Settings::ANDPY_CONFS_VIBRATE_KEY("Vibrate");
String Settings::ANDPY_CONFS_PREDICTION_KEY("Prediction");
Boolean Settings::sKeySound = FALSE;
Boolean Settings::sVibrate = FALSE;
Boolean Settings::sPrediction = FALSE;
AutoPtr<Settings> Settings::sInstance;
Int32 Settings::sRefCount = 0;
AutoPtr<ISharedPreferences> Settings::sSharedPref;

Settings::Settings(
    /* [in] */ ISharedPreferences* pref)
{
    sSharedPref = pref;
    InitConfs();
}

AutoPtr<Settings> Settings::GetInstance(
    /* [in] */ ISharedPreferences* pref)
{
    if (sInstance == NULL) {
        sInstance = new Settings(pref);
    }
    assert (pref == sSharedPref);
    sRefCount++;
    return sInstance;
}

void Settings::WriteBack()
{
    AutoPtr<ISharedPreferencesEditor> editor;
    sSharedPref->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutBoolean(ANDPY_CONFS_VIBRATE_KEY, sVibrate);
    editor->PutBoolean(ANDPY_CONFS_KEYSOUND_KEY, sKeySound);
    editor->PutBoolean(ANDPY_CONFS_PREDICTION_KEY, sPrediction);
    Boolean succeeded = FALSE;
    editor->Commit(&succeeded);
}

void Settings::ReleaseInstance()
{
    sRefCount--;
    if (sRefCount == 0) {
        sInstance = NULL;
    }
}

void Settings::InitConfs()
{
    sSharedPref->GetBoolean(ANDPY_CONFS_KEYSOUND_KEY, TRUE, &sKeySound);
    sSharedPref->GetBoolean(ANDPY_CONFS_VIBRATE_KEY, FALSE, &sVibrate);
    sSharedPref->GetBoolean(ANDPY_CONFS_PREDICTION_KEY, TRUE, &sPrediction);
}

Boolean Settings::GetKeySound()
{
    return sKeySound;
}

void Settings::SetKeySound(
    /* [in] */ Boolean v)
{
    if (sKeySound == v) return;
    sKeySound = v;
}

Boolean Settings::GetVibrate()
{
    return sVibrate;
}

void Settings::SetVibrate(
    /* [in] */ Boolean v)
{
    if (sVibrate == v) return;
    sVibrate = v;
}

Boolean Settings::GetPrediction()
{
    return sPrediction;
}

void Settings::SetPrediction(
    /* [in] */ Boolean v)
{
    if (sPrediction == v) return;
    sPrediction = v;
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
