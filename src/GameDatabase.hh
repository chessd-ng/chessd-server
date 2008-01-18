#ifndef GAMEDATABASE_HH
#define GAMEDATABASE_HH

#include <string>
#include <memory>
#include <vector>

#include <pqxx/pqxx>

struct PersistentGame {
    public:
        //int game_id;
        std::vector<std::string> players;
        std::string category;
        std::string result;
        int time_stamp;
        std::string history;
};

class GameDatabase {
    public:

        GameDatabase(pqxx::work& w);

        //PersistentGame* getGame(int game_id) const;

        void insertGame(const PersistentGame& game);

    private:
        pqxx::work& work;
};

#endif
