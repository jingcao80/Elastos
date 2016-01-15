#include "elastos/droid/app/CActivityManagerTaskDescription.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CColor.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::CColor;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

const String ATTR_TASKDESCRIPTION_PREFIX("task_description_");

const String ATTR_TASKDESCRIPTIONLABEL("task_description_label");
const String ATTR_TASKDESCRIPTIONCOLOR("task_description_color");
const String ATTR_TASKDESCRIPTIONICONFILENAM("task_description_icon_filename");

CAR_INTERFACE_IMPL_2(CActivityManagerTaskDescription, Object, IActivityManagerTaskDescription, IParcelable)

CAR_OBJECT_IMPL(CActivityManagerTaskDescription)

CActivityManagerTaskDescription::CActivityManagerTaskDescription()
    : mColorPrimary(0)
{
}

ECode CActivityManagerTaskDescription::constructor()
{
    String nullStr;
    return constructor(nullStr, NULL, 0);
}

ECode CActivityManagerTaskDescription::constructor(
    /* [in] */ const String& label,
    /* [in] */ IBitmap* icon,
    /* [in] */ Int32 colorPrimary)
{
    Int32 alpha;
    AutoPtr<IColor> color;
    CColor::AcquireSingleton((IColor**)&color);
    color->Alpha(colorPrimary, &alpha);
    if ((colorPrimary != 0) && (alpha != 255)) {
        // throw new RuntimeException("A TaskDescription's primary color should be opaque");
        Logger::E("CActivityManagerTaskDescription", "A TaskDescription's primary color should be opaque");
        return E_RUNTIME_EXCEPTION;
    }

    mLabel = label;
    mIcon = icon;
    mColorPrimary = colorPrimary;
    return NOERROR;
}

ECode CActivityManagerTaskDescription::constructor(
    /* [in] */ const String& label,
    /* [in] */ Int32 colorPrimary,
    /* [in] */ const String& iconFilename)
{
    FAIL_RETURN(constructor(label, NULL, colorPrimary))
    mIconFilename = iconFilename;
    return NOERROR;
}

ECode CActivityManagerTaskDescription::constructor(
    /* [in] */ const String& label,
    /* [in] */ IBitmap* icon)
{
    return constructor(label, icon, 0);
}

ECode CActivityManagerTaskDescription::constructor(
    /* [in] */ const String& label)
{
    return constructor(label, NULL, 0);
}

ECode CActivityManagerTaskDescription::constructor(
    /* [in] */ IActivityManagerTaskDescription* td)
{
    td->GetLabel(&mLabel);
    td->GetIcon((IBitmap**)&mIcon);
    td->GetPrimaryColor(&mColorPrimary);
    td->GetIconFilename(&mIconFilename);
    return NOERROR;
}

ECode CActivityManagerTaskDescription::SetLabel(
    /* [in] */ const String& label)
{
    mLabel = label;
    return NOERROR;
}

ECode CActivityManagerTaskDescription::SetPrimaryColor(
    /* [in] */ Int32 primaryColor)
{
    AutoPtr<IColor> color;
    CColor::AcquireSingleton((IColor**)&color);
    Int32 alpha;
    color->Alpha(primaryColor, &alpha);
    if ((primaryColor != 0) && (alpha != 255)) {
        // throw new RuntimeException("A TaskDescription's primary color should be opaque");
        Logger::E("CActivityManagerTaskDescription", "A TaskDescription's primary color should be opaque");
        return E_RUNTIME_EXCEPTION;
    }

    mColorPrimary = primaryColor;
    return NOERROR;
}

ECode CActivityManagerTaskDescription::SetIcon(
    /* [in] */ IBitmap* icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode CActivityManagerTaskDescription::SetIconFilename(
    /* [in] */ const String& iconFilename)
{
    mIconFilename = iconFilename;
    mIcon = NULL;
    return NOERROR;
}

ECode CActivityManagerTaskDescription::GetLabel(
    /* [out] */ String* label)
{
    VALIDATE_NOT_NULL(label)
    *label = mLabel;
    return NOERROR;
}

ECode CActivityManagerTaskDescription::GetIcon(
    /* [out] */ IBitmap** bm)
{
    if (mIcon != NULL) {
        *bm = mIcon;
        REFCOUNT_ADD(*bm)
        return NOERROR;
    }

    return LoadTaskDescriptionIcon(mIconFilename, bm);
}

ECode CActivityManagerTaskDescription::GetIconFilename(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mIconFilename;
    return NOERROR;
}

ECode CActivityManagerTaskDescription::GetInMemoryIcon(
    /* [out] */ IBitmap** bm)
{
    VALIDATE_NOT_NULL(bm)
    *bm = mIcon;
    REFCOUNT_ADD(*bm)
    return NOERROR;
}

ECode CActivityManagerTaskDescription::LoadTaskDescriptionIcon(
    /* [in] */ const String& iconFilename,
    /* [out] */ IBitmap** bm)
{
    VALIDATE_NOT_NULL(bm)
    *bm = NULL;

    ECode ec = NOERROR;
    if (!iconFilename.IsNull()) {
        // try {
    ec = ActivityManagerNative::GetDefault()->GetTaskDescriptionIcon(iconFilename, bm);
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        ec = NOERROR;
    }
        // } catch (RemoteException e) {
        // }
    }
    return ec;
}

ECode CActivityManagerTaskDescription::GetPrimaryColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color)
    *color = mColorPrimary;
    return NOERROR;
}

/** @hide */
ECode CActivityManagerTaskDescription::SaveToXml(
    /* [in] */ IXmlSerializer* xs)
{
    assert(0 && "TODO");
    // if (mLabel != NULL) {
    //     out.attribute(NULL, ATTR_TASKDESCRIPTIONLABEL, mLabel);
    // }
    // if (mColorPrimary != 0) {
    //     out.attribute(NULL, ATTR_TASKDESCRIPTIONCOLOR, Integer.toHexString(mColorPrimary));
    // }
    // if (!mIconFilename.IsNull()) {
    //     out.attribute(NULL, ATTR_TASKDESCRIPTIONICONFILENAME, mIconFilename);
    // }
    return NOERROR;
}

ECode CActivityManagerTaskDescription::RestoreFromXml(
    /* [in] */ const String& attrName,
    /* [in] */ const String& attrValue)
{
    assert(0 && "TODO");
    // if (ATTR_TASKDESCRIPTIONLABEL.equals(attrName)) {
    //     setLabel(attrValue);
    // } else if (ATTR_TASKDESCRIPTIONCOLOR.equals(attrName)) {
    //     setPrimaryColor((int) Long.parseLong(attrValue, 16));
    // } else if (ATTR_TASKDESCRIPTIONICONFILENAME.equals(attrName)) {
    //     setIconFilename(attrValue);
    // }
    return NOERROR;
}

ECode CActivityManagerTaskDescription::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (mLabel.IsNull()) {
        dest->WriteInt32(0);
    } else {
        dest->WriteInt32(1);
        dest->WriteString(mLabel);
    }
    if (mIcon == NULL) {
        dest->WriteInt32(0);
    } else {
        dest->WriteInt32(1);
        IParcelable::Probe(mIcon)->WriteToParcel(dest);
    }
    dest->WriteInt32(mColorPrimary);
    if (mIconFilename.IsNull()) {
        dest->WriteInt32(0);
    } else {
        dest->WriteInt32(1);
        dest->WriteString(mIconFilename);
    }
    return NOERROR;
}

ECode CActivityManagerTaskDescription::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 ival;
    source->ReadInt32(&ival);
    if (ival > 0) {
        source->ReadString(&mLabel);
    }

    source->ReadInt32(&ival);
    if (ival > 0) {
        CBitmap::New((IBitmap**)&mIcon);
        IParcelable::Probe(mIcon)->ReadFromParcel(source);
    }

    source->ReadInt32(&ival);
    if (ival > 0) {
        source->ReadString(&mIconFilename);
    }
    return NOERROR;
}

ECode CActivityManagerTaskDescription::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CActivityManagerTaskDescription Label: ");
    sb += mLabel;
    sb += " Icon: ";
    sb += Object::ToString(mIcon);
    sb += " colorPrimary: ";
    sb += mColorPrimary;
    *str = sb.ToString();
    return NOERROR;
}


}// namespace App
}// namespace Droid
}// namespace Elastos
