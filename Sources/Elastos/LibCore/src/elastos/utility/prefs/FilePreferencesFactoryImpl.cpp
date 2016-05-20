
#include "FilePreferencesFactoryImpl.h"
#include "FilePreferencesImpl.h"
#include "CSystem.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;

namespace Elastos {
namespace Utility {
namespace Prefs {

//TODO:
Boolean FilePreferencesFactoryImpl::sInit;// = FilePreferencesFactoryImpl::InitStatic();
AutoPtr<IPreferences> FilePreferencesFactoryImpl::USER_ROOT;
AutoPtr<IPreferences> FilePreferencesFactoryImpl::SYSTEM_ROOT;

CAR_INTERFACE_IMPL(FilePreferencesFactoryImpl, Object, IPreferencesFactory)

Boolean FilePreferencesFactoryImpl::InitStatic()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String property;
    system->GetProperty(String("user.home"), &property);
    USER_ROOT = new FilePreferencesImpl(property + String("/.java/.userPrefs"), TRUE);

    system->GetProperty(String("java.home"), &property);
    SYSTEM_ROOT = new FilePreferencesImpl(property + String("/.systemPrefs"), FALSE);
    return TRUE;
}

FilePreferencesFactoryImpl::FilePreferencesFactoryImpl()
{
}

ECode FilePreferencesFactoryImpl::UserRoot(
    /* [out] */ IPreferences** outpre)
{
    VALIDATE_NOT_NULL(outpre);
    *outpre = USER_ROOT;
    REFCOUNT_ADD(*outpre);
    return NOERROR;
}

ECode FilePreferencesFactoryImpl::SystemRoot(
    /* [out] */ IPreferences** outpre)
{
    VALIDATE_NOT_NULL(outpre);
    *outpre = SYSTEM_ROOT;
    REFCOUNT_ADD(*outpre);
    return NOERROR;
}

ECode FilePreferencesFactoryImpl::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("Elastos.Utility.Prefs.FilePreferencesFactoryImpl");
    return NOERROR;
}

} // namespace Prefs
} // namespace Utility
} // namespace Elastos
