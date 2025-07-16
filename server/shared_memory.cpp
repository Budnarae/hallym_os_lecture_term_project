#include "shared_memory.hpp"

SharedMemory::SharedMemory() noexcept : serv_num(0)
{
	for (int i = 0; i < MAX_SERVER_NUM; i++)
		bzero(serv_names[i], sizeof(char) * 10);
}

const SharedMemory  &SharedMemory::operator=(const SharedMemory &cp) noexcept
{
	serv_num = cp.serv_num;
	for (int i = 0; i < MAX_SERVER_NUM; i++)
	{
		serv_ports[i] = cp.serv_ports[i];
		bzero(serv_names[i], sizeof(char) * 10);
		strcpy(serv_names[i], cp.serv_names[i]);
	}
	return *this;
}

void  SharedMemory::closeServerWithPid(int pid) noexcept
{
	pthread_rwlock_wrlock_naive(&rwlock);
	for (int i = 0; i < serv_num; i++)
	{
		if (serv_pids[i] == pid)
		{
			for (int j = i; j < serv_num - 1; j++)
			{
				serv_pids[j] = serv_pids[j + 1];
				serv_ports[j] = serv_ports[j + 1];
				bzero(serv_names[j], sizeof(char) * 10);
				strcpy(serv_names[j], serv_names[j + 1]);
			}
			for (int k = serv_num; k < MAX_SERVER_NUM; k++)
			{
				serv_pids[k] = 0;
				serv_ports[k] = 0;
				bzero(serv_names[k], sizeof(char) * 10);
			}
			serv_num--;
			break;
		}
	}
	pthread_rwlock_unlock_naive(&rwlock);
}

// 임계 구역의 자원을 읽는 동시에 반환하기 때문에 함수 밖에서 rwlock을 호출해야 함함
pid_t   SharedMemory::getPidWithName(const char *serv_name) const noexcept
{
	for (int i = 0; i < serv_num; i++)
	{
		if (strcmp(serv_names[i], serv_name) == 0)
		{
			return (serv_pids[i]);
		}
	}
	return -1;
}

void    SharedMemory::rename(const char *old_name, const char *new_name)
{
	pthread_rwlock_wrlock_naive(&rwlock);
	for (int i = 0; i < serv_num; i++)
	{
		if (strcmp(serv_names[i], old_name) == 0)
		{
			bzero(serv_names[i], sizeof(char) * 10);
			strcpy(serv_names[i], new_name);
			pthread_rwlock_unlock_naive(&rwlock);
			return ;
		}
	}
	pthread_rwlock_unlock_naive(&rwlock);
	throw InvalidName();
}

void    SharedMemory::printServerInfo() const noexcept
{
	pthread_rwlock_rdlock_naive(&rwlock);
	printf("==============================\n");
	printf("Current Server Number : %d\n", serv_num);
	for (int i = 0; i < serv_num; i++)
	{
		printf("++++++++++++++++++++++++++++++\n");
		printf("Server Name : %s\n", serv_names[i]);
		printf("Server Process Id : %d\n", serv_pids[i]);
		printf("Server Port : %d\n", serv_ports[i]);
	}
	printf("==============================\n");
	pthread_rwlock_unlock_naive(&rwlock);
}

bool    SharedMemory::isServerNameValid(const char *name) const noexcept
{
	for (int i = 0; i < shm->serv_num; i++)
	{
		if (strcmp(shm->serv_names[i], name) == 0)
			return true;
	}
	return false;
}

SharedMemory        *shm;
pthread_rwlock_t    rwlock;