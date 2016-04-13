
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/server/AppOpsPolicy.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/internal/utility/XmlUtils.h"

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IAppOpsManagerHelper;
using Elastos::Droid::App::CAppOpsManagerHelper;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::ICloseable;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Slogger;

using Org::Xmlpull::V1::IXmlPullParserFactory;
using Org::Xmlpull::V1::IXmlPullParserFactoryHelper;
using Org::Xmlpull::V1::CXmlPullParserFactoryHelper;

namespace Elastos {
namespace Droid {
namespace Server {

const String AppOpsPolicy::TAG("AppOpsPolicy");
const Boolean AppOpsPolicy::DEBUG = FALSE;

//=======================================================================
// AppOpsPolicy::PolicyPkg::
//=======================================================================

ECode AppOpsPolicy::PolicyPkg::constructor(
    /* [in] */ String packageName,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 show,
    /* [in] */ String type)
{
    SparseArray::constructor();

    mPackageName = packageName;
    mMode = mode;
    mShow = show;
    mType = type;
    return NOERROR;
}

ECode AppOpsPolicy::PolicyPkg::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = "PolicyPkg [packageName=";
    *result += mPackageName;
    *result += ", mode=";
    *result += mMode;
    *result += ", show=";
    *result += mShow;
    *result += ", type=";
    *result += mType;
    *result += "]";
    return NOERROR;
}

//=======================================================================
// AppOpsPolicy::PolicyOp::
//=======================================================================

AppOpsPolicy::PolicyOp::PolicyOp(
    /* [in] */ Int32 op,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 show)
{
    mOp = op;
    mMode = mode;
    mShow = show;
}

ECode AppOpsPolicy::PolicyOp::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = "PolicyOp [op=";
    *result += mOp;
    *result += ", mode=";
    *result += mMode;
    *result += ", show=";
    *result += mShow;
    *result += "]";
    return NOERROR;
}

//=======================================================================
// AppOpsPolicy::
//=======================================================================
CAR_INTERFACE_IMPL(AppOpsPolicy, Object, IAppOpsPolicy)

ECode AppOpsPolicy::constructor(
    /* [in] */ IFile* file,
    /* [in] */ IContext* context)
{
    mFile = file;
    mContext = context;
    CHashMap::New((IHashMap**)&mPolicy);
    return NOERROR;
}

Int32 AppOpsPolicy::StringToControl(
    /* [in] */ const String& show)
{
    if (show.EqualsIgnoreCase("true")) {
        return CONTROL_SHOW;
    }
    else if (show.EqualsIgnoreCase("false")) {
        return CONTROL_NOSHOW;
    }
    return CONTROL_UNKNOWN;
}

ECode AppOpsPolicy::ReadPolicy()
{
    AutoPtr<IFileInputStream> stream;
    synchronized (this) {
        ECode ec = CFileInputStream::New(mFile, (IFileInputStream**)&stream);
        if (FAILED(ec)) {
            Slogger::I(TAG, "App ops policy file (%s) not found; Skipping.", TO_CSTR(mFile));
            return NOERROR;
        }

        Boolean success = FALSE;
        // try {
        AutoPtr<IXmlPullParserFactoryHelper> hlp;
        CXmlPullParserFactoryHelper::AcquireSingleton((IXmlPullParserFactoryHelper**)&hlp);
        AutoPtr<IXmlPullParserFactory> fct;
        hlp->NewInstance((IXmlPullParserFactory**)&fct);
        AutoPtr<IXmlPullParser> parser;
        fct->NewPullParser((IXmlPullParser**)&parser);
        parser->SetInput(IInputStream::Probe(stream), String(NULL));
        Int32 type = 0;
        success = TRUE;
        while ((parser->Next(&type), type) != IXmlPullParser::START_TAG
                && type != IXmlPullParser::END_DOCUMENT) {
            ;
        }
        if (type != IXmlPullParser::START_TAG) {
            // throw new IllegalStateException("no start tag found");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        Int32 outerDepth = 0;
        parser->GetDepth(&outerDepth);
        Int32 dpt = 0;
        while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
                && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&dpt), dpt) > outerDepth)) {
            if (type == IXmlPullParser::END_TAG
                    || type == IXmlPullParser::TEXT) {
                continue;
            }

            String tagName;
            parser->GetName(&tagName);
            if (tagName.Equals("user-app")
                    || tagName.Equals("system-app")) {
                ReadDefaultPolicy(parser, tagName);
            }
            else if (tagName.Equals("application")) {
                ReadApplicationPolicy(parser);
            }
            else {
                Slogger::W(TAG, "Unknown element under <appops-policy>: ",
                        (const char*)tagName);
                XmlUtils::SkipCurrentTag(parser);
            }
        }
        // } catch (IllegalStateException e) {
        //     Slog.w(TAG, "Failed parsing " + e);
        // } catch (NullPointerException e) {
        //     Slog.w(TAG, "Failed parsing " + e);
        // } catch (NumberFormatException e) {
        //     Slog.w(TAG, "Failed parsing " + e);
        // } catch (XmlPullParserException e) {
        //     Slog.w(TAG, "Failed parsing " + e);
        // } catch (IOException e) {
        //     Slog.w(TAG, "Failed parsing " + e);
        // } catch (IndexOutOfBoundsException e) {
        //     Slog.w(TAG, "Failed parsing " + e);
        // } finally {
        if (!success) {
            mPolicy->Clear();
        }
            // try {
        ICloseable::Probe(stream)->Close();
            // } catch (IOException e) {
            // }
        // }
    }
    return NOERROR;
}

void AppOpsPolicy::ReadDefaultPolicy(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ String packageName)
{
    if (!packageName.EqualsIgnoreCase("user-app")
            && !packageName.EqualsIgnoreCase("system-app")) {
        return;
    }
    String perm;
    parser->GetAttributeValue(String(NULL), String("permission"), &perm);
    AutoPtr<IAppOpsManagerHelper> hlp;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&hlp);
    Int32 mode = 0;
    hlp->StringToMode(perm, &mode);
    String aVal;
    parser->GetAttributeValue(String(NULL), String("show"), &aVal);
    Int32 show = StringToControl(aVal);
    if (mode == IAppOpsManager::MODE_ERRORED && show == CONTROL_UNKNOWN) {
        return;
    }

    AutoPtr<IInterface> pPkg;
    mPolicy->Get(CoreUtils::Convert(packageName), (IInterface**)&pPkg);
    AutoPtr<PolicyPkg> pkg = (PolicyPkg*)IObject::Probe(pPkg);
    if (pkg == NULL) {
        pkg = new PolicyPkg();
        pkg->constructor(packageName, mode, show, packageName);
        mPolicy->Put(CoreUtils::Convert(packageName), ICloneable::Probe(pkg));
    }
    else {
        Slogger::W(TAG, "Duplicate policy found for package: %s of type: %s",
                (const char*)packageName, (const char*)packageName);
        pkg->mMode = mode;
        pkg->mShow = show;
    }
}

void AppOpsPolicy::ReadApplicationPolicy(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 outerDepth = 0;
    parser->GetDepth(&outerDepth);
    Int32 type = 0;
    Int32 dpt = 0;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&dpt), dpt) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }
        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals("pkg")) {
            ReadPkgPolicy(parser);
        }
        else {
            Slogger::W(TAG,
                    "Unknown element under <application>: ",
                            (const char*)tagName);
            XmlUtils::SkipCurrentTag(parser);
        }
    }
}

void AppOpsPolicy::ReadPkgPolicy(
    /* [in] */ IXmlPullParser* parser)
{
    String packageName;
    parser->GetAttributeValue(String(NULL), String("name"), (String*)&packageName);
    if (packageName.IsNull())
        return;
    String appType;
    parser->GetAttributeValue(String(NULL), String("type"), &appType);
    if (appType == NULL) {
        return;
    }

    String perm;
    parser->GetAttributeValue(String(NULL), String("permission"), &perm);
    AutoPtr<IAppOpsManagerHelper> hlp;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&hlp);
    Int32 mode = 0;
    hlp->StringToMode(perm, &mode);
    String strShow;
    parser->GetAttributeValue(String(NULL), String("show"), &strShow);
    Int32 show = StringToControl(strShow);
    String key = packageName + "." + appType;
    AutoPtr<IInterface> p;
    mPolicy->Get(CoreUtils::Convert(key), (IInterface**)&p);
    AutoPtr<PolicyPkg> pkg = (PolicyPkg*)IObject::Probe(p);
    if (pkg == NULL) {
        pkg = new PolicyPkg();
        pkg->constructor(packageName, mode, show, appType);
        mPolicy->Put(CoreUtils::Convert(key), ICloneable::Probe(pkg));
    }
    else {
        Slogger::W(TAG, "Duplicate policy found for package: %s of type: %s",
                (const char*)packageName, (const char*)appType);
        pkg->mMode = mode;
        pkg->mShow = show;
    }

    Int32 outerDepth = 0;
    parser->GetDepth(&outerDepth);
    Int32 type = 0;
    Int32 dpt = 0;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && (type != IXmlPullParser::END_TAG || (parser->GetDepth(&dpt), dpt) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }
        String tagName;
        parser->GetName(&tagName);
        if (tagName.Equals("op")) {
            ReadOpPolicy(parser, pkg);
        }
        else {
            Slogger::W(TAG, "Unknown element under <pkg>: ", (const char*)tagName);
            XmlUtils::SkipCurrentTag(parser);
        }
    }
}

void AppOpsPolicy::ReadOpPolicy(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ PolicyPkg* pkg)
{
    if (pkg == NULL) {
        return;
    }
    String opName;
    parser->GetAttributeValue(String(NULL), String("name"), &opName);
    if (opName.IsNull()) {
        Slogger::W(TAG, "Op name is null");
        return;
    }
    AutoPtr<IAppOpsManagerHelper> hlp;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&hlp);
    Int32 code = 0;
    hlp->StringOpToOp(opName, &code);
    if (code == IAppOpsManager::OP_NONE) {
        Slogger::W(TAG, "Unknown Op: %s", (const char*)opName);
        return;
    }
    String perm;
    parser->GetAttributeValue(String(NULL), String("permission"), &perm);
    Int32 mode = 0;
    hlp->StringToMode(perm, &mode);
    String strShow;
    parser->GetAttributeValue(String(NULL), String("show"), &strShow);
    Int32 show = StringToControl(strShow);
    if (mode == IAppOpsManager::MODE_ERRORED && show == CONTROL_UNKNOWN) {
        return;
    }
    AutoPtr<IInterface> p;
    pkg->Get(code, (IInterface**)&p);
    AutoPtr<PolicyOp> op = (PolicyOp*)IObject::Probe(p);
    if (op == NULL) {
        op = new PolicyOp(code, mode, show);
        pkg->Put(code, (IInterface*)(IObject*)op.Get());
    }
    else {
        Slogger::W(TAG, "Duplicate policy found for package: %s type: %s op: %d",
            pkg->mPackageName.string(), pkg->mType.string(), op->mOp);
        op->mMode = mode;
        op->mShow = show;
    }
}

ECode AppOpsPolicy::DebugPoilcy()
{
    AutoPtr<ISet> s;
    mPolicy->GetEntrySet((ISet**)&s);
    AutoPtr<IIterator> iterator;
    s->GetIterator((IIterator**)&iterator);
    Boolean bHasNxt = FALSE;
    while ((iterator->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> nxt;
        iterator->GetNext((IInterface**)&nxt);
        AutoPtr<IMapEntry> pNxt = IMapEntry::Probe(nxt);
        AutoPtr<IInterface> pKey;
        pNxt->GetKey((IInterface**)&pKey);
        String key;
        ICharSequence::Probe(pKey)->ToString(&key);
        if (DEBUG) {
            Slogger::D(TAG, "Key: %s", key.string());
        }
        AutoPtr<IInterface> pPkg;
        mPolicy->Get(pKey, (IInterface**)&pPkg);
        AutoPtr<PolicyPkg> pkg = (PolicyPkg*)IObject::Probe(pPkg);
        if (pkg == NULL) {
            if (DEBUG) {
                Slogger::D(TAG, "Pkg is NULL for key: %s", key.string());
            }
            continue;
        }
        if (DEBUG) {
            String str;
            pkg->ToString(&str);
            Slogger::D(TAG, str);
        }
        Int32 size = 0;
        pkg->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> val;
            pkg->ValueAt(i, (IInterface**)&val);
            AutoPtr<PolicyOp> op = (PolicyOp*)IObject::Probe(val);
            if (DEBUG) {
                String str;
                op->ToString(&str);
                Slogger::D(TAG, str);
            }
        }
    }
    return NOERROR;
}

String AppOpsPolicy::GetAppType(
    /* [in] */ String packageName)
{
    String appType(NULL);
    AutoPtr<IApplicationInfo> appInfo;
    if (mContext != NULL) {
        // try {
        AutoPtr<IPackageManager> pm;
        mContext->GetPackageManager((IPackageManager**)&pm);
        pm->GetApplicationInfo(
                packageName, 0, (IApplicationInfo**)&appInfo);
        // } catch (NameNotFoundException e) {
        //     appInfo = NULL;
        // }
        if (appInfo != NULL) {
            Int32 flags = 0;
            appInfo->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
                appType = "system-app";
            }
            else {
                appType = "user-app";
            }
        }
    }
    else {
        Slogger::E(TAG, "Context is null");
    }
    return appType;
}

ECode AppOpsPolicy::IsControlAllowed(
    /* [in] */ Int32 code,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean isShow = TRUE;
    Int32 show = CONTROL_UNKNOWN;
    AutoPtr<PolicyPkg> pkg;
    String key;
    String type;

    if (mPolicy == NULL) {
        *result = isShow;
        return NOERROR;
    }

    type = GetAppType(packageName);
    if (!type.IsNull()) {
        key = type;
        AutoPtr<IInterface> p;
        mPolicy->Get(CoreUtils::Convert(key), (IInterface**)&p);
        pkg = (PolicyPkg*)IObject::Probe(p);
        if (pkg != NULL && pkg->mShow != CONTROL_UNKNOWN) {
            show = pkg->mShow;
        }
    }
    key = packageName;
    if (!type.IsNull()) {
        key += ".";
        key += type;
    }
    AutoPtr<IInterface> pVal;
    mPolicy->Get(CoreUtils::Convert(key), (IInterface**)&pVal);
    pkg = (PolicyPkg*)IObject::Probe(pVal);
    if (pkg != NULL) {
        if (pkg->mShow != CONTROL_UNKNOWN) {
            show = pkg->mShow;
        }
        AutoPtr<IInterface> pOp;
        pkg->Get(code, (IInterface**)&pOp);
        AutoPtr<PolicyOp> op = (PolicyOp*)IObject::Probe(pOp);
        if (op != NULL) {
            if (op->mShow != CONTROL_UNKNOWN) {
                show = op->mShow;
            }
        }
    }
    if (show == CONTROL_NOSHOW) {
        isShow = FALSE;
    }
    *result = isShow;
    return NOERROR;
}

ECode AppOpsPolicy::GetDefualtMode(
    /* [in] */ Int32 code,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 mode = IAppOpsManager::MODE_ERRORED;
    AutoPtr<PolicyPkg> pkg;
    String key;
    String type;

    if (mPolicy == NULL) {
        *result = mode;
        return NOERROR;
    }
    if (DEBUG){
        Slogger::D(TAG, "Default mode requested for op=%d package=%s",
            code, packageName.string());
    }
    type = GetAppType(packageName);
    if (!type.IsNull()) {
        // Get value based on 'type'
        key = type;
        AutoPtr<IInterface> p;
        mPolicy->Get(CoreUtils::Convert(key), (IInterface**)&p);
        pkg = (PolicyPkg*)IObject::Probe(p);
        if (pkg != NULL && pkg->mMode != IAppOpsManager::MODE_ERRORED) {
            if (DEBUG) {
                Slogger::D(TAG, "Setting value based on type: %p", pkg.Get());
            }
            mode = pkg->mMode;
        }
    }
    // Get value based on 'pkg'.
    key = packageName;
    if (!type.IsNull()) {
        key += ".";
        key += type;
    }
    AutoPtr<IInterface> pVal;
    mPolicy->Get(CoreUtils::Convert(key), (IInterface**)&pVal);
    pkg = (PolicyPkg*)IObject::Probe(pVal);
    if (pkg != NULL) {
        if (pkg->mMode != IAppOpsManager::MODE_ERRORED) {
            if (DEBUG) {
                Slogger::D(TAG, "Setting value based on packageName: %p", pkg.Get());
            }
            mode = pkg->mMode;
        }
        // Get value base on 'op'
        AutoPtr<IInterface> pOp;
        pkg->Get(code, (IInterface**)&pOp);
        AutoPtr<PolicyOp> op = (PolicyOp*)IObject::Probe(pOp);
        if (op != NULL) {
            if (op->mMode != IAppOpsManager::MODE_ERRORED) {
                if (DEBUG) {
                    Slogger::D(TAG, "Setting value based on op: %p", op.Get());
                }
                mode = op->mMode;
            }
        }
    }
    if (DEBUG) {
        Slogger::D(TAG, "Returning mode=%d", mode);
    }
    *result = mode;
    return NOERROR;
}

}// Server
}// Droid
}// Elastos
