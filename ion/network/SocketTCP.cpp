///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		SocketTCP.cpp
// Date:		23rd June 2016
// Authors:		Matt Phillips
// Description:	Network TCP socket base
///////////////////////////////////////////////////

#include <core/debug/Debug.h>
#include "SocketTCP.h"

#include <sstream>
#include <iomanip>

namespace ion
{
	namespace network
	{
		SocketTCP::SocketTCP()
			: m_winSocket(INVALID_SOCKET)
		{
			int result = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
			if(result != 0)
			{
				debug::Error("Error initialising WinSock");
			}
		}

		SocketTCP::~SocketTCP()
		{
			if(m_winSocket != INVALID_SOCKET)
			{
				shutdown(m_winSocket, SD_SEND);
				closesocket(m_winSocket);
				WSACleanup();
			}
		}

		bool SocketTCP::Listen(const Address<IPAddressV4>& address)
		{
			return false;
		}

		bool SocketTCP::Connect(const Address<IPAddressV4>& address)
		{
			addrinfo* result = NULL;
			addrinfo hints;

			memory::MemSet(&hints, 0, sizeof(hints));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;

			//Get address and port as string
			std::string addressString;
			std::string portString;
			address.GetAddress(addressString);
			address.GetPort(portString);

			//Query address info
			int getAddrResult = getaddrinfo(addressString.c_str(), portString.c_str(), &hints, &result);
			if(getAddrResult == 0)
			{
				//Create socket
				m_winSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

				if(m_winSocket != INVALID_SOCKET)
				{
					//Connect
					int connectResult = connect(m_winSocket, result->ai_addr, (int)result->ai_addrlen);

					//Cleanup addr info
					freeaddrinfo(result);

					if(connectResult != SOCKET_ERROR)
					{
						//Success
						return true;
					}
					else
					{
						//Failure, delete socket and shutdown WSA
						closesocket(m_winSocket);
						m_winSocket = INVALID_SOCKET;
						WSACleanup();
						return false;
					}
				}
				else
				{
					//Failure, cleanup addr info and shutdown WSA
					freeaddrinfo(result);
					WSACleanup();
					return false;
				}
			}
			else
			{
				//Failure, shutdown WSA
				WSACleanup();
				return false;
			}
		}

		int SocketTCP::Send(const std::vector<u8>& data)
		{
			int result = send(m_winSocket, (const char*)data.data(), data.size(), 0);
			return (result == SOCKET_ERROR) ? -1 : result;
		}

		int SocketTCP::Recv(std::vector<u8>& data)
		{
			int result = recv(m_winSocket, (char*)data.data(), data.size(), 0);
			return (result == SOCKET_ERROR) ? -1 : result;
		}
	}
}