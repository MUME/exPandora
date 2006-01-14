#ifndef PROXY
#define PROXY


#include <qobject.h>
#include <QTcpSocket>
#include <QTcpServer>
#include "Component.h"


class Proxy : public Component {

		Q_OBJECT
		
	signals:
		void analyzeUserStream(char *);
		void analyzeMudStream(char *);


	public slots:
		void processUserStream();
		void processMudStream();
		void acceptConnection();
		void resetServer();

	public:
		Proxy();
		~Proxy() {}
		Qt::ConnectionType requiredConnectionType(const QString &) {return Qt::QueuedConnection;}
		void init();
		
		
	private:
		
		QTcpServer * server;
		QString remoteHost;
		int remotePort;
		int localPort;
		QTcpSocket * mudSocket;
		QTcpSocket * userSocket;
		char buffer[ 8192 ];
		char * purgeProtocolSequences(char * input, int length);
	};
#endif

#ifdef DMALLOC
#include <mpatrol.h>
#endif
