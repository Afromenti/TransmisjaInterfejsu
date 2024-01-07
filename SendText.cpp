#include <iostream>
#include <windows.h>
#include <fstream>
using namespace std;

LPCTSTR portError = "Niewlasciwa nazwa portu lub port jest juz aktywny";
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

void openPort(HANDLE &hCommDev, LPCTSTR portName, DCB dcb)
{
    hCommDev = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL,OPEN_EXISTING, 0, NULL);
    if(hCommDev != INVALID_HANDLE_VALUE)
    {
        dcb.DCBlength = sizeof(dcb);
        if(GetCommState(hCommDev, &dcb) == 0)
        {
            cout << "GetCommState Error." << "\n";
            CloseHandle(hCommDev);
        }
        /*
        cout << "Obecne ustawienia portu to: " << "\n";
        cout << "Predkosc transmisji: " << dcb.BaudRate << "\n";
        cout << "Sprawdzanie parzystosci: " << dcb.fParity << "\n";
        cout << "Ustawienie parzystosci: " << int(dcb.Parity) << "\n";
        cout << "Bity stopu: " << int(dcb.StopBits) << "\n";
        cout << "Bity danych: " << int(dcb.ByteSize) << "\n";
        cout << "Kontrola linii DTR: " << dcb.fDtrControl << "\n";
        */
    }
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

 
void readFileTxt(string name)
{
    char character;
    ifstream inputFile(name);

    if (!inputFile.is_open()) 
    {
        cerr << "File open error. " << name << endl;
        return;
    }

    while (inputFile.get(character)) 
    {
        txtArray[indexx++] = character;
    }
}

void sendFileTxt(HANDLE &hCommDev)
{
    unsigned long blockSize = 5555;
    unsigned long remainingBytes = indexx % blockSize;
    unsigned long howManyTimes = indexx / blockSize;
    unsigned long iterator = 1;
    unsigned long lastIndex = 0;

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
    
    readFileTxt("image.txt");
    openPort(hCommDev, portName, dcb);
    setCommParametres(hCommDev, commTimeouts, 0, 1000, 0, 0, 0);   
    //cout << txtArray;
    sendFileTxt(hCommDev);
    //CloseHandle(hCommDev);

    cout << "\n" << "Przesylanie pliku zakonczylo sie powodzeniem" << endl;
    return 0;

}
