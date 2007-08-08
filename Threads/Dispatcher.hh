#ifndef DISPATCH_HH
#define DISPATCH_HH

#include "Message.hh"
#include "Queue.hh"
#include "Task.hh"
// #include "handlers.hh"

#include <boost/function.hpp>

namespace Threads {

	class Dispatcher : public Task {
		public:
			Dispatcher();
			~Dispatcher();
			void post(Message* message);
			virtual void run();
			void stop();

			/* creates a function wrapper through the tunnel */
			template <class R_TYPE> boost::function<R_TYPE ()>
				createTunnel(const boost::function<R_TYPE ()>& slot, bool sync = false) {
					R_TYPE (Dispatcher::*p)(const boost::function<R_TYPE ()>&,bool) = &Dispatcher::callback<R_TYPE>;
					return boost::bind(
							boost::mem_fun(this, p),
							slot, sync);
				}
			template <class R_TYPE, class T1> boost::slot<R_TYPE, T1>
				createTunnel(const boost::slot<R_TYPE, T1>& slot, bool sync = false) {
					R_TYPE (Dispatcher::*p)(T1,const boost::slot<R_TYPE, T1>&,bool) = &Dispatcher::callback<R_TYPE, T1>;
					return boost::bind(
							boost::mem_fun(this, p),
							slot, sync);
				}
			template <class R_TYPE, class T1, class T2> boost::slot<R_TYPE, T1, T2>
				createTunnel(const boost::slot<R_TYPE>& slot, bool sync = false) {
					R_TYPE (Dispatcher::*p)(T1,T2,const boost::slot<R_TYPE, T1, T2>&,bool) = &Dispatcher::callback<R_TYPE, T1, T2>;
					return boost::bind(
							boost::mem_fun(this, p),
							slot, sync);
				}

		private:
			Queue<Message*> queue;

			bool running;

			void dispatch();
			void _stop();

			/* define a callback for 0 parameters */
			template <class R_TYPE> R_TYPE callback(const boost::slot<R_TYPE>&
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
					const boost::slot<R_TYPE, T1>& slot,
					bool sync) {
				TypedMessage<R_TYPE>* message = new TypedMessage<R_TYPE>(
						boost::bind(slot, t), sync);
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
					const boost::slot<R_TYPE, T1>& slot,
					bool sync) {
				TypedMessage<R_TYPE>* message = new TypedMessage<R_TYPE>(
						boost::bind(slot, t1, t2), sync);
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

}
#endif
