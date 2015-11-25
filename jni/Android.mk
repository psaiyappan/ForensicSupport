LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
	./jni/regex \
	./jni/libinotifytools

LOCAL_MODULE    := inotifywait
LOCAL_SRC_FILES := wrap.c common.c libinotifytools/inotifytools.c libinotifytools/redblack.c
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)