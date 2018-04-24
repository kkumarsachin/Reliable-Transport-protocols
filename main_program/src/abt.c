#include "../include/simulator.h"
#include<string.h>
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

/********* STUDENTS WRITE THE NEXT SIX ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */

struct pkt sndpkt;
struct pkt ackretransmit;
int wait_for_ACK=0;
int A_sequence_no;
int B_sequence_no_expected ;

int input_increment = 0;
int output_increment=0;
struct msg sender_buffer[1000];
struct msg current_message;
struct pkt previous_packet;
 struct pkt ackpacket;


int calculate_checksum(struct pkt pack)
{
    int sum = 0;
    sum += (pack.seqnum+pack.acknum);
    //printf("%d",sum);
    for(int i=0;i<20;i++)
       sum+=pack.payload[i];
    return sum;
        
}

void A_output(message)
  struct msg message;
{
    sender_buffer[input_increment] = message;
    input_increment++;
    
     
    //printf("%s\n",message.data);
    if(!wait_for_ACK)
       {
              current_message = sender_buffer[output_increment];
              output_increment++;
        for (int i=0;i<20;i++)
             sndpkt.payload[i]=current_message.data[i]; 
      
       sndpkt.seqnum = A_sequence_no;
       
       sndpkt.checksum = calculate_checksum(sndpkt);
       
        wait_for_ACK = 1;
        previous_packet = sndpkt;
        tolayer3(0,sndpkt);   //udt_send
        starttimer(0, 20.0);
        
      }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
 
   
   if ((packet.checksum == calculate_checksum(packet))&&packet.acknum ==A_sequence_no){
           
           stoptimer(0);
           A_sequence_no= !A_sequence_no ;
           wait_for_ACK = 0;
          
           


       }

}

/* called when A's timer goes off */
void A_timerinterrupt()
{
   
   tolayer3(0, previous_packet);
   starttimer(0, 20.0);
   wait_for_ACK = 1;
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
   wait_for_ACK = 0;
    A_sequence_no= 0;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{

  

   if((packet.checksum ==calculate_checksum(packet))&&(packet.seqnum==B_sequence_no_expected))
       
     {

       
        
         tolayer5(1,packet.payload);

         
         ackpacket.acknum = packet.seqnum;
         
         ackpacket.checksum = calculate_checksum(ackpacket);
       
         tolayer3(1,ackpacket);
         B_sequence_no_expected = !B_sequence_no_expected ;
     

       }
     else if ((packet.checksum ==calculate_checksum(packet))&&(packet.seqnum!=B_sequence_no_expected))
        {
          
             tolayer3(1,ackpacket);
	     return;
         }

        
         
        

     
}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
   B_sequence_no_expected = 0;

}
