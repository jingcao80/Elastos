
#ifndef __ELASTOS_DROID_SERVER_AM_CPENDINGINTENTRECORD_H__
#define __ELASTOS_DROID_SERVER_AM_CPENDINGINTENTRECORD_H__

#include "_Elastos_Droid_Server_Am_CPendingIntentRecord.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IIActivityContainer;
using Elastos::Droid::Content::IIIntentSender;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ActivityRecord;
class CActivityManagerService;

CarClass(CPendingIntentRecord)
    , public Object
    , public IPendingIntentRecord
    , public IIIntentSender
    , public IBinder
{
public:
    class Key : public Object
    {
    public:
        Key(
            /* [in] */ Int32 t,
            /* [in] */ const String& p,
            /* [in] */ ActivityRecord * a,
            /* [in] */ const String& w,
            /* [in] */ Int32 r,
            /* [in] */ ArrayOf<IIntent*>* i,
            /* [in] */ ArrayOf<String>* it,
            /* [in] */ Int32 f,
            /* [in] */ IBundle* o,
            /* [in] */ Int32 userId);

        ~Key();

        CARAPI Equals(
            /* [in] */ IInterface* other,
            /* [out] */ Boolean* result);

        CARAPI_(Boolean) Equals(
            /* [in] */ Key* otherObj);

        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI_(String) ToString();

        CARAPI_(String) TypeName();

        inline Boolean operator == (Key* other)
        {
            return this->Equals(other);
        }

    public:
        static const Int32 ODD_PRIME_NUMBER = 37;

        Int32 mType;
        String mPackageName;
        ActivityRecord* mActivity; // weak-ref
        String mWho;
        Int32 mRequestCode;
        AutoPtr<IIntent> mRequestIntent;
        String mRequestResolvedType;
        AutoPtr<IBundle> mOptions;
        AutoPtr< ArrayOf<IIntent*> > mAllIntents;
        AutoPtr< ArrayOf<String> > mAllResolvedTypes;
        Int32 mFlags;
        Int32 mHashCode;
        Int32 mUserId;
    };

public:
    CPendingIntentRecord();

    ~CPendingIntentRecord();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle32 owner,
        /* [in] */ Handle32 k,
        /* [in] */ Int32 u);

    CARAPI Send(
        /* [in] */ Int32 code,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IIntentReceiver* finishedReceiver,
        /* [in] */ const String& requiredPermission,
        /* [out] */ Int32* result);

    CARAPI CompleteFinalize();

    void Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    CARAPI_(Int32) SendInner(
        /* [in] */ Int32 code,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IIntentReceiver* finishedReceiver,
        /* [in] */ const String& requiredPermission,
        /* [in] */ IBinder* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 flagsMask,
        /* [in] */ Int32 flagsValues,
        /* [in] */ IBundle* options,
        /* [in] */ IIActivityContainer* container);

public:
    CActivityManagerService* mOwner;
    AutoPtr<Key> mKey;
    Int32 mUid;
    AutoPtr<IWeakReference> mRef;
    Boolean mSent;
    Boolean mCanceled;

    String mStringName;
    String mLastTagPrefix;
    String mLastTag;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Am::CPendingIntentRecord)
DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Am::CPendingIntentRecord::Key)

#endif //__ELASTOS_DROID_SERVER_AM_CPENDINGINTENTRECORD_H__
