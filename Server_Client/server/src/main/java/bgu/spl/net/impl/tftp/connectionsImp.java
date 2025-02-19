package bgu.spl.net.impl.tftp;

import java.io.IOException;
import java.util.Map.Entry;
import java.util.concurrent.ConcurrentHashMap;

import bgu.spl.net.srv.ConnectionHandler;
import bgu.spl.net.srv.Connections;

public class connectionsImp<T> implements Connections<T> {

private ConcurrentHashMap<Integer, ConnectionHandler<T>> clientToHandler = new ConcurrentHashMap<>();


@Override
public void connect(int connectionId, ConnectionHandler<T> handler) {
    // TODO Auto-generated method stub
    clientToHandler.putIfAbsent(connectionId, handler);
}

@Override
public boolean send(int connectionId, T msg) {
    // TODO Auto-generated method stub
    if (clientToHandler.contains(connectionId)) {
        clientToHandler.get(connectionId).send(msg);    
        return true;
    }
    else
        return false;

}

@Override
public void disconnect(int connectionId){
    // TODO Auto-generated method stub
    if (clientToHandler.contains(connectionId)) {
        try {
            clientToHandler.get(connectionId).close();
        } catch (IOException e) {}  
        clientToHandler.remove(connectionId);
    }
}

@Override
public void sendToAll(T msg) {
    // TODO Auto-generated method stub

    for( Entry<Integer, ConnectionHandler<T>> client : this.clientToHandler.entrySet()){
        this.send(client.getKey(), msg);

    }

}



}
