#ifndef GAMEPROTOOL_HH
#define GAMEPROTOOL_HH

#include "XMP/Xml.hh"

class GameProtocol {
	public:
		GameProtocol(const std::string& path);

		~GameProtocol();

		static std::string parseGameQuery(XML::Tag& query);
	private:
		XMP::Description game_query_desc;

};

#endif
