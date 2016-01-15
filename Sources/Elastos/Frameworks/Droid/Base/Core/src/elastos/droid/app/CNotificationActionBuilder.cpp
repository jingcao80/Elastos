
/**
* Builder class for {@link Action} objects.
*/
public static final class Builder

private final int mIcon;
private final CharSequence mTitle;
private final PendingIntent mIntent;
private final Bundle mExtras;
private ArrayList<RemoteInput> mRemoteInputs;

/**
 * Construct a new builder for {@link Action} object.
 * @param icon icon to show for this action
 * @param title the title of the action
 * @param intent the {@link PendingIntent} to fire when users trigger this action
 */
public Builder(int icon, CharSequence title, PendingIntent intent) {
    this(icon, title, intent, new Bundle(), null);
}

/**
 * Construct a new builder for {@link Action} object using the fields from an
 * {@link Action}.
 * @param action the action to read fields from.
 */
public Builder(Action action) {
    this(action.icon, action.title, action.actionIntent, new Bundle(action.mExtras),
            action.getRemoteInputs());
}

private Builder(int icon, CharSequence title, PendingIntent intent, Bundle extras,
        RemoteInput[] remoteInputs) {
    mIcon = icon;
    mTitle = title;
    mIntent = intent;
    mExtras = extras;
    if (remoteInputs != null) {
        mRemoteInputs = new ArrayList<RemoteInput>(remoteInputs.length);
        Collections.addAll(mRemoteInputs, remoteInputs);
    }
}

/**
 * Merge additional metadata into this builder.
 *
 * <p>Values within the Bundle will replace existing extras values in this Builder.
 *
 * @see Notification.Action#extras
 */
public Builder addExtras(Bundle extras) {
    if (extras != null) {
        mExtras.putAll(extras);
    }
    return this;
}

/**
 * Get the metadata Bundle used by this Builder.
 *
 * <p>The returned Bundle is shared with this Builder.
 */
public Bundle getExtras() {
    return mExtras;
}

/**
 * Add an input to be collected from the user when this action is sent.
 * Response values can be retrieved from the fired intent by using the
 * {@link RemoteInput#getResultsFromIntent} function.
 * @param remoteInput a {@link RemoteInput} to add to the action
 * @return this object for method chaining
 */
public Builder addRemoteInput(RemoteInput remoteInput) {
    if (mRemoteInputs == null) {
        mRemoteInputs = new ArrayList<RemoteInput>();
    }
    mRemoteInputs.add(remoteInput);
    return this;
}

/**
 * Apply an extender to this action builder. Extenders may be used to add
 * metadata or change options on this builder.
 */
public Builder extend(Extender extender) {
    extender.extend(this);
    return this;
}

/**
 * Combine all of the options that have been set and return a new {@link Action}
 * object.
 * @return the built action
 */
public Action build() {
    RemoteInput[] remoteInputs = mRemoteInputs != null
            ? mRemoteInputs.toArray(new RemoteInput[mRemoteInputs.size()]) : null;
    return new Action(mIcon, mTitle, mIntent, mExtras, remoteInputs);
}

