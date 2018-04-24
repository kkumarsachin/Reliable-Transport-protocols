#include "../include/simulator.h"
#include<stdio.h>

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */

int N; //window size
int D;
int sender_base;
int receiver_base;
int nextseqnum;
float updated_time = 0.0;

int input_increment = 0;
int output_increment=0;

struct msg current_message;
struct pkt current_packet;
struct pkt sndpkt[1000];
struct pkt rcvpkt[1000];
struct msg sender_buffer[1000];
struct pkt ackpacket;

struct ack_pkt{
   struct pkt A_pack;
   int ACK_received;
   float timestamp;
   
};

 struct ack_pkt ack_packet[1000];


int calculate_checksum(struct pkt pack)
{
    int sum = 0;
    sum += (pack.seqnum+pack.acknum);
    //printf("%d",sum);
    for(int k=0;k<20;k++)
       sum+=pack.payload[k];
    return sum;
        
}


void A_output(message)
  struct msg message;
{
    sender_buffer[input_increment] = message;
    input_increment++;
   
   if((sender_base<=nextseqnum)&&(nextseqnum<sender_base+N))
   {  
     current_message = sender_buffer[output_increment];
      output_increment++; 
       for (int i=0;i<20;i++)
             current_packet.payload[i]=current_message.data[i];
        current_packet.seqnum = nextseqnum;
        current_packet.checksum = calculate_checksum(current_packet);
        sndpkt[nextseqnum] = current_packet;
        ack_packet[nextseqnum].A_pack = current_packet;
        
        
        tolayer3(0,sndpkt[nextseqnum]);
          
          ack_packet[nextseqnum].timestamp = get_sim_time();
        
       
       if(sender_base==nextseqnum)
           {starttimer(0,30.0); 
           
           }
      nextseqnum++;
      
       
    }

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
  
   float ACK_received_time = get_sim_time();
   if (packet.checksum == calculate_checksum(packet))
    {
     
      if(sender_base<=packet.acknum<sender_base+N)
       {
          
          ack_packet[packet.acknum].ACK_received = 1;

          if(packet.seqnum==sender_base)
            {
             
             for(int n = sender_base;n<=nextseqnum ;n++)
               {
                 
                 if(ack_packet[n].ACK_received == 0||ack_packet[n].ACK_received == -1)
                   {
                      sender_base = n;
                      
                      break;
                    }
               }
               
             if((sender_base) == nextseqnum)
                {
                  stoptimer(0); 
                  }
             else
               {
                 
                 stoptimer(0);
                 float min = ack_packet[sender_base].timestamp;
                 for (int r =sender_base+1;r<nextseqnum;r++)
                    {
                      
                      if (ack_packet[r].ACK_received==0)
                       
                        {
                          
                          if(ack_packet[r].timestamp<min)
                            min = ack_packet[r].timestamp;
                        }

                     }
                    
                   float TIMEOUT = 30.0 - (ACK_received_time-min);
                    
                   starttimer(0,TIMEOUT);
            
               }
                 
           
             }

        }


   }   

}

/* called when A's timer goes off */
void A_timerinterrupt()
{
    int min_index = sender_base;
    float min2 = ack_packet[sender_base].timestamp;
    for (int r =sender_base+1;r<nextseqnum;r++)
       {
          
          if (ack_packet[r].ACK_received==-1)
                       
            {
              
              if(ack_packet[r].timestamp<min2)
                {
                  min2 = ack_packet[r].timestamp;
                  min_index = r;
  
                }
             }

        }
                 

    tolayer3(0,sndpkt[min_index]);
    
    updated_time = get_sim_time();
    ack_packet[min_index].timestamp = updated_time;
   

    min_index = sender_base;
    min2 = ack_packet[sender_base].timestamp;
     
    

   for (int r =sender_base+1;r<nextseqnum;r++)
       {
          
          if (ack_packet[r].ACK_received==-1)
                       
            {
              
              if(ack_packet[r].timestamp<min2)
                {
                  min2 = ack_packet[r].timestamp;
                   min_index = r;
  
                }
             }

        }

  

  float TIMEOUT1 = 30.0 - (updated_time - min2);
  
  starttimer(0,TIMEOUT1);
    
       
  
   
}  


/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  sender_base = 0;
  nextseqnum = 0;
  N = getwinsize();
 
for(int k=0;k<1000;k++) 
 {
   ack_packet[k].ACK_received = -1;
   ack_packet[k].timestamp =0;
 }

}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{
  char pakdata[20] ;
  for(int l =0;l<20;l++)
      pakdata[l] = packet.payload[l];
 int sum = 0;
    sum += (packet.seqnum+packet.acknum);
    
    for(int k=0;k<20;k++)
       sum+=pakdata[k];
  
  

  if((packet.checksum == sum)&&(receiver_base<=packet.seqnum)&&(packet.seqnum<(receiver_base+D)))
    {
      
       ackpacket.acknum = packet.seqnum;
       ackpacket.seqnum = packet.seqnum;  
       ackpacket.checksum = calculate_checksum(ackpacket);
       tolayer3(1,ackpacket);
       
       for(int s=0;s<20;s++)
        ackpacket.payload[s] = pakdata[s];
       

       rcvpkt[packet.seqnum] = ackpacket;
          
          
       if(packet.seqnum==receiver_base)
          {
             
            for(int m=receiver_base;m<1000;m++)
               {
                if(rcvpkt[m].acknum!=-1)
                 {
                   tolayer5(1,rcvpkt[m].payload);
                   
                   receiver_base++;
                   
                 }
              else
                  break;
               }
        
           }
         

     }

  else if((packet.checksum == sum)&&((receiver_base-D)<=packet.seqnum)&&(packet.seqnum<receiver_base))
   {
       ackpacket.acknum = packet.seqnum;
       ackpacket.seqnum = packet.seqnum;  
       ackpacket.checksum = calculate_checksum(ackpacket);
       
       tolayer3(1,ackpacket);
    }
     
     
     
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  receiver_base = 0; 
  D = getwinsize();
   for(int f=0;f<1000;f++) 
 {
   rcvpkt[f].acknum = -1;
   
 }
  
}
