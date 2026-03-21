#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

// サーバー側でHPを管理する
int hero_hp = 100;
int demon_hp = 300;

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    std::cout << "魔王（サーバー）起動：ポート8080で待機中...\n";
    int new_socket = accept(server_fd, nullptr, nullptr);
    std::cout << "勇者が接続してきた！戦闘開始！\n";

    char buffer[1024] = {0};

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(new_socket, buffer, 1024);
        if (valread <= 0) {
            std::cout << "勇者が通信を切断した。\n";
            break;
        }

        std::string command(buffer);
        std::string result_msg = "";

        // 【拡張】勇者からのコマンド(文字列)を判定する論理
        if (command == "1") {
            demon_hp -= 20;
            result_msg += "勇者の攻撃！魔王に20のダメージ！\n";
        } else if (command == "2") {
            hero_hp += 30;
            result_msg += "勇者は回復魔法を唱えた！HPが30回復した！\n";
        } else if (command == "3") {
            result_msg += "勇者は身を固めている！\n";
        } else {
            result_msg += "勇者は混乱している...（無効なコマンド）\n";
        }

        // 魔王の反撃（自動処理）
        if (demon_hp > 0) {
            if (command == "3") {
                hero_hp -= 10;
                result_msg += "魔王の反撃！勇者に10のダメージ！\n";
            } else {
                hero_hp -= 30;
                result_msg += "魔王の反撃！勇者に30のダメージ！\n";
            }
         }


        // 状況のまとめを作成
        result_msg += "現在のHP -> 勇者: " + std::to_string(hero_hp) + " / 魔王: " + std::to_string(demon_hp);

        // クライアント（勇者）に結果を送信
        send(new_socket, result_msg.c_str(), result_msg.length(), 0);
        
        // サーバー側の画面にも状況を表示
        std::cout << "処理完了: 勇者HP=" << hero_hp << " 魔王HP=" << demon_hp << "\n";
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
