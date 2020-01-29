#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <string.h>
#include <arpa/inet.h>
#include<pthread.h>
#include <unistd.h>
int SERV_PORT=101;

#define MAXNAME 2000
struct g_record_s {
	int x;
	int y;
	int z;		
};
struct g_record_s record;
int watch_value=0;
int condition_value=0;
void function_C(int c)
{
	memset(&record,0xc, sizeof(record));
	if (c == 3) {
		watch_value ++;
	}
	condition_value ++;

	
}

int function_C2(int c)
{
	int sum=0;

	if (c <= 1) {
		return 0;	
	}
	sum = c + function_C2(c-1);
	return sum;
}

void function_B(int b)
{
	int sum;

	memset(&record, 0xb, sizeof(record));
	b ++;
	function_C(b);
	sum = function_C2(4);	
}
void function_A(int a)
{
	struct A_s {
		int x;
		int y;
		char z[10];
	};
	struct A_s aa;
	int i;

	memset(&record,0xaa,sizeof(record));
	aa.x = 10;
	aa.y = 20;
	strcpy(aa.z,"test");
	function_B(a);
}


extern int errno;
long byte_count=0;
long packet_count=0;
void atomic_test()
{
	int var =0;
	__sync_add_and_fetch(&var,1);
//	printf("var=%d\n",var);
}
void *thread(void *ptr)
{
	int num=0;
	int count = 0;
	while (1){
		printf("%d:packet count=%ld\n",num++,packet_count);
		packet_count=0;
		sleep(1);
		function_A(count ++);
	}
}
pthread_t thread1, thread2;
int main(int argc, char *argv[]){
        int socket_fd;   /* file description into transport */
        int recfd; /* file descriptor to accept        */
        int length; /* length of address structure      */
        int nbytes; /* the number of read **/
        char buf[BUFSIZ];
        struct sockaddr_in myaddr; /* address of this service */
        struct sockaddr_in client_addr; /* address of client    */
        /*
         *      Get a socket into UDP/IP
         */
	if (argc ==2) {
		int port ;
		port = atoi(argv[1]);
		SERV_PORT = port;
	}	
	printf("listen on port %d\n",SERV_PORT);	
        if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) <0) {
                perror ("socket failed");
                exit(EXIT_FAILURE);
        }
        /*
         *    Set up our address
         */
        bzero ((char *)&myaddr, sizeof(myaddr));
        myaddr.sin_family = AF_INET;
        myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        myaddr.sin_port = htons(SERV_PORT);

        /*
         *     Bind to the address to which the service will be offered
         */
        if (bind(socket_fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) <0) {
                perror ("bind failed\n");
                exit(1);
        }

        /*
         * Loop continuously, waiting for datagrams
         * and response a message
         */
        length = sizeof(client_addr);
        printf("Server is ready to receive !!\n");
        printf("Can strike Cntrl-c to stop Server >>\n");

	pthread_create(&thread1, NULL, *thread, (void *) 1);
        
	 while (1) {
                if ((nbytes = recvfrom(socket_fd, &buf, MAXNAME, 0, (struct sockaddr*)&client_addr, (socklen_t *)&length)) <0) {
                        perror ("could not read datagram!!");
                        continue;
                }
		byte_count += nbytes;
		packet_count ++;

                //printf("Received data form %s : %d\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
                //printf("%s\n", buf);

                /* return to client */
		/*
                if (sendto(socket_fd, &buf, nbytes, 0, (struct sockaddr*)&client_addr, length) < 0) {

                        perror("Could not send datagram!!\n");
                  	continue;
                }
		*/
                //printf("Can Strike Crtl-c to stop Server >>\n");
        }
	printf("byte_count=%ld\n",byte_count);
	printf("packet_count=%ld\n",packet_count);
	return 0;
}
