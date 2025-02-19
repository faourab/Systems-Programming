package bgu.spl.net.api;

import java.io.FileNotFoundException;

public interface MessagingProtocol<T> {
 
    /**
     * process the given message 
     * @param msg the received message
     * @return the response to send or null if no response is expected by the client
     * @throws FileNotFoundException 
     */
    T process(T msg) ;
 
    /**
     * @return true if the connection should be terminated
     */
    boolean shouldTerminate();
 
}