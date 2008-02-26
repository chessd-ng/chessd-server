#ifndef I18N_HH
#define I18N_HH

#include <map>
#include <string>
#include <stdint.h>

class I18n {
    public:

        uint32_t getTextCode(const std::string& text_name) const;

        const std::string& getText(const std::string& text_name, const std::string& lang) const;

        void loadLangs(const std::string& locales_path);

        //const std::string& getText(uint32_t text_code, const std::string lang) const;

    private:

        void loadCodeTable(const std::string& file_name);

        void loadLang(const std::string& file_name);

        
        std::map<std::string, uint32_t> code_map;

        std::map<std::string, std::map<std::string, std::string> > langs;
};

extern I18n i18n;

#endif
