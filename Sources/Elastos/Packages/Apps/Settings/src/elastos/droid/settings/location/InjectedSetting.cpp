
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/settings/location/InjectedSetting.h"
#include "elastos/droid/settings/location/SettingsInjector.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

//===============================================================================
//                  InjectedSetting
//===============================================================================

CAR_INTERFACE_IMPL(InjectedSetting, Object, IInjectedSetting)

InjectedSetting::InjectedSetting(
    /* [in] */ const String& packageName,
    /* [in] */ const String& className,
    /* [in] */ const String& title,
    /* [in] */ Int32 iconId,
    /* [in] */ const String& settingsActivity)
{
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    preconditions->CheckNotNull(CoreUtils::Convert(packageName), String("packageName"));
    mPackageName = packageName;
    preconditions->CheckNotNull(CoreUtils::Convert(className), String("className"));
    mClassName = className;
    preconditions->CheckNotNull(CoreUtils::Convert(title), String("title"));
    mTitle = title;
    mIconId = iconId;
    preconditions->CheckNotNull(CoreUtils::Convert(settingsActivity));
    mSettingsActivity = settingsActivity;
}

AutoPtr<InjectedSetting> InjectedSetting::NewInstance(
    /* [in] */ const String& packageName,
    /* [in] */ const String& className,
    /* [in] */ const String& title,
    /* [in] */ Int32 iconId,
    /* [in] */ const String& settingsActivity)
{
    if (packageName.IsNull() || className.IsNull() ||
            TextUtils::IsEmpty(title) || TextUtils::IsEmpty(settingsActivity)) {
        if (Logger::IsLoggable(SettingsInjector::TAG, Logger::WARN)) {
            Logger::W(SettingsInjector::TAG, "Illegal setting specification: package=%s, class=%s, title=%s, settingsActivity=%s",
                    packageName.string(), className.string(), title.string(), settingsActivity.string());
        }
        return NULL;
    }
    AutoPtr<InjectedSetting> is = new InjectedSetting(packageName, className, title, iconId, settingsActivity);
    return is;
}

ECode InjectedSetting::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder builder("");
    builder += "InjectedSetting{";
    builder += "mPackageName='";
    builder += mPackageName;
    builder += '\'';
    builder += ", mClassName='";
    builder += mClassName;
    builder += '\'';
    builder += ", label=";
    builder += mTitle;
    builder += ", iconId=";
    builder += mIconId;
    builder += ", settingsActivity='";
    builder += mSettingsActivity;
    builder += '\'';
    builder += '}';
    *str = builder.ToString();
    return NOERROR;
}

AutoPtr<IIntent> InjectedSetting::GetServiceIntent()
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetClassName(mPackageName, mClassName);
    return intent;
}

ECode InjectedSetting::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (TO_IINTERFACE(this) == IInterface::Probe(o)) {
        *result = TRUE;
        return NOERROR;
    }
    if (IInjectedSetting::Probe(o) == NULL) return NOERROR;

    InjectedSetting* that = (InjectedSetting*)IInjectedSetting::Probe(o);

    *result = mPackageName.Equals(that->mPackageName) && mClassName.Equals(that->mClassName)
            && mTitle.Equals(that->mTitle) && mIconId == that->mIconId
            && mSettingsActivity.Equals(that->mSettingsActivity);
    return NOERROR;
}

ECode InjectedSetting::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code)

    Int32 result = mPackageName.GetHashCode();
    result = 31 * result + mClassName.GetHashCode();
    result = 31 * result + mTitle.GetHashCode();
    result = 31 * result + mIconId;
    result = 31 * result + mSettingsActivity.GetHashCode();
    *code = result;
    return NOERROR;
}

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos