#ifndef MESSAGE_HH
#define MESSAGE_HH

#include "Semaphore.hh"
#include "Queue.hh"

#include <memory>

#include <boost/function.hpp>

namespace Threads {

	class Message{
		public:
			Message(bool sync) : semaphore(sync?new Semaphore:0) { }
			virtual ~Message() {
				if(this->semaphore)
					delete this->semaphore;
			}
			virtual void send() = 0;
			bool isSync() { return this->semaphore != 0; }
		protected:
			Semaphore* semaphore;
	};

	template <class R_TYPE> class TypedMessage;

	template <class R_TYPE> class TypedMessage : public Message {
		public:
			typedef boost::function<R_TYPE() > message_function;

			TypedMessage(const message_function& message, bool sync)
				: Message(sync), message(message) { }

			virtual ~TypedMessage() { }

			virtual void send() {
				this->ret = this->message();
				if(this->semaphore)
					this->semaphore->post();
			}

			R_TYPE wait() {
				this->semaphore->wait();
				return this->ret;
			}

		private:
			message_function message;
			R_TYPE ret;
	};

	template <> class TypedMessage<void> : public Message {
		public:
			typedef boost::function<void()> message_function;

			TypedMessage(const message_function& message, bool sync)
				: Message(sync), message(message) { }

			virtual ~TypedMessage() { }

			virtual void send() {
				this->message();
				if(this->semaphore)
					this->semaphore->post();
			}

			void wait() {
				this->semaphore->wait();
			}

		private:
			message_function message;
	};

}

#endif
