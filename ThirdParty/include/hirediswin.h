
#ifdef HIREDISDLL
#define MY_EXPORT __declspec(dllexport)
#else
#define MY_EXPORT __declspec(dllimport)

#define redisConnect redisConnectWin
#define redisCommand redisCommandWin
#define freeReplyObject freeReplyObjectWin
#define redisReaderCreate redisReaderCreateWin
#define redisReaderFree redisReaderFreeWin
#define redisReaderFeed redisReaderFeedWin
#define redisReaderGetReply redisReaderGetReplyWin
#define redisvFormatCommand redisvFormatCommandWin
#define redisFormatCommand redisFormatCommandWin
#define redisFormatCommandArgv redisFormatCommandArgvWin
#define redisConnectWithTimeout redisConnectWithTimeoutWin
#define redisConnectNonBlock redisConnectNonBlockWin
#define redisConnectBindNonBlock redisConnectBindNonBlockWin
#define redisConnectUnix redisConnectUnixWin
#define redisConnectUnixWithTimeout redisConnectUnixWithTimeoutWin
#define redisConnectUnixNonBlock redisConnectUnixNonBlockWin
#define redisConnectFd redisConnectFdWin
#define redisSetTimeout redisSetTimeoutWin
#define redisEnableKeepAlive redisEnableKeepAliveWin
#define redisFree redisFreeWin
#define redisFreeKeepFd redisFreeKeepFdWin
#define redisBufferRead redisBufferReadWin
#define redisBufferWrite redisBufferWriteWin
#define redisGetReply redisGetReplyWin
#define redisGetReplyFromReader redisGetReplyFromReaderWin
#define redisAppendFormattedCommand redisAppendFormattedCommandWin
#define redisvAppendCommand redisvAppendCommandWin
#define redisAppendCommand redisAppendCommandWin
#define redisAppendCommandArgv redisAppendCommandArgvWin
#define redisvCommand redisvCommandWin
#define redisCommandArgv redisCommandArgvWin
#endif

#include <stdio.h> /* for size_t */
#include <stdarg.h> /* for va_list */

#ifndef WIN32_INTEROP_TYPES_HIREDIS_H
#define WIN32_INTEROP_TYPES_HIREDIS_H

/* On 64-bit *nix and Windows use different data type models: LP64 and LLP64 respectively.
 * The main difference is that 'long' is 64-bit on 64-bit *nix and 32-bit on 64-bit Windows.
 * The Posix version of Redis makes many assumptions about long being 64-bit and the same size
 * as pointers.
 * To deal with this issue, we replace all occurrences of 'long' in antirez code with our own typedefs,
 * and make those definitions 64-bit to match antirez' assumptions.
 * This enables us to have merge check script to verify that no new instances of 'long' go unnoticed.
*/

typedef __int64           PORT_LONGLONG;
typedef unsigned __int64  PORT_ULONGLONG;
typedef double            PORT_LONGDOUBLE;

#ifdef _WIN64
typedef __int64           ssize_t;
typedef __int64           PORT_LONG;
typedef unsigned __int64  PORT_ULONG;
#else
typedef long              ssize_t;
typedef long              PORT_LONG;
typedef unsigned long     PORT_ULONG;
#endif

#ifdef _WIN64
#define PORT_LONG_MAX     _I64_MAX
#define PORT_LONG_MIN     _I64_MIN
#define PORT_ULONG_MAX    _UI64_MAX
#else
#define PORT_LONG_MAX     LONG_MAX
#define PORT_LONG_MIN     LONG_MIN
#define PORT_ULONG_MAX    ULONG_MAX
#endif

/* The maximum possible size_t value has all bits set */
#define MAX_SIZE_T        (~(size_t)0)

typedef int               pid_t;

#ifndef mode_t
#define mode_t            unsigned __int32
#endif

/* sha1 */
#ifndef u_int32_t
typedef unsigned __int32  u_int32_t;
#endif

#endif




#ifndef __HIREDIS_H
#define __HIREDIS_H
//#include <stdio.h> /* for size_t */
//#include <stdarg.h> /* for va_list */
//#ifndef _WIN32
//#include <sys/time.h> /* for struct timeval */
//#else
////#include "../../src/Win32_Interop/win32_types_hiredis.h"
////#include "C:/redis/include/Win32_Interop/win32_types_hiredis.h"
//#endif

#define HIREDIS_MAJOR 0
#define HIREDIS_MINOR 11
#define HIREDIS_PATCH 0

#define REDIS_ERR -1
#define REDIS_OK 0

/* When an error occurs, the err flag in a context is set to hold the type of
 * error that occured. REDIS_ERR_IO means there was an I/O error and you
 * should use the "errno" variable to find out what is wrong.
 * For other values, the "errstr" field will hold a description. */
#define REDIS_ERR_IO 1 /* Error in read or write */
#define REDIS_ERR_EOF 3 /* End of file */
#define REDIS_ERR_PROTOCOL 4 /* Protocol error */
#define REDIS_ERR_OOM 5 /* Out of memory */
#define REDIS_ERR_OTHER 2 /* Everything else... */

/* Connection type can be blocking or non-blocking and is set in the
 * least significant bit of the flags field in redisContext. */
#define REDIS_BLOCK 0x1

/* Connection may be disconnected before being free'd. The second bit
 * in the flags field is set when the context is connected. */
#define REDIS_CONNECTED 0x2

/* The async API might try to disconnect cleanly and flush the output
 * buffer and read all subsequent replies before disconnecting.
 * This flag means no new commands can come in and the connection
 * should be terminated once all replies have been read. */
#define REDIS_DISCONNECTING 0x4

/* Flag specific to the async API which means that the context should be clean
 * up as soon as possible. */
#define REDIS_FREEING 0x8

/* Flag that is set when an async callback is executed. */
#define REDIS_IN_CALLBACK 0x10

/* Flag that is set when the async context has one or more subscriptions. */
#define REDIS_SUBSCRIBED 0x20

/* Flag that is set when monitor mode is active */
#define REDIS_MONITORING 0x40

#define REDIS_REPLY_STRING 1
#define REDIS_REPLY_ARRAY 2
#define REDIS_REPLY_INTEGER 3
#define REDIS_REPLY_NIL 4
#define REDIS_REPLY_STATUS 5
#define REDIS_REPLY_ERROR 6

#define REDIS_READER_MAX_BUF (1024*16)  /* Default max unused reader buffer. */

#define REDIS_KEEPALIVE_INTERVAL 15 /* seconds */

#ifdef __cplusplus
extern "C" {
#endif

/* This is the reply object returned by redisCommand() */
typedef struct redisReply {
    int type; /* REDIS_REPLY_* */
    PORT_LONGLONG integer; /* The integer when type is REDIS_REPLY_INTEGER */
    int len; /* Length of string */
    char *str; /* Used for both REDIS_REPLY_ERROR and REDIS_REPLY_STRING */
    size_t elements; /* number of elements, for REDIS_REPLY_ARRAY */
    struct redisReply **element; /* elements vector for REDIS_REPLY_ARRAY */
} redisReply;

typedef struct redisReadTask {
    int type;
    int elements; /* number of elements in multibulk container */
    int idx; /* index in parent (array) object */
    void *obj; /* holds user-generated value for a read task */
    struct redisReadTask *parent; /* parent task */
    void *privdata; /* user-settable arbitrary field */
} redisReadTask;

typedef struct redisReplyObjectFunctions {
    void *(*createString)(const redisReadTask*, char*, size_t);
    void *(*createArray)(const redisReadTask*, int);
    void *(*createInteger)(const redisReadTask*, PORT_LONGLONG);
    void *(*createNil)(const redisReadTask*);
    void (*freeObject)(void*);
} redisReplyObjectFunctions;

/* State for the protocol parser */
typedef struct redisReader {
    int err; /* Error flags, 0 when there is no error */
    char errstr[128]; /* String representation of error when applicable */

    char *buf; /* Read buffer */
    size_t pos; /* Buffer cursor */
    size_t len; /* Buffer length */
    size_t maxbuf; /* Max length of unused buffer */

    redisReadTask rstack[9];
    int ridx; /* Index of current read task */
    void *reply; /* Temporary reply pointer */

    redisReplyObjectFunctions *fn;
    void *privdata;
} redisReader;

/* Context for a connection to Redis */
typedef struct redisContext {
    int err; /* Error flags, 0 when there is no error */
    char errstr[128]; /* String representation of error when applicable */
    int fd;
    int flags;
    char *obuf; /* Write buffer */
    redisReader *reader; /* Protocol reader */
} redisContext;

#ifdef __cplusplus
}
#endif

#endif


MY_EXPORT redisContext *redisConnectWin(const char *ip, int port);
MY_EXPORT void *redisCommandWin(redisContext *c, const char *format, ...);
MY_EXPORT void freeReplyObjectWin(void *reply);


MY_EXPORT redisReader *redisReaderCreateWin(void);
MY_EXPORT void redisReaderFreeWin(redisReader *r);
MY_EXPORT int redisReaderFeedWin(redisReader *r, const char *buf, size_t len);
MY_EXPORT int redisReaderGetReplyWin(redisReader *r, void **reply);
MY_EXPORT int redisvFormatCommandWin(char **target, const char *format, va_list ap);
MY_EXPORT int redisFormatCommandWin(char **target, const char *format, ...);
MY_EXPORT int redisFormatCommandArgvWin(char **target, int argc, const char **argv, const size_t *argvlen);
MY_EXPORT redisContext *redisConnectWithTimeoutWin(const char *ip, int port, const struct timeval tv);
MY_EXPORT redisContext *redisConnectNonBlockWin(const char *ip, int port);
MY_EXPORT redisContext *redisConnectBindNonBlockWin(const char *ip, int port, const char *source_addr);
MY_EXPORT redisContext *redisConnectUnixWin(const char *path);
MY_EXPORT redisContext *redisConnectUnixWithTimeoutWin(const char *path, const struct timeval tv);
MY_EXPORT redisContext *redisConnectUnixNonBlockWin(const char *path);
MY_EXPORT redisContext *redisConnectFdWin(int fd);
MY_EXPORT int redisSetTimeoutWin(redisContext *c, const struct timeval tv);
MY_EXPORT int redisEnableKeepAliveWin(redisContext *c);
MY_EXPORT void redisFreeWin(redisContext *c);
MY_EXPORT int redisFreeKeepFdWin(redisContext *c);
MY_EXPORT int redisBufferReadWin(redisContext *c);
MY_EXPORT int redisBufferWriteWin(redisContext *c, int *done);
MY_EXPORT int redisGetReplyWin(redisContext *c, void **reply);
MY_EXPORT int redisGetReplyFromReaderWin(redisContext *c, void **reply);
MY_EXPORT int redisAppendFormattedCommandWin(redisContext *c, const char *cmd, size_t len);
MY_EXPORT int redisvAppendCommandWin(redisContext *c, const char *format, va_list ap);
MY_EXPORT int redisAppendCommandWin(redisContext *c, const char *format, ...);
MY_EXPORT int redisAppendCommandArgvWin(redisContext *c, int argc, const char **argv, const size_t *argvlen);
MY_EXPORT void *redisvCommandWin(redisContext *c, const char *format, va_list ap);
MY_EXPORT void *redisCommandArgvWin(redisContext *c, int argc, const char **argv, const size_t *argvlen);

