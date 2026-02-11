#include <string>
#include <iostream>
#include <unistd.h> //contains various constants
#include <random>
#include <set>
#include <fstream>
#include <ctime>    //time(NULL)
#include <cstdlib> //rand(), srand()

#include "SIMPLESOCKET.H"

using namespace std;


TCPclient c;

int tactic1();  //Zeilenweiser Durchgang
int tactic2();  //Durchgang im Schachbrett Muster
int tactic3();  //Zufaellige Reihenfolge


unsigned int tries = 100; //Anzahl der Durchlaeufe

//Hilfsfunktion fuer den Fisher-Yates Shuffle fuer tactic3
void FYShuffle(int arr[], int size);
int getX(int idx); //Konvertiert idx in x
int getY(int idx); //Konvertiert idx in y


int main(){
    //Client Setup
    cout << "Client initiated" << endl;

    //Fuer die tactic3 und die dazugehoerige Shuffle Funktion
    srand(time(NULL));

    // Verbindung zum Server wird hergestellt
    string host = "localhost";
    cout << "Establishing Connection ..." << endl;
    c.conn(host, 2030);

    //CSV Datei fuer die statistische Auswertung
    ofstream StatisticalData;
    StatisticalData.open("StatisticalData.csv");

    if(StatisticalData.is_open()){
        cout << "The File for the saving of statistical Data was openend sucessfully, Proceeding ..." << endl;
    }
    else{
        cout << "Unexpected Error opening the file" << endl;
    }

    //Auswaehlen der Strategie
    int strategy;
    cout << "What Strategy should be statistically analysed ?:" << endl;
    cout << "1. Strategy 1 (line by line Pattern)" << endl;
    cout << "2. Strategy 2 (Chessboard Pattern)" << endl;
    cout << "3. Strategy 3 (Random Pattern)" << endl;
    cin >> strategy;

    switch(strategy){
        case 1: cout << "Strategy 1 was choosen" << endl;
                break;
        case 2: cout << "Strategy 2 was choosen" << endl;
                break;
        case 3: cout << "Strategy 3 was choosen" << endl;
                break;

        default:
                cout << "Invalid Strategy!" << endl;
                sleep(15);
                return 0;
        }

    // Durchgang von mehreren Spielen
    int atmpsArr[100];
    for(int n = 0; n < (int)tries; n++){
        atmpsArr[n] = 0;

        //Anfrage an den Server fuer ein neues Spiel
        c.sendData("NEWGAME");
        string msg = c.receive(32); //Empfang
        //Erwartet wird "NEWGAME"

        int currRun = 0;

        switch(strategy){
        case 1: currRun = tactic1();
                break;
        case 2: currRun = tactic2();
                break;
        case 3: currRun = tactic3();
                break;
        default:
                cout << "Invalid Strategy!" << endl;
                sleep(15);
                return 1;
        }
        atmpsArr[n] = currRun;

        //Fehler-Abfang
        if(atmpsArr[n] == 0){
            cout << "ERROR! No Strategy is implemented, terminating ..." << endl;
            sleep(15);
            return 0;
        }
        StatisticalData << (n+1) << ";" << currRun << endl;
        cout << "Game " << (n+1) << ": needed " << to_string(atmpsArr[n]) << " attemps" << endl;
    }

    //Schließen der CSV Datei
    StatisticalData.close();
    if(StatisticalData.is_open() == false){
            cout << "Statistical Data exported" << endl;
    }
    else{
            cout << "Unknown Error while trying to export Statistical Data" << endl;
    }

    sleep(50);

    return 0;

}

/*
- tactic1:
- Geht das Feld zeilenweise durch
- Zum Beispiel Zeile y = 1..10, Spalte x = 1..10
*/

int tactic1(){
    string msg;
    int attemps = 0;
    for(int i = 1; i <= 10; i++){
        for(int j = 1; j <= 10; j++){
            attemps++;
            msg = "SHOT[" + to_string(j) + "," + to_string(i) + "]";
            c.sendData(msg);
            msg = c.receive(32);

            if(msg == "GAME_OVER"){
                return attemps;
            }
        }
    }
    //Fehlerabfang, falls nie alle Schiffe versenkt werden
        return -1;
}

/*
- tactic2:
- Geht das Feld im Schachbrettmuster durch
- Erst Felder (x,y) mit (x+y)%2 == 0
- Dann Felder (x,y) mit (x+y)%2 == 1
*/

int tactic2(){
    string msg;
    int attemps = 0;

    //Erster Durchgang: (x+y) % 2 == 0
    for(int i = 1; i <= 10; i++){
        for(int j = 1; j <= 10; j++){
            if(((i + j) % 2) == 0){
                attemps++;
                msg = "SHOT[" + to_string(j) + "," + to_string(i) + "]";
                c.sendData(msg);

                msg = c.receive(64);

                if(msg == "GAME_OVER"){
                    return attemps;
                }
            }
        }
    }

    //Zweiter Durchgang: (x+y) % 2 == 1
    for(int i = 1; i <= 10; i++){
        for(int j = 1; j <= 10; j++){
            if(((i + j) % 2) == 1){
                attemps++;
                msg = "SHOT[" + to_string(j) + "," + to_string(i) + "]";
                c.sendData(msg);

                msg = c.receive(64);


                if(msg == "GAME_OVER"){
                    return attemps;
                }
            }
        }
    }
    //Fehlerabfang, falls nie alle Schiffe versenkt werden
    return -1;
}

/*
- tactic3:
- Zufaellige Reihenfolge aller Felder(1..10,1..10)
- Es wird definiert:
- idx = (y-1)*10 + (x-1) -> also von 0 bis 99
- x = (idx % 10) +1
- y = (idx / 10) +1
- Folgend wird das Array mit dem FY-Shuffle gemischt
*/

int tactic3(){
    //1-D Array mit den Werten 0 bis 99
    int field1[100];
    for(int i = 0; i < 100; i++){
        field1[i] = i;
    }

    //FY-Shuffle fuer das 1-D Array
    FYShuffle(field1, 100);

    int attempts = 0;
    for(int i = 0; i < 100; i++){
        //idx => (x,y)
        int idx = field1[i];
        int x = getX(idx); //Spalte
        int y = getY(idx); //Zeile

        attempts++;
        string msg = "SHOT[" + to_string(x) + "," + to_string(y) + "]";
        c.sendData(msg);

        string reply = c.receive(64);


        if(reply == "GAME_OVER"){
            return attempts;
        }
    }
    //Fehlerabfang, falls nie alle Schiffe versenkt werden
    return -1;
}


/*
- Fisher-Yates Shuffle
- basierend auf rand() und srand(time(NULL))
*/

void FYShuffle(int arr[], int size){
    for(int i = size -1; i > 0; i--){
        //Zufallsindex j in [0..i]
        int j = rand() % (i+1);
        //vertauschen
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

// Umrechnen idx in x
int getX(int idx){
    // idx=0..99 => x =(idx%10)+1
    return (idx%10)+1;
}

// Umrechnen idx in y
int getY(int idx){
    // idx=0..99 => x =(idx%10)+1
    return (idx/10)+1;
}



