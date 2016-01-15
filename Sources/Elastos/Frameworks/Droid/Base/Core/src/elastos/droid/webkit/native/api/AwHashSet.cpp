
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/webkit/native/api/AwHashSet.h"
#include "elastos/droid/webkit/native/api/HashSet_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Api {

Boolean AwHashSet::Add(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* p0)
{
    AutoPtr<ISet> mObj = ISet::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwHashSet", "AwHashSet::Add, mObj is NULL");
        return FALSE;
    }
    return mObj->Add(p0);
}

void AwHashSet::Clear(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ISet> mObj = ISet::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwHashSet", "AwHashSet::Clear, mObj is NULL");
        return;
    }
    mObj->Clear();
}

AutoPtr<IInterface> AwHashSet::Clone(
    /* [in] */ IInterface* obj)
{
    Logger::E("AwHashSet", "AwHashSet::Clone, no implementation");
    //this method will not be called, if called, then implement
    return NULL;
    /*
    AutoPtr<ISet> mObj = ISet::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwHashSet", "AwHashSet::Clone, mObj is NULL");
        return NULL;
    }
    return mObj->Clone();
    */
}

Boolean AwHashSet::Contains(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* p0)
{
    Logger::E("AwHashSet", "AwHashSet::Contains, no implementation");
    //this method will not be called, if called, then implement
    return FALSE;
    /*
    AutoPtr<ISet> mObj = ISet::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwHashSet", "AwHashSet::Contains, mObj is NULL");
        return FALSE;
    }
    return mObj->Contains(p0);
    */
}

Boolean AwHashSet::IsEmpty(
    /* [in] */ IInterface* obj)
{
    Logger::E("AwHashSet", "AwHashSet::IsEmpty, no implementation");
    //this method will not be called, if called, then implement
    return FALSE;
    /*
    AutoPtr<ISet> mObj = ISet::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwHashSet", "AwHashSet::IsEmpty, mObj is NULL");
        return FALSE;
    }
    return mObj->IsEmpty();
    */
}

AutoPtr<IInterface> AwHashSet::Iterator(
    /* [in] */ IInterface* obj)
{
    Logger::E("AwHashSet", "AwHashSet::Iterator, no implementation");
    //this method will not be called, if called, then implement
    return NULL;
    /*
    AutoPtr<ISet> mObj = ISet::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwHashSet", "AwHashSet::Iterator, mObj is NULL");
        return NULL;
    }
    return mObj->Iterator();
    */
}

Boolean AwHashSet::Remove(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* p0)
{
    AutoPtr<ISet> mObj = ISet::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwHashSet", "AwHashSet::Remove, mObj is NULL");
        return FALSE;
    }
    return mObj->Remove(p0);
}

Int32 AwHashSet::Size(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ISet> mObj = ISet::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwHashSet", "AwHashSet::Size, mObj is NULL");
        return 0;
    }
    Int32 size;
    mObj->GetSize(&size);
    return size;
}

AutoPtr<IInterface> AwHashSet::ConstructorJUHS()
{
    Logger::E("AwHashSet", "AwHashSet::ConstructorJUHS, no implementation");
    //this method will not be called, if called, then implement
    return NULL;
}

AutoPtr<IInterface> AwHashSet::ConstructorJUHSI(
    /* [in] */ Int32 p0)
{
    Logger::E("AwHashSet", "AwHashSet::ConstructorJUHSI, no implementation");
    //this method will not be called, if called, then implement
    return NULL;
}

AutoPtr<IInterface> AwHashSet::ConstructorJUHSIF(
    /* [in] */ Int32 p0,
    /* [in] */ Float p1)
{
    Logger::E("AwHashSet", "AwHashSet::ConstructorJUHSIF, no implementation");
    //this method will not be called, if called, then implement
    return NULL;
}

AutoPtr<IInterface> AwHashSet::ConstructorJUHSJUC(
    /* [in] */ IInterface* extends)
{
    Logger::E("AwHashSet", "AwHashSet::ConstructorJUHSJUC, no implementation");
    //this method will not be called, if called, then implement
    return NULL;
}

} // namespace Api
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
