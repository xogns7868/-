// Student ID : 20153216
// Name : TaeHoon Lee

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
	struct sockaddr_in server, remote;
	int request_sock, new_sock;
	int bytesread;
	socklen_t addrlen;
	char buf[BUFSIZ];

	if (argc != 2) {
		(void) fprintf(stderr,"usage: %s portnum \n",argv[0]);
		exit(1);
	}

	int portnum = atoi(argv[1]);

	if ((request_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		exit(1);
	}
	
	printf("Student ID : 20153216\n");
	printf("Name : TaeHoon Lee\n");

	// Create a Server Socket



    /* Fill the client socket address struct */
    server.sin_family = AF_INET; // Protocol Family
    server.sin_port = htons(portnum); // Port number
    server.sin_addr.s_addr = INADDR_ANY; // AutoFill local address

    /* Bind a special Port */
    if( bind(request_sock, (struct sockaddr*)&server, sizeof(struct sockaddr)) < 0 )
    {
        exit(1);
    }
    if(listen(request_sock,5) < 0)
    {
        exit(1);
    }
    while(1)
    {
		
        addrlen = sizeof(struct sockaddr_in);

        if ((new_sock = accept(request_sock, (struct sockaddr *)&remote, &addrlen)) <= 0) 
        {
             exit(1);
        }
		else{
			printf("Connection : Host IP %s, Port %d, socket %d\n",inet_ntoa(remote.sin_addr),remote.sin_port,remote.sin_family);
		
		bytesread = read(new_sock,buf,sizeof(buf) - 1);	
		if(bytesread <= 0) { 
			close(new_sock); 
			break; 
		}
		printf("%s\b",buf);
		buf[bytesread] = '\0';
		char *ptr = strtok(buf,"GET /");
		FILE *fp = fopen(ptr,"r");	
		if(fp <= 0){
			printf("Server Error : No such file ./%s!\n",ptr);
			char res_msg[BUFSIZ] = "HTTP/1.0 ";
			strcat(res_msg,"404 NOT FOUND\r\n");
			strcat(res_msg,"Connection: close\r\n");
			strcat(res_msg,"ID: 20153216\r\n");
			strcat(res_msg,"Name: TaeHoon Lee\r\n");
			strcat(res_msg,"Content-Length: 0");
			strcat(res_msg,"\r\nContent-Type: text/html\r\n\r\n");
			send(new_sock,res_msg,strlen(res_msg),0);
			//fclose(fp);
			int t = 1;
			setsockopt(new_sock,SOL_SOCKET,SO_REUSEADDR,&t,sizeof(int));
		//		continue;
		}
		else{
			fseek(fp,0,SEEK_END);
		 	int size = ftell(fp);
			fseek(fp,0,SEEK_SET); 
			char fsize[BUFSIZ];
			sprintf(fsize, "%d",size);
	    	char res_msg[BUFSIZ] = "HTTP/1.0 ";
			strcat(res_msg,"200 OK\r\n");
			strcat(res_msg,"Connection: close\r\n");
			strcat(res_msg,"ID: 20153216\r\n");
			strcat(res_msg,"Name: TaeHoon Lee\r\n");
			strcat(res_msg,"Content-Length: ");
			strcat(res_msg,fsize);
			strcat(res_msg,"\r\nContent-Type: text/html\r\n\r\n");
			send(new_sock,res_msg,strlen(res_msg),0);
			char sendfile[size + 1];
			memset(sendfile,0,size + 1);
			char strTemp[size + 1];
			memset(strTemp,0,sizeof(strTemp));
			int f_size = 0;	
			/*while((bytesread = fread(sendfile,sizeof(char),size, fp))>0)
        	{
        	    send(new_sock,sendfile,bytesread,0);
				f_size += bytesread;
				memset(sendfile,0,sizeof(sendfile));
       		}*/
			while(!feof(fp)){
				fgets(strTemp,sizeof(strTemp),fp);
				f_size += strlen(strTemp);
				strcat(sendfile,strTemp);
				memset(strTemp,0,sizeof(strTemp));
			}
			send(new_sock,sendfile,strlen(sendfile),0);
			fclose(fp);
			printf("finish %d %d\n",size,f_size);
			int t = 1;
			setsockopt(new_sock,SOL_SOCKET,SO_REUSEADDR,&t,sizeof(int));
			}
		}
	}	
}
	

