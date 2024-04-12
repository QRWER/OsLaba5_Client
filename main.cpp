#include <iostream>
#include <string>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)//для inet_addr

bool printCausedBy(int Result, const char* nameOfOper) {
    if (!Result) {
        std::cout << "Connection closed.\n";
        return false;
    }
    else if (Result < 0) {
        std::cout << nameOfOper;
        std::cerr << "failed:\n", WSAGetLastError();
        return false;
    }
    return true;
}

int main() {
    //Загрузка библиотеки
    WSAData wsaData; //создаем структуру для загрузки
    //запрашиваемая версия библиотеки winsock
    WORD DLLVersion = MAKEWORD(2, 1);
    //проверка подключения библиотеки
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        std::cerr << "Error: failed to link library.\n";
        return 1;
    }
    //Заполняем информацию об адресе сокета
    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //адрес
    addr.sin_port = htons(1111); //порт
    addr.sin_family = AF_INET; //семейство протоколов
    //сокет для прослушвания порта
    SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
    //проверка на подключение к серверу
    if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr))) {
        std::cout << "I don't see a Pot =(.\n";
        return 1;
    }
    HANDLE hMutex;
    hMutex = OpenMutexA(SYNCHRONIZE, FALSE, "mutex");
    if(hMutex == NULL){
        std::cerr << "Error open semaphore.\n";
        return GetLastError();
    }
    WaitForSingleObject(hMutex, INFINITE);
    std::cout << "Sheff: Do you wanna eat something?\nMe: ";
    std::string answer;
    getline(std::cin, answer);
    while(answer != "Yes" && answer != "No"){
        std::cout << "Sheff: That's not what I expected to hear. I'll ask you again.\nSheff: Do you wanna eat something?\nMe: ";
        getline(std::cin, answer);
    }
    if(answer == "Yes") {
        printCausedBy(send(Connection, answer.data(), sizeof(answer), NULL), "Send");
        std::string food;
        printCausedBy(recv(Connection, food.data(), sizeof(food), NULL), "Recv");
        std::cout << "Sheff: Take the " << food << " enjoy your meal.\n";
    }
    else
        std::cout << "Sheff: So why did you come here? Get out of here.\n";
    Sleep(5000);
    CloseHandle(hMutex);
    if (closesocket(Connection) == SOCKET_ERROR)
        std::cerr << "Failed to terminate connection. Error code: " << WSAGetLastError();
    WSACleanup();
    return 0;
}
