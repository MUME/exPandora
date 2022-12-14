#ifndef TELNETFILTER
#define TELNETFILTER

#include "Component.h"






class TelnetFilter : public Component {
	private:
		Q_OBJECT
		char * purgeProtocolSequences( char * input, int length );

	public:
		TelnetFilter() {}
		virtual Qt::ConnectionType requiredConnectionType(const QString &); 

	public slots:

		/** these methods make a "clean" copy of the passed string,
		 *  wake the lexer thread and return
		 */
		void analyzeMudStream( char * input, int length );
		void analyzeUserStream( char * input, int length );


	signals:
		void newMudInput( char * );
		void newUserInput( char * );
};

#endif

#ifdef DMALLOC
#include <mpatrol.h>
#endif
