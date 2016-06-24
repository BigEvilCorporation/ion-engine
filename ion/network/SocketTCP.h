///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		SocketTCP.h
// Date:		23rd June 2016
// Authors:		Matt Phillips
// Description:	Network TCP socket base
///////////////////////////////////////////////////

#pragma once

#include "Socket.h"

#if defined ION_PLATFORM_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2def.h>
#endif

namespace ion
{
	namespace network
	{
		class SocketTCP : public Socket
		{
		public:
			SocketTCP();
			~SocketTCP();

			virtual bool Listen(const Address<IPAddressV4>& address);
			virtual bool Connect(const Address<IPAddressV4>& address);
			virtual int Send(const std::vector<u8>& data);
			virtual int Recv(std::vector<u8>& data);

		private:
#if defined ION_PLATFORM_WINDOWS
			//TODO: Move to common location
			WSADATA m_wsaData;

			//Winsock socket
			SOCKET m_winSocket;
#endif
		};
	}
}