

import java.io.*;
import java.lang.*;
import java.net.ServerSocket;
import java.net.*;
import java.util.*; 
import java.nio.ByteBuffer;


public class Ack extends Thread {
    public static int last_seq;
    public static DatagramSocket sender_Socket;
    public static int sender_Emulator_port;
    public static InetAddress ip_Address;
    public Ack(int emulator_Sender_port, int seqnum, int emulator_port,InetAddress address) {
        last_seq = seqnum;
        try{
            sender_Socket = new DatagramSocket(emulator_Sender_port);
        }catch (IOException e) {
            System.err.println("Ack: fail to create socket");
        }
        ip_Address = address;
        sender_Emulator_port = emulator_port;
    }
    public void run() {
        int new_base = 0;
        int expect_seq = 0;
        int old_seq = -1;
        try{
        	PrintWriter ack_file = new PrintWriter("ack.log");
            while(true) {
                //System.out.println("Ack: start");
                byte[] pkt = new byte[512];
                DatagramPacket ack_pkt = new DatagramPacket(pkt, pkt.length);
                sender_Socket.receive(ack_pkt); 
                byte[] ack_data = ack_pkt.getData();
                ByteBuffer buffer = ByteBuffer.wrap(ack_data);
                //get detail
                int type = buffer.getInt();
            	int seq_num = buffer.getInt();
                //System.out.println("Ack before:"+"seq:"+seq_num+ "exp_seq:" + expect_seq%32 );
                // if(expect_seq%32 == seq_num){
                //     expect_seq += 1;
                //     //new_base = seq_num+1;
                //     new_base+=1;
                //     Sender.set_base(new_base);
                //     if (new_base == Sender.get_nextseq()) { 
                //         Sender.stop_timer();
                //     } else {
                //         Sender.set_timer();
                //         // Sender.task = new TimeoutTask();
                //         // Sender.timer.schedule(Sender.task,100);
                //     }
                if(old_seq == seq_num){
                    continue;
                }else{
                    old_seq = seq_num;
                    while(new_base%32 != (seq_num+1)%32){
                        new_base+=1;
                    }
                    //new_base = seq_num+1;
                    //new_base+=1;
                    expect_seq = new_base;
                    Sender.set_base(new_base);
                    if(new_base != Sender.get_nextseq()){
                        Sender.set_timer();
                    }
                }
                    
                
                ack_file.println(seq_num);
                //System.out.println("Ack:"+"seq:"+seq_num+ "exp_seq:" + expect_seq%32 + " new_base:"+ new_base + " last_seq:" + last_seq);
                if (type == 2 && new_base > last_seq) { //EOT
                    //System.out.println("ACK: ack eot " + seq_num);
                    //Sender.set_base(new_base);
                    break;
                }
                
            }
            ack_file.close();
            //System.out.println("Ack: end");
        }catch (IOException e) {
            System.err.println("Ack: fail to receive ack");
        }
        
    }
}