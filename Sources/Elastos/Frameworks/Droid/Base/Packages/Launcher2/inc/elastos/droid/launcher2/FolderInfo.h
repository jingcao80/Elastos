#ifndef  __ELASTOS_DROID_LAUNCHER2_FOLDERINFO_H__
#define  __ELASTOS_DROID_LAUNCHER2_FOLDERINFO_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Represents a folder containing shortcuts or apps.
 */
class FolderInfo
    : public ItemInfo
{
public:
    FolderInfo();

    /**
     * Add an app or shortcut
     *
     * @param item
     */
    CARAPI Add(
        /* [in] */ IShortcutInfo* item);

    /**
     * Remove an app or shortcut. Does not change the DB.
     *
     * @param item
     */
    CARAPI Remove(
        /* [in] */ IShortcutInfo* item);

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    //@Override
    CARAPI OnAddToDatabase(
        /* [in] */ IContext* context,
        /* [in] */ IContentValues* values);

    CARAPI AddListener(
        /* [in] */ IFolderListener* listener);

    CARAPI RemoveListener(
        /* [in] */ IFolderListener* listener);

    CARAPI ItemsChanged();

    //@Override
    CARAPI Unbind();

public:
    /**
     * Whether this folder has been opened
     */
    Boolean mOpened;

    /**
     * The apps and shortcuts
     */
    AutoPtr<IArrayList> mContents;

    AutoPtr<IArrayList> mListeners;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_FOLDERINFO_H__