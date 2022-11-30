#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <strings.h>
#include <bits/stdc++.h>
#include <pthread.h>

using namespace std;

// Message record
struct message {
   int    ivalue;
   double dvalue;
   char   cvalue[56];
   char   mname[21];
};

int connection;
message incmessage;
char name[21];

void* listener(void* arg)
   {
      pthread_detach(pthread_self());
      int* temp = (int*)arg;
      int connection = *temp;

      for(;;)
      {
         read(connection, (char*)&incmessage, sizeof(message));
         char sender[21];
         if(strcmp(incmessage.mname, name) == 0)
         {
            strcpy(sender, "\033[1;32mYou\033[0m");

            if(strcmp(incmessage.cvalue, "bye") == 0)
            {
               break;
            }
         }
         else{
            strcpy(sender, incmessage.mname);
         }
         cout << "\033[1;31m" << sender << "\033[0m: " << incmessage.cvalue << endl;
      }

      //pthread_exit(NULL);
      return 0;
   }

int main(int argc, char** argv)
{
   struct addrinfo *myinfo; // Address record
   char hostname[81];
   char portnum[81];
   message mymessage;
   int sockdesc;
   int value;
   char chat[56];
   pthread_t ptid;

   if ( argc == 1 )
   {
      cout << "Usage: client <host> [<portnum>]" << endl;
      exit(0);
   }

   strcpy(hostname, argv[1]);
   if ( argc == 3 )
   {
      strcpy(portnum, argv[2] );
   }
   else
   {
      strcpy(portnum, "2000");
   }

   cout << "Attempting to connect..." << endl;

   // Use AF_UNIX for unix pathnames instead
   // Use SOCK_DGRAM for UDP datagrams
   sockdesc = socket(AF_INET, SOCK_STREAM, 0);
   if ( sockdesc < 0 )
   {
      cout << "Error creating socket" << endl;
      exit(0);
   }

   // Set up the address record
   if ( getaddrinfo(hostname, portnum, NULL, &myinfo) != 0 )
   {
      cout << "Error getting address" << endl;
      exit(0);
   }

   // Connect to the host
   connection = connect(sockdesc, myinfo->ai_addr, myinfo->ai_addrlen);
   if ( connection < 0 )
   {
      cout << "Error in connect" << endl;
      exit(0);
   }
   cout << "Connected!" << endl;

   cout << "Enter your chat name: " << endl;
   cin.getline(name, 20);

   if(pthread_create(&ptid, NULL, &listener, (void*)&sockdesc))
   {
      cout << "Thread creation failed!" << endl;
   }

   cout << "Begin chatting: " << endl;
   for(;;)
   {
      mymessage.ivalue = 1;
      mymessage.dvalue = 32.4;
      memcpy(mymessage.mname, name, sizeof(mymessage.mname));
      cin.getline(chat, 55);
      strcpy(mymessage.cvalue, chat);

      write(sockdesc, (char*)&mymessage, sizeof(message));

      if(strcmp(chat, "bye") == 0)
      {
         break;
      }

      if(strcmp(chat, "change name") == 0)
      {
         cout << "Enter new chat name: " << endl;
         cin.getline(name, 20);
         memcpy(mymessage.mname, name, sizeof(mymessage.mname));
      }

   }
      // Close the socket
      pthread_join(ptid, NULL);
      close(sockdesc);
   return 0;

}
