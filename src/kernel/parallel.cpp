#include "parallel.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace {

class Pool {
    public:
        Pool() {
            unsigned n = std::thread::hardware_concurrency();

            for( unsigned i = 1; i < n; ++i ) {
                mWorkers.emplace_back( [this, i] { loop( ( int )i ); } );
            }
        }

        ~Pool() {
            {
                std::lock_guard<std::mutex> lock( mMutex );
                mQuit = true;
            }
            mStart.notify_all();

            for( std::thread& t : mWorkers ) {
                t.join();
            }
        }

        int size() const {
            return ( int )mWorkers.size() + 1;
        }

        void run( long count, parallel_job job, void* ctx ) {
            if( count <= 0 ) {
                return;
            }

            if( mWorkers.empty() || count == 1 ) {
                for( long i = 0; i < count; ++i ) {
                    job( ctx, i, 0 );
                }

                return;
            }

            {
                std::lock_guard<std::mutex> lock( mMutex );
                mJob = job;
                mCtx = ctx;
                mCount = count;
                mNext = 0;
                mBusy = mWorkers.size();
                ++mGeneration;
            }
            mStart.notify_all();

            work( 0 );

            std::unique_lock<std::mutex> lock( mMutex );
            mDone.wait( lock, [this] { return mBusy == 0; } );
        }

    private:
        void work( int thread ) {
            long i;

            while( ( i = mNext.fetch_add( 1 ) ) < mCount ) {
                mJob( mCtx, i, thread );
            }
        }

        void loop( int thread ) {
            unsigned long seen = 0;

            for( ;; ) {
                {
                    std::unique_lock<std::mutex> lock( mMutex );
                    mStart.wait( lock, [&] { return mQuit || mGeneration != seen; } );

                    if( mQuit ) {
                        return;
                    }

                    seen = mGeneration;
                }

                work( thread );

                {
                    std::lock_guard<std::mutex> lock( mMutex );

                    if( --mBusy == 0 ) {
                        mDone.notify_one();
                    }
                }
            }
        }

        std::vector<std::thread> mWorkers;
        std::mutex mMutex;
        std::condition_variable mStart, mDone;
        unsigned long mGeneration = 0;
        size_t mBusy = 0;
        bool mQuit = false;

        parallel_job mJob = nullptr;
        void* mCtx = nullptr;
        long mCount = 0;
        std::atomic<long> mNext{0};
};

Pool& pool() {
    static Pool p;
    return p;
}

}

extern "C" int parallel_threads( void ) {
    return pool().size();
}

extern "C" void parallel_for( long count, parallel_job job, void* ctx ) {
    pool().run( count, job, ctx );
}
