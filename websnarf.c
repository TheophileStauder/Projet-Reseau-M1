/**
 * Projet Réseau M1
 *
 *Websnarf C version
 *Author : Stauder Theophile and Laconi Maela
 *
 *
 *For more information : http://www.unixwiz.net/tools/websnarf.html
 */


#include <stdio.h>      
#include <stdlib.h>
#include <time.h>     
#include <string.h>     
#include <unistd.h>     
#include <sys/types.h>      
#include <sys/socket.h>     
#include <netinet/in.h>     
#include <netdb.h>     
#include <memory.h>     
#include <errno.h>     
#include <arpa/inet.h> 



struct sockaddr_in adresse;

int creersock( u_short port) {


  
  int sock, retour;

  sock = socket(AF_INET,SOCK_STREAM,0);

  if (sock<0) {
    perror ("ERREUR OUVERTURE");
    return(-1);
  }
  
  
  adresse.sin_family = AF_INET;

  adresse.sin_port = htons(port);
  adresse.sin_addr.s_addr=INADDR_ANY;
  
  
  int optval; 
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

  int newsockfd, s, sock, clientlenght;
  int annee,mois,jour,heure,min,sec;
  u_short port;
  char msg [BUFSIZ];
  int timeout = 5;
  struct sockaddr_in cli_addr;
  clientlenght = sizeof(cli_addr);

  
  
  if (argv[1] == NULL ){
      printf("Erreur : PORT incorrect ");
      exit(0);
  }
  
  port=(u_short) atoi(argv[1]);

  sock=creersock (port);

  listen (sock,5);
  printf("Websnarf C version listening on port %d (timeout=%d secs)\n", port,timeout);
  
  for ( ; ; )  {
     
      newsockfd = accept (sock, (struct sockaddr *)&cli_addr, &clientlenght);

      if ( fork() == 0 ) {
            close(sock);
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
              
                /*Recuperation de l'adresse IP du client*/
                struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&cli_addr;
                struct in_addr ipAddr = pV4Addr->sin_addr;
                char ipClient[INET_ADDRSTRLEN];
                inet_ntop( AF_INET, &ipAddr, ipClient, INET_ADDRSTRLEN );


                /*Recuperation de l'adresse IP du server*/
                struct sockaddr_in* pV4AddrServ = (struct sockaddr_in*)&adresse;
                struct in_addr ipAddrServ = pV4AddrServ->sin_addr;
                char ipServ[INET_ADDRSTRLEN];
                inet_ntop( AF_INET, &ipAddrServ, ipServ, INET_ADDRSTRLEN );


                /*Recuperation de la date et l'heure*/
                time_t t = time(NULL);
                struct tm tm = *localtime(&t);
                annee = tm.tm_year + 1900;
                mois = tm.tm_mon + 1;
                jour = tm.tm_mday;
                heure = tm.tm_hour;
                min = tm.tm_min;
                sec = tm.tm_sec;

                FILE *fp;
                fp = fopen("./test.txt", "a+");
                printf("%d/%d/%d %d:%d:%d  %s -> %s : %s",jour,mois,annee,heure,min,sec,ipClient,ipServ,msg);
                fprintf(fp, "%d/%d/%d %d:%d:%d  %s -> %s : %s",jour,mois,annee,heure,min,sec,ipClient,ipServ,msg);
                fclose(fp);
                close(newsockfd);
            }         
            exit(1);   
      } 
      close(newsockfd);   
      
  } 

  close(sock);   
  return 0;
}
