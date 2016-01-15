#ifndef __ELASTOS_UTILITY_PREFS_FILEPREFERENCESFACTORYIMPL_H__
#define __ELASTOS_UTILITY_PREFS_FILEPREFERENCESFACTORYIMPL_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"

namespace Elastos {
namespace Utility {
namespace Prefs {

class FilePreferencesFactoryImpl
    : public Object
    , public IPreferencesFactory
{
public:
    CAR_INTERFACE_DECL();

    FilePreferencesFactoryImpl();

    CARAPI UserRoot(
        /* [out] */ IPreferences** outpre);

    CARAPI SystemRoot(
        /* [out] */ IPreferences** outpre);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    static CARAPI_(Boolean) InitStatic();

private:
    static Boolean sInit;
    //  user root preferences
    static AutoPtr<IPreferences> USER_ROOT;

    //  system root preferences
    static AutoPtr<IPreferences> SYSTEM_ROOT;

};

} // namespace Prefs
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__PREFS_FILEPREFERENCESFACTORYIMPL_H__
