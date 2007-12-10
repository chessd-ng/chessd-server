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
			/*template <class CALLABLE> typename boost::function<boost::result_of<CALLABLE()> ()>
				createTunnel0(const CALLABLE& function, bool sync = false) {
					typedef typename boost::result_of<CALLABLE()>::type R_TYPE;
					R_TYPE (Dispatcher::*p)(const boost::function<R_TYPE ()>&,bool)
						= &Dispatcher::_call0<R_TYPE>;
					return boost::function<R_TYPE ()>(boost::bind(
							p, this,
							function, sync));
				}
			template <class CALLABLE, class T1> typename boost::function<boost::result_of<CALLABLE(T1)> ()>
				createTunnel1(const CALLABLE& function, bool sync = false) {
					typedef typename boost::result_of<CALLABLE()>::type R_TYPE;
					R_TYPE (Dispatcher::*p)(T1, const boost::function<R_TYPE ()>&,bool)
						= &Dispatcher::_call1<R_TYPE>;
					return boost::function<R_TYPE ()>(boost::bind(
							p, this,
							_1,
							function,
							sync));
				}
			template <class R_TYPE, class T1> boost::function<R_TYPE (T1)>
				createTunnel1(const boost::function<R_TYPE (T1)>& function, bool sync = false) {
					R_TYPE (Dispatcher::*p)(T1, const boost::function<R_TYPE (T1)>&,bool)
						= &Dispatcher::_call1<R_TYPE, T1>;
					return boost::function<R_TYPE (T1)>(boost::bind(
							p, this,
							_1,
							function,
							sync));
				}
			template <class R_TYPE, class T1, class T2> boost::function<R_TYPE (T1, T2)>
				createTunnel2(const boost::function<R_TYPE (T1, T2)>& function, bool sync = false) {
					R_TYPE (Dispatcher::*p)(T1,T2,const boost::function<R_TYPE (T1, T2)>&,bool)
						= &Dispatcher::_call2<R_TYPE, T1, T2>;
					return boost::function<R_TYPE (T1, T2)> (boost::bind(
							p, this,
							_1, _2,
							function,
							sync));
				}*/

			template <class CALLABLE> void queue(const CALLABLE& function) {
				typedef TypedMessage<CALLABLE> message_type;
				message_type* message = new message_type(function);
				this->_queue.push(message);
			}

		private:

			/* define a call for 0 parameters */
			/*template <class CALLABLE> typename boost::result_of<CALLABLE()>::type
				_call0(const CALLABLE& function, bool sync = false) {
				typedef TypedMessage<boost::result_of<CALLABLE()>::type, CALLABLE> message_type;
				message_type* message = new message_type(function, sync);
				if(sync) {
					std::auto_ptr<message_type> m_ptr(message);
					return message->wait();
				} else {
					return R_TYPE();
				}
			}*/

			/* define a call for 1 parameter */
			/*template <class CALLABLE, class T1> boost::result_of<CALLABLE()>::type _call1(
					T1 t,
					const CALLABLE& function,
					bool sync = false) {
				typedef typename boost::result_of<CALLABLE()>::type R_TYPE;
				typedef typename TypedMessage<R_TYPE, boost::function<R_TYPE()> > message_type;
				message_type* message = new message_type(boost::bind(function, t), sync);
				this->post(message);
				if(sync) {
					std::auto_ptr<message_type> m_ptr(message);
					return message->wait();
				} else {
					return R_TYPE();
				}
			}*/

			/* define a call for 1 parameter */
			/*template <class CALLABLE, class T1, class T2> boost::result_of<CALLABLE()>::type _call2(
					T1 t1,
					T2 t2,
					const CALLABLE& function,
					bool sync = false) {
				typedef typename boost::result_of<CALLABLE()>::type R_TYPE;
				typedef typename TypedMessage<R_TYPE, boost::function<R_TYPE()> > message_type;
				message_type* message = new message_type(boost::bind(function, t1, t2), sync);
				this->post(message);
				if(sync) {
					std::auto_ptr<message_type> m_ptr(message);
					return message->wait();
				} else {
					return R_TYPE();
				}
			}*/

			Task task;

			Queue<Message*> _queue;

			bool running;

			void dispatch();

			void _stop();

	};

}
#endif
