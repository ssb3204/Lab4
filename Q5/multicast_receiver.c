
 #include <sys/socket.h> 
 #include <arpa/inet.h> 
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #define MAXRECVSTRING 255

 int main(int argc, char *argv[])
 {
 int sock; 
 struct sockaddr_in mAddr; 
 char *mIP; 
 unsigned int mPort;


 char buf[MAXRECVSTRING + 1]; /* Buffer for received string */
 int bufLen; /* Length of received string */
 struct ip_mreq mReq; /* Multicast address join
 structure */

 if (argc != 3) {
 fprintf(stderr, "Usage: %s <멀티캐스트 IP 주소> <멀티캐스트 포트>\n",argv[0]);
 exit(1);
 }

 mIP = argv[1];
mPort = atoi(argv[2]);


 if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
 perror("소켓 생성 실패");
 exit(1);
 }


 memset(&mAddr, 0, sizeof(mAddr));
 mAddr.sin_family = AF_INET;
 mAddr.sin_addr.s_addr = htonl(INADDR_ANY);
 mAddr.sin_port = htons(mPort);


 if (bind(sock, (struct sockaddr *) &mAddr, sizeof(mAddr)) < 0) {
 perror("bind() 실패");
 exit(1);
 }


 mReq.imr_multiaddr.s_addr = inet_addr(mIP);
 mReq.imr_interface.s_addr = htonl(INADDR_ANY);


 if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &mReq, sizeof(mReq)) < 0) {
 perror("소켓 옵션 설정 실패");
 exit(1);
 }


 memset(buf, 0, MAXRECVSTRING);
 if ((bufLen = recvfrom(sock, buf, MAXRECVSTRING, 0, NULL, 0)) < 0) {
 perror("수신 실패");
 exit(1);
 }
 printf("Received: %s\n", buf); 
 close(sock);
 exit(0);
 }
