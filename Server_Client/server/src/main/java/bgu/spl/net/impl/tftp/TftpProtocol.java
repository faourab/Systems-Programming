package bgu.spl.net.impl.tftp;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import bgu.spl.net.api.BidiMessagingProtocol;

import bgu.spl.net.srv.Connections;

public class TftpProtocol implements BidiMessagingProtocol<byte[]>  {
    private volatile boolean terminate = false;
    private int connectionId;
    private  Connections<byte[]> connections;
    private boolean login = false;
    private String name = " ";
    private boolean h = false;
    private String File_name = "";
    FileOutputStream fileTowrite;
    private short block_number = 0;
    private BlockingQueue<byte[]> packets = new LinkedBlockingQueue<>();
 
    @Override
    public void start(int connectionId, Connections<byte[]> connections) {
        // TODO implement this
        this.connectionId = connectionId;
        this.connections = connections;
        }

    @Override
    public byte[] process(byte[] message) {
      
        
        // TODO implement this
        
        byte [] b = new byte []{message[0] , message[1]};

        
        
        short opcode = ( short ) ((( short ) b [0]) << 8 | ( short ) ( b [1]) );
     //   System.out.println(opcode);
        
        if (opcode == 1){
            block_number = 0;
            if (!login) {
                byte[] msg = errors("User not logged in");      
                byte[] res = new byte[msg.length+5];
                res[0] = ((byte)0);
                res[1] = ((byte)5);
                res[2] = ((byte)0);
                res[3] = ((byte)6);
                res[res.length-1] = ((byte)0);
                int j = 4;
                for (int i = 0; i < msg.length; i++) {
                        res[j]=msg[i];
                        j++;    
                }
                return res; 
            }
            else{
            File_name = getfilename(message);
           
            File f = new File(System.getProperty("user.dir") + "\\server\\Flies\\" + File_name);
            if (f.exists()){
                if (f.length()%512==0) {
                    h =true;
                }
                if (block_number == 0) {
                    byte[] res = new byte[4];
                    res[0] = ((byte)0);
                    res[1] = ((byte)4);
                    res[2] = ((byte)0);
                    res[3] = ((byte)0);
                    block_number++;
                    // we put this here because we want to divide the data just one time
                    Path path = Paths.get(System.getProperty("user.dir") + "\\server\\Flies\\" + File_name);
                    byte[] arr;
                    try {
                        arr = Files.readAllBytes(path);
                        dividetopackets(arr);
                    } catch (IOException e) {}
                 
                   // return res;    
                }

                if (!packets.isEmpty()) {
                    return  packets.remove();
                }
            }            
            
            else{
                byte[] msg = errors("File not found");      
                byte[] res = new byte[msg.length+5];
                res[0] = ((byte)0);
                res[1] = ((byte)5);
                res[2] = ((byte)0);
                res[3] = ((byte)1);
                res[res.length-1] = ((byte)0);
                int j = 4;
                for (int i = 0; i < msg.length; i++) {
                        res[j] = msg[i];
                        j++;    
                }
                return res; 
            }
        }
        }
        else if (opcode == 2){
            if (!login) {
                byte[] msg = errors("User not logged in");      
                byte[] res = new byte[msg.length+5];
                res[0] = ((byte)0);
                res[1] = ((byte)5);
                res[2] = ((byte)0);
                res[3] = ((byte)6);
                res[res.length-1] = ((byte)0);
                int j = 4;
                for (int i = 0; i < msg.length; i++) {
                        res[j] = msg[i];
                        j++;    
                }
                return res; 
            }
            
            for(int i = 2; i < message.length; i++){
                if (message[i] == 0) break;
                File_name += new String(message, i, 1, StandardCharsets.UTF_8);
            }
            System.out.println(File_name);
            byte[] bd = File_name.getBytes();
            for (int i = 0; i < bd.length; i++) {
                System.out.println(bd[i]);               
            }

            File folder = new File(System.getProperty("user.dir") + "\\server\\Flies\\");
            File file = new File( folder.getPath() + "\\" + File_name);

          //  System.out.println(file.getPath());
            if (file.exists()) {
                byte[] msg = errors("File already exists");      
                byte[] res = new byte[msg.length+5];
                res[0] = ((byte)0);
                res[1] = ((byte)5);
                res[2] = ((byte)0);
                res[3] = ((byte)5);
                res[res.length-1] = ((byte)0);
                int j = 4;
                for (int i = 0; i < msg.length; i++) {
                        res[j] = msg[i];
                        j++;    
                }
                return res;
            }
            else{ 
                byte[] res = new byte[4];
                res[0] = ((byte)0);
                res[1] = ((byte)4);
                res[2] = ((byte)0);
                res[3] = ((byte)0);
                //  System.out.println("WRQ "+File_name+" complete");
                
                try {
                    file.createNewFile();
                   // Bcast((byte) 1, file.getName());
                    this.fileTowrite = new FileOutputStream(file);

                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
                return res;
            }
        }
        else if (opcode == 3){


            short packet_size = ( short ) ((( short ) message [2]) << 8 | ( short ) ( message [3]) );
            short block_number1 = ( short ) ((( short ) message [4]) << 8 | ( short ) ( message [5]) );
            
            //System.out.println("packet size: " +packet_size);
            byte[] data = new byte[(int) (packet_size)];
            
            for(int i = 0; i < (int)(packet_size); i++){
                data[i] = message[i + 6];
            }
            
            //System.out.println("blocks = " + block_number1 + " ,data: " + packet_size  );
           // System.out.println(new String(data, 0, data.length, StandardCharsets.UTF_8));

            // if (File_name.equals("")){
            //                         byte[] msg = errors("File not found");      
            //                         byte[] res = new byte[msg.length+5];
            //                         res[0] = ((byte)0);
            //                         res[1] = ((byte)5);
            //                         res[2] = ((byte)0);
            //                         res[3] = ((byte)1);
            //                         res[res.length-1] = ((byte)0);
            //                         int j = 4;
            //                         for (int i = 0; i < msg.length; i++) {
            //                                 res[j] = msg[i];
            //                                 j++;    
            //                         }
            //                         return res;                   
            //                         }
                            

                try {
                    this.fileTowrite.write(data);
                    this.fileTowrite.flush();

                    if (packet_size <  512){
                        this.fileTowrite.close();
                    }

                } catch (IOException ioe) {
                    ioe.printStackTrace();
                }            
            
            
            return new byte[]{(byte)0, (byte)4, message[4], message[5]};       

        }
        else if (opcode == 4){
            // System.out.print(packets.size());
            if(!packets.isEmpty()){
                //block_number++;
                return packets.remove();
            }else if(h){
                short a = (short)block_number ;
                byte [] a_bytes = new byte []{( byte )  ( a >> 8) , ( byte ) ( a & 0xff ) };     
                h =false;
                return new byte[]{0,3,0,0,a_bytes[0],a_bytes[1]};
            }else{
                return null;
            }

            
        }

        else if (opcode == 6){
            block_number =0;
            if (!login) {
                byte[] msg = errors("User not logged in");      
                byte[] res = new byte[msg.length+5];
                res[0] = ((byte)0);
                res[1] = ((byte)5);
                res[2] = ((byte)0);
                res[3] = ((byte)6);
                res[res.length-1] = ((byte)0);
                int j = 4;
                for (int i = 0; i < msg.length; i++) {
                        res[j]=msg[i];
                        j++;    
                }
                return res;       
            }
           
                 

            List<byte[]> DIR = new LinkedList<>();
            File folder = new File(System.getProperty("user.dir") + "\\server\\Flies\\");
            
            File[] listOfFiles = folder.listFiles();
            for (File file : listOfFiles) {
                byte[]res1 = file.getName().getBytes();
                DIR.add(res1);  
            }
            int count = 0;
            for (int i = 0; i < DIR.size(); i++) {
                count += DIR.get(i).length;
            }
            count +=  DIR.size();
            int index = 0;
            byte[]res = new byte[count];
            for (byte[] BYTES_NAME : DIR) {
                for (byte byt : BYTES_NAME) {
                    res[index] = byt;
                    index++;
                }
                res[index]=(byte)0;
                index++;
            }
            if (block_number==0) {
                block_number++;
                dividetopackets(res);                
            }           
            
            return packets.remove();
        
        
    }   
        else if (opcode == 7){      
            if (!login){
                login = true;
                name = getfilename(message);
                byte[] res = new byte[4];
                res[0] = ((byte)0);
                res[1] = ((byte)4);
                res[2] = ((byte)0);
                res[3] = ((byte)0);
                name = " ";
                return res;
            }else{
                byte[] msg = errors("User already logged in");      
                byte[] res = new byte[msg.length+5];
                res[0] = ((byte)0);
                res[1] = ((byte)5);
                res[2] = ((byte)0);
                res[3] = ((byte)7);
                res[res.length-1] = ((byte)0);
                int j = 4;
                for (int i = 0; i < msg.length; i++) {
                        res[j]=msg[i];
                        j++;    
                }
                return res; 
            }  
        }       
        else if (opcode == 8){
            if (!login) {
                byte[] msg = errors("User not logged in");      
                byte[] res = new byte[msg.length+5];
                res[0] = ((byte)0);
                res[1] = ((byte)5);
                res[2] = ((byte)0);
                res[3] = ((byte)6);
                res[res.length-1] = ((byte)0);
                int j = 4;
                for (int i = 0; i < msg.length; i++) {
                        res[j]=msg[i];
                        j++;    
                }
                return res; 
            }
            String fileName = getfilename(message);
            File f = new File(System.getProperty("user.dir") + """\\server\\Flies\\" + fileName);
            if (f.exists()){
                f.delete();
               // Bcast((byte) 0, f.getName());
                byte[] res = new byte[4];
                res[0] = ((byte)0);
                res[1] = ((byte)4);
                res[2] = ((byte)0);
                res[3] = ((byte)0);
                File_name = " ";
                return res;
                  
            }else{
                File folder = new File(System.getProperty("user.dir") + """\\server\\Flies\\");
                File[] listOfFiles = folder.listFiles();
                for (File file : listOfFiles) {
                    System.out.println(file.getName());
                }
                byte[] msg = errors("File not found "+fileName.length());      
                byte[] res = new byte[msg.length+5];
                res[0] = ((byte)0);
                res[1] = ((byte)5);
                res[2] = ((byte)0);
                res[3] = ((byte)1);
                res[res.length-1] = ((byte)0);
                int j = 4;
                for (int i = 0; i < msg.length; i++) {
                        res[j]=msg[i];
                        j++;    
                }
                return res;  
            }

            
        }   
        else if (opcode == 9){
            String ans = "BCAST";
            if (message[3]==0){
                
                ans += " del";    
            }else{
                ans += " add";
            }
            int count = 0;
            for (int i = 3; i < message.length; i++) {
                if (message[i] == 0){
                    if (message.length<511 && message[i+1] != 0) {
                        throw new IllegalArgumentException("incorrect name (contain 0)");
                    }
                    break;
                }
                else
                    count++; 
            }
            byte[] res = new byte[count];
            for (int i = 2; i < message.length; i++) {
                for (int j = 0; j < res.length; j++) {
                    res[j] = message[i];
                }
                
            }
           File_name = new String(res,StandardCharsets.UTF_8);
           ans += File_name;
           File_name =" ";
        //    System.out.println(ans);
        }   
        else if (opcode == 10){
            terminate = true;
            byte[] res = new byte[4];
            res[0] = ((byte)0);
            res[1] = ((byte)4);
            res[2] = ((byte)0);
            res[3] = ((byte)0);
            connections.disconnect(connectionId);
            name = " ";
            return res;
        }   

        return null;
    }
    @Override
    public boolean shouldTerminate() {
        // TODO implement this
        return terminate;
    }

    public void dividetopackets(byte[] message){
        // System.out.println("message Length"+ message.length);
        int msglen = 0;
        while (msglen < message.length) {
            byte[] packet = new byte[Math.min(512, message.length - msglen)+6];
            packet[0] = (byte)0;
            packet[1] = (byte)3;
            short PACKETSIZE = (short) Math.min(512, message.length - msglen);
            byte [] c_bytes = new byte []{(byte) (  PACKETSIZE >> 8) , (byte) (PACKETSIZE & 0xff)};
            packet[2]=c_bytes[0];
            packet[3]=c_bytes[1];
            short bs = (short) block_number;
            byte [] b_bytes = new byte []{(byte) (  bs >> 8) , (byte) (bs & 0xff)};
            packet[4]=b_bytes[0];
            packet[5]=b_bytes[1];
            for (int i = 6; i < packet.length; i++) {
                packet[i] = message[msglen];
                //System.out.println(packet[i]);
                msglen++;
                
            }
            packets.add(packet);
            block_number++;

        }          
        
    } 

    private void Bcast(byte operationType, String filename){

        byte[] filenameBytes = filename.getBytes();

        byte[] message = new byte[2 + 1 + filenameBytes.length + 1];

        message[0] = (byte) 0;
        message[1] = (byte) 9;

        message[2] = operationType;

        for(int i= 3; i < message.length - 1; i++){
            message[i] = filenameBytes[i- 3];
        }

        message[message.length - 1] = (byte) 0;


        connections.sendToAll(message);

    }

    public String getfilename(byte[]message){
        
        int count = 0;

        for(int i = 2; i < message.length; i++){
            if(message[i] == 0 ) break;
            count++;
            
        }

        for(int i = message.length - 1; i >= 2; i--){
            if (message[i] == 0 && i <count) throw new IllegalArgumentException("name could not contain zero!");
        }

        byte[] res = new byte[count];
         
        for (int i = 0; i < count; i++) {
                res[i] = message[i + 2];
            
        }
       return new String(res,StandardCharsets.UTF_8);
        
    }
public byte[] errors(String error_msg){
   
    byte[] ans = error_msg.getBytes();
    return ans;
}
    
}
