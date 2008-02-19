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

#ifndef DISPATCH_HH
#define DISPATCH_HH

//#include "Message.hh"
#include "Queue.hh"
#include "Task.hh"
#include "Util/Timer.hh"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_map.hpp>

typedef boost::function<void()> Message;

namespace Threads {

	class Dispatcher {
		public:
			Dispatcher();

			~Dispatcher();

			void run();

			void start();

			void stop();

			void queue(const Message& message) {
				this->_queue.push(message);
			}

			void schedule(const Message& message, Util::Time when) {
                this->queue(boost::bind(&Dispatcher::_schedule, this, message, when));
			}

		private:

			void _schedule(const Message& message, Util::Time when) {
				this->agenda.insert(std::make_pair(when, message));
			}

			Task task;

			Queue<Message> _queue;

            std::map<Util::Time, Message> agenda;

			bool running;

			void dispatch();

			void _stop();

	};

}
#endif
