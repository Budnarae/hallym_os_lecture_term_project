#include "safe_syscall.hpp"

void    print_error(const char *sys) noexcept
{
	fprintf(stderr, "Error : %s failed - Errno : %d\n", sys, errno);
}

void    print_error_exit(const char *sys) noexcept
{
	print_error(sys);
	exit(1);
}

int shmget_strict(key_t key, size_t size, int shmflg) noexcept
{
	int shm_id = shmget(key, size, shmflg);
	if (shm_id == -1)
		print_error_exit("shmget");
	return shm_id;
}

void    *shmat_strict(int shmid, const void *shmaddr, int shmflg) noexcept
{
	void    *shm = shmat(shmid, shmaddr, shmflg);
	if (shm == (void *)-1)
		print_error_exit("shmat");
	return shm;
}

int     socket_strict(int domain, int type, int protocol) noexcept
{
	int sock_fd = socket(domain, type, protocol);
	if (sock_fd == -1)
		print_error_exit("socket");
	return sock_fd;
}

int     bind_strict(int fd, const sockaddr *addr, socklen_t len) noexcept
{
	int ret = bind(fd, addr, len);
	if (ret == -1)
		print_error_exit("bind");
	return ret;
}

int     listen_strict(int sock, int backlog) noexcept
{
	int ret = listen(sock, backlog);
	if (ret == -1)
		print_error_exit("listen");
	return ret;
}

int     fcntl_strict(int fd, int cmd) noexcept
{
	int ret = fcntl(fd, cmd);
	if (ret == -1)
		print_error_exit("fcntl");
	return ret;
}

int     fcntl_strict(int fd, int cmd, int args) noexcept
{
	int ret = fcntl(fd, cmd, args);
	if (ret == -1)
		print_error_exit("fcntl");
	return ret;
}

int     epoll_create_strict(int size) noexcept
{
	int epoll_fd = epoll_create(size);
	if (epoll_fd == -1)
		print_error_exit("epoll_create");
	return epoll_fd;
}

int     epoll_ctl_strict(int epfd, int op, int fd, epoll_event *event) noexcept
{
	int ret = epoll_ctl(epfd, op, fd, event);
	if (ret == -1)
		print_error_exit("epoll_ctl");
	return ret;
}

pid_t   fork_naive() noexcept
{
	pid_t   pid = fork();
	if (pid == -1)
		print_error("fork");
	return pid;
}

int     pthread_rwlock_wrlock_naive(pthread_rwlock_t *rwlock) noexcept
{
	int ret = pthread_rwlock_wrlock(rwlock);
	if (ret != 0)
		print_error("pthread_rwlock_wrlock");
	return ret;
}

int     pthread_rwlock_rdlock_naive(pthread_rwlock_t *rwlock) noexcept
{
	int ret = pthread_rwlock_rdlock(rwlock);
	if (ret != 0)
		print_error("pthread_rwlock_wrlock");
	return ret;
}

int     pthread_rwlock_unlock_naive(pthread_rwlock_t *rwlock) noexcept
{
	int ret = pthread_rwlock_unlock(rwlock);
	if (ret != 0)
		print_error("pthread_rwlock_unlock");
	return ret;
}

int     kill_naive(pid_t pid, int sig) noexcept
{
	int ret = kill(pid, sig);
	if (ret == -1)
		print_error("kill");
	return ret;
}

int     epoll_wait_naive(int epfd, epoll_event *events, int maxevents, int timeout) noexcept
{
	int event_count = epoll_wait(epfd, events, maxevents, timeout);
	if (event_count == -1)
		print_error("epoll_wait");
	return event_count;
}

int     epoll_ctl_naive(int epfd, int op, int fd, epoll_event *event) noexcept
{
	int ret = epoll_ctl(epfd, op, fd, event);
	if (ret == -1)
		print_error("epoll_ctl");
	return ret;
}

int     accept_naive(int fd, sockaddr *addr, socklen_t *addr_len) noexcept
{
	int clnt_fd = accept(fd, addr, addr_len);
	if (clnt_fd == -1)
		print_error("accept");
	return clnt_fd;
}

int     fcntl_naive(int fd, int cmd) noexcept
{
	int ret = fcntl(fd, cmd);
	if (ret == -1)
		print_error("fcntl");
	return ret;
}

int     fcntl_naive(int fd, int cmd, int args) noexcept
{
	int ret = fcntl(fd, cmd, args);
	if (ret == -1)
		print_error("fcntl");
	return ret;
}

int     socket_naive(int domain, int type, int protocol) noexcept
{
	int sock_fd = socket(domain, type, protocol);
	if (sock_fd == -1)
		print_error("socket");
	return sock_fd;
}

int		connect_naive(int fd, const sockaddr *addr, socklen_t len) noexcept
{
	int sock = connect(fd, addr, len);
	if (sock == -1)
		print_error("connect");
	return sock;
}
