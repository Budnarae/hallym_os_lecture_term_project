#include "exception.hpp"

/* 예외 조건 확인에 사용되는 함수들 */

bool isArgsBlank(const vector<string> &args) noexcept
{
	for (const string &s : args)
	{
		if (s.size() == 0)
			return true;
	}
	return false;
}

bool isCharInvalid(const vector<string> &args) noexcept
{
	for (int i = 1, s = args.size(); i < s; i++)
	{
		for (char c : args[i])
		{
			if (!((c >= '0' && c <= '9') || \
				(c >= 'a' && c <= 'z') || \
				(c >= 'A' && c <= 'Z') || \
				(c == ' ')))
				return true;
		}
	}
	return false;
}

/* 예외 객체들 */

const char *InvalidCommand::what() const noexcept
{
	return "Error : Invalid command. Please check command list by using /help command.\n";
}

const char *WrongArgsNum::what() const noexcept
{
	return "Error : Wrong command format. Please check command format by using /help command.\n";
}

const char *InvalidChar::what() const noexcept
{
	return "Error : You can use only alphabet and number to compose argument.\n";
}

const char *BlankArgs::what() const noexcept
{
	return "Error : You can't use blank as argument.\n";
}

const char *TooLongName::what() const noexcept
{
	return "Error : Length of Server name cannot exceed 9.\n";
}

const char *NotConnected::what() const noexcept
{
	return "Error : You are not connected yet.\n";
}

const char *AlreadyConnected::what() const noexcept
{
	return "Error : You are already connected.\n";
}

const char *TooManyServer::what() const noexcept
{
	return "Error : You can't build server more than 5.\n";
}

const char *NoServerToDelete::what() const noexcept
{
	return "Error : There's no server to delete.\n";
}

const char *DuplicateName::what() const noexcept
{
	return "Error : There's already same name. Please check name list by using /list command.\n";
}

const char *InvalidName::what() const noexcept
{
	return "Error : There's no such name. Please check valid name by using /list command.\n";
}

const char *InvalidOption::what() const noexcept
{
	return "Error : There's no such option. Please check valid option by using /help command.\n";
}

const char *NotChannelOperator::what() const noexcept
{
	return "Error : You are not a channel operator.\n";
}

const char *NotInChannel::what() const noexcept
{
	return "Error : Target is not in the channel.\n";
}