/*
 *   Copyright (c) 2007-2008 Raphael H. Ribas,  Eduardo A. Ribas.
 *
 *   This file is part of Chessd.
 *
 *   Chessd is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Chessd is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 */

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
