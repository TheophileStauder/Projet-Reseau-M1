/**
 * serveur.c
 */


#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>     
#include <unistd.h>     
#include <sys/types.h>      
#include <sys/socket.h>     
#include <netinet/in.h>     
#include <netdb.h>     
#include <memory.h>     
#include <errno.h>      


///TEST adresse///
struct in_addr test;

int creersock( u_short port) {

  
  int sock, retour;

  // n (la structure est d?crite dans sys/socket.h)
  struct sockaddr_in adresse;
  test = adresse.sin_addr; ///  TEST adresse


  sock = socket(AF_INET,SOCK_STREAM,0);

  if (sock<0) {
    perror ("ERREUR OUVERTURE");
    return(-1);
  }
  
  
  adresse.sin_family = AF_INET;

  adresse.sin_port = htons(port);
  adresse.sin_addr.s_addr=INADDR_ANY;
  
  
  int optval; 
  //int optlen;
  optval = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
  
  retour = bind (sock,(struct sockaddr *)&adresse,sizeof(adresse));
  
  
  if (retour<0) {
    perror ("IMPOSSIBLE DE NOMMER LA SOCKET");
    return(-1);
  }

  
  return (sock);
}


int main (int argc, char *argv[]) {

  int newsockfd, s, sock;
  u_short port;
  char msg [BUFSIZ];
  int timeout = 5;
  
  if (argv[1] == NULL ){
      printf("Erreur : PORT incorrect ");
      exit(0);
  }
  
  port=(u_short) atoi(argv[1]);

  sock=creersock (port);

  listen (sock,5);
  printf("Websnarf C version listening on port %d (timeout=%d secs)\n", port,timeout);

  
  
  for ( ; ; )  {
     
      
      newsockfd = accept (sock, (struct sockaddr *) 0, (unsigned int*) 0);

      if ( fork() == 0 ) {
            //close ( sock ) ;
            if (newsockfd == -1) {
                perror("Erreur accept");
                return(-1);
            }
            else{
                //printf("Accept reussi");
            }
            
            s = read(newsockfd, msg, 1024);
            

            if (s == -1){
                perror("Problemes");
                exit (1) ; 
            }
            else{
                msg[s] = 0;
                printf("Message: %s", msg);
              
                //close(newsockfd);


                ///TEST adresse///
                char str[INET_ADDRSTRLEN];
                inet_ntop( AF_INET, &test, str, INET_ADDRSTRLEN );
                //printf("ADRESSE : %s\n", str);

                FILE *fp;
                fp = fopen("./test.txt", "a+");
                fprintf(fp, "Message : %s",msg);
                fclose(fp);
                
            }         
                
      }    
      
  } 

  close(sock);   
  return 0;
}
