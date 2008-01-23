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

#include "Dispatcher.hh"

namespace Threads {

	Dispatcher::Dispatcher() : 
		task(boost::bind(&Dispatcher::run, this)),
		running(false) { }

	Dispatcher::~Dispatcher() {
	}

	void Dispatcher::start() {
		this->running = true;
		this->task.start();
	}

	void Dispatcher::run() {
		while(this->running) {
			Message* message = this->_queue.pop();
			message->send();
			delete message;
		}
	}

	void Dispatcher::stop() {
		/* A stop message is sent to the dispatcher instead of just
		 * setting running to false. This we make sure the dispatcher
		 * will stop. */
		if(this->running == true) {
			this->queue(boost::bind(&Dispatcher::_stop, this));
			this->task.join();
		}
	}

	void Dispatcher::_stop() {
		this->running = false;
	}

	void Dispatcher::dispatch() {
	}

}
