package bgu.spl.net.impl.tftp;

import java.util.Arrays;

import bgu.spl.net.api.MessageEncoderDecoder;

public class TftpEncoderDecoder implements MessageEncoderDecoder<byte[]> {
    //TODO: Implement here the TFTP encoder and decoder
    private byte[] bytes = new byte[512]; //start with 512
    private int len = 0;
    @Override
    public byte[] decodeNextByte(byte nextByte) {
        // TODO: implement this

        if (len >= bytes.length) {
            bytes = Arrays.copyOf(bytes, len * 2);
        }
        
        bytes[len]=nextByte;
        len++;

        if (len >= 2) {
        short opcode = ( short ) ((( short ) bytes [0]) << 8 | ( short ) ( bytes [1]) );

        if(opcode == 10 ){
            len = 0;
            byte[] resp =bytes;
            bytes = new byte[512];
            return resp; 
        }
        if(opcode == 6 ){
            len = 0;
            byte[] resp =bytes;
            bytes = new byte[512];
            return resp; 
        }    
        if (opcode == 4 && len == 4){
            len = 0;
            byte[] resp =bytes;
            bytes = new byte[512];
            return resp;
        }
        if (opcode == 3){
            
            if (len >= 4){
                short pack_size = ( short ) ((( short ) bytes [2]) << 8 | ( short ) ( bytes [3]) );
                if (len == 6 + pack_size){
                    len = 0;
                    byte[] resp =bytes;
                    bytes = new byte[512];
                    return resp;
                }
                
            }
            
        }
        if (nextByte == 0 && len > 0 && (opcode!=10 && opcode != 6 && opcode!=4 && opcode !=3)){
            len = 0;
            byte[] resp =bytes;
            bytes = new byte[512];
            return resp;
        }
        }
        return null;
    }
    @Override
    public byte[] encode(byte[] message) { 
        //TODO: implement this
        return message;
    }
}