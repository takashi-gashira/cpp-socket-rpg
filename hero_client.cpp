#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    std::cout << "勇者「魔王の城に突撃する！」\n";
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "接続失敗...魔王は不在のようだ。\n";
        return -1;
    }
    std::cout << "勇者「魔王との通信回線を確立した！」\n\n";

    char buffer[1024] = {0};

    // 【チャットの論理】
    while (true) {
        // 1. 勇者（あなた）の攻撃/メッセージを入力する
        std::cout << "勇者（あなた）の行動を入力: ";
        std::string msg;
        std::getline(std::cin, msg);

        // 終了コマンド
        if (msg == "逃げる") break;

        // 2. 魔王へ送る
        send(sock, msg.c_str(), msg.length(), 0);

        // 3. 魔王からの反撃を待つ
        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, 1024);
        if (valread <= 0) {
            std::cout << "魔王との通信が切断された。\n";
            break;
        }
        std::cout << "魔王: " << buffer << "\n";
    }

    close(sock);
    return 0;
}
