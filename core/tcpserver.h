#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <string>
#include <vector>
#include <tr1/functional>
#include <tr1/memory>

#include "nocopyable.h"
#include "address.h"
#include "threadutil.h"
#include "connevent.h"

namespace tnet 
{   
    class IOLoop;
    class IOLoopThreadPool;
    
    class Signaler;
    class Acceptor;
    class Connection;
    class ConnLoopPool;
    class Timer;
    class ConnChecker;

    class TcpServer : public nocopyable
    {
    public:
        TcpServer(int acceptLoopNum, int connLoopNum, int maxConnections);
        ~TcpServer();
      
        typedef std::tr1::shared_ptr<Connection> ConnectionPtr_t;
        typedef std::tr1::function<void (const ConnectionPtr_t&, ConnEvent, const char*, int)> ConnectionFunc_t; 
      
        int listen(const Address& addr, const ConnectionFunc_t& func);
       
        void setConnLoopIOInterval(int milliseconds);

        void setConnCheckRepeat(int seconds);
        void setConnCheckStep(int step);
        void setConnTimeout(int seconds);

        typedef std::tr1::function<void (int)> SignalFunc_t;
        void addSignal(int signum, const SignalFunc_t& func);

        void start();
        void stop();

    private:
        void onNewConnection(int sockFd, const ConnectionFunc_t& func);

        void newConnectionInLoop(IOLoop* loop, int sockFd, const ConnectionFunc_t& func);

        void deleteConnection(const ConnectionPtr_t& conn);
        void deleteConnectionInLoop(const ConnectionPtr_t& conn);

        const std::vector<ConnectionPtr_t>& getConnections() { return m_connections; }

    private:
        Acceptor* m_acceptor;

        IOLoopThreadPool* m_connPool;
        std::vector<IOLoop*> m_connLoops;
    
        ConnChecker* m_connChecker;

        IOLoop* m_mainLoop;

        Signaler* m_signaler;

        std::vector<ConnectionPtr_t> m_connections;
    
        int m_maxConnections;
        volatile int m_curConnections;

        int m_acceptLoopNum;
        int m_connLoopNum;

        SpinLock m_lock;
    };
        
}

#endif

