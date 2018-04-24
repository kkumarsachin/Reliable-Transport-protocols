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
//int A_sequence_num; //has range from 0 to N+1
int B_sequence_num;  //has range from 0 to N+1
int N; //window size
int base;
int nextseqnum;
int expectedseqnum;

int input_increment = 0;
int output_increment=0;

struct pkt sndpkt[1000];
struct msg sender_buffer[1000];
struct msg current_message;
struct pkt current_packet;
struct pkt ackpacket;
struct pkt previous_ackpacket;


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
   
   if(nextseqnum<base+N)
   {  
     current_message = sender_buffer[output_increment];
      output_increment++; 
       for (int i=0;i<20;i++)
             current_packet.payload[i]=current_message.data[i];
        current_packet.seqnum = nextseqnum;
        current_packet.checksum = calculate_checksum(current_packet);
        sndpkt[nextseqnum] = current_packet;
        tolayer3(0,sndpkt[nextseqnum]);
       if(base==nextseqnum)
           starttimer(0,30.0);  
      nextseqnum++;
       
    }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
   if (packet.checksum == calculate_checksum(packet))
    {

           base = packet.acknum+1;
           if (base==nextseqnum)
              stoptimer(0);
          else
             {
                 stoptimer(0);
                  starttimer(0,30.0);
             }

                 
                 

     }

}

/* called when A's timer goes off */
void A_timerinterrupt()
{
   starttimer(0,30.0);
   for (int i = base; i<nextseqnum;i++)
      tolayer3(0,sndpkt[i]);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  base = 0;
  nextseqnum = 0;
  N = getwinsize();

}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{
    if ((packet.checksum == calculate_checksum(packet))&&(packet.seqnum == expectedseqnum))
     {
         tolayer5(1,packet.payload);
          ackpacket.acknum = expectedseqnum;
          ackpacket.seqnum = expectedseqnum;
          ackpacket.checksum = calculate_checksum(ackpacket);
          previous_ackpacket = ackpacket;
          tolayer3(1,ackpacket);
          expectedseqnum++;

     }
  
   else
    {
       tolayer3(1,previous_ackpacket);
     }

}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
   expectedseqnum = 0;
    previous_ackpacket.acknum = -1;
    previous_ackpacket.seqnum = -1;
    previous_ackpacket.checksum = calculate_checksum(ackpacket);
   
}
