#include <iostream>
#include <string>

#include "Core.hh"

using namespace std;

const char* secret = "secret";

void log_traffic(const std::string& data, bool incoming) {
	cout << "-----------------------------" << endl;
	cout << (incoming ? "Incoming!" : "Outcoming!") << endl;
	cout << "-----------" << endl;
	cout << data << endl;
	cout << "-----------------------------" << endl;

}

int main(void) {
	try {
		XML::Tag* config = XML::loadXmlFile("config.xml");
		Core::init(*config);
		Core& core = Core::singleton();
		delete config;
		try {
			core.connect();
		} catch(const char* error) {
			cout << error << endl;
			return 1;
		}
		string cmd;
		cin >> cmd;
		Core::destroy();
	} catch (const char* msg) {
		cout << "Error: " << msg << endl;
	}
	return 0;
	/* Stream stream("jabber:component:accept");
	Dispatcher stream_dispatch;
	Dispatcher component_dispatch;
	Component component(stream_dispatch.createWrapper(sigc::slot<void, Tag*>(sigc::mem_fun(&stream, &Stream::send))), secret);
	StreamListener stream_listener(component_dispatch.createWrapper(sigc::slot<void, Tag*>(sigc::mem_fun(&component, &Component::handleTag))), &stream);
	stream.setLogHandler(sigc::ptr_fun(log_traffic));
	component.setConnectionHandler(sigc::ptr_fun(connection_handler));
	StanzaSender sender = sigc::mem_fun(component, &Component::send);
	JabberNode* node = new JabberNode(JID("chess.shiva"), sender);
	component.setRootStanzaHandler(mem_fun(*node, &JabberNode::handleStanza));
	Disco* disco = new Disco(sender);
	node->setIqHandler(sigc::mem_fun(*disco, &Disco::handleIqInfo), 
			"http://jabber.org/protocol/disco#info");
	node->setIqHandler(sigc::mem_fun(*disco, &Disco::handleIqItems), 
			"http://jabber.org/protocol/disco#items");
	DiscoNode* dnode = new DiscoNode("Servidor de Xadrez", "bot",
			"chess.server");
	dnode->features().insert("http://c3sl.ufpr.br/xadrez");
	dnode->items().insert(DiscoItem("jogo1", JID("jogo1@chess.shiva")));
	dnode->items().insert(DiscoItem("jogo2", JID("jogo2@chess.shiva")));
	disco->nodes().insert(make_pair(make_pair(JID("chess.shiva"), ""), dnode));
	JabberNode* nodej1 = new JabberNode(JID("jogo1@chess.shiva"), sender);
	JabberNode* nodej2 = new JabberNode(JID("jogo2@chess.shiva"), sender);
	component.setStanzaHandler(mem_fun(*nodej1, &JabberNode::handleStanza),
			"jogo1");
	component.setStanzaHandler(mem_fun(*nodej2, &JabberNode::handleStanza),
			"jogo2");
	Disco* discojogo1 = new Disco(sender);
	Disco* discojogo2 = new Disco(sender);
	nodej1->setIqHandler(sigc::mem_fun(*discojogo1, &Disco::handleIqInfo), 
			"http://jabber.org/protocol/disco#info");
	nodej1->setIqHandler(sigc::mem_fun(*discojogo1, &Disco::handleIqItems), 
			"http://jabber.org/protocol/disco#items");
	nodej2->setIqHandler(sigc::mem_fun(*discojogo2, &Disco::handleIqInfo), 
			"http://jabber.org/protocol/disco#info");
	nodej2->setIqHandler(sigc::mem_fun(*discojogo2, &Disco::handleIqItems), 
			"http://jabber.org/protocol/disco#items");
	DiscoNode* dnodej1 = new DiscoNode("Jogo 1", "game", "chess");
	Muc* muc = new Muc(sender, dnodej1, JID("jogo1@chess.shiva"));
	discojogo1->nodes().insert(make_pair(make_pair(JID("jogo1@chess.shiva"),""),
				dnodej1));
	discojogo2->nodes().insert(make_pair(make_pair(JID("jogo2@chess.shiva"),""),
				new DiscoNode("Jogo 2", "game", "chess")));
	nodej1->setPresenceHandler(sigc::mem_fun(muc, &Muc::handlePresence));
	nodej1->setMessageHandler(sigc::mem_fun(muc, &Muc::handleGroupChat), "groupchat");
	cout << "connecting..." << endl;
	if(stream.connect("localhost", 5560)!=0) {
		cout << "connectio failed" << endl;
		return 1;
	}
	stream_listener.start();
	stream_dispatch.start();
	component_dispatch.start();
	cout << "authing..." << endl;
	string cmd;
	while(cin >> cmd and cmd!="quit");
	stream_listener.shutdown();
	stream_dispatch.shutdown();
	component_dispatch.shutdown();
	stream_listener.join();
	stream_dispatch.join();
	component_dispatch.join();

	return 0; */
}
