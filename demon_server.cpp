#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <thread>

class Character {
public:
    int hp;
    bool is_poisoned;

    Character(int initial_hp) {
        hp = initial_hp;
        is_poisoned = false;
    }

    void take_damage(int damage) {
        hp -= damage;
        if (hp < 0) {
            hp = 0;
        }
    }
};
// サーバー側でHPを管理する
Character hero(100);
Character demon(300);

void handle_hero(int client_socket) {

    char buffer[1024] = {0};

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(client_socket, buffer, 1024);
        if (valread <= 0) {
            std::cout << "勇者が通信を切断した。\n";
            break;
        }

        std::string command(buffer);
        std::string result_msg = "";
        std::ofstream save_file("save.txt");

        // 【拡張】勇者からのコマンド(文字列)を判定する論理
        if (command == "1") {
            demon.take_damage(20);
            result_msg += "勇者の攻撃！魔王に20のダメージ！\n";
        } else if (command == "2") {
            hero.hp += 30;
            result_msg += "勇者は回復魔法を唱えた！HPが30回復した！\n";
        } else if (command == "3") {
            result_msg += "勇者は身を固めている！\n";
        } else if (command == "4") {
            demon.is_poisoned = true;
            result_msg += "勇者は毒の魔法を唱えた！魔王は毒状態になった！\n";
        } else if (command == "6") {
            save_file << hero.hp << "\n" << demon.hp << "\n" << demon.is_poisoned << "\n";
            save_file.close();
            result_msg += "魔王の城の現在の状態が『save.txt』に記録された！\n";
        } else {
            result_msg += "勇者は混乱している...（無効なコマンド）\n";
        }

        // 魔王の反撃（自動処理）
        if (demon.hp > 0 && command != "6") {
            if (command == "3") {
                hero.hp -= 10;
                result_msg += "魔王の反撃！勇者に10のダメージ！\n";
            } else {
                hero.hp -= 30;
                result_msg += "魔王の反撃！勇者に30のダメージ！\n";
            }
         }

         // 状態異常付与
         if (demon.is_poisoned == true && command != "6") {
            demon.hp -= 10;
            result_msg += "【毒】魔王は毒で10のダメージを受けた！\n";
        }


        // 状況のまとめを作成
        result_msg += "現在のHP -> 勇者: " + std::to_string(hero.hp) + " / 魔王: " + std::to_string(demon.hp);

        // クライアント（勇者）に結果を送信
        send(client_socket, result_msg.c_str(), result_msg.length(), 0);

        // サーバー側の画面にも状況を表示
        std::cout << "処理完了: 勇者HP=" << hero.hp << " 魔王HP=" << demon.hp << "\n";
    }
        close(client_socket);

}

int main() {
    std::ifstream load_file("save.txt");

    // ファイルが無事に開けたか（前回セーブしたか）を確認
    if (load_file.is_open()) {
    load_file >> hero.hp >> demon.hp >> demon.is_poisoned;
    load_file.close();

    std::cout << "【システム】前回のセーブデータを読み込みました！\n";
        std::cout << "復元HP -> 勇者: " << hero.hp << " / 魔王: " << demon.hp << "\n";
    } else {
        std::cout << "【システム】セーブデータがありません。初めからスタートします。\n";
    }
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    std::cout << "魔王（マルチスレッド対応版）起動：ポート8080で待機中...\n";

    // 受付係の無限ループ
    while (true) {
        // 1. 勇者が来るのを待つ（ここで一時停止して待機する）
        int new_socket = accept(server_fd, nullptr, nullptr);
        std::cout << "勇者が接続してきた！戦闘開始！\n";
        std::cout << "【システム】新しい勇者が魔王の城に侵入しました！\n";

        // 2. 【究極のハッキング】案内係（スレッド）を生み出して丸投げする！
        // std::thread(実行したい関数, 渡したいデータ).detach();
        std::thread(handle_hero, new_socket).detach();

        // 分身を生み出したら、受付係はすぐにループの先頭に戻り、次の勇者を待つ！
    }

    close(server_fd);
    return 0;
}
