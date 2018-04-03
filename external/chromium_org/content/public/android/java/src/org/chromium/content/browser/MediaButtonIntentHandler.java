package org.chromium.media;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.media.AudioManager;
import android.util.Log;
import android.view.KeyEvent;


/**
 * M: for media button intents. To play/pause the media source.
 */
public class MediaButtonIntentHandler extends BroadcastReceiver {

    private static final String TAG = "MediaButtonIntentHandler";
    private ComponentName mMBIRec;
    private final Context mContext;
    private static MediaButtonIntentHandler sHandler;
    private boolean mRegisted;

    private MediaButtonIntentHandler(Context context) {
        Log.d(TAG, "Constructor: context: " + context);
        mContext = context;
    }

    /**
     * To get the instance of the handler.
     *
     * @return MediaButtonIntentHandler the instance
     */
    public static MediaButtonIntentHandler getInstance() {
        Log.d(TAG, "getInstance: sHandler: " + sHandler);
        if (sHandler != null) {
            return sHandler;
        } else {
            Log.e(TAG, "Can not get instance: context is null");
            return null;
        }
    }

    /**
     * To get the instance of the handler.
     *
     * @param context the context of the application
     * @return MediaButtonIntentHandler the instance
     */
    public static MediaButtonIntentHandler getInstance(Context context) {
        Log.d(TAG, "getInstance: context: " + context + ", sHandler: " + sHandler);
        if (sHandler == null) {
            sHandler = new MediaButtonIntentHandler(context);
        }
        return sHandler;
    }

    /**
     * To register the AUDIO_SERVICE handler.
     *
     * @param am the audio manager object
     */
    public void registeHandler(AudioManager am) {
        Log.d(TAG, "registeHanler called, mRegisted: " + mRegisted);
        if (mRegisted) {
            return;
        }
        if (mMBIRec == null) {
            mMBIRec = new ComponentName(
                mContext.getPackageName(), MediaButtonIntentHandler.class.getName());
        }
        AudioManager manager = (am != null) ? am :
            (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);
        manager.registerMediaButtonEventReceiver(mMBIRec);
        mRegisted = true;
    }

    /**
     * To unregister the AUDIO_SERVICE handler.
     *
     * @param am the audio manager object
     */
    public void unRegisteHandler(AudioManager am) {
        Log.d(TAG, "unRegisteHanler, mRegisted: " + mRegisted);
        if (!mRegisted) {
            return;
        }
        AudioManager manager = (am != null) ? am :
            (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);
        manager.unregisterMediaButtonEventReceiver(mMBIRec);
        mRegisted = false;
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        String intentAction = intent.getAction();
        Log.d(TAG, "onReceive intentAction: " + intentAction);
        if (Intent.ACTION_MEDIA_BUTTON.equals(intentAction)) {
            KeyEvent event = (KeyEvent)
                    intent.getParcelableExtra(Intent.EXTRA_KEY_EVENT);
            Log.d(TAG, "onReceive event: " + event);
            if (event == null) {
                return;
            }

            int keycode = event.getKeyCode();
            int action = event.getAction();
            Log.d(TAG, "onReceive keycode: " + keycode + ", action: " + action);

            switch (keycode) {
                case KeyEvent.KEYCODE_MEDIA_PAUSE:
                    if (action == KeyEvent.ACTION_UP) {
                        /**
                         * should pause the media file
                         */
                    }
                    break;

                case KeyEvent.KEYCODE_MEDIA_PLAY:
                    if (action == KeyEvent.ACTION_UP) {
                        /**
                         * should play the media file
                         */
                    }
                    break;

                default:
                    break;
            }
        }
    }

}

