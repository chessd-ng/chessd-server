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
			Message() { }
			virtual ~Message() { }
			virtual void send() = 0;
		protected:
	};

	template <class CALLABLE> class TypedMessage : public Message {
		public:
			typedef CALLABLE message_function;

			TypedMessage(const message_function& message)
				: message(message) { }

			virtual ~TypedMessage() { }

			virtual void send() {
				this->message();
			}

		private:
			message_function message;
	};


	template <class CALLABLE> Message* createMessage(const CALLABLE& callable) {
		return new TypedMessage<CALLABLE>(callable);
	}
}

#endif
