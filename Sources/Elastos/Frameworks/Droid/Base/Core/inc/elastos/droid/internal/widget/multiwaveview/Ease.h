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

#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_EASE_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_EASE_H__

#include "Elastos.Droid.Animation.h"
#include "elastos/droid/ext/frameworkext.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Animation::ITimeInterpolator;

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {
namespace Multiwaveview {

class Ease
    : public Object
{
public:
    class Linear
        : public Object
    {
    public:
        class TimeInterpolator
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

    public:
        static AutoPtr<TimeInterpolator> mEaseNone;
    };

    class Cubic
        : public Object
    {
    public:
        class TimeInterpolatorIn
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorInOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

    public:
        static AutoPtr<TimeInterpolatorIn> mEaseIn;

        static AutoPtr<TimeInterpolatorOut> mEaseOut;

        static AutoPtr<TimeInterpolatorInOut> mEaseInOut;
    };

    class Quad
        : public Object
    {
    public:
        class TimeInterpolatorIn
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorInOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

    public:
        static AutoPtr<TimeInterpolatorIn> mEaseIn;

        static AutoPtr<TimeInterpolatorOut> mEaseOut;

        static AutoPtr<TimeInterpolatorInOut> mEaseInOut;
    };

    class Quart
        : public Object
    {
    public:
        class TimeInterpolatorIn
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorInOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

    public:
        static AutoPtr<TimeInterpolatorIn> mEaseIn;

        static AutoPtr<TimeInterpolatorOut> mEaseOut;

        static AutoPtr<TimeInterpolatorInOut> mEaseInOut;
    };

    class Quint
        : public Object
    {
    public:
        class TimeInterpolatorIn
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorInOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

    public:
        static AutoPtr<TimeInterpolatorIn> mEaseIn;

        static AutoPtr<TimeInterpolatorOut> mEaseOut;

        static AutoPtr<TimeInterpolatorInOut> mEaseInOut;
    };

    class Sine
        : public Object
    {
    public:
        class TimeInterpolatorIn
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

        class TimeInterpolatorInOut
            : public Object
            , public ITimeInterpolator
        {
        public:
            CAR_INTERFACE_DECL()

            CARAPI GetInterpolation(
                /* [in] */ Float input,
                /* [out] */ Float* res);

            CARAPI HasNativeInterpolator(
                /* [out] */ Boolean* res);
        };

    public:
        static AutoPtr<TimeInterpolatorIn> mEaseIn;

        static AutoPtr<TimeInterpolatorOut> mEaseOut;

        static AutoPtr<TimeInterpolatorInOut> mEaseInOut;
    };

public:
    static const Float DOMAIN;
    static const Float DURATION;
    static const Float START;
};

} // namespace Multiwaveview
} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_EASE_H__
