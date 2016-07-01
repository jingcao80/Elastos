#ifndef __ELASTOS_APPS_CONTACTS_COMMON_DATABASE_CONTACTUPDATEUTILS_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_DATABASE_CONTACTUPDATEUTILS_H__

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Database {

/**
 * Static methods to update contact information.
 */
class ContactUpdateUtils
    : public Object
{
public:
    static CARAPI_(void) SetSuperPrimary(
        /* [in] */ IContext* context,
        /* [in] */ Int64 dataId);

private:
    static const String TAG; // = ContactUpdateUtils.class.getSimpleName();
};

} // Database
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_DATABASE_CONTACTUPDATEUTILS_H__
