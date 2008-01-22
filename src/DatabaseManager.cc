#include "DatabaseManager.hh"

#include "Util/utils.hh"

#include <boost/bind.hpp>

#include <iostream>

struct pqxx_transaction : public pqxx::transactor<>
{
    public:

        pqxx_transaction(const Transactor& transactor) : transactor(transactor) { }

        void operator()(pqxx::work& work)
        {
            DatabaseInterface interface(work);
            this->transactor(interface);
        }

        void on_abort(const char* msg) throw()
        {
            std::cout << "Transaction aborted: " << msg << std::endl;
        }

    private:

        Transactor transactor;
};

DatabaseManager::DatabaseManager(const XML::Tag& config) :
    colector_task(boost::bind(&DatabaseManager::colector, this))
{
    this->connection_string =
        " host=" + config.getAttribute("host") +
        " port=" + config.getAttribute("port") +
        " dbname=" + config.getAttribute("db_name") +
        " user=" + config.getAttribute("user") +
        " password=" + config.getAttribute("password") +
        " sslmode=disable";

    // Check if we can connect to the database
    pqxx::connection* conn = new pqxx::connection(this->connection_string);

    this->free_connections.push(conn);

    this->colector_task.start();
}

DatabaseManager::~DatabaseManager()
{
    this->running_tasks.push(0);
    this->colector_task.join();

    pqxx::connection* connection = 0;

    while(this->free_connections.try_pop(connection)) {
        delete connection;
    }
}

void DatabaseManager::execTransaction(const Transactor& transactor)
{
    pqxx_transaction t(transactor);

    pqxx::connection* conn = 0;

    try {
        if(not this->free_connections.try_pop(conn)) {
            std::cout << "new connection to the database" << std::endl;
            conn = new pqxx::connection(this->connection_string);
        }
    } catch (pqxx::broken_connection) {
        /* if we cant create another connect, we should just wait */
        conn = this->free_connections.pop();
    }

    conn->perform(t);

    this->free_connections.push(conn);
}

void DatabaseManager::queueTransaction(const Transactor& transaction)
{
    Threads::Task* task =
        new Threads::Task(boost::bind(
                    &DatabaseManager::execTransaction,
                    this,
                    transaction));

    task->start();

    this->running_tasks.push(task);
}

void DatabaseManager::colector()
{
    Threads::Task* task = 0;
    while((task = this->running_tasks.pop()) != 0) {
        task->join();
        delete task;
    }
}
