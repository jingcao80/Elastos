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

#ifndef __ELASTOS_DROID_MEDIA_MEDIASCANNERCONNECTION_H__
#define __ELASTOS_DROID_MEDIA_MEDIASCANNERCONNECTION_H__

namespace Elastos {
namespace Droid {
namespace Media {

class MediaScannerConnection
    : public Object
    , public IMediaScannerConnection
    , public IServiceConnection
{
public:
    class MyMediaScannerListener
        : public Object
        , public IIMediaScannerListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyMediaScannerListener(
            /* [in] */ IWeakReference* host);

        CARAPI ScanCompleted(
            /* [in] */ const String& path,
            /* [in] */ IUri* uri);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };


public:
    MediaScannerConnection();

    virtual ~MediaScannerConnection();

    CAR_INTERFACE_DECL()

};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif __ELASTOS_DROID_MEDIA_MEDIASCANNERCONNECTION_H__