/**    chat_client **/
 
#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

using namespace std;

#define BUF_SIZE 4096
#define NORMAL_SIZE 20
 
void* send_msg(void* arg);
void* recv_msg(void* arg);
void error_handling(const char* msg);
 
int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void* thread_return;
 
    if (argc!=3)
    {
        printf(" Usage : %s <ip> <port>\n", argv[0]);
        exit(1);
    }
 
	char	*clnt_ip = argv[1];
	int serv_port = atoi(argv[2]);
    sock=socket(PF_INET, SOCK_STREAM, 0);
 
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(clnt_ip);
    serv_addr.sin_port = htons(serv_port);
 
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling(" connect() error");
 
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    close(sock);
    return 0;
}
 
void* send_msg(void* arg)
{
    int sock=*((int*)arg);
 
    /** request command guide **/
    const char	*help = "/help\n";
    write(sock, help, strlen(help));
 
    while(1)
    {
        string input;
		if (!getline(cin, input, '\n')) {
			// Ctrl+D 또는 입력 스트림 오류 발생
			cout << "입력 스트림에 오류가 발생했습니다. 서버에 종료 요청을 보냅니다." << endl;
			string quit_cmd = "/quit\n";
			write(sock, quit_cmd.c_str(), quit_cmd.length());
			close(sock);
			return (void *)0;
		}
		input += "\n";	
		
        write(sock, input.c_str(), input.length());
		if (input == "/quit\n")
		{
			close(sock);
			return (void *)0;
		}
    }
	close(sock);
    return NULL;
}
 
void* recv_msg(void* arg)
{
    int sock=*((int*)arg);
    char msg[BUF_SIZE];
    int str_len;
 
    while(1)
    {
		bzero(msg, sizeof(char) * BUF_SIZE);
        str_len = read(sock, msg, BUF_SIZE);
        if (str_len <= 0)
		{
			close(sock);
			return (void *)0;
		}
        fputs(msg, stdout);
    }
	close(sock);
    return NULL;
}
 
void error_handling(const char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}