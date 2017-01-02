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

#include "elastos/droid/content/Context.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(Context, Object, IContext)

Context::Context()
{
}

Context::~Context()
{
}

ECode Context::RegisterComponentCallbacks(
    /* [in] */ IComponentCallbacks* componentCallback)
{
    AutoPtr<IContext> context;
    FAIL_RETURN(GetApplicationContext((IContext**) &context));
    return context->RegisterComponentCallbacks(componentCallback);
}

ECode Context::UnregisterComponentCallbacks(
    /* [in] */ IComponentCallbacks* componentCallback)
{
    AutoPtr<IContext> context;
    FAIL_RETURN(GetApplicationContext((IContext**) &context));
    return context->UnregisterComponentCallbacks(componentCallback);
}

ECode Context::GetText(
    /* [in] */ Int32 resId,
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text)
    *text = NULL;
    AutoPtr<IResources> resources;
    FAIL_RETURN(GetResources((IResources**) &resources));
    return resources->GetText(resId, text);
}

ECode Context::GetString(
    /* [in] */ Int32 resId,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);
    AutoPtr<IResources> resources;
    FAIL_RETURN(GetResources((IResources**) &resources));
    return resources->GetString(resId, str);
}

ECode Context::GetString(
    /* [in] */ Int32 resId,
    /* [in] */ ArrayOf<IInterface*>* formatArgs,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);
    AutoPtr<IResources> resources;
    FAIL_RETURN(GetResources((IResources**) &resources));
    return resources->GetString(resId, formatArgs, str);
}

ECode Context::GetDrawable(
    /* [in] */ Int32 id,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    *drawable = NULL;

    AutoPtr<IResourcesTheme> theme;
    FAIL_RETURN(GetTheme((IResourcesTheme**)&theme))
    AutoPtr<IResources> resources;
    FAIL_RETURN(GetResources((IResources**) &resources));

    return resources->GetDrawable(id, theme, drawable);
}

ECode Context::GetThemeResId(
    /* [out] */ Int32* resId)
{
    VALIDATE_NOT_NULL(resId)
    *resId = 0;
    return NOERROR;
}

ECode Context::ObtainStyledAttributes(
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** styles)
{
    VALIDATE_NOT_NULL(styles)
    *styles = NULL;
    AutoPtr<IResourcesTheme> theme;
    FAIL_RETURN(GetTheme((IResourcesTheme**) &theme));
    return theme->ObtainStyledAttributes(attrs, styles);
}

ECode Context::ObtainStyledAttributes(
    /* [in] */ Int32 resid,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** styles)
{
    VALIDATE_NOT_NULL(styles)
    *styles = NULL;
    AutoPtr<IResourcesTheme> theme;
    FAIL_RETURN(GetTheme((IResourcesTheme**) &theme));
    return theme->ObtainStyledAttributes(resid, attrs, styles);
}

ECode Context::ObtainStyledAttributes(
    /* [in] */ IAttributeSet* set,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** styles)
{
    VALIDATE_NOT_NULL(styles)
    *styles = NULL;
    AutoPtr<IResourcesTheme> theme;
    FAIL_RETURN(GetTheme((IResourcesTheme**) &theme));
    return theme->ObtainStyledAttributes(set, attrs, 0, 0, styles);
}

ECode Context::ObtainStyledAttributes(
    /* [in] */ IAttributeSet* set,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes,
    /* [out] */ ITypedArray** styles)
{
    VALIDATE_NOT_NULL(styles)
    *styles = NULL;
    AutoPtr<IResourcesTheme> theme;
    FAIL_RETURN(GetTheme((IResourcesTheme**) &theme));
    return theme->ObtainStyledAttributes(set, attrs, defStyleAttr, defStyleRes, styles);
}

ECode Context::StartActivityAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    //Not implemented. Must override in a subclass.
    return E_RUNTIME_EXCEPTION;
}

ECode Context::StartActivityAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options,
    /* [in] */ IUserHandle* userId)
{
    //Not implemented. Must override in a subclass.
    return E_RUNTIME_EXCEPTION;
}

ECode Context::IsRestricted(
    /* [out] */ Boolean* isRestricted)
{
    VALIDATE_NOT_NULL(isRestricted)
    *isRestricted = FALSE;
    return NOERROR;
}

} //Content
} //Droid
} //Elastos
