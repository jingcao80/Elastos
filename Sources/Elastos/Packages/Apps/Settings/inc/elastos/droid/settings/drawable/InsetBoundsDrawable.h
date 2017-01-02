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

package com.android.settings.drawable;

using Elastos::Droid::Graphics::Drawable::IDrawable;

/**
 * Wrapper around another {@link Drawable} that insets requested bounds by a
 * specific amount.
 */
public class InsetBoundsDrawable extends DrawableWrapper {
    private final Int32 mInsetBoundsSides;

    public InsetBoundsDrawable(Drawable drawable, Int32 insetBoundsSides) {
        Super(drawable);
        mInsetBoundsSides = insetBoundsSides;
    }

    //@Override
    CARAPI SetBounds(Int32 left, Int32 top, Int32 right, Int32 bottom) {
        super->SetBounds(left + mInsetBoundsSides, top, right - mInsetBoundsSides, bottom);
    }
}
