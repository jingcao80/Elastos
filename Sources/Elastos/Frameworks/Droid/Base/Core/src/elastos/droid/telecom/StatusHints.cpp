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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/StatusHints.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/os/CBundle.h"

using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Os::CBundle;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// StatusHints::
//===============================================================
CAR_INTERFACE_IMPL_2(StatusHints, Object, IStatusHints, IParcelable)

ECode StatusHints::constructor()
{
    return NOERROR;
}

ECode StatusHints::constructor(
    /* [in] */ IComponentName* packageName,
    /* [in] */ ICharSequence* label,
    /* [in] */ Int32 iconResId,
    /* [in] */ IBundle* extras)
{
    mPackageName = packageName;
    mLabel = label;
    mIconResId = iconResId;
    mExtras = extras;
    return NOERROR;
}

ECode StatusHints::GetPackageName(
    /* [out] */ IComponentName** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPackageName;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode StatusHints::GetLabel(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mLabel;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode StatusHints::GetIconResId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIconResId;
    return NOERROR;
}

ECode StatusHints::GetIcon(
    /* [in] */ IContext* context,
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IDrawable> dr = GetIcon(context, mIconResId);
    *result = dr;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode StatusHints::GetExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode StatusHints::WriteToParcel(
    /* [in] */ IParcel* out)
{
    if (mPackageName != NULL) {
        out->WriteInt32(1);
        IParcelable::Probe(mPackageName)->WriteToParcel(out);
    }
    else {
        out->WriteInt32(0);
    }
    String str;
    mLabel->ToString(&str);
    out->WriteString(str);
    out->WriteInt32(mIconResId);
    if (mExtras != NULL) {
        out->WriteInt32(1);
        IParcelable::Probe(mExtras)->WriteToParcel(out);
    }
    else {
        out->WriteInt32(0);
    }

    return NOERROR;
}

ECode StatusHints::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    Int32 value;
    in->ReadInt32(&value);
    if (value != 0) {
        mPackageName = NULL;
        CComponentName::New((IComponentName**)&mPackageName);
        IParcelable::Probe(mPackageName)->ReadFromParcel(in);
    }
    else {
        mPackageName = NULL;
    }

    mLabel = NULL;
    String str;
    in->ReadString(&str);
    CString::New(str, (ICharSequence**)&mLabel);

    in->ReadInt32(&mIconResId);

    in->ReadInt32(&value);
    if (value != 0) {
        mExtras = NULL;
        CBundle::New((IBundle**)&mExtras);
        IParcelable::Probe(mExtras)->ReadFromParcel(in);
    }
    else {
        mExtras = NULL;
    }

    return NOERROR;
}

AutoPtr<IDrawable> StatusHints::GetIcon(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> packageContext;
    // try {
    String name;
    mPackageName->GetPackageName(&name);
    context->CreatePackageContext(name, 0, (IContext**)&packageContext);
    // } catch (PackageManager.NameNotFoundException e) {
    //     Log.e(this, e, "Cannot find package %s", mPackageName.getPackageName());
    //     return null;
    // }
    // try {
    AutoPtr<IDrawable> res;
    packageContext->GetDrawable(resId, (IDrawable**)&res);
    return res;
    // } catch (MissingResourceException e) {
    //     Log.e(this, e, "Cannot find icon %d in package %s",
    //             resId, mPackageName.getPackageName());
    //     return null;
    // }
}

ECode StatusHints::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (other != NULL && IStatusHints::Probe(other) != NULL) {
        AutoPtr<IStatusHints> otherHints = IStatusHints::Probe(other);
        AutoPtr<IComponentName> otherName, name;
        otherHints->GetPackageName((IComponentName**)&otherName);
        GetPackageName((IComponentName**)&name);

        AutoPtr<ICharSequence> otherLabel, label;
        otherHints->GetLabel((ICharSequence**)&otherLabel);
        GetLabel((ICharSequence**)&label);

        Int32 otheId = 0, id = 0;
        otherHints->GetIconResId(&otheId);
        GetIconResId(&id);

        AutoPtr<IBundle> otherExt, ext;
        otherHints->GetExtras((IBundle**)&otherExt);
        GetExtras((IBundle**)&ext);

        *result = Object::Equals(otherName, name) &&
                Object::Equals(otherLabel, label) &&
                (otheId == id) &&
                Object::Equals(otherExt, ext);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode StatusHints::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    Int32 hc = 0;
    IObject::Probe(mPackageName)->GetHashCode(&hc);
    *result += hc;
    hc = 0;
    IObject::Probe(mLabel)->GetHashCode(&hc);
    *result += hc;
    *result += mIconResId;
    hc = 0;
    IObject::Probe(mExtras)->GetHashCode(&hc);
    *result += hc;
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos
