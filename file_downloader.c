// Student ID : 20153216
// Name : Lee TaeHoon

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROMPT() {printf("\n> ");fflush(stdout);}
#define GETCMD "wget"
#define QUITCMD "quit"


int main(int argc, char *argv[]) {
	int socktoserver = -1;
	char buf[BUFSIZ];
	struct hostent *hostp;
	struct sockaddr_in server;
	char msg[BUFSIZ*500];
	char data[BUFSIZ+1];
	printf("Student ID : 20153216\n");
	printf("Name : TaeHoon Lee\n");

	PROMPT(); 

	for (;;) {
		socktoserver = socket(AF_INET, SOCK_STREAM, 0);
		char fname[BUFSIZ] = "GET ";	
		if(socktoserver < 0 ){
			perror("socket");
			exit(0);
		}
		if (!fgets(buf, BUFSIZ - 1, stdin)) {
			if (ferror(stdin)) {
				perror("stdin");
				exit(1);
			}
			exit(0);
		}

		char *cmd = strtok(buf, " \t\n\r");

		if((cmd == NULL) || (strcmp(cmd, "") == 0)) {
			PROMPT(); 
			continue;
		} else if(strcasecmp(cmd, QUITCMD) == 0) {
				exit(0);
		}

		if(!strcasecmp(cmd, GETCMD) == 0) {
			printf("Wrong command %s\n", cmd);
			PROMPT(); 
			continue;
		}		
		// connect to a server
		char *hostname = strtok(NULL, " \t\n\r");
		char *pnum = strtok(NULL, " ");
		char *filename = strtok(NULL, " \t\n\r");
		hostp = gethostbyname(hostname);
		strcat(fname,filename);
		strcat(fname," HTTP/1.0\r\n");
		strcat(fname,"Host: ");
		strcat(fname,hostname);
		strcat(fname,"\r\n");
		strcat(fname,"User-agent: HW1/1.0\r\n");
		strcat(fname,"ID: 20153216\r\n");
		strcat(fname,"Name: TaeHoon Lee\r\n");
		strcat(fname,"Connection: close\r\n\r\n");
		if((hostp = gethostbyname(hostname)) == 0) {
			printf("%s: unknown host\n",hostname);
			printf("cannot connect to server %s %s\n",hostname,pnum);
			PROMPT();
			continue;
		}	
		memset((void *) &server, 0, sizeof (server));
		server.sin_family = AF_INET;
		memcpy((void *) &server.sin_addr, hostp->h_addr, hostp->h_length);
		server.sin_port = htons((u_short)atoi(pnum));

		if(connect(socktoserver, (struct sockaddr*)&server, sizeof(server)) < 0)
        {	(void) close(socktoserver);
     		printf("connect error");
     		exit(0);
		}
		send(socktoserver,fname,strlen(fname),0);
		memset(data,0,sizeof(data));
		memset(msg,0,sizeof(msg));
		char *fptr[sizeof(filename)] = {NULL, };
		int idx = 0;
		filename = strtok(filename,"/");
		while(filename != NULL){
			fptr[idx] = filename;
			idx++;
			filename = strtok(NULL, "/");
		}
		int file_size = 0;
		int r_file_size = 0;
		int response_msg_size = 0;
		int f_size[100];
		int f_size_idx = 0;
		int download_level = 0;
		int Lcheck = 0;
		int Typecheck = 0;
		int check = 0;
		int j = 0;
		char *status;
		FILE *fd;
		while(1) { 
			int recv_len = recv(socktoserver, data, BUFSIZ, 0); 
			if(recv_len == 0) { 
				break; 
			} 
			for(int i = 0; i < recv_len; i++){
				msg[j++] = data[i]; 
			}
			if(check == 0){
				char *ptr;
				if((ptr = strstr(msg, "200"))){
					check = 1;
				}
				else{			
					//printf("%s",msg);
					ptr = strstr(msg,"HTTP/1.1 ");
					ptr = strtok(ptr, "\r\n");
					ptr = ptr + 9;
					status = ptr;
					break;
				}
			}
			file_size += recv_len;
			memset(data,0,sizeof(data));	
			for(int i = 0; i < file_size; i++){
				if(msg[i] == '\r' && msg[i+1] == '\n' && msg[i+2] == '\r' && msg[i+3] == '\n'){
					response_msg_size = i + 4;
					break;
				}
			}
			if(Lcheck == 0){
				char *ptr = strstr(msg, "Content-Length: ");
				ptr = ptr + 16;
				r_file_size = atoi(ptr);
				printf("Total Size %d bytes\n",r_file_size);
				Lcheck = 1;
			}
			if(Typecheck == 0){
				char *ptr = strstr(msg, "Type");
				ptr = ptr + 6;
				ptr = strtok(ptr,": ");
				if((ptr = strstr(ptr, "Image"))){
					fd = fopen(fptr[idx-1],"wb");
					Typecheck = 1;	
				}
				else{
					fd = fopen(fptr[idx-1],"w");
					Typecheck = 1;		
				}
			}
			if(((file_size - response_msg_size)*100/r_file_size)>= download_level+10 && download_level<=100){
				download_level=((file_size-response_msg_size)*100/r_file_size) - ((file_size-response_msg_size)*100/r_file_size)%10;
				printf("Current Downloading %d/%d (bytes) %d%s\n",file_size-response_msg_size,r_file_size,download_level, "%");
			}
		}
		if(check == 1){
		printf("Download Complete : %s, %d/%d\n",fptr[idx-1],r_file_size,r_file_size);
		fwrite((void *)(msg+response_msg_size),1,r_file_size,fd);
		fclose(fd);
		memset(data,0,sizeof(data));
		memset(msg,0,sizeof(msg));
		PROMPT();
		}
		else{
			printf("%s\n", status);
			PROMPT();
		}
	}
} 


		

