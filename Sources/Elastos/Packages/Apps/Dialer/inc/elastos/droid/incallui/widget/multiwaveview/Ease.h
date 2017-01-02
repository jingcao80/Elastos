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

#ifndef __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_EASE_H__
#define __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_EASE_H__

#include "Elastos.Droid.Animation.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {
namespace Widget {
namespace MultiwaveView {

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
        static AutoPtr<TimeInterpolator> sEaseNone;
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
        static AutoPtr<TimeInterpolatorIn> sEaseIn;
        static AutoPtr<TimeInterpolatorOut> sEaseOut;
        static AutoPtr<TimeInterpolatorInOut> sEaseInOut;
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
        static AutoPtr<TimeInterpolatorIn> sEaseIn;
        static AutoPtr<TimeInterpolatorOut> sEaseOut;
        static AutoPtr<TimeInterpolatorInOut> sEaseInOut;
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
        static AutoPtr<TimeInterpolatorIn> sEaseIn;
        static AutoPtr<TimeInterpolatorOut> sEaseOut;
        static AutoPtr<TimeInterpolatorInOut> sEaseInOut;
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
        static AutoPtr<TimeInterpolatorIn> sEaseIn;
        static AutoPtr<TimeInterpolatorOut> sEaseOut;
        static AutoPtr<TimeInterpolatorInOut> sEaseInOut;
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
        static AutoPtr<TimeInterpolatorIn> sEaseIn;
        static AutoPtr<TimeInterpolatorOut> sEaseOut;
        static AutoPtr<TimeInterpolatorInOut> sEaseInOut;
    };

public:
    static const Float DOMAIN;
    static const Float DURATION;
    static const Float START;
};

} // namespace MultiwaveView
} // namespace Widget
} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_EASE_H__
