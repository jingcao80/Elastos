//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_SETTINGS_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_SETTINGS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.InputMethod.Pinyin.h"
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
