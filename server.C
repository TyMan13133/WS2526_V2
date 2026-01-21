/*
 * server.C
 *
 *  Created on: 11.09.2019
 *      Author: aml
 */
#include <cstdio> // standard input and output library
#include <cstdlib> // this includes functions regarding memory allocation
#include <cstring> // contains string functions
#include <cerrno> //It defines macros for reporting and retrieving error conditions through error codes
#include <ctime> //contains various functions for manipulating date and time

#include <unistd.h> //contains various constants
#include <sys/types.h> //contains a number of basic derived types that should be used whenever appropriate
#include <arpa/inet.h> // defines in_addr structure
#include <sys/socket.h> // for socket creation
#include <netinet/in.h> //contains constants and structures needed for internet domain addresses


#include "TASK3.H"
#include "SIMPLESOCKET.H"

/**
*   \class MySrv
*   \brief Classee definieert unser Komuniktionsprotkol
*
*/

class MySrv : public TCPserver {
public:
    MySrv (int port, int bsize) : TCPserver(port,bsize) {w=new TASK3::World();}; //sicherstellen das eine welt vorhanden ist
protected:
    TASK3::World *w;//objekt weld mit dem namen w
    string myResponse(string string);

};

int main(){
	srand(time(nullptr));

	MySrv srv(2030,25);
	srv.run();
}

string MySrv :: myResponse(string input){
    int x,y, e; //komunikation code hier hin
    //vergleich vom string 0 bis 7 wenn ja dann OK1
    if (input.compare(0,7,"NEWGAME")== 0){

        if (w!= nullptr){
            delete w;
        }

        w = new TASK3::World ();

        return string("OK");
    }

    if (input.compare(0,5,"SHOT[")== 0){

        e = sscanf (input.c_str(),"SHOT[%d,%d]",&x,&y);
        if(e!=2){
            return string ("ERROR");
        }else{
            return (to_string(x+y));
        }

    }


    return string ("UNKOWNCMD");
}
