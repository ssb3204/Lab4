 #include <stdio.h>
 #include <sys/socket.h> 
 #include <arpa/inet.h> 
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>

 int main(int argc, char *argv[])
 {
 int sock;
 struct sockaddr_in mAddr; 
 char *mIP; 
 unsigned short mPort; 
 char *buf; 
 unsigned char mTTL; 
 unsigned int bufLen; 

if ((argc < 4) || (argc > 5)) {
 fprintf(stderr, "Usage: %s <멀티캐스트 IP 주소> <포트 번호> <문자열> <TTL>\n",argv[0]);
 exit(1);
}

 mIP = argv[1];
 mPort = atoi(argv[2]);
 buf = argv[3];
 mTTL = atoi(argv[4]);

 if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
 perror("소켓 생성 실패");
 exit(1);
 }


 if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &mTTL, sizeof(mTTL)) < 0) {
 perror("소켓 옵션 설정 실패");
 exit(1);
 }


 memset(&mAddr, 0, sizeof(mAddr));
 mAddr.sin_family = AF_INET;
 mAddr.sin_addr.s_addr = inet_addr(mIP);
 mAddr.sin_port = htons(mPort);

 bufLen = strlen(buf);
 if (sendto(sock, buf, bufLen, 0, (struct sockaddr *)
 &mAddr, sizeof(mAddr)) != bufLen) {
perror("sendto() 전송 실패");
 exit(1);
 }
}
