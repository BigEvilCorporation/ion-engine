///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		XML.h
// Date:		15th February 2014
// Authors:		Matt Phillips
// Description:	XML reader
///////////////////////////////////////////////////

#pragma once

#include "core/Types.h"
#include "core/cryptography/Hash.h"

#include <string>
#include <map>
#include <vector>

#include <TinyXML/tinyxml2.h>

namespace ion
{
	namespace io
	{
		class XML
		{
		public:
			XML();
			~XML();

			//Load XML doc from file
			bool Load(const std::string& filename);

			//Hierarchy
			int GetNumChildren() const;
			const XML* GetChild(int index) const;
			const XML* FindChild(const std::string& name) const;
			XML* AddChild(const std::string& name);

			//Node data
			const std::string& GetData() const { return m_data; }

			//Attributes
			bool GetAttribute(const std::string& name, std::string& value) const;
			bool GetAttribute(const std::string& name, int& value) const;
			bool GetAttribute(const std::string& name, float& value) const;
			bool GetAttribute(const std::string& name, bool& value) const;

			void SetAttribute(const std::string& name, const std::string& value);
			void SetAttribute(const std::string& name, int value);
			void SetAttribute(const std::string& name, float value);
			void SetAttribute(const std::string& name, bool value);

		protected:
			void ParseTinyXmlElement(const tinyxml2::XMLElement& element);

			std::string m_name;
			std::string m_data;
			std::map<std::string, std::string> m_attributes;
			std::vector< std::pair<u32, XML> > m_children;
		};
	}
}