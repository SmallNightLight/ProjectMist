#pragma once
#include <string>

namespace NetworkLib
{
	class IClient
	{
	public:
		virtual ~IClient() = default;
		virtual bool HasMessages() = 0;
		virtual void Send(const std::string& message) = 0;
		virtual std::string PopMessage() = 0;
	};
}