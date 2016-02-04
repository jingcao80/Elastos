
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/ActivityInfo.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Os::Build;
using Elastos::Droid::Content::Res::CConfiguration;

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

static AutoPtr< ArrayOf<Int32> > Init_CONFIG_NATIVE_BITS()
{
    AutoPtr< ArrayOf<Int32> > int32Array = ArrayOf<Int32>::Alloc(14);
    (*int32Array)[0] = CConfiguration::NATIVE_CONFIG_MNC;//,                    // MNC
    (*int32Array)[1] = CConfiguration::NATIVE_CONFIG_MCC;//,                    // MCC
    (*int32Array)[2] = CConfiguration::NATIVE_CONFIG_LOCALE;//,                 // LOCALE
    (*int32Array)[3] = CConfiguration::NATIVE_CONFIG_TOUCHSCREEN;//,            // TOUCH SCREEN
    (*int32Array)[4] = CConfiguration::NATIVE_CONFIG_KEYBOARD;//,               // KEYBOARD
    (*int32Array)[5] = CConfiguration::NATIVE_CONFIG_KEYBOARD_HIDDEN;//,        // KEYBOARD HIDDEN
    (*int32Array)[6] = CConfiguration::NATIVE_CONFIG_NAVIGATION;//,             // NAVIGATION
    (*int32Array)[7] = CConfiguration::NATIVE_CONFIG_ORIENTATION;//,            // ORIENTATION
    (*int32Array)[8] = CConfiguration::NATIVE_CONFIG_SCREEN_LAYOUT;//,          // SCREEN LAYOUT
    (*int32Array)[9] = CConfiguration::NATIVE_CONFIG_UI_MODE;//,                // UI MODE
    (*int32Array)[10] = CConfiguration::NATIVE_CONFIG_SCREEN_SIZE;//,            // SCREEN SIZE
    (*int32Array)[11] = CConfiguration::NATIVE_CONFIG_SMALLEST_SCREEN_SIZE,   // SMALLEST SCREEN SIZE
    (*int32Array)[12] = CConfiguration::NATIVE_CONFIG_DENSITY;//,                // DENSITY
    (*int32Array)[13] = CConfiguration::NATIVE_CONFIG_LAYOUTDIR;//,              // LAYOUT DIRECTION
    return int32Array;
}

INIT_PROI_3 AutoPtr< ArrayOf<Int32> > ActivityInfo::CONFIG_NATIVE_BITS = Init_CONFIG_NATIVE_BITS();

CAR_INTERFACE_IMPL_2(ActivityInfo, ComponentInfo, IActivityInfo, IParcelable)

ActivityInfo::ActivityInfo()
    : mTheme(0)
    , mLaunchMode(0)
    , mDocumentLaunchMode(0)
    , mPersistableMode(0)
    , mMaxRecents(0)
    , mFlags(0)
    , mScreenOrientation(SCREEN_ORIENTATION_UNSPECIFIED)
    , mConfigChanges(0)
    , mSoftInputMode(0)
    , mUiOptions(0)
{}

ActivityInfo::~ActivityInfo()
{}

ECode ActivityInfo::constructor()
{
    return ComponentInfo::constructor();
}

ECode ActivityInfo::constructor(
    /* [in] */ IActivityInfo* origObj)
{
    VALIDATE_NOT_NULL(origObj)
    FAIL_RETURN(ComponentInfo::constructor(IComponentInfo::Probe(origObj)))

    ActivityInfo* orig = (ActivityInfo*)origObj;
    mTheme = orig->mTheme;
    mLaunchMode = orig->mLaunchMode;
    mPermission = orig->mPermission;
    mTaskAffinity = orig->mTaskAffinity;
    mTargetActivity = orig->mTargetActivity;
    mFlags = orig->mFlags;
    mScreenOrientation = orig->mScreenOrientation;
    mConfigChanges = orig->mConfigChanges;
    mSoftInputMode = orig->mSoftInputMode;
    mUiOptions = orig->mUiOptions;
    mParentActivityName = orig->mParentActivityName;
    mMaxRecents = orig->mMaxRecents;
    return NOERROR;
}

Int32 ActivityInfo::ActivityInfoConfigToNative(
    /* [in] */ Int32 input)
{
    Int32 output = 0;
    for (Int32 i = 0; i < CONFIG_NATIVE_BITS->GetLength(); i++) {
        if ((input & (1 << i)) != 0) {
            output |= (*CONFIG_NATIVE_BITS)[i];
        }
    }
    return output;
}

ECode ActivityInfo::GetRealConfigChanged(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 targetSdkVersion = 0;
    mApplicationInfo->GetTargetSdkVersion(&targetSdkVersion);
    if(targetSdkVersion < Build::VERSION_CODES::HONEYCOMB_MR2) {
        *value = (mConfigChanges | IActivityInfo::CONFIG_SCREEN_SIZE
                        | IActivityInfo::CONFIG_SMALLEST_SCREEN_SIZE);
    }
    else {
        *value = mConfigChanges;
    }

    return NOERROR;
}

ECode ActivityInfo::GetThemeResource(
    /* [out] */ Int32* theme)
{
    VALIDATE_NOT_NULL(theme);

    if (mTheme !=0) {
        *theme = mTheme;
    }
    else {
        mApplicationInfo->GetTheme(theme);
    }
    return NOERROR;
}

String ActivityInfo::PersistableModeToString()
{
    switch(mPersistableMode) {
        case PERSIST_ROOT_ONLY: return String("PERSIST_ROOT_ONLY");
        case PERSIST_NEVER: return String("PERSIST_NEVER");
        case PERSIST_ACROSS_REBOOTS: return String("PERSIST_ACROSS_REBOOTS");
        default: {
            StringBuilder sb("UNKNOWN");
            sb += mPersistableMode;
            return sb.ToString();
        }
    }
}

ECode ActivityInfo::Dump(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    // super.dumpFront(pw, prefix);
    // if (permission != null) {
    //     pw.println(prefix + "permission=" + permission);
    // }
    // pw.println(prefix + "taskAffinity=" + taskAffinity
    //         + " targetActivity=" + targetActivity);
    // if (launchMode != 0 || flags != 0 || theme != 0) {
    //     pw.println(prefix + "launchMode=" + launchMode
    //             + " flags=0x" + Integer.toHexString(flags)
    //             + " theme=0x" + Integer.toHexString(theme));
    // }
    // if (screenOrientation != SCREEN_ORIENTATION_UNSPECIFIED
    //         || configChanges != 0 || softInputMode != 0) {
    //     pw.println(prefix + "screenOrientation=" + screenOrientation
    //             + " configChanges=0x" + Integer.toHexString(configChanges)
    //             + " softInputMode=0x" + Integer.toHexString(softInputMode));
    // }
    // if (uiOptions != 0) {
    //     pw.println(prefix + " uiOptions=0x" + Integer.toHexString(uiOptions));
    // }
    // super.dumpBack(pw, prefix);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode ActivityInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    Int32 hashCode;
    THIS_PROBE(IObject)->GetHashCode(&hashCode);
    str->AppendFormat("ActivityInfo{%s %s}", StringUtils::ToHexString(hashCode).string(), mName.string());
    return NOERROR;
}

ECode ActivityInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(source != NULL);

    ComponentInfo::ReadFromParcel(source);
    source->ReadInt32(&mTheme);
    source->ReadInt32(&mLaunchMode);
    source->ReadString(&mPermission);
    source->ReadString(&mTaskAffinity);
    source->ReadString(&mTargetActivity);
    source->ReadInt32(&mFlags);
    source->ReadInt32(&mScreenOrientation);
    source->ReadInt32(&mConfigChanges);
    source->ReadInt32(&mSoftInputMode);
    source->ReadInt32(&mUiOptions);
    source->ReadString(&mParentActivityName);
    source->ReadInt32(&mPersistableMode);
    source->ReadInt32(&mMaxRecents);
    return NOERROR;
}

ECode ActivityInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    ComponentInfo::WriteToParcel(dest);
    dest->WriteInt32(mTheme);
    dest->WriteInt32(mLaunchMode);
    dest->WriteString(mPermission);
    dest->WriteString(mTaskAffinity);
    dest->WriteString(mTargetActivity);
    dest->WriteInt32(mFlags);
    dest->WriteInt32(mScreenOrientation);
    dest->WriteInt32(mConfigChanges);
    dest->WriteInt32(mSoftInputMode);
    dest->WriteInt32(mUiOptions);
    dest->WriteString(mParentActivityName);
    dest->WriteInt32(mPersistableMode);
    dest->WriteInt32(mMaxRecents);
    return NOERROR;
}

ECode ActivityInfo::GetTheme(
    /* [out] */ Int32* theme)
{
    VALIDATE_NOT_NULL(theme);
    *theme = mTheme;
    return NOERROR;
}

ECode ActivityInfo::SetTheme(
    /* [in] */ Int32 theme)
{
    mTheme = theme;
    return NOERROR;
}

ECode ActivityInfo::GetLaunchMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mLaunchMode;
    return NOERROR;
}

ECode ActivityInfo::SetLaunchMode(
    /* [in] */ Int32 mode)
{
    mLaunchMode = mode;
    return NOERROR;
}

ECode ActivityInfo::GetDocumentLaunchMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mDocumentLaunchMode;
    return NOERROR;
}

ECode ActivityInfo::SetDocumentLaunchMode(
    /* [in] */ Int32 mode)
{
    mDocumentLaunchMode = mode;
    return NOERROR;
}

ECode ActivityInfo::GetPersistableMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mPersistableMode;
    return NOERROR;
}

ECode ActivityInfo::SetPersistableMode(
    /* [in] */ Int32 mode)
{
    mPersistableMode = mode;
    return NOERROR;
}

ECode ActivityInfo::GetMaxRecents(
    /* [out] */ Int32* max)
{
    VALIDATE_NOT_NULL(max);
    *max = mMaxRecents;
    return NOERROR;
}

ECode ActivityInfo::SetMaxRecents(
    /* [in] */ Int32 max)
{
    mMaxRecents = max;
    return NOERROR;
}

ECode ActivityInfo::GetPermission(
    /* [out] */ String* permission)
{
    VALIDATE_NOT_NULL(permission);
    *permission = mPermission;
    return NOERROR;
}

ECode ActivityInfo::SetPermission(
    /* [in] */ const String& permission)
{
    mPermission = permission;
    return NOERROR;
}

ECode ActivityInfo::GetTaskAffinity(
    /* [out] */ String* task)
{
    VALIDATE_NOT_NULL(task);
    *task = mTaskAffinity;
    return NOERROR;
}

ECode ActivityInfo::SetTaskAffinity(
    /* [in] */ const String& task)
{
    mTaskAffinity = task;
    return NOERROR;
}

ECode ActivityInfo::GetTargetActivity(
    /* [out] */ String* target)
{
    VALIDATE_NOT_NULL(target);
    *target = mTargetActivity;
    return NOERROR;
}

ECode ActivityInfo::SetTargetActivity(
    /* [in] */ const String& target)
{
    mTargetActivity = target;
    return NOERROR;
}

ECode ActivityInfo::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;
    return NOERROR;
}

ECode ActivityInfo::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode ActivityInfo::GetScreenOrientation(
    /* [out] */ Int32* orientation)
{
    VALIDATE_NOT_NULL(orientation);
    *orientation = mScreenOrientation;
    return NOERROR;
}

ECode ActivityInfo::SetScreenOrientation(
    /* [in] */ Int32 orientation)
{
    mScreenOrientation = orientation;
    return NOERROR;
}

ECode ActivityInfo::GetConfigChanges(
    /* [out] */ Int32* changes)
{
    VALIDATE_NOT_NULL(changes);
    *changes = mConfigChanges;
    return NOERROR;
}

ECode ActivityInfo::SetConfigChanges(
    /* [in] */ Int32 changes)
{
    mConfigChanges = changes;
    return NOERROR;
}

ECode ActivityInfo::GetSoftInputMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mSoftInputMode;
    return NOERROR;
}

ECode ActivityInfo::SetSoftInputMode(
    /* [in] */ Int32 mode)
{
    mSoftInputMode = mode;
    return NOERROR;
}

ECode ActivityInfo::GetUiOptions(
    /* [out] */ Int32* uiOptions)
{
    VALIDATE_NOT_NULL(uiOptions);
    *uiOptions = mUiOptions;
    return NOERROR;
}

ECode ActivityInfo::SetUiOptions(
    /* [in] */ Int32 uiOptions)
{
    mUiOptions = uiOptions;
    return NOERROR;
}

ECode ActivityInfo::GetParentActivityName(
    /* [out] */ String* parentActivityName)
{
    VALIDATE_NOT_NULL(parentActivityName);
    *parentActivityName = mParentActivityName;
    return NOERROR;
}

ECode ActivityInfo::SetParentActivityName(
    /* [in] */ const String& parentActivityName)
{
    mParentActivityName = parentActivityName;
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
