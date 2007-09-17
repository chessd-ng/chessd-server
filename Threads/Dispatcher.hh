#ifndef DISPATCH_HH
#define DISPATCH_HH

#include "Message.hh"
#include "Queue.hh"
#include "Task.hh"
// #include "handlers.hh"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace Threads {

	class Dispatcher {
		public:
			Dispatcher();

			~Dispatcher();

			void run();

			void start();

			void stop();

			/* creates a function wrapper through the tunnel */
			template <class R_TYPE> boost::function<R_TYPE ()>
				createTunnel(const boost::function<R_TYPE ()>& function, bool sync = false) {
					R_TYPE (Dispatcher::*p)(const boost::function<R_TYPE ()>&,bool)
						= &Dispatcher::call<R_TYPE>;
					return boost::function<R_TYPE ()>(boost::bind(
							p, this,
							function, sync));
				}
			template <class R_TYPE, class T1> boost::function<R_TYPE (T1)>
				createTunnel(const boost::function<R_TYPE (T1)>& function, bool sync = false) {
					R_TYPE (Dispatcher::*p)(const boost::function<R_TYPE (T1)>&,T1,bool)
						= &Dispatcher::call<R_TYPE, T1>;
					return boost::function<R_TYPE (T1)>(boost::bind(
							p, this,
							function,
							_1,
							sync));
				}
			template <class R_TYPE, class T1, class T2> boost::function<R_TYPE (T1, T2)>
				createTunnel(const boost::function<R_TYPE (T1, T2)>& function, bool sync = false) {
					R_TYPE (Dispatcher::*p)(const boost::function<R_TYPE (T1, T2)>&,T1,T2,bool)
						= &Dispatcher::call<R_TYPE, T1, T2>;
					return boost::function<R_TYPE (T1, T2)> (boost::bind(
							p, this,
							function,
							_1, _2,
							sync));
				}

			/* define a call for 0 parameters */
			template <class R_TYPE> R_TYPE call(const boost::function<R_TYPE>&
					function, bool sync) {
				TypedMessage<R_TYPE>* message = new TypedMessage<R_TYPE>(function, sync);
				this->post(message);
				if(sync) {
					std::auto_ptr< TypedMessage<R_TYPE> > m_ptr(message);
					return message->wait();
				} else {
					/* leave the message to the dispatcher */
					return R_TYPE();
				}
			}

			/* define a call for 1 parameter */
			template <class R_TYPE, class T1> R_TYPE call(
					T1 t,
					const boost::function<R_TYPE (T1)>& function,
					bool sync) {
				TypedMessage<R_TYPE>* message = new TypedMessage<R_TYPE>(
						boost::bind(function, t), sync);
				this->post(message);
				if(sync) {
					std::auto_ptr< TypedMessage<R_TYPE> > m_ptr(message);
					return message->wait();
				} else {
					/* leave the message to the dispatcher */
					return R_TYPE();
				}
			}

			/* define a call for 2 parameters */
			template <class R_TYPE, class T1, class T2> R_TYPE call(
					T1 t1, T2 t2,
					const boost::function<R_TYPE (T1, T2)>& function,
					bool sync) {
				TypedMessage<R_TYPE>* message = new TypedMessage<R_TYPE>(
						boost::bind(function, t1, t2), sync);
				this->post(message);
				if(sync) {
					std::auto_ptr< TypedMessage<R_TYPE> > m_ptr(message);
					return message->wait();
				} else {
					/* leave the message to the dispatcher */
					return R_TYPE();
				}
			}

		private:

			Task task;

			Queue<Message*> queue;

			bool running;

			void post(Message* message);

			void dispatch();

			void _stop();

	};

}
#endif
