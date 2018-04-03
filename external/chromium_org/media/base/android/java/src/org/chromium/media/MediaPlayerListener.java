/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.media;

import android.content.Context;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.util.Log;
import android.widget.Toast;

import org.chromium.base.CalledByNative;
import org.chromium.base.JNINamespace;

// This class implements all the listener interface for android mediaplayer.
// Callbacks will be sent to the native class for processing.
@JNINamespace("media")
class MediaPlayerListener implements MediaPlayer.OnPreparedListener,
    MediaPlayer.OnCompletionListener,
    MediaPlayer.OnBufferingUpdateListener,
    MediaPlayer.OnSeekCompleteListener,
    MediaPlayer.OnVideoSizeChangedListener,
    MediaPlayer.OnErrorListener,
    MediaPlayer.OnInfoListener,
    AudioManager.OnAudioFocusChangeListener {
    // These values are mirrored as enums in media/base/android/media_player_bridge.h.
    // Please ensure they stay in sync.
    private static final int MEDIA_ERROR_FORMAT = 0;
    private static final int MEDIA_ERROR_DECODE = 1;
    private static final int MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK = 2;
    private static final int MEDIA_ERROR_INVALID_CODE = 3;

    // These values are copied from android media player.
    public static final int MEDIA_ERROR_MALFORMED = -1007;
    public static final int MEDIA_ERROR_TIMED_OUT = -110;

    // Used to determine the class instance to dispatch the native call to.
    private long mNativeMediaPlayerListener = 0;
    private final Context mContext;

    /// M: for new logs
    private static final String TAG = "MediaPlayerListener";
    /// M: for Media button intents handler
    private static MediaButtonIntentHandler sMediaButton;
    /// M: for toast when audio/video is not supported.
    private static boolean sCanHandleOnInfo;

    private MediaPlayerListener(long nativeMediaPlayerListener, Context context) {
        Log.d(TAG, "Constructor: context: " +
                  context + ", native listener: " + nativeMediaPlayerListener);
        mNativeMediaPlayerListener = nativeMediaPlayerListener;
        mContext = context;
    }

    /// M: for toast when audio/video is not supported.
    @Override
    public boolean onInfo(MediaPlayer mp, int what, int extra) {
        Log.d(TAG, "onInfo: what: " + what + ", extra: " + extra);
        if (what == MediaPlayer.MEDIA_INFO_AUDIO_NOT_SUPPORTED && sCanHandleOnInfo) {
            sCanHandleOnInfo = false;
            Toast.makeText(mContext, com.mediatek.internal.R.string.media_info_audio_not_supported,
                Toast.LENGTH_SHORT).show();
            return true;
        } else if (what == MediaPlayer.MEDIA_INFO_VIDEO_NOT_SUPPORTED && sCanHandleOnInfo) {
            sCanHandleOnInfo = false;
            Toast.makeText(mContext, com.mediatek.internal.R.string.media_info_video_not_supported,
                Toast.LENGTH_SHORT).show();
            return true;
        }
        return false;
    }

    @Override
    public boolean onError(MediaPlayer mp, int what, int extra) {
        int errorType;
        Log.d(TAG, "onError: error: " + what + ", data: " + extra);
        switch (what) {
            case MediaPlayer.MEDIA_ERROR_UNKNOWN:
                switch (extra) {
                    case MEDIA_ERROR_MALFORMED:
                        errorType = MEDIA_ERROR_DECODE;
                        break;
                    case MEDIA_ERROR_TIMED_OUT:
                        errorType = MEDIA_ERROR_INVALID_CODE;
                        break;
                    default:
                        errorType = MEDIA_ERROR_FORMAT;
                        break;
                }
                break;
            case MediaPlayer.MEDIA_ERROR_SERVER_DIED:
                errorType = MEDIA_ERROR_DECODE;
                break;
            case MediaPlayer.MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK:
                errorType = MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK;
                break;
            default:
                // There are some undocumented error codes for android media player.
                // For example, when surfaceTexture got deleted before we setVideoSuface
                // to NULL, mediaplayer will report error -38. These errors should be ignored
                // and not be treated as an error to webkit.
                errorType = MEDIA_ERROR_INVALID_CODE;
                break;
        }
        nativeOnMediaError(mNativeMediaPlayerListener, errorType);
        /// M: for Media button intent handler
        AudioManager am = (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);
        sMediaButton.unRegisteHandler(am);
        Log.d(TAG, "onError: errorType: " + errorType + ", finish native error handler");
        return true;
    }

    @Override
    public void onVideoSizeChanged(MediaPlayer mp, int width, int height) {
        Log.d(TAG, "onVideoSizeChanged: width: " + width + ", height: " + height);
        nativeOnVideoSizeChanged(mNativeMediaPlayerListener, width, height);
    }

    @Override
    public void onSeekComplete(MediaPlayer mp) {
        Log.d(TAG, "onSeekComplete");
        nativeOnSeekComplete(mNativeMediaPlayerListener);
    }

    @Override
    public void onBufferingUpdate(MediaPlayer mp, int percent) {
        Log.d(TAG, "onBufferingUpdate: percent: " + percent);
        nativeOnBufferingUpdate(mNativeMediaPlayerListener, percent);
    }

    @Override
    public void onCompletion(MediaPlayer mp) {
        Log.d(TAG, "onCompletion");
        nativeOnPlaybackComplete(mNativeMediaPlayerListener);
        /// M: for Media button intent handler
        AudioManager am = (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);
        sMediaButton.unRegisteHandler(am);
    }

    @Override
    public void onPrepared(MediaPlayer mp) {
        Log.d(TAG, "onPrepared");
        nativeOnMediaPrepared(mNativeMediaPlayerListener);
    }

    @Override
    public void onAudioFocusChange(int focusChange) {
        Log.d(TAG, "onAudioFocusChange: focusChange: " + focusChange);
        if (focusChange == AudioManager.AUDIOFOCUS_LOSS ||
                focusChange == AudioManager.AUDIOFOCUS_LOSS_TRANSIENT) {
            nativeOnMediaInterrupted(mNativeMediaPlayerListener);
        }
        Log.d(TAG, "onAudioFocusChange interruption finished");
    }

    @CalledByNative
    public void releaseResources() {
        Log.d(TAG, "releaseResources");
        if (mContext != null) {
            // Unregister the wish for audio focus.
            AudioManager am = (AudioManager) mContext.getSystemService(Context.AUDIO_SERVICE);
            if (am != null) {
                am.abandonAudioFocus(this);
                /// M: for Media button intent handler
                sMediaButton.unRegisteHandler(am);
            }
        }
    }

    @CalledByNative
    private static MediaPlayerListener create(long nativeMediaPlayerListener,
            Context context, MediaPlayerBridge mediaPlayerBridge) {
        final MediaPlayerListener listener =
                new MediaPlayerListener(nativeMediaPlayerListener, context);
        if (mediaPlayerBridge != null) {
            mediaPlayerBridge.setOnBufferingUpdateListener(listener);
            mediaPlayerBridge.setOnCompletionListener(listener);
            mediaPlayerBridge.setOnErrorListener(listener);
            mediaPlayerBridge.setOnPreparedListener(listener);
            mediaPlayerBridge.setOnSeekCompleteListener(listener);
            mediaPlayerBridge.setOnVideoSizeChangedListener(listener);
            /// M: for toast when audio/video is not supported.
            mediaPlayerBridge.setOnInfoListener(listener);
        }
        /// M: to avoid too much toast when OnInfo callback invoked while seeking the video
        sCanHandleOnInfo = true;
        Log.d(TAG, "create: native listener: " + nativeMediaPlayerListener +
                    ", context: " + context + ", bridge: " + mediaPlayerBridge);

        AudioManager am = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
        if (am != null) {
            /// M: for Media button intent handler
            sMediaButton = MediaButtonIntentHandler.getInstance(context);
            am.requestAudioFocus(
                    listener,
                    AudioManager.STREAM_MUSIC,
                    // Request permanent focus.
                    AudioManager.AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK);
            Log.d(TAG, "Audio focus requested and listener created: " + listener);
        }
        return listener;
    }

    /**
     * See media/base/android/media_player_listener.cc for all the following functions.
     */
    private native void nativeOnMediaError(
            long nativeMediaPlayerListener,
            int errorType);

    private native void nativeOnVideoSizeChanged(
            long nativeMediaPlayerListener,
            int width, int height);

    private native void nativeOnBufferingUpdate(
            long nativeMediaPlayerListener,
            int percent);

    private native void nativeOnMediaPrepared(long nativeMediaPlayerListener);

    private native void nativeOnPlaybackComplete(long nativeMediaPlayerListener);

    private native void nativeOnSeekComplete(long nativeMediaPlayerListener);

    private native void nativeOnMediaInterrupted(long nativeMediaPlayerListener);
}
