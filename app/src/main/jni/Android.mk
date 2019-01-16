LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := encrypt
LOCAL_SRC_FILES := main.c
LOCAL_LDLIBS	:= -llog -lz
include $(BUILD_SHARED_LIBRARY)