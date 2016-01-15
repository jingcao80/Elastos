

/**
 * The AppTask allows you to manage your own application's tasks.
 * See {@link android.app.ActivityManager#getAppTasks()}
 */
public static class AppTask {
    private IAppTask mAppTaskImpl;

    /** @hide */
    public AppTask(IAppTask task) {
        mAppTaskImpl = task;
    }

    /**
     * Finishes all activities in this task and removes it from the recent tasks list.
     */
    public void finishAndRemoveTask() {
        try {
            mAppTaskImpl.finishAndRemoveTask();
        } catch (RemoteException e) {
            Slog.e(TAG, "Invalid AppTask", e);
        }
    }

    /**
     * Get the RecentTaskInfo associated with this task.
     *
     * @return The RecentTaskInfo for this task, or null if the task no longer exists.
     */
    public RecentTaskInfo getTaskInfo() {
        try {
            return mAppTaskImpl.getTaskInfo();
        } catch (RemoteException e) {
            Slog.e(TAG, "Invalid AppTask", e);
            return null;
        }
    }

    /**
     * Bring this task to the foreground.  If it contains activities, they will be
     * brought to the foreground with it and their instances re-created if needed.
     * If it doesn't contain activities, the root activity of the task will be
     * re-launched.
     */
    public void moveToFront() {
        try {
            mAppTaskImpl.moveToFront();
        } catch (RemoteException e) {
            Slog.e(TAG, "Invalid AppTask", e);
        }
    }

    /**
     * Start an activity in this task.  Brings the task to the foreground.  If this task
     * is not currently active (that is, its id < 0), then a new activity for the given
     * Intent will be launched as the root of the task and the task brought to the
     * foreground.  Otherwise, if this task is currently active and the Intent does not specify
     * an activity to launch in a new task, then a new activity for the given Intent will
     * be launched on top of the task and the task brought to the foreground.  If this
     * task is currently active and the Intent specifies {@link Intent#FLAG_ACTIVITY_NEW_TASK}
     * or would otherwise be launched in to a new task, then the activity not launched but
     * this task be brought to the foreground and a new intent delivered to the top
     * activity if appropriate.
     *
     * <p>In other words, you generally want to use an Intent here that does not specify
     * {@link Intent#FLAG_ACTIVITY_NEW_TASK} or {@link Intent#FLAG_ACTIVITY_NEW_DOCUMENT},
     * and let the system do the right thing.</p>
     *
     * @param intent The Intent describing the new activity to be launched on the task.
     * @param options Optional launch options.
     *
     * @see Activity#startActivity(android.content.Intent, android.os.Bundle)
     */
    public void startActivity(Context context, Intent intent, Bundle options) {
        ActivityThread thread = ActivityThread.currentActivityThread();
        thread.getInstrumentation().execStartActivityFromAppTask(context,
                thread.getApplicationThread(), mAppTaskImpl, intent, options);
    }

    /**
     * Modify the {@link Intent#FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS} flag in the root
     * Intent of this AppTask.
     *
     * @param exclude If true, {@link Intent#FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS} will
     * be set; otherwise, it will be cleared.
     */
    public void setExcludeFromRecents(boolean exclude) {
        try {
            mAppTaskImpl.setExcludeFromRecents(exclude);
        } catch (RemoteException e) {
            Slog.e(TAG, "Invalid AppTask", e);
        }
    }
}