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
        std::cout << "接続失敗...\n";
        return -1;
    }

    char buffer[1024] = {0};

    while (true) {
        // 【拡張】コマンドメニューの表示
        std::cout << "\n=== コマンドを選べ ===\n";
        std::cout << "1: 攻撃する\n";
        std::cout << "2: 回復する\n";
        std::cout << "3: 防御する\n";
        std::cout << "4: 毒魔法を唱える\n";
        std::cout << "5: 逃げる\n";
        std::cout << "入力 > ";
        
        std::string command;
        std::getline(std::cin, command);

        if (command == "5") break;

        // サーバー（魔王）にコマンドの数字だけを送信する
        send(sock, command.c_str(), command.length(), 0);

        // サーバーからの結果（HPはどうなったか、反撃は来たか）を待つ
        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, 1024);
        if (valread <= 0) break;
        
        std::cout << "\n【システムメッセージ】\n" << buffer << "\n";
    }

    close(sock);
    return 0;
}
