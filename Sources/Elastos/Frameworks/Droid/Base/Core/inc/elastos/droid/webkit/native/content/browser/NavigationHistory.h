// wuweizuo automatic build .h file from .java file.
// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_NAVIGATIONHISTORY_H_
#define _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_NAVIGATIONHISTORY_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/content/browser/NavigationEntry.h"

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

/**
  * {@link NavigationHistory} captures a snapshot of the navigation history of a
  * {@link ContentViewCore}. It is a copy and will not be updated as navigation
  * occurs on the source {@link ContentViewCore}.
  */
class NavigationHistory : public Object
{
public:
    /* package */
    virtual CARAPI SetCurrentEntryIndex(
        /* [in] */ Int32 currentEntryIndex);

    /**
      * @return The number of entries in the history.
      */
    virtual CARAPI_(Int32) GetEntryCount();

    /**
      * Returns the {@link NavigationEntry} for the given index.
      */
    virtual CARAPI_(AutoPtr<NavigationEntry>) GetEntryAtIndex(
        /* [in] */ Int32 index);

    /**
      * Returns the index of the entry the {@link ContentViewCore} was navigated to
      * when the history was fetched.
      */
    virtual CARAPI_(Int32) GetCurrentEntryIndex();

protected:
    virtual CARAPI AddEntry(
        /* [in] */ NavigationEntry* entry);

private:
    ///*const*/ AutoPtr< IArrayList<NavigationEntry> > mEntries;
    AutoPtr<IArrayList> mEntries;
    Int32 mCurrentEntryIndex;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_NAVIGATIONHISTORY_H_
