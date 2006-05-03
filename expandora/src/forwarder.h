#ifndef FORWARDER_H 
#define FORWARDER_H 

#define PROXY_BUFFER_SIZE 8192


#define IAC 255
#define DONT 254
#define DO 253
#define WONT 252
#define WILL 251
#define SB 250
#define TN_GA 249
#define TN_EL 248
#define TN_EC 247
#define TN_AYT 246
#define TN_AO 245
#define TN_IP 244
#define BREAK 243
#define TN_DM 242
#define TN_NOP 241
#define SE 240
#define TN_EOR 239
#define TN_ABORT 238
#define TN_SUSP 237
#define TN_EOF 236
#define LAST_TN_CMD 236



#include <QMutex>
#include <QThread>




class ProxySocket {
    /* connection sockets */
    int sock;
    QMutex mutex;
    
public:
    int mainState;
    int subState;
    char                          buffer[PROXY_BUFFER_SIZE];
    int                             length;
    
    ProxySocket() {}
    ProxySocket(bool xml);
    
    /* xml flags */
    bool xmlMode;
    bool xmlTogglable;

    QByteArray  subchars;
    QByteArray  fragment;

    void close();
    void clear();
    void send_line(char *line);
    int getSocket();
    bool isXmlMode();
    void setXmlMode( bool b );
    bool isXmlTogglable();
    void setXmlTogglable( bool b );
    
    bool isConnected();
    void setConnection(int sock);
    bool openConnection(QByteArray name, int port); 
    
    void nonblock();

    
    int read();     // read stuff in internal buffer 
    int read(char *buf, int len);
    void write(char *buf, int len);
};



/* PROXY THREAD DEFINES */
class Proxy : public QThread {
        ProxySocket             mud;
        ProxySocket             user;
        int                             proxy_hangsock;

        int      loop();
        bool    mudEmulation;
    
        bool connectToMud();
        void incomingConnection();
        void sendMudEmulationGreeting();
    
public:

        void run();
        int init();
        void send_line_to_user(char *line);
        void send_line_to_mud(char *line);
        bool isMudEmulation() { return mudEmulation; }
        void setMudEmulation(bool b) { mudEmulation = b; }
        void shutdown();
        
};

/* PROX THREAD ENDS */

extern class Proxy proxy;


#endif
