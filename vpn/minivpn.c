/*
 * tunproxy.c --- small demo program for tunneling over UDP with tun/tap
 *
 * Copyright (C) 2003  Philippe Biondi <phil@secdev.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#include <openssl/evp.h>
#include <openssl/hmac.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <netinet/in.h>
#include <string.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <getopt.h>
#include <sys/ioctl.h>

#include <memory.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <openssl/rsa.h>       /* SSLeay stuff */
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

//#define exit(a)

#define PERROR(x)          do { perror(x); exit(1); } while (0)
#define ERROR(x, args ...) do { fprintf(stderr,"ERROR:" x, ## args); exit(1); } while (0)
#define CHK_NULL(x)        if ((x)==NULL) exit (1)
#define CHK_ERR(err,s)     if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err)       if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }

//#define SLEEP(a)           if (DEBUG) {sleep(a);}
#define SLEEP(a)             {sleep(a);}

#define ABORT()            {killpg(getpgid(getpid()),SIGTERM);}

#define CLIENT 0
#define SERVER 1

#define MAX_KEY_LENGTH   (32)
#define IV_LENGTH        (16)
#define BUFFER_LENGTH    (4096)
#define HASH_LENGTH      (32)
#define MAX_BLOCK_LENGTH (BUFFER_LENGTH + EVP_MAX_BLOCK_LENGTH + HASH_LENGTH)

#define POKE_INTERVAL_IN_SECS   3

/* from cli.cpp */
#define CCERTF "client.crt"
#define CKEYF  "client.key"
#define CCACERT "ca.crt"

/* from serv.cpp */
/* define HOME to be dir for key and cert files... */
#define HOME "./"
/* Make these what you want for cert & key files */
#define SCERTF  HOME "server.crt"
#define SKEYF   HOME "server.key"
#define SCACERT HOME "ca.crt"


char KEY[MAX_KEY_LENGTH+1] = "Wazaaaaaaaaaaahhhh !";
unsigned char IV[IV_LENGTH] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
char pokemsg[BUFFER_LENGTH]; /* Used for client/server "poke" communication */
char msg[BUFFER_LENGTH]; /* Used for client/server SSL comm while tunnels are off and messages from parent/child */
char buf[MAX_BLOCK_LENGTH]; /* Used exclusively within child processes for tunnel/UDP transfers */
unsigned char out[MAX_BLOCK_LENGTH];
unsigned char md_value[HASH_LENGTH];
char szCommonName[512];
int md_len = 0;
int DEBUG = 0;
char *progname;

void do_encrypt(char *in, int inl, char *out, int *outl)
{
    EVP_CIPHER_CTX ctx;
    int tmpl = 0;

    if (DEBUG) write(1,"0", 1);
    EVP_CIPHER_CTX_init(&ctx);
    if (DEBUG) write(1,"1", 1);
    if(0 == EVP_EncryptInit_ex(&ctx, EVP_aes_128_cbc(), NULL, KEY, IV)) PERROR("EVP_EncryptInit_ex");
    if (DEBUG) write(1,"2", 1);
    if(0 == EVP_EncryptUpdate(&ctx, out, outl, in, inl))                PERROR("EVP_EncryptUpdate");
    if (DEBUG) write(1,"3", 1);
    if(0 == EVP_EncryptFinal_ex(&ctx, out+*outl, &tmpl))                PERROR("EVP_EncryptFinal_ex");
    *outl += tmpl;
    if (DEBUG) write(1,"4", 1);
    EVP_CIPHER_CTX_cleanup(&ctx);
    if (DEBUG) write(1,"5", 1);
}

void do_decrypt(char *in, int inl, char *out, int *outl)
{
    EVP_CIPHER_CTX ctx;
    int tmpl = 0;

    if (DEBUG) write(1,"6", 1);
    EVP_CIPHER_CTX_init(&ctx);
    if (DEBUG) write(1,"7", 1);
    if(0 == EVP_DecryptInit_ex(&ctx, EVP_aes_128_cbc(), NULL, KEY, IV)) PERROR("EVP_DecryptInit_ex");
    if (DEBUG) write(1,"8", 1);
    if(0 == EVP_DecryptUpdate(&ctx, out, outl, in, inl))                PERROR("EVP_DecryptUpdate");
    if (DEBUG) write(1,"9", 1);
    if(0 == EVP_DecryptFinal_ex(&ctx, out+*outl, &tmpl))                PERROR("EVP_DecryptFinal_ex");
    *outl += tmpl;
    if (DEBUG) write(1,"a", 1);
    EVP_CIPHER_CTX_cleanup(&ctx);
    if (DEBUG) write(1,"b", 1);
}

int isEqual(char *h1, char *h2, int len){
    int i = 0, j = 0;
    unsigned char a, b;

    for(i = 0; i < len; i++){
        a = (unsigned char)h1[i];
        b = (unsigned char)h2[i];
#if 1
        for(j = 0; j < 8; j++){
            if((a & 0x01) != (b & 0x01)){
                return 0;
            }

            a = a >> 1;
            b = b >> 1;
        }// for
#else
        if (a != b) {
            return 0;
        }// if
#endif
    }// for

    return 1;
}

void printHash(unsigned char *hash,int len)
{
    int i=0;

    while ( i<len) {
        printf("%02x",(unsigned int)hash[i]);
        i++;
    }
}

void dumpBuf(unsigned char *buf, int len)
{
    int i = 0;

    for (i=0; i<len; i++) {
        printf("%02X",buf[i]);
    }
}

void createHash(unsigned char *in, int inl, unsigned char *md_value, int *md_len)
{
    HMAC_CTX ctx;
    const EVP_MD *md;

    // Create new digest
    OpenSSL_add_all_digests();
    md = EVP_get_digestbyname("sha256");
    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, KEY, sizeof(KEY), md, NULL);
    HMAC_Update(&ctx, in, inl);
    HMAC_Final(&ctx, md_value, md_len);
    if (DEBUG) {printf("in: "); dumpBuf(in,inl); printf(" inl: %d  md_value: ",inl);}
    if (DEBUG) printHash(md_value,*md_len);
    if (DEBUG) printf(" md_len: %d\n",*md_len);
}

void usage()
{
    fprintf(stderr, "Usage: %s [-s port|-c targetip:port] [-e]\n", progname);
    exit(0);
}

int getch()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

void randomizeString(char *pszStr,int len)
{
   int i;
   for (i=0; i<len; i++) {
      pszStr[i] = rand() % 93 + 33;
   }//for
}

void randomizeArray(unsigned char *pArray,int len)
{
   int i;
   for (i=0; i<len; i++) {
      pArray[i] = rand() % 255;
   }//for
}

void displayClientMenu(void)
{
   /* Display menu of choices */
   printf("\n");
   printf("k - Enter KEY\n");
   printf("i - Enter IV\n");
   printf("r - Randomize KEY/IV pair\n");
   printf("c - Clear Session\n");
   printf("s - STOP\n");
   printf("> - ");
   fflush(stdout);
}

int main(int argc, char *argv[])
{
   struct sockaddr_in sin, from, sout;
   struct ifreq ifr;
   int fd_tunnel, fd_udp, fd_tcp, fromlen, soutlen, port, PORT, l;
   char c, *p, *ip;
   fd_set fdset;    
   int outl = sizeof(out);
   int cliserv = -1; /* must be specified on cmd line */
   int flags   = IFF_TUN; /* default */
   char if_name[IFNAMSIZ] = "";
   int i;
   int fds[2];
   pid_t pid;
   
   /* initialize random seed */
   srand(time(NULL));
   
   progname = argv[0];

   while ((c = getopt(argc, argv, "s:c:ehd")) != -1) {
      switch (c) {
         case 'h':
            usage();
         case 'd':
            DEBUG++;
            break;
         case 's':
            cliserv = SERVER;
            PORT = atoi(optarg);
            break;
         case 'c':
            cliserv = CLIENT;
            p = memchr(optarg,':',16);
            if (!p) ERROR("invalid argument : [%s]\n",optarg);
            *p = 0;
            ip = optarg;
            port = atoi(p+1);
            PORT = 0;
            break;
         case 'e':
            flags = IFF_TAP;
            break;
         default:
            usage();
      }//switch
   }//while
   if (cliserv < 0) usage();

   /* Establish the Tunnel connection */
   if ((fd_tunnel = open("/dev/net/tun",O_RDWR)) < 0) PERROR("open");

   memset(&ifr, 0, sizeof(ifr));
   ifr.ifr_flags = flags;
   strncpy(ifr.ifr_name, "toto%d", IFNAMSIZ);
   if (ioctl(fd_tunnel, TUNSETIFF, (void *)&ifr) < 0) PERROR("ioctl");

   printf("Allocated interface %s. Configure and use it\n", ifr.ifr_name);

   /* Establish the UDP Network connection */
   fd_udp = socket(PF_INET, SOCK_DGRAM, 0);
   sin.sin_family = AF_INET;
   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   if (bind(fd_udp,(struct sockaddr *)&sin, sizeof(sin)) < 0) PERROR("UDP-bind");

   /* Establish the TCP Network connection */
   fd_tcp = socket(PF_INET, SOCK_STREAM, 0);
   sin.sin_family = AF_INET;
   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   if (bind(fd_tcp,(struct sockaddr *)&sin, sizeof(sin)) < 0) PERROR("TCP-bind");

   if (cliserv == CLIENT) {
      /* CLIENT */
      int err = -1;
      int sd = -1;
      struct sockaddr_in sa;
      int salen = sizeof(sa);
      SSL_CTX* ctx = NULL;
      SSL*     ssl = NULL;
      X509*    server_cert;
      char*    str = NULL;
      SSL_METHOD *meth = SSLv23_client_method(); //SSLv2_client_method();
      char     input;
      int      cnValid  = 0;
      int      keyValid = 0;
      int      ivValid  = 0;
      int      poke_count = 0;
      
      /* Create a pipe with two ends of pipe placed in fds[2], read_end=0, write_end=1 */
      pipe(fds);
      /* Fork a child process */
      pid = fork();
      if (pid > (pid_t) 0) { /* TCP/SSL get Keys */
         /* Client-Parent process */

         /* close our copy of the read end of pipe */
         close(fds[0]);

         /* Display startup message */
         printf("MiniVPN Client...\n");

         /* Output menu of choices */
         displayClientMenu();
         fflush(stdout);
         
         while(1) {
            /* Wait for TCP or user to make choice */
            FD_ZERO(&fdset);
            FD_SET(STDIN_FILENO,&fdset);
            FD_SET(fd_tcp,&fdset);
            if (select(STDIN_FILENO+fd_tcp+1,&fdset,NULL,NULL,NULL) < 0) PERROR("Client-Parent select");
            if (FD_ISSET(fd_tcp, &fdset)) {
               if (ssl) {
                  /* Process POKE from Server */
                  if (DEBUG) printf("Client Waiting on Server... ");
                  err = SSL_read(ssl,pokemsg,sizeof(pokemsg)-1);
                  if (!strncmp(pokemsg,"POKE",4)) {
                     if (DEBUG) printf("Received '%s', ",pokemsg);
                     if (DEBUG) printf("Sending POKE_ACK, #pokes=%d\n",poke_count++);
                     err = SSL_write(ssl,"POKE_ACK",strlen("POKE_ACK"));
                     if (-1 == err) {ERR_print_errors_fp(stderr); continue;}
                  }//if
                  else if (-1 == err) {
                     PERROR("Client-Parent select");
                  }//else if
               }//if ssl
            }//if fd_tcp
            if (FD_ISSET(STDIN_FILENO, &fdset)) {
               /* Notify the child process to STOP send/receive packets */
               strcpy(msg,"STOP");
               write(fds[1],msg,strlen(msg)+1);
               SLEEP(1);
               
               /* Close down current SSL session */
               printf("Clearing SSL session... ");
               /* send SSL/TLS close_notify */
               if (NULL != ssl) SSL_shutdown(ssl);
               /* Clean up. */
               if (sd >= 0)     {close (sd);        sd =    -1;}
               if (NULL != ssl) {SSL_free(ssl);     ssl = NULL;}
               if (NULL != ctx) {SSL_CTX_free(ctx); ctx = NULL;}
               poke_count = 0;
               printf("Done\n");
               SLEEP(1);

               /* Get User Choice */
               input = getch();
               
               /* Process User Choice */
               if ('k' == tolower(input)) {
                  printf("\nKEY option...");
                  printf("\nEnter CommonName(CN)\n");
                  printf(">");
                  fflush(stdout);
                  memset(szCommonName,0,sizeof(szCommonName));
                  scanf("%s",szCommonName);
                  printf("\nEnter KEY\n");
                  printf("[--------------------------------]\n");
                  printf(">");
                  fflush(stdout);
                  memset(msg,0,sizeof(msg));
                  scanf("%s",msg);
                  memset(KEY,0,sizeof(KEY));
                  strncpy(KEY,msg,MAX_KEY_LENGTH);
               }//if
               else if ('i' == tolower(input)) {
                  printf("\nIV option...");
                  printf("\nEnter CommonName(CN)\n");
                  printf(">");
                  fflush(stdout);
                  memset(szCommonName,0,sizeof(szCommonName));
                  scanf("%s",szCommonName);
                  printf("\nEnter IV (%d numbers)\n",IV_LENGTH);
                  printf("[----------------]\n");
                  printf(">");
                  fflush(stdout);
                  for (i=0; i<IV_LENGTH; i++) IV[i]=getch();
                  getch();
               }//else if
               else if ('r' == tolower(input)) {
                  printf("\nRandomize KEY/IV option...");
                  printf("\nEnter CommonName(CN)\n");
                  printf(">");
                  fflush(stdout);
                  memset(szCommonName,0,sizeof(szCommonName));
                  scanf("%s",szCommonName);
                  printf("Random KEY and IV are being generated... ");
                  /* Randomly generate ... KEY, IV */
                  memset(KEY,0,sizeof(KEY));
                  randomizeString(KEY,sizeof(KEY)-1);
                  randomizeArray(IV,sizeof(IV));
                  printf("Done\n");
               }//else if
               else if ('c' == tolower(input)) {
                  /* Close down current SSL session */
                  printf("Clearing SSL session... ");
                  /* send SSL/TLS close_notify */
                  if (NULL != ssl) SSL_shutdown(ssl);
                  /* Clean up. */
                  if (sd >= 0)     {close (sd);        sd =    -1;}
                  if (NULL != ssl) {SSL_free(ssl);     ssl = NULL;}
                  if (NULL != ctx) {SSL_CTX_free(ctx); ctx = NULL;}
                  poke_count = 0;
                  printf("Done\n");
                  SLEEP(1);
                  continue;               
               }//else if
               else if ('s' == tolower(input)) {
                  printf("\nClosing Tunnel... ");
                  close(fd_tunnel);
                  printf("Cleanup Done\n");
                  printf("GoodBye\n");
                  printf("Killing process tree\n");
                  SLEEP(1);
                  killpg(getpgid(getpid()),SIGTERM); /* Kill the complete process tree including child. */
                  SLEEP(2);
                  printf("Press Control-C to terminate\n");
                  while(1);
               }//else if
               else {
                  /* Output menu of choices */
                  displayClientMenu();
            
                  continue;
               }//else
                 
               /* Echo back to the user the CN + KEY + IV being used */
               if (DEBUG)  printf("CN  is: [%s]\n",szCommonName);
               if (DEBUG)  printf("KEY is: [%s]\n",KEY);
               if (DEBUG) {printf("IV  is: ["); dumpBuf(IV,IV_LENGTH); printf("]\n\n");}
               
               if (DEBUG)  printf("Session: ctx=%p, ssl=%p, sd=%d\n",ctx,ssl,sd);

               /* PKI: Authenticate the Server */
               if (!ctx || !ssl || (sd < 0)) {
                  if (DEBUG) printf("PKI: Authenticating the Server...\n");
                  SSLeay_add_ssl_algorithms();
                  SSL_load_error_strings();
                  ctx = SSL_CTX_new(meth);
                  if (NULL == ctx) {if (DEBUG) printf("ctx error\n");err=-1;continue;};

                  SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL);
                  SSL_CTX_load_verify_locations(ctx,CCACERT,NULL);

                  if (SSL_CTX_use_certificate_file(ctx,CCERTF,SSL_FILETYPE_PEM)<=0){
                     ERR_print_errors_fp(stderr);err=-2;continue;}
                  if (SSL_CTX_use_PrivateKey_file(ctx,CKEYF,SSL_FILETYPE_PEM)  <=0){
                     ERR_print_errors_fp(stderr);err=-3;continue;}
                  if (!SSL_CTX_check_private_key(ctx)){
                     printf("Private key doesn't match cert. public key\n");err=-4;continue;}

                  /* Create a socket and connect to server using normal socket calls. */
                  sd = socket (AF_INET, SOCK_STREAM, 0);
                  if (-1 == sd) {perror("socket");err=-5;continue;}

                  memset(&sa,0,sizeof(sa));
                  sa.sin_family = AF_INET;
                  inet_aton(ip, &sa.sin_addr); /* Server IP */
                  sa.sin_port   = htons(port); /* Server Port */

                  err = connect(sd,(struct sockaddr*)&sa,sizeof(sa));
                  if (-1 == err) {perror("connect");err=-6;continue;}

                  /* Now we have TCP connection. Start SSL negotiation. */
                  ssl = SSL_new(ctx);     if (NULL == ssl) {printf("TCP connect failure\n");err=-7;continue;}
                  SSL_set_fd(ssl,sd);
                  err = SSL_connect(ssl); if (-1 == err) {ERR_print_errors_fp(stderr);err=-8;continue;};

                  /* Following two steps are optional and not required for data exchange to be successful. */

                  /* Get the cipher - opt */
                  if (DEBUG) printf("SSL connection using %s\n",SSL_get_cipher(ssl));

                  /* Get server's certificate (note: beware of dynamic allocation) - opt */
                  server_cert = SSL_get_peer_certificate(ssl);
                  if (NULL == server_cert){err=-9;continue;}
                  if (DEBUG) printf("Server certificate:\n");

                  str = X509_NAME_oneline(X509_get_subject_name(server_cert),0,0);
                  if (NULL == str){err=-10;continue;}
                  //if (subjectstr) strcpy(subjectstr,str);
                  if (DEBUG) printf("\t subject: %s\n",str);
                  OPENSSL_free(str);

                  str = X509_NAME_oneline(X509_get_issuer_name(server_cert),0,0);
                  if (NULL == str){err=-11;continue;}
                  //if (issuerstr) strcpy(issuerstr,str);
                  if (DEBUG) printf("\t issuer: %s\n",str);
                  OPENSSL_free(str);

                  /* We can do all sorts of certificate verification stuff here before
                     deallocating the certificate. */
                  X509_free(server_cert);
               }//if

               /* DATA EXCHANGE AREA */
               if (ssl) {
                  /* Write CommonName (CN) to Server */
                  if (DEBUG) printf("Writing CN [%s] to Server\n",szCommonName);
                  strcpy(msg,"CN:");
                  strcat(msg,szCommonName);
                  err = SSL_write(ssl,msg,strlen(msg));  if (-1 == err) {ERR_print_errors_fp(stderr); continue;}
                  /* Read CN reply from Server */
                  err = SSL_read(ssl,msg,sizeof(msg)-1); if (-1 == err) {ERR_print_errors_fp(stderr); continue;}
                  msg[err] = '\0';
                  if (DEBUG) printf("Server replied with %d chars:'%s'\n",err,msg);
                  if (strncmp(msg,"CN_ACK",6)) {cnValid=0;printf("Client_CN_ACK_Error\n");}
                  else cnValid=1;
                  
                  /* Write KEY to Server */
                  if (DEBUG) printf("Writing KEY [%s] to Server\n",KEY);
                  strcpy(msg,"KEY:");
                  strcat(msg,KEY);
                  err = SSL_write(ssl,msg,strlen(msg));  if (-1 == err) {ERR_print_errors_fp(stderr); continue;}
                  /* Read KEY reply from Server */
                  err = SSL_read(ssl,msg,sizeof(msg)-1); if (-1 == err) {ERR_print_errors_fp(stderr); continue;}
                  msg[err] = '\0';
                  if (DEBUG) printf("Server replied with %d chars:'%s'\n",err,msg);
                  if (strncmp(msg,"KEY_ACK",7)) {keyValid=0;printf("Client_KEY_ACK_Error\n");}
                  else keyValid=1;
                  
                  /* Write IV to Server */
                  if (DEBUG) {printf("Writing IV ["); dumpBuf(IV,IV_LENGTH); printf("] to Server\n");}
                  strcpy(msg,"IV:");
                  memcpy(&msg[3],&IV,sizeof(IV));
                  err = SSL_write(ssl,msg,3+sizeof(IV)); if (-1 == err) {ERR_print_errors_fp(stderr); continue;}
                  /* Read IV reply from Server */
                  err = SSL_read(ssl,msg,sizeof(msg)-1); if (-1 == err) {ERR_print_errors_fp(stderr); continue;}
                  msg[err] = '\0';
                  if (DEBUG) printf("Server replied with %d chars:'%s'\n",err,msg);
                  if (strncmp(msg,"IV_ACK",6)) {ivValid=0;printf("Client_IV_ACK_Error\n");}
                  else ivValid=1;

                  /* Configure Client-Child to enable tunnel */
                  if (cnValid && keyValid && ivValid) {
                     /* Write KEY to the child process */
                     if (DEBUG) printf("Issuing KEY to Client-Child\n");
                     strcpy(msg,"KEY:");
                     strcat(msg,KEY);
                     write(fds[1],msg,strlen(msg)+1);
                     SLEEP(1);
                                    
                     /* Write IV to the child process */
                     if (DEBUG) printf("Issuing IV to Client-Child\n");
                     strcpy(msg,"IV:");
                     memcpy(&msg[3],&IV,sizeof(IV));
                     write(fds[1],msg,3+sizeof(IV));
                     SLEEP(1);
                  
                     /* Initialize FROM to point to Server's ip:port */
                     memset(&from,0,sizeof(from));
                     fromlen = sizeof(from);
                     from.sin_family = AF_INET;
                     from.sin_port = htons(port);
                     inet_aton(ip,&from.sin_addr);

                     /* Write FROM to the child process */
                     if (DEBUG) printf("Informing Client-Child the server's UDP addr+port\n");
                     strcpy(msg,"FROM:");
                     memcpy(&msg[5],&from,sizeof(from));
                     write(fds[1],msg,5+sizeof(from));
                     SLEEP(1);

                     /* Notify the child process to be ready to send/receive packets */
                     if (DEBUG) printf("Issuing START to Client-Child\n");
                     strcpy(msg,"START");
                     write(fds[1],msg,strlen(msg)+1);
                     SLEEP(1);
                  }//if
               }//if ssl
            }//else if STDIN_FILENO
         }//while
       }//if Client-PARENT
       else {                 /* UDP use keys */
          /* Client-Child process */
         int okToStart = 0;
         struct sockaddr_in from;

          /* close our copy of the write end of pipe */
          close(fds[1]);

         /* Set unblock pipe read */
         
         /* Tunnel connection */
         
         /* Keep reading from fds[0], fd_udp, fd_tunnel */
         
         /* If notification is not received, do not send/receive */
         
         /* while(true){if (read(fds[0]) == "start") break;} */
         
         /* while(true){select from fd_udp and fd_tunnel, encryption/decryption, hash, sending/receiving packets} */

          while (1) {
              FD_ZERO(&fdset);
              FD_SET(fds[0],   &fdset); /* Pipe from Parent */
              FD_SET(fd_tunnel,&fdset); /* Tunnel */
              FD_SET(fd_udp,   &fdset); /* Net-UDP */
              if (select(fds[0]+fd_tunnel+fd_udp+1,&fdset,NULL,NULL,NULL) < 0) PERROR("Client-Child select");
              if (FD_ISSET(fds[0], &fdset)) {
                  memset(buf,0,sizeof(buf));
                  l = read(fds[0], buf, sizeof(buf));
                  if (l < 0) PERROR("read");
                  if (!strncmp(buf,"KEY:",4)) {
                  if (DEBUG) printf("KEY: [%s] received from Client-Parent\n",&buf[4]);
                  strncpy(KEY,&buf[4],sizeof(KEY));
               }//if
               else if (!strncmp(buf,"IV:",3)) {
                  if (DEBUG) {printf("IV: [");dumpBuf(&buf[3],sizeof(IV));printf("] received from Client-Parent\n",&buf[3]);}
                  memcpy(IV,&buf[3],sizeof(IV));
               }//else if
               else if (!strncmp(buf,"FROM:",5)) {
                  if (DEBUG) printf("FROM: received from Client-Parent\n");
                  memcpy(&from,&buf[5],sizeof(from));
               }//else if
               else if (!strncmp(buf,"START",5)) {
                  if (DEBUG) printf("START received from Client-Parent\n");
                  okToStart = 1;
                  /*if (DEBUG)*/ printf("Client Tunnel running...\n");
               }//else if
               else if (!strncmp(buf,"STOP",4)) {
                  if (DEBUG) printf("STOP received from Client-Parent\n");
                  okToStart = 0;
                  /*if (DEBUG)*/ printf("Client Tunnel stopped\n");
               }//else if
               else okToStart = 0;
            }//if
              else if (FD_ISSET(fd_tunnel, &fdset)) {
                 if (okToStart) {
                    /* wait for data from tunnel, then encrypt+hash, then send to "remote/from" network socket */
                     if (DEBUG) write(1,">", 1);
                     l = read(fd_tunnel, buf, sizeof(buf));
                     if (l < 0) PERROR("read");
                     else {
                        if (DEBUG) {printf("\nTx=%d[",l); dumpBuf(buf,l); printf("]\n");}
                        outl = sizeof(out);
                        do_encrypt(buf,l,out,&outl);
                        if (outl > MAX_BLOCK_LENGTH) PERROR("Encrypt Too Big");
                        if (DEBUG) {printf("\nEncrypt=%d[",outl); dumpBuf(out,outl); printf("]\n");}
                        createHash(buf, l, md_value, &md_len);
                        if ((outl + md_len) > MAX_BLOCK_LENGTH) PERROR("Encrypt+Hash Too Big");
                        if (DEBUG) {printf("Hash=%d[",md_len); dumpBuf(md_value,md_len); printf("]\n");}
                        for (i=0; i<md_len; i++) {out[outl+i] = md_value[i];}
                        outl += md_len;
                        if (sendto(fd_udp, out, outl, 0, (struct sockaddr *)&from, sizeof(from)) < 0)PERROR("sendto");
                     }//else
                  }//if okToStart
                  else if (!okToStart) { /* read from Tunnel and DISCARD */
                     l = read(fd_tunnel, buf, sizeof(buf));
                     if (l < 0) PERROR("read");
                  }//else if !okToStart
               }//else if
               else if (FD_ISSET(fd_udp, &fdset)) {
                  if (okToStart) {
                     /* wait for data from "remote/from" node, audit connection, decrypt+rehash+audit,
                       then forward to tunnel */
                     if (DEBUG) write(1,"<", 1);
                     /* do *not* use "from" so that we can audit which node the revieved packet comes from */
                     soutlen = sizeof(sout);
                     l = recvfrom(fd_udp, buf, sizeof(buf), 0, (struct sockaddr *)&sout, &soutlen);
                     if (l < 0) {
                         printf("fd_udp=%d,buf=%p,bufsize=%d,flags=%d,sockaddr=%p,soutlen=%p\n",
                                 fd_udp,buf,sizeof(buf),0,&sout,&soutlen);
                         PERROR("recvfrom");
                     }//if
                     else {
                        /* audit to make sure data came from node "established" at connection time */
                        if ((sout.sin_addr.s_addr != from.sin_addr.s_addr) || (sout.sin_port != from.sin_port)) {
#if 0
                             printf("Got packet from  %s:%i instead of %s:%i\n", 
                                        (char *)inet_ntoa(sout.sin_addr/*.s_addr*/), ntohs(sout.sin_port),
                                        (char *)inet_ntoa(from.sin_addr/*.s_addr*/), ntohs(from.sin_port));
#endif
                         }//if
                         else {
                            /* data from correct node, subtract off HASH, decrypt+rehash, audit hash,
                               then forward to tunnel */
                             l -= HASH_LENGTH;
                             outl = sizeof(out);
                             if (DEBUG) {printf("\nRx=%d[",l); dumpBuf(buf,l); printf("]\n");}
                             if (DEBUG) {printf("Hash=%d[",HASH_LENGTH); dumpBuf(&buf[l],HASH_LENGTH); printf("]\n");}
                             do_decrypt(buf,l,out,&outl);
                             if (DEBUG) {printf("\nDecrypt=%d[",outl); dumpBuf(out,outl); printf("]\n");}
                             createHash(out, outl, md_value, &md_len);
                             if (!isEqual(&buf[l], md_value, md_len)) PERROR("HASH");
                             if (write(fd_tunnel, out, outl) < 0) PERROR("write");
                         }//else
                     }//else
                  }//if okToStart
                  else if (!okToStart) { /* receive from node and DISCARD */
                     soutlen = sizeof(sout);
                     l = recvfrom(fd_udp, buf, sizeof(buf), 0, (struct sockaddr *)&sout, &soutlen);
                     if (l < 0) {
                         printf("fd_udp=%d,buf=%p,bufsize=%d,flags=%d,sockaddr=%p,soutlen=%p\n",
                                 fd_udp,buf,sizeof(buf),0,&sout,&soutlen);
                         PERROR("recvfrom");
                     }//if
                  }//else if !okToStart                 
              }//else if
          }//while
       }// else Client-CHILD
    }// if CLIENT
    else {
      /* SERVER */
      int err = -1;
      int listen_sd = fd_tcp;
      int sd = -1;
      //struct sockaddr_in sa_serv;
      struct sockaddr_in sa_cli;
      size_t client_len;
      SSL_CTX* ctx = NULL;
      SSL*     ssl = NULL;
      X509*    client_cert = NULL;
      char*    str = NULL;
      char     subjectstr[512];
      char     issuerstr[512];
      SSL_METHOD *meth = SSLv23_server_method();
      int cnValid  = 0;
      int keyValid = 0;
      int ivValid  = 0;
      struct itimerspec time_period = {{POKE_INTERVAL_IN_SECS,0},{POKE_INTERVAL_IN_SECS,0}}; /* periodic timer */
      struct timeval sel_timeout;
      int timer_fd;
      int result;
      int poke_count = 0;
      
      /* create time to fire every 1 second */
      timer_fd = timerfd_create(CLOCK_MONOTONIC,0);
      if (timer_fd < 0) {printf("Timer fd failed\n");killpg(getpgid(getpid()),SIGTERM);}

      result = timerfd_settime(timer_fd,0,&time_period,NULL);
      if (result < 0) {printf("timer fd set time failed\n");killpg(getpgid(getpid()),SIGTERM);}

      /* Create a pipe with two ends of pipe placed in fds[2], read_end=0, write_end=1 */
      pipe(fds);
      /* Fork a child process */
      pid = fork();
      if (pid > (pid_t) 0) { /* TCP get Keys */
         /* Server-Parent process */

         /* close our copy of the read end of pipe */
         close(fds[0]);

         /* Display startup message */
         printf("MiniVPN Server...\n");         
         fflush(stdout);
         
         while(1) {
            FD_ZERO(&fdset);
            FD_SET(fd_tcp,&fdset);
            FD_SET(timer_fd,&fdset);
            if (select(fd_tcp+timer_fd+1,&fdset,NULL,NULL,NULL) < 0) PERROR("Server-Parent select");
            if (FD_ISSET(timer_fd, &fdset)) {
               int s;
               uint64_t exp;
               s = read(timer_fd, &exp, sizeof(uint64_t));
               
               /* Reset time-interval in case altered */
               time_period.it_interval.tv_sec  = POKE_INTERVAL_IN_SECS;
               time_period.it_interval.tv_nsec = 0;
               time_period.it_value.tv_sec     = POKE_INTERVAL_IN_SECS;
               time_period.it_value.tv_nsec    = 0;
               
               /* Is SSL connection currently active with Client? */
               if (ssl) {
                  /* POKE_Client via SSL*/
                  if (DEBUG) printf("Server POKE Client, Waiting... ");
                  err = SSL_write(ssl,"POKE",strlen("POKE"));
                  memset(pokemsg,0,sizeof(pokemsg));
                  err = SSL_read(ssl,pokemsg,sizeof(msg)-1);
                  if ((-1 == err) || (strncmp(pokemsg,"POKE_ACK",8)))
                  {
                     /* Client did not respond -OR- invalid reponse */
                     if (err >= 0) {
                        pokemsg[err] = '\0';
                        if (DEBUG) printf("Received '%s'\n",pokemsg);
                     }//if
                     else {
                        if (DEBUG) printf("Timeout\n");
                     }//else
                     
                     if (DEBUG) printf("Closing SSL connection with Client\n");
                     
                     /* Close SSL connection with Client. */
                     if (sd >= 0)     {close(sd);         sd =    -1;}
                     if (NULL != ssl) {SSL_free(ssl);     ssl = NULL;}
                     if (NULL != ctx) {SSL_CTX_free(ctx); ctx = NULL;}
                     
                     poke_count = 0;
                     
                     /* Notify the child process to STOP send/receive packets */
                     strcpy(pokemsg,"STOP");
                     write(fds[1],pokemsg,strlen(pokemsg)+1);
                  }//if disconnect Client
                  else {
                     pokemsg[err] = '\0';
                     if (DEBUG) printf("Received '%s', #pokes=%d\n",pokemsg,poke_count++);
                  }//else
               }//if ssl
            }//if timer_fd
            if (FD_ISSET(fd_tcp, &fdset)) {
               /* Notify the child process to STOP send/receive packets */
               strcpy(msg,"STOP");
               write(fds[1],msg,strlen(msg)+1);
               SLEEP(1);

               if (DEBUG) printf("Session: ctx=%p, ssl=%p, sd=%d\n",ctx,ssl,sd);

               /* PKI: Authenticate the Client */
               if (!ctx || !ssl || (sd < 0)) {
                  if (DEBUG) printf("PKI: Authenticating the Client...\n");
                
                  /* SSL preliminaries. We keep the certificate and key with the context. */
                  SSLeay_add_ssl_algorithms();
                  SSL_load_error_strings();
                  ctx = SSL_CTX_new (meth);
                  if (!ctx) {ERR_print_errors_fp(stderr);err=-1;continue;}

                  SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL); /* whether verify the certificate */
                  SSL_CTX_load_verify_locations(ctx,SCACERT,NULL);
        
                  if (SSL_CTX_use_certificate_file(ctx,SCERTF,SSL_FILETYPE_PEM)<=0){
                     ERR_print_errors_fp(stderr);err=-2;continue;}
                  if (SSL_CTX_use_PrivateKey_file(ctx,SKEYF,SSL_FILETYPE_PEM)  <=0){
                     ERR_print_errors_fp(stderr);err=-3;continue;}
                  if (!SSL_CTX_check_private_key(ctx)){
                     fprintf(stderr,"Private key does not match the certificate public key\n");err=-4;continue;}

                  /* Prepare TCP socket for receiving connections */
                  //listen_sd = socket(AF_INET,SOCK_STREAM,0);
                  if (-1 == listen_sd) {perror("socket");err=-5;continue;}
        
                  //memset(&sa_serv,0,sizeof(sa_serv));
                  //sa_serv.sin_family      = AF_INET;
                  //sa_serv.sin_addr.s_addr = INADDR_ANY;
                  //sa_serv.sin_port        = htons(PORT); /* Server Port number */
        
                  //err = bind(listen_sd,(struct sockaddr*)&sa_serv,sizeof(sa_serv));
                  //if (-1 == err) {perror("bind");err=-6;continue;}
                  
                  /* Receive a TCP connection. */
                  err = listen(listen_sd,5);
                  if (-1 == err) {perror("listen");err=-7;continue;}

                  client_len = sizeof(sa_cli);
                  sd = accept(listen_sd,(struct sockaddr*)&sa_cli,&client_len);
                  if (-1 == sd) {perror("accept");err=-8;continue;}
                  //close(listen_sd);

                  //if (DEBUG) printf("Connection from %lx, port %d\n",sa_cli.sin_addr.s_addr,sa_cli.sin_port);
                  if (DEBUG) printf("Connection from ip:port %s:%i established\n",
                                     inet_ntoa(sa_cli.sin_addr/*.s_addr*/),ntohs(sa_cli.sin_port));

                  /* TCP connection is ready. Do server side SSL. */
                  ssl = SSL_new(ctx);     if (NULL == ssl) {printf("TCP connect failure\n");err=-9;continue;}
                  SSL_set_fd(ssl,sd);
                  err = SSL_accept(ssl);  if (-1 == err) {ERR_print_errors_fp(stderr);err=-10;continue;}
                  
                  /* Get the cipher - opt */
                  if (DEBUG) printf("SSL connection using %s\n",SSL_get_cipher(ssl));
        
                  /* Get client's certificate (note: beware of dynamic allocation) - opt */
                  client_cert = SSL_get_peer_certificate(ssl);
                  if (NULL == client_cert) {printf("Client does not have certificate.\n");err=-11;continue;}
                  if (DEBUG) printf("Client certificate:\n");
          
                  str = X509_NAME_oneline(X509_get_subject_name(client_cert),0,0);
                  if (NULL == str) {err=-12;continue;}
                  strncpy(subjectstr,str,sizeof(subjectstr)-1);
                  if (DEBUG) printf("\t subject: %s\n",str);
                  OPENSSL_free(str);
                  
                  str = X509_NAME_oneline(X509_get_issuer_name(client_cert),0,0);
                  if (NULL == str) {err=-13;continue;}
                  strncpy(issuerstr,str,sizeof(issuerstr)-1);
                  if (DEBUG) printf("\t issuer: %s\n",str);
                  OPENSSL_free(str);

                  /* We can do all sorts of certificate verification stuff here before
                     deallocating the certificate. */
                  X509_free (client_cert);
               }//if

               /* DATA EXCHANGE - Receive messages and send replies */
               if (ssl) {
                  /* Read CN from Client */
                  err = SSL_read(ssl,msg,sizeof(msg)-1);
                  if (-1 == err) {ERR_print_errors_fp(stderr);err=-14;continue;}
                  msg[err] = '\0';
                  if (DEBUG) printf("Received from Client %d chars:'%s'\n",err,msg);
                  /* Audit CN from Client */
                  cnValid = 0;
                  if (!strncmp(msg,"CN:",3)) {
                     char *pch_start = strstr(subjectstr,"/CN=");
                     if (NULL != pch_start) {
                        char *pch_end = strstr(&pch_start[1],"/");
                        if (NULL != pch_end) {
                           int cn_len = (int)pch_end - (int)&pch_start[4];
                           if (!strncmp(&pch_start[4],&msg[3],cn_len)) {
                              cnValid=1;
                           }//if
                        }//if
                     }//if
                  }//if
                  /* Write CN reply to Client */
                  if (1 == cnValid) {
                     err = SSL_write(ssl,"CN_ACK",strlen("CN_ACK"));
                     if (-1 == err) {ERR_print_errors_fp(stderr);err=-15;continue;}
                  }//if
                  else {
                     err = SSL_write(ssl,"CN_NACK",strlen("CN_NACK"));
                     if (-1 == err) {ERR_print_errors_fp(stderr);err=-16;continue;}
                  }//else

                  /* Read KEY from Client */
                  err = SSL_read(ssl,msg,sizeof(msg)-1); 
                  if (-1 == err) {ERR_print_errors_fp(stderr);err=-17;continue;}
                  msg[err] = '\0';
                  if (DEBUG) printf("Received from Client %d chars:'%s'\n",err,msg);
                  /* Write KEY reply to Client */
                  if (!strncmp(msg,"KEY:",4)) {
                     strncpy(KEY,&msg[4],MAX_KEY_LENGTH);
                     keyValid=1;
                     err = SSL_write(ssl,"KEY_ACK",strlen("KEY_ACK"));
                     if (-1 == err) {ERR_print_errors_fp(stderr);err=-18;continue;}
                  }//if
                  else {
                     keyValid=0;
                     err = SSL_write(ssl,"KEY_NACK",strlen("KEY_NACK"));
                     if (-1 == err) {ERR_print_errors_fp(stderr);err=-19;continue;}
                  }//else
                  
                  /* Read IV from Client */
                  err = SSL_read(ssl,msg,sizeof(msg)-1);
                  if (-1 == err) {ERR_print_errors_fp(stderr);err=-20;continue;}
                  msg[err] = '\0';
                  if (DEBUG) {printf("Received from Client %d chars:'%c%c%c",err,msg[0],msg[1],msg[2]);
                              dumpBuf(&msg[3],err-3);printf("'\n");}
                  /* Write IV reply to Client */
                  if (!strncmp(msg,"IV:",3)) {
                     memcpy(IV,&msg[3],IV_LENGTH);
                     ivValid=1;
                     err = SSL_write(ssl,"IV_ACK",strlen("IV_ACK"));
                     if (-1 == err) {ERR_print_errors_fp(stderr);err=-21;continue;}
                  }//if
                  else {
                     ivValid=0;
                     err = SSL_write(ssl,"IV_NACK",strlen("IV_NACK"));
                     if (-1 == err) {ERR_print_errors_fp(stderr);err=-22;continue;}
                  }//else

#if 0
                     /* Clean up. */
                     if (sd >= 0)     {close(sd);         sd = -1;}
                     if (NULL != ssl) {SSL_free(ssl);     ssl = NULL;}
                     if (NULL != ctx) {SSL_CTX_free(ctx); ctx = NULL;}
#endif

                  if (cnValid && keyValid && ivValid) {
                     /* Write KEY to the child process */
                     strcpy(msg,"KEY:");
                     strcat(msg,KEY);
                     write(fds[1],msg,strlen(msg)+1);
                     SLEEP(1);
                  
                     /* Write IV to the child process */
                     strcpy(msg,"IV:");
                     memcpy(&msg[3],&IV,sizeof(IV));
                     write(fds[1],msg,3+sizeof(IV));
                     SLEEP(1);
                     
                     /* Initialize FROM to point to Client's ip:port */
                     memset(&from,0,sizeof(from));
                     fromlen = sizeof(from);
                     from = sa_cli;

                     /* Write FROM to the child process */
                     strcpy(msg,"FROM:");
                     memcpy(&msg[5],&from,sizeof(from));
                     write(fds[1],msg,5+sizeof(from));
                     SLEEP(1);

                     /* Notify the child process to be ready to send/receive packets */
                     strcpy(msg,"START");
                     write(fds[1],msg,strlen(msg)+1);
                     SLEEP(1);
                  }//if VALID
               }// ssl
            }//else if TCP
         }// while
      }// if Server-PARENT
      else {                 /* UDP use keys */
         /* Server-Child process */
         int okToStart = 0;
         struct sockaddr_in from;

         /* close our copy of the write end of pipe */
         close(fds[1]);

         /* Set unblock pipe read */

         /* Tunnel connection */

         /* Keep reading from fds[0], fd_udp, fd_tunnel */

         /* If notification is not received, do not send/receive */

         while (1) {
            FD_ZERO(&fdset);
            FD_SET(fds[0],   &fdset); /* Pipe from Parent */
            FD_SET(fd_tunnel,&fdset); /* Tunnel */
            FD_SET(fd_udp,   &fdset); /* Net-UDP */
            if (select(fds[0]+fd_tunnel+fd_udp+1,&fdset,NULL,NULL,NULL) < 0) PERROR("Server-Child select");
            if (FD_ISSET(fds[0], &fdset)) {
               memset(buf,0,sizeof(buf));
               l = read(fds[0], buf, sizeof(buf));
               if (l < 0) PERROR("read");
               if (!strncmp(buf,"KEY:",4)) {
                  if (DEBUG) printf("KEY: [%s] received from Server-Parent\n",&buf[4]);
                  strncpy(KEY,&buf[4],sizeof(KEY));
               }//if
               else if (!strncmp(buf,"IV:",3)) {
                  if (DEBUG) {
                     printf("IV: [");
                     dumpBuf(&buf[3],sizeof(IV));
                     printf("] received from Server-Parent\n",&buf[3]);
                  }//if
                  memcpy(IV,&buf[3],sizeof(IV));
               }//else if
               else if (!strncmp(buf,"FROM:",5)) {
                  if (DEBUG) printf("FROM: received from Server-Parent\n");
                  memcpy(&from,&buf[5],sizeof(from));
               }//else if
               else if (!strncmp(buf,"START",5)) {
                  if (DEBUG) printf("START received from Server-Parent\n");
                  okToStart = 1;
                  /*if (DEBUG)*/ printf("Server Tunnel running...\n");
               }//else if
               else if (!strncmp(buf,"STOP",4)) {
                  if (DEBUG) printf("STOP received from Server-Parent\n");
                  okToStart = 0;
                  /*if (DEBUG)*/ printf("Server Tunnel stopped\n");
               }//else if
               else okToStart = 0;
            }//if
            else if (FD_ISSET(fd_tunnel, &fdset)) {
               if (okToStart) {
                  /* wait for data from tunnel, then encrypt+hash, then send to "remote/from" network socket */
                  if (DEBUG) write(1,">", 1);
                  l = read(fd_tunnel, buf, sizeof(buf));
                  if (l < 0) PERROR("read");
                  else {
                     if (DEBUG) {printf("\nTx=%d[",l); dumpBuf(buf,l); printf("]\n");}
                     outl = sizeof(out);
                     do_encrypt(buf,l,out,&outl);
                     if (outl > MAX_BLOCK_LENGTH) PERROR("Encrypt Too Big");
                     if (DEBUG) {printf("\nEncrypt=%d[",outl); dumpBuf(out,outl); printf("]\n");}
                     createHash(buf, l, md_value, &md_len);
                     if ((outl + md_len) > MAX_BLOCK_LENGTH) PERROR("Encrypt+Hash Too Big");
                     if (DEBUG) {printf("Hash=%d[",md_len); dumpBuf(md_value,md_len); printf("]\n");}
                     for (i=0; i<md_len; i++) {out[outl+i] = md_value[i];}
                     outl += md_len;
                     if (sendto(fd_udp, out, outl, 0, (struct sockaddr *)&from, sizeof(from)) < 0)PERROR("sendto");
                  }//else
               }//if okToStart
               else if (!okToStart) { /* "read" from Tunnel and DISCARD */
                  l = read(fd_tunnel, buf, sizeof(buf));
                  if (l < 0) PERROR("read");
               }//else if !okToStart
            }//else if
            else if (FD_ISSET(fd_udp, &fdset)) {
               if (okToStart) {
                  /* wait for data from "remote/from" node, audit connection, decrypt+rehash+audit,
                     then forward to tunnel */
                  if (DEBUG) write(1,"<", 1);
                  /* do *not* use "from" so that we can audit which node the recieved packet comes from */
                  soutlen = sizeof(sout);
                  l = recvfrom(fd_udp, buf, sizeof(buf), 0, (struct sockaddr *)&sout, &soutlen);
                  if (l < 0) {
                      printf("fd_udp=%d,buf=%p,bufsize=%d,flags=%d,sockaddr=%p,soutlen=%p\n",
                              fd_udp,buf,sizeof(buf),0,&sout,&soutlen);
                      PERROR("recvfrom");
                  }//if
                  else {
                     /* audit to make sure data came from node "established" at connection time */
                     if ((sout.sin_addr.s_addr != from.sin_addr.s_addr) || (sout.sin_port != from.sin_port)) {
#if 0
                        printf("Got packet from  %s:%i instead of %s:%i\n", 
                                 (char *)inet_ntoa(sout.sin_addr/*.s_addr*/), ntohs(sout.sin_port),
                                 (char *)inet_ntoa(from.sin_addr/*.s_addr*/), ntohs(from.sin_port));
                        printf("Updating FROM with this new ip:port\n");
#endif
                        from = sout;
                     }//if
                     else
                     {
                        /* data from correct node, subtract off HASH, decrypt+rehash, audit hash,
                           then forward to tunnel */
                        l -= HASH_LENGTH;
                        outl = sizeof(out);
                        if (DEBUG) {printf("\nRx=%d[",l); dumpBuf(buf,l); printf("]\n");}
                        if (DEBUG) {printf("Hash=%d[",HASH_LENGTH); dumpBuf(&buf[l],HASH_LENGTH); printf("]\n");}
                        do_decrypt(buf,l,out,&outl);
                        if (DEBUG) {printf("\nDecrypt=%d[",outl); dumpBuf(out,outl); printf("]\n");}
                        createHash(out, outl, md_value, &md_len);
                        if (!isEqual(&buf[l], md_value, md_len)) PERROR("HASH");
                        if (write(fd_tunnel, out, outl) < 0) PERROR("write");
                     }//else
                  }//else
               }//if okToStart
               else if (!okToStart) { /* "receive" from node and DISCARD */
                  soutlen = sizeof(sout);
                  l = recvfrom(fd_udp, buf, sizeof(buf), 0, (struct sockaddr *)&sout, &soutlen);
                  if (l < 0) {
                      printf("fd_udp=%d,buf=%p,bufsize=%d,flags=%d,sockaddr=%p,soutlen=%p\n",
                              fd_udp,buf,sizeof(buf),0,&sout,&soutlen);
                      PERROR("recvfrom");
                  }//if
               }//else if !okToStart
              }//else if
          }//while
      }// else Server-CHILD
   }// else SERVER

}//main
