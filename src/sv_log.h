#ifndef __libsv_sv_log_h__
#define __libsv_sv_log_h__

#include <time.h>
#include <string.h>
#include <stdio.h>

#define SV_LOG_LEVEL_NONE 0x00
#define SV_LOG_LEVEL_ERROR 0x01
#define SV_LOG_LEVEL_INFO 0x02
#define SV_LOG_LEVEL_DEBUG 0x03
#define SV_NEW_LINE "\n"

#define _FILE strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__

#ifndef SV_LOG_TAG
#define SV_LOG_TAG "libsv"
#endif

#ifndef SV_LOG_LEVEL
#define SV_LOG_LEVEL SV_LOG_LEVEL_DEBUG
#endif

#ifdef __ANDROID__

#define SV_LOG_FMT "| %-7s | %-15s | %s:%d | "
#define SV_LOG_ARGS _FILE, __FUNCTION__, __LINE__
#define SV_PRINT_FUNC(prio, format, ...) __android_log_print(prio, SV_LOG_TAG, SV_LOG_FMT format, SV_LOG_ARGS, __VAR_ARGS__)

#if SV_LOG_LEVEL >= SV_LOG_LEVEL_DEBUG
LOGD(format, ...)
SV_PRINT_FUNC(ANDROID_LOG_DEBUG, format SV_NEW_LINE, __VA_ARGS__)
#else
LOGD(format, ...)
#endif

#if SV_LOG_LEVEL >= SV_LOG_LEVEL_INFO
    LOGI(format, ...) SV_PRINT_FUNC(ANDROID_LOG_INFO, format SV_NEW_LINE, __VA_ARGS__)
#else
LOGI(format, ...)
#endif

#if SV_LOG_LEVEL >= SV_LOG_LEVEL_ERROR
        LOGE(format, ...) SV_PRINT_FUNC(ANDROID_LOG_ERROR, format SV_NEW_LINE, __VA_ARGS__)
#else
LOGE(format, ...)
#endif

#else // __ANDROID__

static inline char *timenow();
#define SV_LEVEL_TAG_ERROR "ERROR"
#define SV_LEVEL_TAG_INFO "INFO"
#define SV_LEVEL_TAG_DEBUG "DEBUG"
#define SV_LOG_FMT "%s | %-5s | %-15s | %s:%d | "
#define SV_LOG_ARGS(tag) timenow(), tag, _FILE, __FUNCTION__, __LINE__

#define SV_PRINT_FUNC(format, ...) fprintf(stderr, format, __VA_ARGS__)

#if SV_LOG_LEVEL >= SV_LOG_LEVEL_DEBUG
#define LOGD(format, args...) SV_PRINT_FUNC(SV_LOG_FMT format SV_NEW_LINE, SV_LOG_ARGS(SV_LEVEL_TAG_DEBUG), ##args)
#else
#define LOGD(format, args...)
#endif

#if SV_LOG_LEVEL >= SV_LOG_LEVEL_INFO
#define LOGI(format, args...) SV_PRINT_FUNC(SV_LOG_FMT format SV_NEW_LINE, SV_LOG_ARGS(SV_LEVEL_TAG_INFO), ##args)
#else
#define LOGI(format, args...)
#endif

#if SV_LOG_LEVEL >= SV_LOG_LEVEL_ERROR
#define LOGE(format, args...) SV_PRINT_FUNC(SV_LOG_FMT format SV_NEW_LINE, SV_LOG_ARGS(SV_LEVEL_TAG_ERROR), ##args)
#else
#define LOGE(format, args...)
#endif

static inline char *timenow()
{
    static char buffer[64];
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", timeinfo);

    return buffer;
}
#endif // __ANDROID__

#endif // __libsv_sv_log_h__
