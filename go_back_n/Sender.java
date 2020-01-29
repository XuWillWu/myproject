

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.io.*;
import java.util.Scanner;
import java.nio.ByteBuffer;
import java.util.*;



public class Sender{
    //  declare basic variable
    public static int base = 0;
    public static int window_Size = 10;
    public static int next_seqnum = 0;
    public static boolean timer_inuse = false;
    public static boolean running = true;
    public static boolean waiting = false;
    public static DatagramSocket emulator_Socket;
    public static Timer timer = new Timer();
    public static TimeoutTask task = null;
    public static ArrayList<packet> sndpkt = new ArrayList<>();
    public static InetAddress emulator_IP ;
    public static int sender_Emulator_port ;


    public static void main(String argv[]) throws IOException{
        if(argv.length != 4){
            System.err.println("Expect 4 argument");
            System.exit(0);
        }

//  process argument

        emulator_IP = InetAddress.getByName(argv[0]);
        sender_Emulator_port = Integer.parseInt(argv[1]);
        int emulator_Sender_port = Integer.parseInt(argv[2]);
        File file_origin = new File(argv[3]);
        FileInputStream file = new FileInputStream(file_origin);




//  seperate file into packets

        int length = (int)file_origin.length();
        int i;
        if(length / 500 > 0){
            i = 500;
        }else{
            i = length%500;
        }
        byte[] pkt_data = new byte[i];
        //ArrayList<packet> sndpkt = new ArrayList<>();
        try{
            while(file.read(pkt_data)!=-1 ) {
                packet pkt = packet.createPacket(next_seqnum, new String(pkt_data));
                sndpkt.add(pkt);
                next_seqnum++;
                length-=500;
                if(length <= 0){break;}
                if(length / 500 > 0){
                    i = 500;
                }else{
                    i = length%500;
                }
                pkt_data = new byte[i];
                //System.out.println("Sender: newpkt");
            }
        }catch(Exception e){
            //System.out.println(length);
            System.err.println("Sender: fail to read file");
        }
        // Packet packet = new Packet(2,next_seqnum,new String());
        // sndpkt.add(packet);

        
        PrintWriter sender_file = new PrintWriter("seqnum.log");
        PrintWriter time_file = new PrintWriter("timer.log");

        //PrintWriter record = new PrintWriter("record.log");

        int last_seq = next_seqnum-1;

        Ack ack = new Ack(emulator_Sender_port, last_seq, sender_Emulator_port, emulator_IP);
        set_nextseq(0);
        ack.start();
        emulator_Socket = new DatagramSocket();

        long startTime = System.currentTimeMillis();
        
//  loop to send packet
        //check If the sender has a packet to send,
        while(true){
            //record.println("Sender: round");
            if(get_nextseq() > last_seq  && get_base() > last_seq){
                //System.out.println("Sender: send eot");
                break;
                
            }
            if(get_nextseq() < get_base() + window_Size && get_nextseq() <= last_seq){

                //send packet

                packet pkt = sndpkt.get(get_nextseq());
                byte[] get_data = pkt.getUDPdata();
                DatagramPacket sender_pkt  = new DatagramPacket(get_data,get_data.length,emulator_IP,sender_Emulator_port);
                emulator_Socket.send(sender_pkt);
                sender_file.println(get_nextseq()%32);
                //System.out.println("Sender: send packet" + get_nextseq());

                if(get_base() == get_nextseq()){
                    // }, 100);
                    task = new TimeoutTask();
                    timer.schedule(task,100);
                    
                }

                set_nextseq(get_nextseq() + 1);

            }
            

        }
        try{
            packet p = packet.createEOT(last_seq+1);
            DatagramPacket eot = new DatagramPacket(p.getUDPdata(), p.getUDPdata().length, emulator_IP, sender_Emulator_port);
            Sender.sendEOT(eot);
            sender_file.println(next_seqnum%32);
            //System.out.println("Sender: send eot and wait eot from receiver");
            waiting = true;
            //System.out.println("Sender: eot sent");
        }catch(Exception e){
            System.err.println("Sender: fail to make eot");
            
        }
        sender_file.close();
        //record.close();
        try{
            //System.out.println("Sender: wait ack eot");
            ack.join();
            //System.out.println("Sender: exit");
        }catch(Exception e){
            System.err.println("Sender: ack fail to exit ");
        }
       // System.out.println((new Date()).getTime() - startTime);
        time_file.println((new Date()).getTime() - startTime);
        time_file.close();
        System.exit(0);

    }

    
    public static synchronized void set_nextseq(int s) {
        next_seqnum = s;
    }
    public static synchronized int get_nextseq() {
        return next_seqnum;
    }

    public static synchronized void set_base(int seq) {
        base = seq;
    }
    public static synchronized int get_base() {
        return base;
    }
    public static synchronized void sendEOT(DatagramPacket pkt) {
        try{
            emulator_Socket.send(pkt);
        }catch(Exception e){
            System.err.println("Sender: fail to send eot");
        }
    }
    static class TimeoutTask extends TimerTask {
        @Override
        public void run(){
            resend_N();
        }
    }
    public static synchronized void set_timer() {
        //System.out.println("timer in use");
        task = new TimeoutTask();
        timer.schedule(task,100);
    }
    public static synchronized void stop_timer() {
        timer.cancel();
        timer = new Timer();
        // timer_inuse = false;
    }
    public static synchronized void resend_N() {
        timer.cancel();
        timer = new Timer();
        task = new TimeoutTask();
        timer.schedule(task,100);
        //System.out.println("Sender timer:" + "base:" + get_base() + " get_nextseq:" + get_nextseq());
        for(int i = get_base(); i < get_nextseq(); i++){
            packet pkt = sndpkt.get(i);
            byte[] get_data = pkt.getUDPdata();
            DatagramPacket sender_pkt  = new DatagramPacket(get_data,get_data.length,emulator_IP,sender_Emulator_port);
            try{
                emulator_Socket.send(sender_pkt);
            }catch(Exception e){
                System.err.println("Sender:cannot send pkt when time is up");
            }
        }
    }
                        
}
