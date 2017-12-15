//============================================================================
// Name        : file_client.cpp
// Author      : Lars Mortensen
// Version     : 1.0
// Description : file_client in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iknlib.h>

#define BUFFERSIZE 1000
#define PORT 9000

using namespace std;

void receiveFile(string fileName, int socketfd);

int main(int argc, char *argv[])
{
    string ip = argv[1];
    string filepath = argv[2];
    //Define variables
    int sockfd;
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
    serv_addr.sin_addr.s_addr = stoi(ip);
    serv_addr.sin_port = htons(PORT);

    //Connect socket
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
              error("ERROR on connect");

    receiveFile(filepath, sockfd);
}

/**
 * Modtager filstørrelsen og udskriver meddelelsen: "Filen findes ikke" hvis størrelsen = 0
 * ellers
 * Åbnes filen for at skrive de bytes som senere modtages fra serveren (HUSK kun selve filnavn)
 * Modtag og gem filen i blokke af 1000 bytes indtil alle bytes er modtaget.
 * Luk filen, samt input output streams
 *
 * @param fileName Det fulde filnavn incl. evt. stinavn
 * @param sockfd Stream for at skrive til/læse fra serveren
 */
void receiveFile(string fileName, int sockfd)
{
    char* fileNameChar = new char[fileName.length()+1];
    strcpy(fileNameChar, fileName.c_str());

    writeTextTCP(sockfd, fileNameChar);
    string extractedFileName = extractFileName(fileName.c_str());

    ofstream fs(extractedFileName, ios_base::out);

    auto fileSize = getFileSizeTCP(sockfd);

    char* buf = new char[BUFFERSIZE];

    while(fileSize != fs.tellp())
    {
        int bytesRead = read(sockfd, buf, BUFFERSIZE);
        fs.write(buf, bytesRead);
    }

    delete[] buf;
}

