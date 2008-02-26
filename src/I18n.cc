#include "I18n.hh"

#include "XML/Xml.hh"
#include "Util/utils.hh"

I18n i18n;

uint32_t I18n::getTextCode(const std::string& text_name) const {
    std::map<std::string, uint32_t>::const_iterator it = this->code_map.find(text_name);
    if(it == this->code_map.end()) {
        return 0xffffffff;
    } else {
        return it->second;
    }
}

const std::string& I18n::getText(const std::string& text_name, const std::string& lang) const {
    std::map<std::string, std::map<std::string, std::string> >::const_iterator it1;
    std::map<std::string, std::string>::const_iterator it2;

    it1 = this->langs.find(lang);
    if(it1 == this->langs.end())
        return text_name;

    it2 = it1->second.find(text_name);
    if(it2 == it1->second.end())
        return text_name;

    return it2->second;
}

void I18n::loadCodeTable(const std::string& filename) {
    std::auto_ptr<XML::Tag> xml(XML::parseXmlFile(filename));

    foreach(text, xml->tags()) {
        uint32_t code = Util::parse_string<uint32_t>(text->getAttribute("code"));
        const std::string& text_name = text->findCData().data();
        this->code_map[text_name] = code;
    }
}

void I18n::loadLang(const std::string& filename) {
    std::auto_ptr<XML::Tag> xml(XML::parseXmlFile(filename));
    std::string lang = xml->getAttribute("lang");
    foreach(child, xml->tags()) {
        const std::string& text_name = child->findChild("name").findCData().data();
        const std::string& text = child->findChild("text").findCData().data();
        this->langs[lang][text_name] = text;
    }
}

void I18n::loadLangs(const std::string& locales_path) {
    this->loadCodeTable(locales_path + "/texts_code.xml");

    this->loadLang(locales_path + "/pt-br.xml");
    this->loadLang(locales_path + "/en.xml");
}
