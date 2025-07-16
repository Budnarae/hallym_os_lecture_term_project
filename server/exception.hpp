#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <exception>
#include <string>
#include <vector>

using namespace std;

/* 예외 조건 확인에 사용되는 함수들 */

bool isArgsBlank(const vector<string> &args) noexcept;
bool isCharInvalid(const vector<string> &args) noexcept;

/* 예외 객체들 */

class InvalidCommand : public exception
{
	public :
	virtual const char *what() const noexcept;
};

class WrongArgsNum : public exception
{
	public :
	virtual const char *what() const noexcept;
};

class InvalidChar : public exception
{
	public :
	virtual const char *what() const noexcept;
};

class BlankArgs : public exception
{
	public :
	virtual const char *what() const noexcept;
};

class TooLongName : public exception
{
	public :
	virtual const char *what() const noexcept;
};

class NotConnected : public exception
{
	public :
	virtual const char *what() const noexcept;
};

class AlreadyConnected : public exception
{
	public :
	virtual const char *what() const noexcept;
};

class TooManyServer : public exception
{
	public :
	virtual const char *what() const noexcept;
};

class NoServerToDelete : public exception
{
	public :
	virtual const char *what() const noexcept;
};

class DuplicateName : public exception
{
	public :
	virtual const char *what() const noexcept;
};

class InvalidName : public exception
{
	public :
	virtual const char *what() const noexcept;
};

class InvalidOption : public exception
{
	public :
	virtual const char *what() const noexcept;
};

class NotChannelOperator : public exception
{
	public :
	virtual const char *what() const noexcept;
};

class NotInChannel : public exception
{
	public :
	virtual const char *what() const noexcept;
};

#endif