
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <unordered_map>

#include "command.hpp"
#include "safe_syscall.hpp"
#include "sig_handler.hpp"
#include "parse.hpp"

#define SHARED_MEMORY_KEY 1

using namespace std;

int main()
{
	// 서버들의 정보를 저장할 공유 메모리 초기화
	int shm_id  = shmget_strict(SHARED_MEMORY_KEY, sizeof(SharedMemory), IPC_CREAT | 0666);
	shm = reinterpret_cast<SharedMemory *>(shmat_strict(shm_id, NULL, 0));
	*shm = SharedMemory();

	// data race 대책
	pthread_rwlock_init(&rwlock, NULL);

	// 자식 프로세스(서버) 종료 시 자동으로 wait 실행
	signal(SIGCHLD, sig_chld);

	while (1)
	{
		string          input;
		vector<string>  args;
		Command         *cmd;
		
		getline(cin, input);
		args = parse(input);
		
		try
		{
			if (args[0] == "/create")
				cmd = new Create(args);
			else if (args[0] == "/delete")
				cmd = new Delete(args);
			else if (args[0] == "/rename")
				cmd = new RenameServer(args);
			else if (args[0] == "/list")
				cmd = new ListServer(args);
			else if (args[0] == "/quit")
				cmd = new QuitServer(args);
			else if (args[0] == "/help")
				cmd = new HelpServer(args);
			else
				throw InvalidCommand();

			(*cmd)();
			delete cmd;
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what();
		}
	}
	return 0;
}