#ifndef __ELASTOS_DROID_APP_BACKUP_CRESTORESET_H__
#define __ELASTOS_DROID_APP_BACKUP_CRESTORESET_H__

#include "_Elastos_Droid_App_Backup_CRestoreSet.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CarClass(CRestoreSet)
    , public Object
    , public IRestoreSet
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CRestoreSet();

    ~CRestoreSet();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String & name,
        /* [in] */ const String & dev,
        /* [in] */ Int64 token);

    /**
     * Name of this restore set.  May be user generated, may simply be the name
     * of the handset model, e.g. "T-Mobile G1".
     */
    CARAPI GetName(
        /* [out] */ String * name);

    CARAPI SetName(
        /* [in] */ const String& name);

    /**
     * Identifier of the device whose data this is.  This will be as unique as
     * is practically possible; for example, it might be an IMEI.
     */
    CARAPI GetDevice(
        /* [out] */ String * device);

    CARAPI SetDevice(
        /* [in] */ const String& device);

    /**
     * Token that identifies this backup set unambiguously to the backup/restore
     * transport.  This is guaranteed to be valid for the duration of a restore
     * session, but is meaningless once the session has ended.
     */
    CARAPI GetToken(
        /* [out] */ Int64 * token);

    CARAPI SetToken(
        /* [in] */ Int64 token);

public:
    /**
     * Name of this restore set.  May be user generated, may simply be the name
     * of the handset model, e.g. "T-Mobile G1".
     */
    String mName;

    /**
     * Identifier of the device whose data this is.  This will be as unique as
     * is practically possible; for example, it might be an IMEI.
     */
    String mDevice;

    /**
     * Token that identifies this backup set unambiguously to the backup/restore
     * transport.  This is guaranteed to be valid for the duration of a restore
     * session, but is meaningless once the session has ended.
     */
    Int64 mToken;
};

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_BACKUP_CRESTORESET_H__
