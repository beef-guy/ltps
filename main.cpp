/*
    @copyright gurotopia (c) 25-6-2024
    @author leeendl | English & Japanese Comments, 英語と日本語のコメント

    Project has open arms for contribution! | このプロジェクトは貢献を歓迎します！
*/
#include "include/pch.hpp"
#include "include/database/items.hpp" // @note items.dat reading | items.datのバイトを処理する
#include "include/network/compress.hpp" // @note isalzman's compressor | ENetの圧縮
#include "include/database/peer.hpp" // @note peer class | peerクラス
#include "include/event_type/event_type"

#include <fstream>
#include <future>

int main()
{
    {
        ENetCallbacks callbacks{
            .malloc = &malloc, 
            .free = &free, 
            .no_memory = []() { printf("ENet memory overflow"); }
        };
        enet_initialize_with_callbacks(ENET_VERSION, &callbacks);
        printf("ENet initialize success! (v%d.%d.%d)\n", ENET_VERSION_MAJOR, ENET_VERSION_MINOR, ENET_VERSION_PATCH);
    } // @note delete callbacks | 解放する: callbacks
    server = enet_host_create({
        .host = in6addr_any, 
        .port = 17091
    }, 
    ENET_PROTOCOL_MAXIMUM_PEER_ID, 2);

    server->checksum = enet_crc32;
    enet_host_compress_with_range_coder(server);
    {
        std::ifstream file("items.dat", std::ios::binary | std::ios::ate);
        if (!file.is_open()) printf("failed to open items.dat\n");
        std::streampos size = file.tellg();
        im_data.resize(im_data.size() + size);
        im_data[0] = std::byte{ 04};
        im_data[4] = std::byte{ 0x10 };
        *reinterpret_cast<int*>(&im_data[8]) = -1; // @note ff ff ff ff
        im_data[16] = std::byte{ 0x08 };
        *reinterpret_cast<std::streampos*>(&im_data[56]) = size;
        file
            .seekg(0, std::ios::beg) // @note start from beginning of items.dat | 先頭から開始する
            .read(reinterpret_cast<char*>(&im_data[60]), size);
    } // @note delete file, size and closes file | ファイルを閉じ、sizeを解放する
    cache_items();

    ENetEvent event{};
    std::vector<std::thread> threads{};
    while(true)
        while (enet_host_service(server, &event, 1) > 0)
            if (const auto i = event_pool.find(event.type); i != event_pool.end())
                threads.emplace_back([=] { i->second(event); }).join(); // @todo
    return 0;
}
