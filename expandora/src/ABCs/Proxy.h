#ifndef ENGINE
#define ENGINE

class Engine;

/** the proxy should handle all the protocol-dependent things and only pass on data that 
    actually has something to do with the game
    This data should be delivered line by line to each of the engines via analyzeMudStream(...)
    and analyzeUserStream(...). If one engine returns false from these methods the respective text is
    immediately dropped */

class Proxy {
 public:
  void attachEngine(Engine * e);
  
  /** connect to a mud server */
  void connectServer(char * host, int port);
  
  /** accept one user connection on that port 
      would it be useful to extend this to accepting several connections? */ 
  void acceptConnection(int port);

}

#endif
