#include "sig_handler.hpp"

void    sig_chld(int sig) noexcept
{
	(void)sig;

	pid_t pid;
	int status;

	pid = wait(&status);
	shm->closeServerWithPid(pid);
	printf("child %d terminated with %d exit status\n", pid, status);
}