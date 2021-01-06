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
#include <sys/stat.h>    
#include <netinet/in.h>       
#include <netdb.h>     
#include <memory.h>     
#include <errno.h>     
#include <arpa/inet.h> 
#include <fcntl.h>



#define DEFAULT_PORT 80

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

void print_man(){
  printf("--------------HELP--------------\n\n");
  printf("Avaible options are :\n");
  printf("[-p]                  To specify the port number , default is 80 and you have to run with sudo this programm to allow to use private port \n\n");
  printf("\n[-f] filename.txt     To specify the filename where logs will be save, default filename is logs.txt\n\n,");
  printf("\n[-t] timeout          To specify the timeout, how long a connection with a client will take before the server end it\n\n");
  printf("\n[-m] maxSizeMessage   To specify the max number of character the request which will be log\n\n");
  printf("\n[-h]                  To access to the help manual ( where you are now ) \n\n");

}

int main (int argc, char *argv[]) {

  int newsockfd, s, sock, clientlenght;
  int annee,mois,jour,heure,min,sec;
  int timeout = 5;
  int maxSize = 10;
  int option;
  int saveDirectoryBool = 0;
  u_short port = DEFAULT_PORT;
  struct sockaddr_in cli_addr;

  char msg [BUFSIZ];
  char ipClient[INET_ADDRSTRLEN];
  char ipServ[INET_ADDRSTRLEN];
  char filename[100] = "logs.txt";;
  char directorySave[100] = "default";
  clientlenght = sizeof(cli_addr);

  while((option = getopt(argc, argv, ":hf:t:hp:m:d:")) != -1){ //get option from the getopt() method
      switch(option){
         case 'h':
            //printf("Given Option: %c\n", option); //DEBUG
            print_man();
            exit(1);
            break;
          case 'p':
            //printf("Given Option: %c\n", option); //DEBUG
            port = atoi(optarg);
            break;
         case 'f':
            //printf("Given Option: %c\n", option); //DEBUG
            //printf("Given File: %s\n", optarg);   //DEBUG
            strcpy(filename, optarg);
            printf("LOG FILE IS %s\n", filename);
            break;
         case 't': 
            //printf("Given Option: %c\n", option); //DEBUG
            //printf("Given timeout: %s\n",optarg); //DEBUG
            timeout = atoi(optarg);
            if(!timeout){perror("Timeout invalid , prompt a integer");return 0;}
            break;
          case 'm': 
            //printf("Given Option: %c\n", option); //DEBUG
            //printf("Given MessageSize: %s\n",optarg); //DEBUG
            maxSize = atoi(optarg);
            if(maxSize<0){maxSize = 10;}
            break;
          case 'd': 
            //printf("Given Option: %c\n", option); //DEBUG
            //printf("Given MessageSize: %s\n",optarg); //DEBUG
            strcpy(directorySave, optarg);
            printf("DIRECTORY is %s\n", directorySave);
            int result = mkdir(directorySave, 0777);
            saveDirectoryBool = 1;
            break;
         case ':':
            printf("option needs a value\n");  //DEBUG
            exit(1);
            break;
         case '?': //used for some unknown options
            printf("UNKNOW OPTION: %c\n", optopt);
            printf("run './websnarf -h ' for reading help manual:\n");
            exit(1);
            break;
      }
   }
  

  sock=creersock (port);

  listen (sock,5);
  printf("Websnarf C version listening on port %d (timeout=%d secs)\n", port ,timeout);
  
  for ( ; ; )  {

      newsockfd = accept (sock, (struct sockaddr *)&cli_addr, &clientlenght);

      /* Change the socket into non-blocking state  + start of the timeout time */
      fcntl(newsockfd, F_SETFL, O_NONBLOCK); 
      time_t start = time (NULL);

      if ( fork() == 0 ) {

          close(sock);
          if (newsockfd == -1) {
              //perror("Erreur accept");
              perror("Permission denied : [port 80]");
              exit(0);
          }
          else{
            //printf("Accept reussi");    
          }
          
          while((int) (time (NULL) - start) < timeout){
              s = read(newsockfd, msg, 1024);
              
            

              if (s == -1){
                  //Do nothing  (any message received)
              }
              else{
                  msg[s] = 0;
                  
                  //On réduit la taille du message reçu à maxSize
                  char target[maxSize];
                  strncpy(target, msg, maxSize);
                  target[maxSize] = '\0'; // IMPORTANT!


                  /*Recuperation de l'adresse IP du client*/
                  struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&cli_addr;
                  struct in_addr ipAddr = pV4Addr->sin_addr;
                  inet_ntop( AF_INET, &ipAddr, ipClient, INET_ADDRSTRLEN );

                  /*Recuperation de l'adresse IP du server*/
                  struct sockaddr_in* pV4AddrServ = (struct sockaddr_in*)&adresse;
                  struct in_addr ipAddrServ = pV4AddrServ->sin_addr;
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


                  char path[100];
                  if(saveDirectoryBool){
                      //COncaténation du nom de fichier sous forme IPclient->IPserveur
                      strcat(path,directorySave);
                      strcat(path,"/");
                      strcat(path,ipClient);
                      strcat(path, " -> ");
                      strcat(path, ipServ);

                      //printf("path is : %s\n",path ); //DEBUG
                      FILE *fp;
                      fp = fopen(path, "a+");
                      if(!target[0] == '\0'){
                          fprintf(fp, "\n%d/%d/%d %d:%d:%d : %s",jour,mois,annee,heure,min,sec,target);
                          printf("\n%d/%d/%d %d:%d:%d : %s",jour,mois,annee,heure,min,sec,target); //DEBUG
                          //printf("DANS SAVE DIR\n");
                      }
                      fclose(fp);
                  }else{
                      FILE *fp;
                      fp = fopen(filename, "a+");
                      if(! target[0] == '\0'){
                        
                          fprintf(fp, "\n%d/%d/%d %d:%d:%d  %s -> %s : %s",jour,mois,annee,heure,min,sec,ipClient,ipServ,target);
                          printf("\n%d/%d/%d %d:%d:%d  %s -> %s : %s",jour,mois,annee,heure,min,sec,ipClient,ipServ,target); //DEBUG
                          //printf("SAVE LOG\n");
                      }
                      fclose(fp);
                  }
                  
                  
              }
          }
          close(newsockfd);
          printf("\ntimeout reached for %s",ipClient);        
          exit(1);   
      } 
      close(newsockfd);   
  }
  close(sock);   
  return 0;
}
