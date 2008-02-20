/*
 *   Copyright (c) 2007-2008 C3SL.
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

#ifndef FILA_HH
#define FILA_HH 

#include "Mutex.hh"
#include "Condition.hh"

#include <queue>
#include <semaphore.h>

namespace Threads {

    template <class T>
        class Queue {
            private:
                std::queue<T> queue;
                Condition condition;
                size_t wait_count, tickets;
            public:
                Queue() : wait_count(0), tickets(0) { }
                ~Queue() { }
                T pop() {
                    condition.lock();
                    if(tickets == 0) {
                        ++wait_count;
                        condition.wait();
                    } else {
                        --tickets;
                    }
                    T tmp = queue.front();
                    queue.pop();
                    condition.unlock();
                    return tmp;
                }
                bool pop(T& item, Util::Time time) {
                    condition.lock();
                    if(tickets == 0) {
                        ++wait_count;
                        if(not condition.wait(time)) {
                            condition.unlock();
                            --wait_count;
                            return false;
                        }
                    } else {
                        --tickets;
                    }
                    item = queue.front();
                    queue.pop();
                    condition.unlock();
                    return true;
                }
                bool try_pop(T& t) {
                    condition.lock();
                    if(tickets == 0) {
                        condition.unlock();
                        return false;
                    }
                    --tickets;
                    t = queue.front();
                    queue.pop();
                    condition.unlock();
                    return true;
                }
                void push(const T& t) {
                    condition.lock();
                    queue.push(t);
                    if(wait_count > 0) {
                        --wait_count;
                        condition.signal();
                    } else
                        ++tickets;
                    condition.unlock();
                }
                bool try_push(const T& t) {
                    condition.lock();
                    if(wait_count == 0) {
                        condition.unlock();
                        return false;
                    }
                    queue.push(t);
                    --wait_count;
                    condition.signal();
                    condition.unlock();
                    return true;
                }
        };
}

#endif
