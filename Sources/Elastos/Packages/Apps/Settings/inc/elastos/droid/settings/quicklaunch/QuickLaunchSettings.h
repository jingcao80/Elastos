

package com.android.settings.quicklaunch;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Provider::Settings::IBookmarks;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IAdapterView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;


/**
 * Settings activity for quick launch.
 * <p>
 * Shows a list of possible shortcuts, the current application each is bound to,
 * and allows choosing a new bookmark for a shortcut.
 */
public class QuickLaunchSettings extends SettingsPreferenceFragment implements
        AdapterView.OnItemLongClickListener, DialogInterface.OnClickListener {

    private static const String TAG = "QuickLaunchSettings";

    private static const String KEY_SHORTCUT_CATEGORY = "shortcut_category";

    private static const Int32 DIALOG_CLEAR_SHORTCUT = 0;

    private static const Int32 REQUEST_PICK_BOOKMARK = 1;

    private static const Int32 COLUMN_SHORTCUT = 0;
    private static const Int32 COLUMN_TITLE = 1;
    private static const Int32 COLUMN_INTENT = 2;
    private static const String[] sProjection = new String[] {
            Bookmarks.SHORTCUT, Bookmarks.TITLE, Bookmarks.INTENT
    };
    private static const String sShortcutSelection = Bookmarks.SHORTCUT + "=?";

    private Handler mUiHandler = new Handler();

    private static const String DEFAULT_BOOKMARK_FOLDER = "@quicklaunch";
    /** Cursor for Bookmarks provider. */
    private Cursor mBookmarksCursor;
    /** Listens for changes to Bookmarks provider. */
    private BookmarksObserver mBookmarksObserver;
    /** Used to keep track of which shortcuts have bookmarks. */
    private SparseBooleanArray mBookmarkedShortcuts;

    /** Preference category to hold the shortcut preferences. */
    private PreferenceGroup mShortcutGroup;
    /** Mapping of a shortcut to its preference. */
    private SparseArray<ShortcutPreference> mShortcutToPreference;

    /** The bookmark title of the shortcut that is being cleared. */
    private CharSequence mClearDialogBookmarkTitle;
    private static const String CLEAR_DIALOG_BOOKMARK_TITLE = "CLEAR_DIALOG_BOOKMARK_TITLE";
    /** The shortcut that is being cleared. */
    private Char32 mClearDialogShortcut;
    private static const String CLEAR_DIALOG_SHORTCUT = "CLEAR_DIALOG_SHORTCUT";

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        AddPreferencesFromResource(R.xml.quick_launch_settings);

        mShortcutGroup = (PreferenceGroup) FindPreference(KEY_SHORTCUT_CATEGORY);
        mShortcutToPreference = new SparseArray<ShortcutPreference>();
        mBookmarksObserver = new BookmarksObserver(mUiHandler);
        InitShortcutPreferences();
        mBookmarksCursor = GetActivity()->GetContentResolver().Query(Bookmarks.CONTENT_URI,
                sProjection, NULL, NULL, NULL);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        mBookmarksCursor = GetActivity()->GetContentResolver().Query(Bookmarks.CONTENT_URI,
                sProjection, NULL, NULL, NULL);
        GetContentResolver()->RegisterContentObserver(Bookmarks.CONTENT_URI, TRUE,
                mBookmarksObserver);
        RefreshShortcuts();
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        GetContentResolver()->UnregisterContentObserver(mBookmarksObserver);
    }

    //@Override
    CARAPI OnStop() {
        super->OnStop();
        mBookmarksCursor->Close();
    }

    //@Override
    CARAPI OnActivityCreated(Bundle state) {
        super->OnActivityCreated(state);

        GetListView()->SetOnItemLongClickListener(this);

        if (state != NULL) {
            // Restore the clear dialog's info
            mClearDialogBookmarkTitle = state->GetString(CLEAR_DIALOG_BOOKMARK_TITLE);
            mClearDialogShortcut = (Char32) state->GetInt(CLEAR_DIALOG_SHORTCUT, 0);
        }
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);

        // Save the clear dialog's info
        outState->PutCharSequence(CLEAR_DIALOG_BOOKMARK_TITLE, mClearDialogBookmarkTitle);
        outState->PutInt(CLEAR_DIALOG_SHORTCUT, mClearDialogShortcut);
    }

    //@Override
    public Dialog OnCreateDialog(Int32 id) {
        switch (id) {

            case DIALOG_CLEAR_SHORTCUT: {
                // Create the dialog for clearing a shortcut
                return new AlertDialog->Builder(GetActivity())
                        .SetTitle(GetString(R::string::quick_launch_clear_dialog_title))
                        .SetMessage(GetString(R::string::quick_launch_clear_dialog_message,
                                mClearDialogShortcut, mClearDialogBookmarkTitle))
                        .SetPositiveButton(R::string::quick_launch_clear_ok_button, this)
                        .SetNegativeButton(R::string::quick_launch_clear_cancel_button, this)
                        .Create();
            }
        }

        return super->OnCreateDialog(id);
    }

    private void ShowClearDialog(ShortcutPreference pref) {

        if (!pref->HasBookmark()) return;

        mClearDialogBookmarkTitle = pref->GetTitle();
        mClearDialogShortcut = pref->GetShortcut();
        ShowDialog(DIALOG_CLEAR_SHORTCUT);
    }

    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        if (mClearDialogShortcut > 0 && which == AlertDialog.BUTTON_POSITIVE) {
            // Clear the shortcut
            ClearShortcut(mClearDialogShortcut);
        }
        mClearDialogBookmarkTitle = NULL;
        mClearDialogShortcut = 0;
    }

    private void ClearShortcut(Char32 shortcut) {
        GetContentResolver()->Delete(Bookmarks.CONTENT_URI, sShortcutSelection,
                new String[] { String->ValueOf((Int32) shortcut) });
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        if (!(preference instanceof ShortcutPreference)) return FALSE;

        // Open the screen to pick a bookmark for this shortcut
        ShortcutPreference pref = (ShortcutPreference) preference;
        Intent intent = new Intent(GetActivity(), BookmarkPicker.class);
        intent->PutExtra(BookmarkPicker.EXTRA_SHORTCUT, pref->GetShortcut());
        StartActivityForResult(intent, REQUEST_PICK_BOOKMARK);

        return TRUE;
    }

    public Boolean OnItemLongClick(AdapterView<?> parent, View view, Int32 position, Int64 id) {

        // Open the clear shortcut dialog
        Preference pref = (Preference) GetPreferenceScreen()->GetRootAdapter().GetItem(position);
        if (!(pref instanceof ShortcutPreference)) return FALSE;
        ShowClearDialog((ShortcutPreference) pref);
        return TRUE;
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        if (resultCode != Activity.RESULT_OK) {
            return;
        }

        if (requestCode == REQUEST_PICK_BOOKMARK) {

            // Returned from the 'pick bookmark for this shortcut' screen
            if (data == NULL) {
                Logger::W(TAG, "Result from bookmark picker does not have an intent.");
                return;
            }

            Char32 shortcut = data->GetCharExtra(BookmarkPicker.EXTRA_SHORTCUT, (Char32) 0);
            UpdateShortcut(shortcut, data);

        } else {
            super->OnActivityResult(requestCode, resultCode, data);
        }
    }

    private void UpdateShortcut(Char32 shortcut, Intent intent) {
        // Update the bookmark for a shortcut
        // Pass an empty title so it gets resolved each time this bookmark is
        // displayed (since the locale could change after we insert into the provider).
        Bookmarks->Add(GetContentResolver(), intent, "", DEFAULT_BOOKMARK_FOLDER, shortcut, 0);
    }

    private ShortcutPreference GetOrCreatePreference(Char32 shortcut) {
        ShortcutPreference pref = mShortcutToPreference->Get(shortcut);
        if (pref != NULL) {
            return pref;
        } else {
            Logger::W(TAG, "Unknown shortcut '" + shortcut + "', creating preference anyway");
            return CreatePreference(shortcut);
        }
    }

    private ShortcutPreference CreatePreference(Char32 shortcut) {
        ShortcutPreference pref = new ShortcutPreference(GetActivity(), shortcut);
        mShortcutGroup->AddPreference(pref);
        mShortcutToPreference->Put(shortcut, pref);
        return pref;
    }

    private void InitShortcutPreferences() {

        /** Whether the shortcut has been seen already.  The array index is the shortcut. */
        SparseBooleanArray shortcutSeen = new SparseBooleanArray();
        KeyCharacterMap keyMap = KeyCharacterMap->Load(KeyCharacterMap.VIRTUAL_KEYBOARD);

        // Go through all the key codes and create a preference for the appropriate keys
        for (Int32 keyCode = KeyEvent->GetMaxKeyCode() - 1; keyCode >= 0; keyCode--) {
            // Get the label for the primary Char32 on the key that produces this key code
            Char32 shortcut = (Char32) Character->ToLowerCase(keyMap->GetDisplayLabel(keyCode));
            if (shortcut == 0 || shortcutSeen->Get(shortcut, FALSE)) continue;
            // TODO: need a to tell if the current keyboard can produce this key code, for now
            // only allow the letter or digits
            if (!Character->IsLetterOrDigit(shortcut)) continue;
            shortcutSeen->Put(shortcut, TRUE);

            CreatePreference(shortcut);
        }
    }

    private synchronized void RefreshShortcuts() {
        Cursor c = mBookmarksCursor;
        if (c == NULL) {
            // Haven't finished querying yet
            return;
        }

        if (!c->Requery()) {
            Logger::E(TAG, "Could not requery cursor when refreshing shortcuts.");
            return;
        }

        /**
         * We use the previous bookmarked shortcuts array to filter out those
         * shortcuts that had bookmarks before this method call, and don't after
         * (so we can set the preferences to be without bookmarks).
         */
        SparseBooleanArray noLongerBookmarkedShortcuts = mBookmarkedShortcuts;
        SparseBooleanArray newBookmarkedShortcuts = new SparseBooleanArray();
        while (c->MoveToNext()) {
            Char32 shortcut = Character->ToLowerCase((Char32) c->GetInt(COLUMN_SHORTCUT));
            if (shortcut == 0) continue;

            ShortcutPreference pref = GetOrCreatePreference(shortcut);
            CharSequence title = Bookmarks->GetTitle(GetActivity(), c);

            /*
             * The title retrieved from Bookmarks->GetTitle() will be in
             * the original boot locale, not the current locale.
             * Try to look up a localized title from the PackageManager.
             */
            Int32 intentColumn = c->GetColumnIndex(Bookmarks.INTENT);
            String intentUri = c->GetString(intentColumn);
            PackageManager packageManager = GetPackageManager();
            try {
                Intent intent = Intent->ParseUri(intentUri, 0);
                ResolveInfo info = packageManager->ResolveActivity(intent, 0);
                if (info != NULL) {
                    title = info->LoadLabel(packageManager);
                }
            } catch (URISyntaxException e) {
                // Just use the non-localized title, then.
            }

            pref->SetTitle(title);
            pref->SetSummary(GetString(R::string::quick_launch_shortcut,
                    String->ValueOf(shortcut)));
            pref->SetHasBookmark(TRUE);

            newBookmarkedShortcuts->Put(shortcut, TRUE);
            if (noLongerBookmarkedShortcuts != NULL) {
                // After this loop, the shortcuts with value TRUE in this array
                // will no longer have bookmarks
                noLongerBookmarkedShortcuts->Put(shortcut, FALSE);
            }
        }

        if (noLongerBookmarkedShortcuts != NULL) {
            for (Int32 i = noLongerBookmarkedShortcuts->Size() - 1; i >= 0; i--) {
                if (noLongerBookmarkedShortcuts->ValueAt(i)) {
                    // True, so there is no longer a bookmark for this shortcut
                    Char32 shortcut = (Char32) noLongerBookmarkedShortcuts->KeyAt(i);
                    ShortcutPreference pref = mShortcutToPreference->Get(shortcut);
                    if (pref != NULL) {
                        pref->SetHasBookmark(FALSE);
                    }
                }
            }
        }

        mBookmarkedShortcuts = newBookmarkedShortcuts;

        c->Deactivate();
    }

    private class BookmarksObserver extends ContentObserver {

        public BookmarksObserver(Handler handler) {
            Super(handler);
        }

        //@Override
        CARAPI OnChange(Boolean selfChange) {
            super->OnChange(selfChange);

            RefreshShortcuts();
        }
    }
}
