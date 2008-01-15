#ifndef DATABASE_HH
#define DATABASE_HH

#include <string>

#include <pqxx/pqxx>

#include <boost/bind.hpp>

#include "Threads/Task.hh"

#include "Threads/Queue.hh"

struct DatabaseManagerParams {
    std::string db_name;
    std::string host;
    int port;
    std::string user;
    std::string password;

    DatabaseManagerParams(
            std::string db_name,
            std::string host,
            int port,
            std::string user,
            std::string password);
};


class DatabaseManager {
    public:

        DatabaseManager(const DatabaseManagerParams& params);

        ~DatabaseManager();

        template <class TRANSACTOR>
        void queueTransaction(TRANSACTOR& transaction) {

            pqxx::connection* conn = 0;

            if(not this->free_connections.try_pop(conn)) {
                conn = new pqxx::connection(this->connection_string);
            }

            Threads::Task* task =
                new Threads::Task(boost::bind(
                    &DatabaseManager::execTransaction<TRANSACTOR>,
                    this,
                    conn,
                    transaction));
            
            task->start();

            this->running_tasks.push(task);
        }

    private:

        template <class TRANSACTOR>
        void execTransaction(pqxx::connection* conn, TRANSACTOR& transaction) {
            conn->perform(transaction);
            this->free_connections.push(conn);
        }

        void colector();

        DatabaseManagerParams params;

        std::string connection_string;
        
        Threads::Queue<pqxx::connection*> free_connections;

        Threads::Queue<Threads::Task*> running_tasks;

        Threads::Task colector_task;

};

#endif
