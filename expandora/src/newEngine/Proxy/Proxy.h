#ifndef PROXY
#define PROXY


#include <qobject.h>
#include <QTcpSocket>
#include <QTcpServer>
#include "Component.h"


class Proxy : public Component {

		Q_OBJECT
		
	signals:
		void analyzeUserStream( char *, int );
		void analyzeMudStream( char *, int );


	public slots:
		void processUserStream();
		void processMudStream();
		void acceptConnection();
		void resetServer();

	public:
		Proxy();
		~Proxy() {}
		
		void start();
		
		
	private:
		
		QTcpServer server;
		QString remoteHost;
		int remotePort;
		int localPort;
		QTcpSocket * mudSocket;
		QTcpSocket * userSocket;
		char buffer[ 8192 ];

	};
#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
