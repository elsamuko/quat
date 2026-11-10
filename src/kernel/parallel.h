#ifndef QUAT_PARALLEL_H
#define QUAT_PARALLEL_H

/* Minimal thread pool interface for the (C) kernel. Implemented in parallel.cpp */

#if defined(_MSC_VER)
#   define QUAT_THREAD_LOCAL __declspec(thread)
#elif defined(__cplusplus)
#   define QUAT_THREAD_LOCAL thread_local
#else
#   define QUAT_THREAD_LOCAL _Thread_local
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void ( *parallel_job )( void* ctx, long index, int thread );

/* number of threads used by parallel_for (including the calling thread) */
int parallel_threads( void );

/* calls job(ctx, i, thread) for i in [0, count) distributed over all threads, */
/* returns when all calls are done. thread is in [0, parallel_threads()), */
/* the calling thread has number 0. */
void parallel_for( long count, parallel_job job, void* ctx );

#ifdef __cplusplus
}
#endif

#endif
