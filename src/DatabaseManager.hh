#ifndef DATABASE_HH
#define DATABASE_HH

#include <string>

#include <pqxx/pqxx>

#include <boost/function.hpp>

#include "Threads/Task.hh"

#include "Threads/Queue.hh"

#include "XML/Xml.hh"

#include "DatabaseInterface.hh"

typedef boost::function<void (DatabaseInterface&)> Transactor;

class DatabaseManager {
    public:

        DatabaseManager(const XML::Tag& config);

        ~DatabaseManager();

        void queueTransaction(const Transactor& transaction);

    private:

        void colector();

        void execTransaction(const Transactor& transactor);

        std::string connection_string;
        
        Threads::Queue<pqxx::connection*> free_connections;

        Threads::Queue<Threads::Task*> running_tasks;

        Threads::Task colector_task;

};

#endif
