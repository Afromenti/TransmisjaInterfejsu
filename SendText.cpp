#include <iostream>
#include <windows.h>
#include <fstream>
using namespace std;

LPCTSTR portError = "Niewlasciwa nazwa portu lub port jest juz aktywny";
unsigned long blockSize = 10000;
unsigned char txtArray[9999999];
unsigned int indexx = 0;

void print(unsigned char *array, unsigned long size)
{
    for(unsigned int i = 0; i < size; i++)
    {
        cout << array[i];
    }
}

unsigned char *cuttingMainArray(unsigned long blockSize, unsigned long *lastIndex)
{
     unsigned char *tempArray = new unsigned char [blockSize];

     for (unsigned int i = 0; i < blockSize; i++)
     {
        tempArray[i] = txtArray[*lastIndex];  
        *lastIndex += 1;
     }
    
    return tempArray;
}

bool openPort(HANDLE &hCommDev, LPCTSTR portName, DCB dcb)
{
    hCommDev = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL,OPEN_EXISTING, 0, NULL);

    if(hCommDev != INVALID_HANDLE_VALUE)
    {
        dcb.DCBlength = sizeof(dcb);
        if(GetCommState(hCommDev, &dcb) == 0)
        {
            cout << "GetCommState Error." << "\n";
            return false;
            CloseHandle(hCommDev);
        }
        
        cout << "\nObecne ustawienia portu to: " << "\n";
        cout << "Predkosc transmisji: " << dcb.BaudRate << "\n";
        cout << "Sprawdzanie parzystosci: " << dcb.fParity << "\n";
        cout << "Ustawienie parzystosci: " << int(dcb.Parity) << "\n";
        cout << "Bity stopu: " << int(dcb.StopBits) << "\n";
        cout << "Kontrola linii DTR: " << dcb.fDtrControl << "\n";
       
       return true;
    }
    cout << "Blad uchwytu \n";
    CloseHandle(hCommDev);
    return false;

}

bool setCommParametres(HANDLE hCommDev, COMMTIMEOUTS commTimeouts, unsigned long ReadIntervalTimeout, unsigned long ReadTotalTimeoutMultiplier, unsigned long ReadTotalTimeoutConstant, unsigned long WriteTotalTimeoutMultiplier, unsigned long WriteTotalTimeoutConstant)
{    
    if(GetCommTimeouts (hCommDev, &commTimeouts) == 0)
    {
        cout << "Timeouts error." << "\n";
        return false;
    }

    commTimeouts.ReadIntervalTimeout = ReadIntervalTimeout;
    commTimeouts.ReadTotalTimeoutConstant = ReadTotalTimeoutConstant;
    commTimeouts.ReadTotalTimeoutMultiplier = ReadTotalTimeoutMultiplier;
    commTimeouts.WriteTotalTimeoutConstant = WriteTotalTimeoutConstant;
    commTimeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier;

    if(SetCommTimeouts(hCommDev, &commTimeouts) == 0)
    {
        cout << "Błąd wykonania funkcji SetCommTimeouts()\n";
        CloseHandle(hCommDev);
        return false;
    }
    return true;
 }

void writeSerialPort(unsigned char *buffer, HANDLE hCommDev, unsigned long blockSize)
{
    DWORD numberOfBytesWritten = 0;
    WriteFile(hCommDev, buffer, blockSize, &numberOfBytesWritten, NULL);
}

void readSerialPort(HANDLE hCommDev, unsigned long blockSize)
{
    unsigned char *bytesRead = new unsigned char [blockSize];
    DWORD numberOfBytesRead = 0;

    ReadFile (hCommDev, bytesRead, blockSize, &numberOfBytesRead, 0);
    print(bytesRead, blockSize);

    delete bytesRead;
 }

 
bool readFileTxt(string name)
{
    char character;
    ifstream inputFile(name);

    if (!inputFile.is_open()) 
    {
        cerr << "File open error. " << "Bledna sciezka" << endl;
        inputFile.close();
        return false;
    }

    while (inputFile.get(character)) 
    {
        txtArray[indexx++] = character;
    }

    inputFile.close();
    return true;
}

void sendFileTxt(HANDLE &hCommDev)
{
    unsigned long remainingBytes = indexx % blockSize;
    unsigned long howManyTimes = indexx / blockSize;
    unsigned long iterator = 1;
    unsigned long lastIndex = 0;

    cout << "\nWielkosc bloku wynosi: " << blockSize << "\n";
    cout << "Ile razy dlugosc calego pliku jest wieksza od ustawionego bloku: " << howManyTimes << "\n";
    cout << "Dlugosc wczytanych danych: " << indexx << "\n";
    cout << "Pozostale Bajty: " << remainingBytes << "\n\n";

    for (iterator; iterator <= howManyTimes; iterator++)
    {
        unsigned char *tempArray = cuttingMainArray(blockSize, &lastIndex);
        
        writeSerialPort(tempArray , hCommDev, blockSize);
        readSerialPort(hCommDev, blockSize);

        delete tempArray;
    }

    if (remainingBytes > 0)
    {
        unsigned char *tempArray = cuttingMainArray(blockSize, &lastIndex);
        
        writeSerialPort(tempArray , hCommDev, blockSize);
        readSerialPort(hCommDev, blockSize);

        delete tempArray;
    }

}


int main()
{
    HANDLE hCommDev;
    DCB dcb;
    COMMTIMEOUTS commTimeouts;
    LPCTSTR portName = "COM3";
    string path = "";
    unsigned int bSize = 1;

    while (true)
    {
        cout << "Podaj sciezke do pliku: \n";
        getline(cin, path);

        if(readFileTxt(path))
        {
            cout << "Ladowanie pliku powiodlo sie. \n";
            if(openPort(hCommDev, portName, dcb))
            {
                if(setCommParametres(hCommDev, commTimeouts, 3000, 3000, 3000, 3000, 3000 ))
                {
                    cout << "\nW ilu Bajtach chcesz przesylac pojedynczy blok transmiji? Wielkosc bloku nie moze byc wieksza od predkosci transmisji. \n";

                    cin >> bSize;
                    blockSize = bSize;

                    sendFileTxt(hCommDev);
                    break;
                }   
            }
            break;
        }
    }

    CloseHandle(hCommDev);
    return 0;

}
