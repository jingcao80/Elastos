#ifndef __ELASTOS_DROID_SERVER_BRICKRECEIVER_H__
#define __ELASTOS_DROID_SERVER_BRICKRECEIVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>
#include <Elastos.CoreLibrary.External.h>
#include "elastos/droid/utility/SparseArray.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::SparseArray;
using Elastos::Core::Object;
using Elastos::IO::IFile;
using Elastos::Utility::IHashMap;

using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {

class AppOpsPolicy
    : public Object
    , public IAppOpsPolicy
{
public:
    class PolicyPkg
        : public SparseArray
    {
    public:
        CARAPI constructor(
            /* [in] */ String packageName,
            /* [in] */ Int32 mode,
            /* [in] */ Int32 show,
            /* [in] */ String type);

        //@Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        String mPackageName;
        Int32 mMode;
        Int32 mShow;
        String mType;
    };

    class PolicyOp
        : public Object
    {
    public:
        PolicyOp(
            /* [in] */ Int32 op,
            /* [in] */ Int32 mode,
            /* [in] */ Int32 show);

        //@Override
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        Int32 mOp;
        Int32 mMode;
        Int32 mShow;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IFile* file,
        /* [in] */ IContext* context);

    CARAPI ReadPolicy();

    CARAPI DebugPoilcy();

    CARAPI IsControlAllowed(
        /* [in] */ Int32 code,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI GetDefualtMode(
        /* [in] */ Int32 code,
        /* [in] */ const String& packageName,
        /* [out] */ Int32* result);

    static CARAPI_(Int32) StringToControl(
        /* [in] */ const String& show);

private:
    CARAPI_(void) ReadDefaultPolicy(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ String packageName);

    CARAPI_(void) ReadApplicationPolicy(
        /* [in] */ IXmlPullParser* parser);

    CARAPI_(void) ReadPkgPolicy(
        /* [in] */ IXmlPullParser* parser);

    CARAPI_(void) ReadOpPolicy(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ PolicyPkg* pkg);

    CARAPI_(String) GetAppType(
        /* [in] */ String packageName);

public:
    static const String TAG;
    static const Boolean DEBUG;
    AutoPtr<IFile> mFile;
    AutoPtr<IContext> mContext;

    AutoPtr<IHashMap> mPolicy;
};

} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_BRICKRECEIVER_H__
