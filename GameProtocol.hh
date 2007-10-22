#ifndef GAMEPROTOOL_HH
#define GAMEPROTOOL_HH

#include "XML/Xml.hh"

class GameProtocol {
	public:
		GameProtocol(const std::string& path);

		~GameProtocol();

		static std::string parseGameQuery(XML::Tag& query);

		static std::string parseGameRoomQuery(XML::Tag& query);

		static std::string parseGameMove(XML::Tag& query);

	private:
		XML::Description game_query_desc;

};

#endif
