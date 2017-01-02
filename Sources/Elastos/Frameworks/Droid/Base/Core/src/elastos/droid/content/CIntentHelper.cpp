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

#include "elastos/droid/content/CIntentHelper.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CIntentHelper, Singleton, IIntentHelper)

CAR_SINGLETON_IMPL(CIntentHelper)

ECode CIntentHelper::CreateChooser(
    /* [in] */ IIntent* target,
    /* [in] */ ICharSequence* title,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    AutoPtr<IIntent> it = Intent::CreateChooser(target, title);
    *intent = it;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CIntentHelper::MakeMainActivity(
    /* [in] */ IComponentName* mainActivity,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    AutoPtr<IIntent> it = Intent::MakeMainActivity(mainActivity);
    *intent = it;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CIntentHelper::MakeMainSelectorActivity(
    /* [in] */ const String& selectorAction,
    /* [in] */ const String& selectorCategory,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    AutoPtr<IIntent> it = Intent::MakeMainSelectorActivity(selectorAction, selectorCategory);
    *intent = it;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CIntentHelper::MakeRestartActivityTask(
    /* [in] */ IComponentName* mainActivity,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    AutoPtr<IIntent> it = Intent::MakeRestartActivityTask(mainActivity);
    *intent = it;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CIntentHelper::GetIntent(
    /* [in] */ const String& uri,
    /* [out] */ IIntent** intent)
{
    return Intent::GetIntent(uri, intent);
}

ECode CIntentHelper::ParseUri(
    /* [in] */ const String& uri,
    /* [in] */ Int32 flags,
    /* [out] */ IIntent** intent)
{
    return Intent::ParseUri(uri, flags, intent);
}

ECode CIntentHelper::GetIntentOld(
    /* [in] */ const String& uri,
    /* [out] */ IIntent** intent)
{
    return Intent::GetIntentOld(uri, intent);
}

ECode CIntentHelper::ParseIntent(
    /* [in] */ IResources* resources,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IIntent** intent)
{
    return Intent::ParseIntent(resources, parser, attrs, intent);
}

ECode CIntentHelper::RestoreFromXml(
    /* [in] */ IXmlPullParser* in,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    AutoPtr<IIntent> temp = Intent::RestoreFromXml(in);
    *intent = temp;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CIntentHelper::NormalizeMimeType(
    /* [in] */ const String& type,
    /* [out] */ String* mimeType)
{
    VALIDATE_NOT_NULL(mimeType)
    *mimeType = Intent::NormalizeMimeType(type);
    return NOERROR;
}

ECode CIntentHelper::IsAccessUriMode(
    /* [in] */ Int32 modeFlags,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = Intent::IsAccessUriMode(modeFlags);
    return NOERROR;
}

}
}
}

