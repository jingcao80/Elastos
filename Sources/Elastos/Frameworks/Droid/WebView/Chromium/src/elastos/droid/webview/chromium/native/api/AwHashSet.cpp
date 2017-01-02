//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/webkit/webview/chromium/native/api/AwHashSet.h"
#include "elastos/droid/webkit/webview/chromium/native/api/HashSet_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
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
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
