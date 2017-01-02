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

#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/view/ContextThemeWrapper.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Content::Res::CResources;

namespace Elastos {
namespace Droid {
namespace View {


CAR_INTERFACE_IMPL(ContextThemeWrapper, ContextWrapper, IContextThemeWrapper)

ContextThemeWrapper::ContextThemeWrapper()
{
}

ContextThemeWrapper::~ContextThemeWrapper()
{
    mInflater = NULL;
    mTheme = NULL;
    mOverrideConfiguration = NULL;
    mResources = NULL;
}

ECode ContextThemeWrapper::constructor()
{
    return ContextWrapper::constructor(NULL);
}

ECode ContextThemeWrapper::constructor(
    /* [in] */ IContext* base,
    /* [in] */ Int32 themeres)
{
    return constructor(base, themeres, TRUE);
}

ECode ContextThemeWrapper::constructor(
    /* [in] */ IContext* base,
    /* [in] */ Int32 themeres,
    /* [in] */ Boolean holdBaseContext)
{
    FAIL_RETURN(ContextWrapper::constructor(base, holdBaseContext));
    mThemeResource = themeres;
    return NOERROR;
}

ECode ContextThemeWrapper::ApplyOverrideConfiguration(
    /* [in] */ IConfiguration* overrideConfiguration)
{
    if (mResources != NULL) {
        // throw new IllegalStateException("getResources() has already been called");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mOverrideConfiguration != NULL) {
        // throw new IllegalStateException("Override configuration has already been set");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return CConfiguration::New(overrideConfiguration, (IConfiguration**)&mOverrideConfiguration);
}

ECode ContextThemeWrapper::GetResources(
    /* [out] */ IResources** resources)
{
    VALIDATE_NOT_NULL(resources)
    if (mResources != NULL) {
        *resources = mResources;
        REFCOUNT_ADD(*resources);
        return NOERROR;
    }
    if (mOverrideConfiguration == NULL) {
        ContextWrapper::GetResources((IResources**)&mResources);
        *resources = mResources;
        REFCOUNT_ADD(*resources);
        return NOERROR;
    }
    else {
        AutoPtr<IContext> resc;
        FAIL_RETURN(CreateConfigurationContext(mOverrideConfiguration, (IContext**)&resc));
        FAIL_RETURN(resc->GetResources((IResources**)&mResources));
        *resources = mResources;
        REFCOUNT_ADD(*resources);
        return NOERROR;
    }
}

ECode ContextThemeWrapper::SetTheme(
    /* [in] */ Int32 resId)
{
    mThemeResource = resId;
    return InitializeTheme();
}

ECode ContextThemeWrapper::GetThemeResId(
    /* [out] */ Int32* resId)
{
    VALIDATE_NOT_NULL(resId)
    *resId = mThemeResource;
    return NOERROR;
}

ECode ContextThemeWrapper::GetTheme(
    /* [in] */ IResourcesTheme** theme)
{
    if (mTheme == NULL) {
        AutoPtr<IApplicationInfo> appInfo;
        GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 version;
        appInfo->GetTargetSdkVersion(&version);
        mThemeResource = CResources::SelectDefaultTheme(mThemeResource, version);
        FAIL_RETURN(InitializeTheme());
    }

    *theme = mTheme;
    REFCOUNT_ADD(*theme);
    return NOERROR;
}

ECode ContextThemeWrapper::GetSystemService(
    /* [in] */ const String& name,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    if (IContext::LAYOUT_INFLATER_SERVICE.Equals(name)) {
        if (mInflater == NULL) {
            AutoPtr<ILayoutInflater> inflater;
            LayoutInflater::From(mBase, (ILayoutInflater**)&inflater);
            inflater->CloneInContext(this, (ILayoutInflater**)&mInflater);
        }
        *object = mInflater;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }
    return mBase->GetSystemService(name, object);
}

/**
 * Called by {@link #setTheme} and {@link #getTheme} to apply a theme
 * resource to the current Theme object.  Can override to change the
 * default (simple) behavior.  This method will not be called in multiple
 * threads simultaneously.
 *
 * @param theme The Theme object being modified.
 * @param resid The theme style resource being applied to <var>theme</var>.
 * @param first Set to true if this is the first time a style is being
 *              applied to <var>theme</var>.
 */
ECode ContextThemeWrapper::OnApplyThemeResource(
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ Int32 resid,
    /* [in] */ Boolean first)
{
    return theme->ApplyStyle(resid, TRUE);
}

ECode ContextThemeWrapper::InitializeTheme()
{
    Boolean first = mTheme == NULL;
    if (first) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        resources->NewTheme((IResourcesTheme**)&mTheme);
        AutoPtr<IResourcesTheme> theme;
        mBase->GetTheme((IResourcesTheme**)&theme);
        if (theme != NULL) {
            mTheme->SetTo(theme);
        }
    }
    return OnApplyThemeResource(mTheme, mThemeResource, first);
}

} // namespace View
} // namespace Droid
} // namespace Elastos
