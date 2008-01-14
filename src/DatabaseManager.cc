#include "DatabaseManager.hh"

#include "Util/utils.hh"


DatabaseManagerParams::DatabaseManagerParams(
        std::string db_name,
        std::string host,
        int port,
        std::string user,
        std::string password) :
    db_name(db_name),
    host(host),
    port(port),
    user(user),
    password(password)
{
}

DatabaseManager::DatabaseManager(const DatabaseManagerParams& params) :
    params(params),
    colector_task(boost::bind(&DatabaseManager::colector, this))
{
    this->connection_string =
        " host=" + params.host +
        " port=" + Util::int2str(params.port) +
        " dbname=" + params.db_name +
        " user=" + params.user +
        " password=" + params.password +
        " sslmode=disable";

    // Check if we can connect to the database
    pqxx::connection* conn = new pqxx::connection(this->connection_string);

    this->free_connections.push(conn);

    this->colector_task.start();
}

DatabaseManager::~DatabaseManager() {
    this->running_tasks.push(0);
    this->colector_task.join();

    pqxx::connection* connection = 0;

    while(this->free_connections.try_pop(connection)) {
        delete connection;
    }
}

void DatabaseManager::colector() {
    Threads::Task* task = 0;
    while((task = this->running_tasks.pop()) != 0) {
        task->join();
        delete task;
    }
}
