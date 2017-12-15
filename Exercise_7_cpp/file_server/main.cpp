//============================================================================
// Name        : file_server.cpp
// Author      : Lars Mortensen
// Version     : 1.0
// Description : file_server in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iknlib.h>

#define PORT 9000
#define BUFFERSIZE 1000

using namespace std;

void sendFile(string fileName, long fileSize, int outToClient);

/**
 * main starter serveren og venter på en forbindelse fra en klient
 * Læser filnavn som kommer fra klienten.
 * Undersøger om filens findes på serveren.
 * Sender filstørrelsen tilbage til klienten (0 = Filens findes ikke)
 * Hvis filen findes sendes den nu til klienten
 *
 * HUSK at lukke forbindelsen til klienten og filen nÃ¥r denne er sendt til klienten
 *
 * @throws IOException
 *
 */
int main(int argc, char *argv[])
{
    //Define variables
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[BUFFERSIZE];
    struct sockaddr_in serv_addr, cli_addr;

    //Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR when opening socket");

    //bzero clears data
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    //Bind socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
              error("ERROR on binding");

    while(1)
    {
        //Waiting for client connect
        listen(sockfd, 1);
        clilen = sizeof(cli_addr);

        //Accept
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0)
            error("ERROR on accept");

        bzero(buffer, BUFFERSIZE);

        //Reading from socket
        string fileName = readTextTCP(buffer, BUFFERSIZE, newsockfd);

        //find file with matching fileName, fileSize
        long size = check_File_Exists(buffer);
        if(size < 0)
            error("No file found");
        else
            //sendFile(string fileName, long fileSize, int outToClient);
            sendFile(fileName, size, newsockfd);

        printf("Client is requesting: %s\n", fileName);

        //close
        close(newsockfd);
        }

    //close
    close(sockfd);

    return 0;
}

/**
 * Sender filen som har navnet fileName til klienten
 *
 * @param fileName Filnavn som skal sendes til klienten
 * @param fileSize Størrelsen på filen, 0 hvis den ikke findes
 * @param outToClient Stream som der skrives til socket
     */
void sendFile(string fileName, long fileSize, int outToClient)
{

   //Open file
   ifstream fs(fileName, ifstream::in);
   if(!fs.good())
       error("ÅÅHHH NEJ filen eksisterer ikke måske????");

   //Start from spot 0 in the filestream
   fs.seekg(0);

   char* fileSizeChar = new char[256];
   sprintf(fileSizeChar, "%ld", fileSize);

   //Send filesize
   writeTextTCP(outToClient, fileSizeChar);

   char* buf = new char[BUFFERSIZE];

   //Send file
   while(fileSize != fs.tellg())
   {
       int blocks = (fileSize - fs.tellg()) < 1000 ? (fileSize - fs.tellg()) : 1000;
       fs.read(buf, blocks);
       send(outToClient, buf, blocks, NULL);
   }

   delete[] buf;
}

