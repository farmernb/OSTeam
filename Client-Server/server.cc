#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <map>
using namespace std;

// Message record
struct message {
   int    ivalue;
   double dvalue;
   char   cvalue[56];
   char   mname[21];
};

struct threadData {
   int connection;
   int index;
};

int connections[10];
pthread_t ptid[10];
map<int, int> pid;

void postMessage(message outmessage)
{
   message byeMessage;
   strcpy(byeMessage.mname, "Server");
   // if(strcmp(outmessage.cvalue, "bye") == 0)
   // {
   //    char temp[56] = "Goodbye ";
   //    strcat(temp, outmessage.mname);
   //    strcat(temp, "!");

   //    strcpy(byeMessage.cvalue, temp);
   // }

   for(int connection : connections)
   {
      if(connection != 0)
      {
         write(connection, (char *) &outmessage, sizeof(outmessage));
         // if(strcmp(outmessage.cvalue, "bye") == 0){
         //    write(connection, (char *) &byeMessage, sizeof(message));
         // }
      }
   }
}

void* clientThread(void* arg)
{
   pthread_detach(pthread_self());
   struct threadData *myData;
   myData = (struct threadData *) arg;
   message clientmessage;
   pid[pthread_self()] = myData -> index;

   for(;;)
   {
      read(connections[pid[pthread_self()]], (char*)&clientmessage, sizeof(message));
         
      postMessage(clientmessage);

      if(strcmp(clientmessage.cvalue, "bye") == 0)
      {
         close(connections[pid[pthread_self()]]);
         cout << "Removed client thread for connection: " << connections[pid[pthread_self()]] << endl;
         connections[pid[pthread_self()]] = 0;
         pid.erase(pthread_self());
         break;
      }
   }
   //pthread_exit(NULL);
   return 0;
}

int main(int argc, char** argv )
{
   int sockdesc;            // Socket descriptor
   struct addrinfo *myinfo; // Address record
   char portnum[81];
   message mymessage;
   int connection;
   int value;
   int threadIndex = 0;
   threadData tInfo;

   // Create the socket with domain, type, protocol as
   //    internet, stream, default
   sockdesc = socket(AF_INET, SOCK_STREAM, 0);
   if ( sockdesc < 0 )
   {
      cout << "Error creating socket" << endl;
      exit(0);
   }

   // Get the port number from the command line
   if ( argc > 1 )
   {
      strcpy(portnum, argv[1]);
   }
   else
   {
      strcpy(portnum, "2000");
   }

   // Set up the address record
   if ( getaddrinfo("127.0.0.1", portnum, NULL, &myinfo) != 0 )
   {
      cout << "Error getting address" << endl;
      exit(0);
   }

   // Bind the socket to an address
   if (bind(sockdesc, myinfo->ai_addr, myinfo->ai_addrlen) < 0 )
   {
      cout << "Error binding to socket" << endl;
      exit(0);
   }

   // Now listen to the socket
   if ( listen(sockdesc, 1) < 0 )
   {
      cout << "Error in listen" << endl;
      exit(0);
   }

   // Note: this loop is typical of servers that accept multiple
   // connections on the same port - normally, after accept( )
   // returns, you'd fork off a process to handle that request, or
   // create a thread to do the same, passing the returned value as
   // a parameter for read( ) and write( ) to use -
   // that is, accept( ) returns a new descriptor - 
   // you actually talk on a different socket in the child.
   // The main program would then loop around, and wait at accept( )
   // for another request for a connection, then hand that new
   // connection off to a child or thread, etc.
   // So the server runs forever - see the for loop parameters -
   // well, until you kill it manually.  Here, we break from
   // the for loop after one message.
   for(;;)
   {
      // Accept a connect, check the returned descriptor
      connection = accept(sockdesc, NULL, NULL);
      if ( connection < 0 )
      {
         cout << "Error in accept" << endl;
         exit(0);
      }
      else
      {
         //finding an empty space in the connections array and pthread_t array
         for(int i = 0; i < sizeof(connections); i++)
         {
            if(connections[i] == 0)
            {
               threadIndex = i;
               break;
            }
         }

         tInfo.connection = connection;
         tInfo.index = threadIndex;

         if(pthread_create(&ptid[threadIndex], NULL, &clientThread, (void*)&tInfo))
         {
            cout << "Thread creation failed!" << endl;
         }
         connections[threadIndex] = connection;

         cout << "Created client thread for connection: " << tInfo.connection << endl;
      }

   } // for

   cout << "Got here somehow" << endl;
   return 0;

} // main( )


