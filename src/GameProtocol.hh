#ifndef GAMEPROTOOL_HH
#define GAMEPROTOOL_HH

#include "XML/Xml.hh"

class GameProtocol {
	public:
		static std::string parseQuery(XML::Tag& query);

		static std::string parseMove(XML::Tag& query);

		static void init(const std::string& path);

		//static std::string parseGameRoomQuery(XML::Tag& query);

	private:
		GameProtocol() { }

		~GameProtocol() { }

		static XML::Description query_desc;
		static XML::Description move_desc;

};

#endif
