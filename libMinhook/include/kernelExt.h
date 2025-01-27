#pragma once

extern "C"
{
	int get_page_table_stats(int vm, unsigned long long Table, int *totalOut, int *AvailableOut);
	int sysctlbyname(const char *name, void *oldp, size_t *oldlenp, const void *newp, size_t newlen);
	int sysctl(int *name, unsigned int namelen, void *oldp, size_t *oldlenp, void *newp, size_t newlen);
	int ptrace(int request, int pid, void *addr, int data);
	int wait4(int wpid, int *status, int options, struct rusage *rusage);
	int waitpid(int wpid, int *status, int options);
	int ioctl(int fd, unsigned long request, ...);
	int getpid(void);
	int sceDbgInstallExceptionHandler(int en, SceDbgExceptionHandler handler);
	int sceDbgRemoveExceptionHandler(int en);
	int sigemptyset(sigset_t *set);

	// thread
	int pthread_suspend_user_context_np(thread *thr);
	int pthread_resume_user_context_np(thread *thr);
	int pthread_get_user_context_np(thread *thr, SceDbgUcontext *context);
	int pthread_set_user_context_np(thread *thr, SceDbgUcontext *context);
	int pthread_suspend_np(thread *thr);
	int pthread_resume_np(thread *thr);
	int pthread_suspend_all_np();
	int pthread_resume_all_np();
}
