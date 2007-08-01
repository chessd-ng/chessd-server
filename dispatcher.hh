#ifndef DISPATCH_HH
#define DISPATCH_HH

#include <sigc++/sigc++.h>
#include "message.hh"
#include "fila.hh"
#include "task.hh"
#include "handlers.hh"

class Dispatcher : public Task {
	public:
		Dispatcher();
		~Dispatcher();
		void post(Message* message);
		void dispatch();
		virtual void run();
		void shutdown();

		/* creates a function wrapper through the tunnel */
		template <class R_TYPE> sigc::slot<R_TYPE>
		createTunnel(const sigc::slot<R_TYPE>& slot, bool sync = false) {
			R_TYPE (Dispatcher::*p)(const sigc::slot<R_TYPE>&,bool) = &Dispatcher::callback<R_TYPE>;
			return sigc::bind(
					sigc::mem_fun(this, p),
					slot, sync);
		}
		template <class R_TYPE, class T1> sigc::slot<R_TYPE, T1>
		createTunnel(const sigc::slot<R_TYPE, T1>& slot, bool sync = false) {
			R_TYPE (Dispatcher::*p)(T1,const sigc::slot<R_TYPE, T1>&,bool) = &Dispatcher::callback<R_TYPE, T1>;
			return sigc::bind(
					sigc::mem_fun(this, p),
					slot, sync);
		}
		template <class R_TYPE, class T1, class T2> sigc::slot<R_TYPE, T1, T2>
		createTunnel(const sigc::slot<R_TYPE>& slot, bool sync = false) {
			R_TYPE (Dispatcher::*p)(T1,T2,const sigc::slot<R_TYPE, T1, T2>&,bool) = &Dispatcher::callback<R_TYPE, T1, T2>;
			return sigc::bind(
					sigc::mem_fun(this, p),
					slot, sync);
		}

	private:
		Fila<Message*> fila;
		bool running;

		ShutdownHandler shutdown_handler;

		/* define a callback for 0 parameters */
		template <class R_TYPE> R_TYPE callback(const sigc::slot<R_TYPE>&
				slot, bool sync) {
			TypedMessage<R_TYPE>* message = new TypedMessage<R_TYPE>(slot, sync);
			this->post(message);
			if(sync) {
				std::auto_ptr< TypedMessage<R_TYPE> > m_ptr(message);
				return message->wait();
			} else {
				/* leave the message to the dispatcher */
				return R_TYPE();
			}
		}

		/* define a callback for 1 parameter */
		template <class R_TYPE, class T1> R_TYPE callback(
				T1 t,
				const sigc::slot<R_TYPE, T1>& slot,
				bool sync) {
			TypedMessage<R_TYPE>* message = new TypedMessage<R_TYPE>(
					sigc::bind(slot, t), sync);
			this->post(message);
			if(sync) {
				std::auto_ptr< TypedMessage<R_TYPE> > m_ptr(message);
				return message->wait();
			} else {
				/* leave the message to the dispatcher */
				return R_TYPE();
			}
		}

		/* define a callback for 2 parameters */
		template <class R_TYPE, class T1, class T2> R_TYPE callback(
				T1 t1, T2 t2,
				const sigc::slot<R_TYPE, T1>& slot,
				bool sync) {
			TypedMessage<R_TYPE>* message = new TypedMessage<R_TYPE>(
					sigc::bind(slot, t1, t2), sync);
			this->post(message);
			if(sync) {
				std::auto_ptr< TypedMessage<R_TYPE> > m_ptr(message);
				return message->wait();
			} else {
				/* leave the message to the dispatcher */
				return R_TYPE();
			}
		}
};
#endif
