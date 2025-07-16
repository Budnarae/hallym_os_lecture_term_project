#ifndef SHARED_MEMORY_HPP
#define SHARED_MEMORY_HPP

#include <cstring>
#include "safe_syscall.hpp"
#include "exception.hpp"

#define MAX_SERVER_NUM 5

struct SharedMemory
{
	int     serv_num;
	int     serv_pids[MAX_SERVER_NUM];
	int     serv_ports[MAX_SERVER_NUM];
	char    serv_names[MAX_SERVER_NUM][10];

	SharedMemory() noexcept;
	const SharedMemory  &operator=(const SharedMemory &cp) noexcept;
	void    closeServerWithPid(int pid) noexcept;
	pid_t   getPidWithName(const char *serv_name) const noexcept;
	void    rename(const char *old_name, const char *new_name);
	void    printServerInfo() const noexcept;
	bool    isServerNameValid(const char *name) const noexcept;
};

extern SharedMemory     *shm;
extern pthread_rwlock_t rwlock;

#endif