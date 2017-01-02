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

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/api/AwSurface.h"
#include "elastos/droid/webkit/webview/chromium/native/api/Surface_dec.h"
//TODO #include "elastos/droid/view/CSurface.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::View::ISurface;
//TODO using Elastos::Droid::View::CSurface;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Api {

void AwSurface::Finalize(
    /* [in] */ IInterface* obj)
{
    Logger::E("AwSurface", "AwSurface::Finalize, no implementation");
    //this method will not be called, if called, then implement
    /*
    AutoPtr<AwSurface> mObj = (AwSurface*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwSurface", "AwSurface::Finalize, mObj is NULL");
        return;
    }
    mObj->Finalize();
    */
}

void AwSurface::ReleaseResources(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ISurface> mObj = ISurface::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwSurface", "AwSurface::ReleaseResources, mObj is NULL");
        return;
    }
    mObj->ReleaseResources();
}

Boolean AwSurface::IsValid(
    /* [in] */ IInterface* obj)
{
    Logger::E("AwSurface", "AwSurface::Finalize, no implementation");
    //this method will not be called, if called, then implement
    return FALSE;
    /*
    AutoPtr<AwSurface> mObj = (AwSurface*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwSurface", "AwSurface::IsValid, mObj is NULL");
        return FALSE;
    }
    return mObj->IsValid();
    */
}

AutoPtr<IInterface> AwSurface::LockCanvas(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* p0)
{
    Logger::E("AwSurface", "AwSurface::Finalize, no implementation");
    return NULL;
    //this method will not be called, if called, then implement
    /*
    AutoPtr<AwSurface> mObj = (AwSurface*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwSurface", "AwSurface::LockCanvas, mObj is NULL");
        return NULL;
    }
    return mObj->LockCanvas(p0);
    */
}

void AwSurface::UnlockCanvasAndPost(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* p0)
{
    Logger::E("AwSurface", "AwSurface::Finalize, no implementation");
    //this method will not be called, if called, then implement
    /*
    AutoPtr<AwSurface> mObj = (AwSurface*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwSurface", "AwSurface::UnlockCanvasAndPost, mObj is NULL");
        return;
    }
    mObj->UnlockCanvasAndPost(p0);
    */
}

void AwSurface::UnlockCanvas(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* p0)
{
    Logger::E("AwSurface", "AwSurface::Finalize, no implementation");
    //this method will not be called, if called, then implement
    /*
       AutoPtr<AwSurface> mObj = (AwSurface*)(IObject::Probe(obj));
       if (NULL == mObj)
       {
       Logger::E("AwSurface", "AwSurface::UnlockCanvas, mObj is NULL");
       return;
       }
       mObj->UnlockCanvas(p0);
     */
}

Int32 AwSurface::DescribeContents(
    /* [in] */ IInterface* obj)
{
    Logger::E("AwSurface", "AwSurface::Finalize, no implementation");
    //this method will not be called, if called, then implement
    return -1;
    /*
    AutoPtr<AwSurface> mObj = (AwSurface*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwSurface", "AwSurface::DescribeContents, mObj is NULL");
        return 0;
    }
    return mObj->DescribeContents();
    */
}

void AwSurface::ReadFromParcel(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* p0)
{
    Logger::E("AwSurface", "AwSurface::Finalize, no implementation");
    //this method will not be called, if called, then implement
    /*
    AutoPtr<AwSurface> mObj = (AwSurface*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwSurface", "AwSurface::ReadFromParcel, mObj is NULL");
        return;
    }
    mObj->ReadFromParcel(p0);
    */
}

void AwSurface::WriteToParcel(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* p0,
    /* [in] */ Int32 p1)
{
    Logger::E("AwSurface", "AwSurface::Finalize, no implementation");
    //this method will not be called, if called, then implement
    /*
    AutoPtr<AwSurface> mObj = (AwSurface*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwSurface", "AwSurface::WriteToParcel, mObj is NULL");
        return;
    }
    mObj->WriteToParcel(p0, p1);
    */
}

String AwSurface::ToString(
    /* [in] */ IInterface* obj)
{
    Logger::E("AwSurface", "AwSurface::Finalize, no implementation");
    //this method will not be called, if called, then implement
    return String(NULL);
    /*
    AutoPtr<AwSurface> mObj = (AwSurface*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwSurface", "AwSurface::ToString, mObj is NULL");
        return String(NULL);
    }
    return mObj->ToString();
    */
}

AutoPtr<IInterface> AwSurface::Constructor(
    /* [in] */ IInterface* p0)
{
    Logger::I("AwSurface", "AwSurface::Constructor");
    AutoPtr<ISurfaceTexture> st = ISurfaceTexture::Probe(p0);
    AutoPtr<ISurface> surface;
    //CSurface::New(st, (ISurface**)&surface);
    return surface;
}

} // namespace Api
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
