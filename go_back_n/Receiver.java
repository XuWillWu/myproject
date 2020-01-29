

import java.io.*;
import java.lang.*;
import java.net.ServerSocket;
import java.net.*;
import java.util.*; 
import java.nio.ByteBuffer;

public class Receiver{
	public static void main(String argv[]) throws IOException{
		if(argv.length != 4){
			System.err.println("Expect 4 argument");
			System.exit(0);
		}

//  process argument

		InetAddress emulator_IP = InetAddress.getByName(argv[0]);
		int receiver_Emulator_port = Integer.parseInt(argv[1]);
		int emulator_Receiver_port = Integer.parseInt(argv[2]);
		FileOutputStream file = new FileOutputStream(argv[3]);
        PrintWriter receiver_file = new PrintWriter("arrival.log");

		int expect_seq_num = 0;
        int old_seq=0;
		DatagramSocket receiver_Socket = new DatagramSocket(emulator_Receiver_port);
		DatagramSocket emulator_Socket = new DatagramSocket();
		while(true){
			//receive packet
            try{
    			byte[] pkt_size = new byte[512];
                DatagramPacket pkt_get = new DatagramPacket(pkt_size,pkt_size.length);
                receiver_Socket.receive(pkt_get);
                byte[] pkt_data = pkt_get.getData();
                ByteBuffer buffer = ByteBuffer.wrap(pkt_data);
                //get detail
                int type = buffer.getInt();
            	int seq_num = buffer.getInt();
                int length = buffer.getInt();
                byte data[] = new byte[length];
                buffer.get(data, 0, length);
                receiver_file.println(seq_num%32);
                //System.out.println("receiver: receive pkt " + seq_num + " " + expect_seq_num%32);

            	
        		if(type == 2 && seq_num == expect_seq_num%32){
        			packet pkt = packet.createEOT(expect_seq_num);
        			byte[] get_data = pkt.getUDPdata();
        			DatagramPacket receiver_pkt  = new DatagramPacket(get_data,get_data.length,emulator_IP,receiver_Emulator_port);
        			emulator_Socket.send(receiver_pkt);
                    //System.out.println("receiver: receive eot and send eot then exit" + seq_num);
                    receiver_file.close();
                    file.close();
        			System.exit(0);
        		}else if(seq_num == expect_seq_num%32 ){
                    old_seq = seq_num;
        			packet pkt = packet.createACK(seq_num);
        			byte[] get_data = pkt.getUDPdata();
        			DatagramPacket receiver_pkt  = new DatagramPacket(get_data,get_data.length,emulator_IP,receiver_Emulator_port);
        			emulator_Socket.send(receiver_pkt);
                    //System.out.println("receiver: send expected ACK " + seq_num);
                    file.write(data,0,length);
        			expect_seq_num++;
        		}else{//not expect
                    //System.out.println("receiver: send unexpected ACK " + seq_num + "waiting for" + (old_seq+1));
        			packet pkt = packet.createACK(old_seq);
        			byte[] get_data = pkt.getUDPdata();
        			DatagramPacket receiver_pkt  = new DatagramPacket(get_data,get_data.length,emulator_IP,receiver_Emulator_port);
        			emulator_Socket.send(receiver_pkt);
        		
            	}
            }catch(Exception e){
                System.err.println("Ack: fail to make eot");
                System.exit(0);
            }
		}
	}
}