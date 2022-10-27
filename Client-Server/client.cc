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
using namespace std;

// Message record
struct message {
   int    ivalue;
   double dvalue;
   char   cvalue[56];
};

int main(int argc, char** argv)
{
   struct addrinfo *myinfo; // Address record
   int sockdesc;
   char hostname[81];
   char portnum[81];
   message mymessage;
   int connection;
   int value;

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

   // Use AF_UNIX for unix pathnames instead
   // Use SOCK_DGRAM for UDP datagrams
   cout << "Before socket" << endl;
   sockdesc = socket(AF_INET, SOCK_STREAM, 0);
   if ( sockdesc < 0 )
   {
      cout << "Error creating socket" << endl;
      exit(0);
   }

   // Set up the address record
   cout << "Before getaddrinfo" << endl;
   if ( getaddrinfo(hostname, portnum, NULL, &myinfo) != 0 )
   {
      cout << "Error getting address" << endl;
      exit(0);
   }

   // Connect to the host
   cout << "Before connect" << endl;
   connection = connect(sockdesc, myinfo->ai_addr, myinfo->ai_addrlen);
   if ( connection < 0 )
   {
      cout << "Error in connect" << endl;
      exit(0);
   }
   cout << "Client connection = " << connection << endl;

   mymessage.ivalue = 1;
   mymessage.dvalue = 34.2;
   strcpy(mymessage.cvalue, "Client message");

   // Display the message to be sent
   cout << "Client sends: " << endl;
   cout << "  ivalue: " << mymessage.ivalue << endl;
   cout << "  dvalue: " << mymessage.dvalue << endl;
   cout << "  cvalue: " << mymessage.cvalue << endl;

   // Send the message to the server
   write(sockdesc, (char*)&mymessage, sizeof(message));

   // Read back exactly one message
   value = read(sockdesc, (char*)&mymessage, sizeof(message));

   // Display the received message
   cout << "Client gets back: " << endl;
   cout << "  ivalue: " << mymessage.ivalue << endl;
   cout << "  dvalue: " << mymessage.dvalue << endl;
   cout << "  cvalue: " << mymessage.cvalue << endl;

   // Close the socket
   close(sockdesc);

   return 0;

}

