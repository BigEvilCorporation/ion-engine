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
// Description:	Network address
///////////////////////////////////////////////////

#pragma once

#include <core/Types.h>
#include <core/memory/Memory.h>
#include <core/containers/FixedArray.h>

#include <string>
#include <sstream>

namespace ion
{
	namespace network
	{
		typedef FixedArray<u8,4> IPAddressV4;
		typedef FixedArray<u32, 4> IPAddressV6;
		typedef u32 Port;

		template <typename ADDR_TYPE> class Address
		{
		public:
			Address();
			Address(const ADDR_TYPE& address, const Port& port);
			Address(const std::string& addressString);

			const ADDR_TYPE& GetAddress() const;
			const Port& GetPort() const;
			const void GetAddress(std::string& addressString) const;
			const void GetPort(std::string& portString) const;

		private:
			ADDR_TYPE m_address;
			Port m_port;
		};
	}
}

namespace ion
{
	namespace network
	{
		template <typename ADDR_TYPE> Address<ADDR_TYPE>::Address()
		{
			memory::MemSet(&m_address, 0, sizeof(ADDR_TYPE));
			m_port = 0;
		}

		template <typename ADDR_TYPE> Address<ADDR_TYPE>::Address(const ADDR_TYPE& address, const Port& port)
		{
			memory::MemCopy(&m_address, &address, sizeof(ADDR_TYPE));
			m_port = port;
		}

		template <typename ADDR_TYPE> Address<ADDR_TYPE>::Address(const std::string& addressString)
		{

		}

		template <typename ADDR_TYPE> const ADDR_TYPE& Address<ADDR_TYPE>::GetAddress() const
		{
			return m_address;
		}

		template <typename ADDR_TYPE> const Port& Address<ADDR_TYPE>::GetPort() const
		{
			return m_port;
		}

		template <typename ADDR_TYPE> const void Address<ADDR_TYPE>::GetAddress(std::string& addressString) const
		{
			std::stringstream addressStream;
			addressStream << std::dec;

			int addressSize = m_address.GetSize();
			for(int i = 0; i < addressSize; i++)
			{
				addressStream << (u32)m_address[i];

				if(i != (addressSize - 1))
				{
					addressStream << ".";
				}
			}
			
			addressString = addressStream.str();
		}

		template <typename ADDR_TYPE> const void Address<ADDR_TYPE>::GetPort(std::string& portString) const
		{
			std::stringstream portStream;
			portStream << m_port;
			portString = portStream.str();
		}
	}
}