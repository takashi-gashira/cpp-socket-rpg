#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring> // memsetを使うため

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // ポート開放したまま他プロセスが残ってエラーになるのを防ぐ魔法のオプション
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    std::cout << "魔王「ポート8080で待っているぞ...」\n";
    int new_socket = accept(server_fd, nullptr, nullptr);
    std::cout << "魔王「来たな勇者！通信回線を確立した！」\n\n";

    char buffer[1024] = {0};

    // 【チャットの論理】無限ループで「受信」と「送信」を繰り返す
    while (true) {
        memset(buffer, 0, sizeof(buffer)); // 受信箱を一旦空っぽにする
        
        // 1. 勇者からのメッセージを待つ（受信するまでここで止まる）
        int valread = read(new_socket, buffer, 1024);
        if (valread <= 0) { // 勇者が逃げた（通信切断）場合
            std::cout << "魔王「ふはは、勇者は逃げ出したか...」\n";
            break;
        }
        std::cout << "勇者: " << buffer << "\n";

        // 2. 魔王（あなた）の反撃を入力する
        std::cout << "魔王（あなた）の返答を入力: ";
        std::string reply;
        std::getline(std::cin, reply); // キーボードからの入力を読み取る

        // 3. 勇者へ送り返す
        // .c_str() は C++の文字列を、ネットワーク用の生のデータ（C言語の文字配列）に変換する論理です
        send(new_socket, reply.c_str(), reply.length(), 0); 
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
