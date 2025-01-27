#pragma once

extern "C"
{
    typedef register_t mc_register_t;
    typedef ucontext_t SceDbgUcontext;
    typedef void (*SceDbgExceptionHandler)(int, SceDbgUcontext *);

    // Threading structure
    struct pthread_attr
    {
        int sched_policy;
        int sched_inherit;
        int prio;
        int suspend;
#define THR_STACK_USER 0x100 /* 0xFF reserved for <pthread.h> */
        int flags;
        void *stackaddr_attr;
        size_t stacksize_attr;
        size_t guardsize_attr;
        /* cpuset_t *cpuset; */
        size_t cpusetsize;
    };

    // Thread structure
    struct thread
    {
        int tid;
        char _0x04[0x34];
        thread *next;
        char _0x40[0x40];
        void *(*routine)(void *);
        void *arg;
        pthread_attr attr;
    };
}
