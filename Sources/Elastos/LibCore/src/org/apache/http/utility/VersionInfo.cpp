
#include "Elastos.CoreLibrary.IO.h"
#include "org/apache/http/utility/VersionInfo.h"
#include "elastos/core/CString.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/core/Thread.h"
#include "elastos/utility/CProperties.h"
#include "elastos/utility/CArrayList.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::IO::IInputStream;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::IProperties;
using Elastos::Utility::CProperties;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Utility {

CAR_INTERFACE_IMPL(VersionInfo, Object, IVersionInfo)

VersionInfo::VersionInfo(
    /* [in] */ const String& pckg,
    /* [in] */ const String& module,
    /* [in] */ const String& release,
    /* [in] */ const String& time,
    /* [in] */ const String& clsldr)
{
    Init(pckg, module, release, time, clsldr);
}

ECode VersionInfo::Init(
    /* [in] */ const String& pckg,
    /* [in] */ const String& module,
    /* [in] */ const String& release,
    /* [in] */ const String& time,
    /* [in] */ const String& clsldr)
{
    if (pckg.IsNull()) {
        Logger::E("VersionInfo", "Package identifier must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mInfoPackage = pckg;
    mInfoModule = !module.IsNull() ? module  : UNAVAILABLE;
    mInfoRelease = !release.IsNull() ? release : UNAVAILABLE;
    mInfoTimestamp = !time.IsNull() ? time    : UNAVAILABLE;
    mInfoClassloader = !clsldr.IsNull() ? clsldr  : UNAVAILABLE;
    return NOERROR;
}

ECode VersionInfo::GetPackage(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mInfoPackage;
    return NOERROR;
}

ECode VersionInfo::GetModule(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mInfoModule;
    return NOERROR;
}

ECode VersionInfo::GetRelease(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mInfoRelease;
    return NOERROR;
}

ECode VersionInfo::GetTimestamp(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mInfoTimestamp;
    return NOERROR;
}

ECode VersionInfo::GetClassloader(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mInfoClassloader;
    return NOERROR;
}

ECode VersionInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb(20 + mInfoPackage.GetLength() + mInfoModule.GetLength() +
            mInfoRelease.GetLength() + mInfoTimestamp.GetLength() + mInfoClassloader.GetLength());

    sb.Append("VersionInfo(");
    sb.Append(mInfoPackage);
    sb.AppendChar(':');
    sb.Append(mInfoModule);

    // If version info is missing, a single "UNAVAILABLE" for the module
    // is sufficient. Everything else just clutters the output.
    if (!UNAVAILABLE.Equals(mInfoRelease)) {
        sb.AppendChar(':');
        sb.Append(mInfoRelease);
    }

    if (!UNAVAILABLE.Equals(mInfoTimestamp)) {
        sb.AppendChar(':');
        sb.Append(mInfoTimestamp);
    }

    sb.AppendChar(')');

    if (!UNAVAILABLE.Equals(mInfoClassloader)) {
        sb.AppendChar('@');
        sb.Append(mInfoClassloader);
    }

    *str = sb.ToString();
    return NOERROR;
}

ECode VersionInfo::LoadVersionInfo(
    /* [in] */ ArrayOf<String>* pckgs,
    /* [in] */ IClassLoader* clsldr,
    /* [out] */ ArrayOf<IVersionInfo*>** _infos)
{
    VALIDATE_NOT_NULL(_infos)
    *_infos = NULL;
    if (pckgs == NULL) {
        Logger::E("VersionInfo", "Package identifier list must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IArrayList> vil;
    CArrayList::New(pckgs->GetLength(), (IArrayList**)&vil);
    for (Int32 i = 0; i < pckgs->GetLength(); i++) {
        AutoPtr<IVersionInfo> vi;
        LoadVersionInfo((*pckgs)[i], clsldr, (IVersionInfo**)&vi);
        if (vi != NULL) {
            IList::Probe(vil)->Add(vi);
        }
    }

    AutoPtr<IList> l = IList::Probe(vil);
    Int32 size;
    l->GetSize(&size);
    AutoPtr< ArrayOf<IInterface*> > inArray = ArrayOf<IInterface*>::Alloc(size);
    AutoPtr< ArrayOf<IInterface*> > outArray;
    l->ToArray(inArray, (ArrayOf<IInterface*>**)&outArray);
    AutoPtr< ArrayOf<IVersionInfo*> > infos = ArrayOf<IVersionInfo*>::Alloc(outArray->GetLength());
    for (Int32 i = 0; i < outArray->GetLength(); ++i) {
        AutoPtr<IVersionInfo> info = IVersionInfo::Probe((*outArray)[i]);
        infos->Set(i, info);
    }
    *_infos = infos;
    REFCOUNT_ADD(*_infos)
    return NOERROR;
}

ECode VersionInfo::LoadVersionInfo(
    /* [in] */ const String& pckg,
    /* [in] */ IClassLoader* inClsldr,
    /* [out] */ IVersionInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = NULL;
    if (pckg.IsNull()) {
        Logger::E("VersionInfo", "Package identifier list must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IClassLoader> clsldr = inClsldr;
    if (clsldr == NULL) {
        AutoPtr<IThread> t = Thread::GetCurrentThread();
        t->GetContextClassLoader((IClassLoader**)&clsldr);
    }

    AutoPtr<IProperties> vip; // version info properties, if available
    // try {
    // org.apache.http      becomes
    // org/apache/http/version.properties
    AutoPtr<IInputStream> is;
    assert(0);
    // clsldr->GetResourceAsStream(pckg.Replace('.', '/') + String("/") + VERSION_PROPERTY_FILE, (IInputStream**)&is);
    if (is != NULL) {
        // try {
        AutoPtr<IProperties> props;
        CProperties::New((IProperties**)&props);
        props->Load(is);
        vip = props;
        // } finally {
        //     is.close();
        // }
    }
    // } catch (IOException ex) {
    //     // shamelessly munch this exception
    // }

    if (vip != NULL) {
        return FromMap(pckg, IMap::Probe(vip), clsldr, info);
    }

    return NOERROR;
}

ECode VersionInfo::FromMap(
    /* [in] */ const String& pckg,
    /* [in] */ IMap* info,
    /* [in] */ IClassLoader* clsldr,
    /* [out] */ IVersionInfo** versionInfo)
{
    VALIDATE_NOT_NULL(versionInfo)
    *versionInfo = NULL;
    if (pckg.IsNull()) {
        Logger::E("VersionInfo", "Package identifier list must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String module = String(NULL);
    String release = String(NULL);
    String timestamp = String(NULL);

    if (info != NULL) {
        AutoPtr<ICharSequence> cs;
        CString::New(PROPERTY_MODULE, (ICharSequence**)&cs);
        AutoPtr<IInterface> value;
        info->Get(cs, (IInterface**)&value);
        AutoPtr<ICharSequence> v = ICharSequence::Probe(value);
        v->ToString(&module);
        if ((!module.IsNull()) && (module.GetLength() < 1)) {
            module = String(NULL);
        }
        cs = NULL;
        CString::New(PROPERTY_RELEASE, (ICharSequence**)&cs);
        value = NULL;
        info->Get(cs, (IInterface**)&value);
        v = ICharSequence::Probe(value);
        v->ToString(&release);
        if ((!release.IsNull()) && ((release.GetLength() < 1) ||
                (release.Equals("${pom.version}")))) {
            release = String(NULL);
        }

        cs = NULL;
        CString::New(PROPERTY_TIMESTAMP, (ICharSequence**)&cs);
        value = NULL;
        info->Get(cs, (IInterface**)&value);
        v = ICharSequence::Probe(value);
        v->ToString(&timestamp);
        if ((!timestamp.IsNull()) && ((timestamp.GetLength() < 1) ||
                (timestamp.Equals("${mvn.timestamp}")))) {
            timestamp = String(NULL);
        }
    } // if info

    String clsldrstr = String(NULL);
    if (clsldr != NULL) {
        IObject::Probe(clsldr)->ToString(&clsldrstr);
    }

    *versionInfo = (IVersionInfo*)new VersionInfo(pckg, module, release, timestamp, clsldrstr);
    REFCOUNT_ADD(*versionInfo)
    return NOERROR;
}

} // namespace Utility
} // namespace Http
} // namespace Apache
} // namespace Org
