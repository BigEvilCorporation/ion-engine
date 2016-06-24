///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Socket.h
// Date:		23rd June 2016
// Authors:		Matt Phillips
// Description:	Network socket base
///////////////////////////////////////////////////

#pragma once

#include <core/Types.h>
#include "Address.h"

#include <vector>

#if defined ION_PLATFORM_WINDOWS
#include <winsock2.h>
#endif

namespace ion
{
	namespace network
	{
		class Socket
		{
		public:
			Socket();
			
			virtual bool Listen(const Address<IPAddressV4>& address) = 0;
			virtual bool Connect(const Address<IPAddressV4>& address) = 0;
			virtual int Send(const std::vector<u8>& data) = 0;
			virtual int Recv(std::vector<u8>& data) = 0;
		};
	}
}