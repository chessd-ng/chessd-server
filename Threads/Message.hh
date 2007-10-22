#ifndef MESSAGE_HH
#define MESSAGE_HH

#include "Semaphore.hh"
#include "Queue.hh"

#include <memory>

#include <boost/function.hpp>
#include <boost/utility/result_of.hpp>

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

	template <class R_TYPE, class CALLABLE> class TypedMessage;

	template <class R_TYPE, class CALLABLE> class TypedMessage : public Message {
		public:
			typedef CALLABLE message_function;

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

	template <class CALLABLE> class TypedMessage<void, CALLABLE> : public Message {
		public:
			typedef CALLABLE message_function;

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

	template <class CALLABLE> Message* createMessage(const CALLABLE& callable, bool sync) {
		return new TypedMessage<typename boost::result_of<CALLABLE()>::type, CALLABLE>(callable, sync);
	}
}

#endif
