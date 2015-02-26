#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>

int isatty(int fd) {
	typedef int (*FuncPtr)(int);
	static FuncPtr realImpl = NULL;
	if ( !realImpl ) {
		void *const handle = dlopen("libc.so.6", RTLD_LAZY);
		const char *error;
		if ( !handle ) {
			fputs(dlerror(), stderr);
			exit(1);
		}
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wpedantic"
		realImpl = (FuncPtr)dlsym(handle, "isatty");
		#pragma GCC diagnostic pop

		error = dlerror();
		if ( error ) {
			fprintf(stderr, "dlsym() failed: %s\n", error);
			exit(1);
		}
	}
	//printf("Calling isatty(%d)\n", fd);
	if ( fd == STDOUT_FILENO || fd == STDERR_FILENO ) {
		return 1;
	} else {
		return realImpl(fd);
	}
}
