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

#ifndef __ELASTOS_DROID_PREFERENCE_PREFERENCEINFLATER_H__
#define __ELASTOS_DROID_PREFERENCE_PREFERENCEINFLATER_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/GenericInflater.h"

namespace Elastos {
namespace Droid {
namespace Preference {

class PreferenceInflater
    : public GenericInflater
    , public IPreferenceInflater
{
public:
    CAR_INTERFACE_DECL()

    PreferenceInflater();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IPreferenceManager* preferenceManager);

    CARAPI constructor(
        /* [in] */ IGenericInflater* original,
        /* [in] */ IPreferenceManager* preferenceManager,
        /* [in] */ IContext* newContext);

    CARAPI CloneInContext(
        /* [in] */ IContext* newContext,
        /* [out] */ IGenericInflater** ret);

protected:
    CARAPI OnCreateCustomFromTag(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IInterface* parentPreference,
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ Boolean* res);

    CARAPI OnMergeRoots(
        /* [in] */ IInterface* givenRoot,
        /* [in] */ Boolean attachToGivenRoot,
        /* [in] */ IInterface* xmlRoot,
        /* [out] */ IInterface** p);

private:
    CARAPI_(void) Init(
        /* [in] */ IPreferenceManager* preferenceManager);

private:
    static const String TAG;
    static const String INTENT_TAG_NAME;
    static const String EXTRA_TAG_NAME;

    AutoPtr<IPreferenceManager> mPreferenceManager;
};

} // Preference
} // Droid
} // Elastos

#endif // __CPREFERENCEINFLATER_H__
