
#include "CPinyinSettings.h"

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

String CPinyinSettings::ANDPY_CONFS_KEYSOUND_KEY = String("Sound");
String CPinyinSettings::ANDPY_CONFS_VIBRATE_KEY = String("Vibrate");
String CPinyinSettings::ANDPY_CONFS_PREDICTION_KEY = String("Prediction");
Boolean CPinyinSettings::sKeySound = FALSE;
Boolean CPinyinSettings::sVibrate = FALSE;
Boolean CPinyinSettings::sPrediction = FALSE;
AutoPtr<PinyinSettings> CPinyinSettings::sInstance;
Int32 CPinyinSettings::sRefCount = 0;
AutoPtr<ISharedPreferences> CPinyinSettings::sSharedPref;

PinyinSettings::PinyinSettings(
    /* [in] */ ISharedPreferences* pref)
{
    CPinyinSettings::sSharedPref = pref;
}

CAR_OBJECT_IMPL(CPinyinSettings);
CAR_INTERFACE_IMPL(CPinyinSettings, Object, ISettings);

ECode CPinyinSettings::GetInstance(
    /* [in] */ ISharedPreferences* pref)
{
    if (sInstance == NULL) {
        sInstance = new PinyinSettings(pref);
        InitConfs();
    }
    assert (pref == sSharedPref);
    sRefCount++;
    return NOERROR;
}

ECode CPinyinSettings::WriteBack()
{
    assert(sSharedPref != NULL);
    AutoPtr<ISharedPreferencesEditor> editor;
    sSharedPref->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutBoolean(ANDPY_CONFS_VIBRATE_KEY, sVibrate);
    editor->PutBoolean(ANDPY_CONFS_KEYSOUND_KEY, sKeySound);
    editor->PutBoolean(ANDPY_CONFS_PREDICTION_KEY, sPrediction);

    Boolean succeded = FALSE;
    return editor->Commit(&succeded);
}

ECode CPinyinSettings::ReleaseInstance()
{
    sRefCount--;
    if (sRefCount == 0) {
        sInstance = NULL;
    }

    return NOERROR;
}

ECode CPinyinSettings::GetKeySound(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = sKeySound;
    return NOERROR;
}

ECode CPinyinSettings::SetKeySound(
    /* [in] */ Boolean v)
{
    if (sKeySound == v) return NOERROR;
    sKeySound = v;
    return NOERROR;
}

ECode CPinyinSettings::GetVibrate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = sVibrate;
    return NOERROR;
}

ECode CPinyinSettings::SetVibrate(
    /* [in] */ Boolean v)
{
    if (sVibrate == v) return NOERROR;
    sVibrate = v;
    return NOERROR;
}

ECode CPinyinSettings::GetPrediction(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = sPrediction;
    return NOERROR;
}

ECode CPinyinSettings::SetPrediction(
    /* [in] */ Boolean v)
{
    if (sPrediction == v) return NOERROR;
    sPrediction = v;
    return NOERROR;
}

void CPinyinSettings::InitConfs()
{
    assert(sSharedPref != NULL);
    sSharedPref->GetBoolean(ANDPY_CONFS_KEYSOUND_KEY, TRUE, &sKeySound);
    sSharedPref->GetBoolean(ANDPY_CONFS_VIBRATE_KEY, FALSE, &sVibrate);
    sSharedPref->GetBoolean(ANDPY_CONFS_PREDICTION_KEY, TRUE, &sPrediction);
}

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
