#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define RSCH_WINDOWS 1
#else
#define RSCH_WINDOWS 0
#endif

#if defined(__APPLE__) || defined(__MACOSX)
#define RSCH_MAC 1
#else
#define RSCH_MAC 0
#endif

#if defined (LINUX) || defined (__linux__)
#define RSCH_LINUX 1
#else
#define RSCH_LINUX 0
#endif

#if RSCH_WINDOWS
#include <crtdbg.h>
#elif RSCH_MAC
#include <malloc/malloc.h>
#elif RSCH_LINUX
#include <malloc.h>
#endif

#if RSCH_WINDOWS
#define NOMINMAX
#include <windows.h>
#elif RSCH_MAC || RSCH_LINUX
#include <pthread.h>
#endif

#include <iostream>
#include <functional>
#include <atomic>
#include <string>
#include <array>

namespace ntlab
{
/**
     * A class to catch any allocation going on in the thread that created it while there is an instance of this class
     * on the stack.
     *
     * Use it like this
     *
     * \code
       struct SomeObj
       {
           int a, b, c;
       }

       void myFunc()
       {
           someUncriticalCalls();

           // the start of the section you want to examine
           {
               ntlab::ScopedAllocationDetection allocationDetection;

               // this will trigger the detection
               std::uniqe_ptr<SomeObj> someObj (new SomeObj);

               // perform some library call to see if it allocates under the hood
               callToSomeClosedSourceLibAPI();
           }

           // this won't trigger the detection
           std::uniqe_ptr<SomeObj> someOtherObj (new SomeObj);
       }
     */
class ScopedAllocationDetector
{
public:
    enum OperationsToCatch
    {
        catchMalloc = 1 << 0,
        catchFree = 1 << 1
    };

    using AllocationCallback =
        std::function<void (size_t numBytesAllocated, const std::string* optionalFileAndLine)>;

    /** Constructor. You can pass a custom callback or just use the default callback that prints to stderr */
    ScopedAllocationDetector (OperationsToCatch operationsToCatch = catchMalloc,
                              AllocationCallback allocationCallback = nullptr,
                              AllocationCallback freeCallback = nullptr);

    ~ScopedAllocationDetector();

    /**
         * A callback invoked if an allocation took place. Can be set through the class constructor or reassigned
         * after construction
         */
    AllocationCallback onAllocation = defaultAllocationCallback;

    /**
         * A callback invoked if a call to free took place. Can be set through the class constructor or reassigned
         * after construction
         */
    AllocationCallback onFree = defaultFreeCallback;

private:
    OperationsToCatch operationsToCatch;

#if RSCH_WINDOWS
    using ThreadID = DWORD;
#else
    using ThreadID = pthread_t;
#endif
    struct DetectorProperties
    {
        ThreadID threadId = ThreadID();
        ScopedAllocationDetector* detector = nullptr;
    };

    static const int maxNumDetectors = 16;
    static std::atomic<int> count;
    static std::array<DetectorProperties, maxNumDetectors> activeDetectors;

    static ThreadID getCurrentThreadID();

    static AllocationCallback defaultAllocationCallback;
    static AllocationCallback defaultFreeCallback;

#if RSCH_WINDOWS

    static int windowsAllocHook (int allocType, void* userData, size_t size, int blockType,
                                 long requestNumber, const unsigned char* filename, int lineNumber);

#elif RSCH_MAC

    typedef void* (*MacSystemMalloc) (malloc_zone_t*, size_t);

    static MacSystemMalloc macSystemMalloc;

    static void* detectingMalloc (malloc_zone_t* zone, size_t size);

#elif RSCH_LINUX
    typedef void* (*LinuxMallocHook) (size_t, const void*);

    static LinuxMallocHook originalMallocHook;

    static void* detectingMalloc (size_t size, const void* caller);

#endif
    static void activateDetection();

    static void endDetection();
};

} // namespace ntlab