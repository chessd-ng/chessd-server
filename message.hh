#ifndef MESSAGE_HH
#define MESSAGE_HH

#include <memory>
#include <sigc++/sigc++.h>
#include "semaphore.hh"
#include "fila.hh"

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
		typedef sigc::slot<R_TYPE> message_slot;

		TypedMessage(const message_slot& message, bool sync)
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
		message_slot message;
		R_TYPE ret;
};

template <> class TypedMessage<void> : public Message {
	public:
		typedef sigc::slot<void> message_slot;
		
		TypedMessage(const message_slot& message, bool sync)
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
		message_slot message;
};

#endif
