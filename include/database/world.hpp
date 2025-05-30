#pragma once

/* fg, bg, hits */
class block 
{
    public:
    short fg{0}, bg{0};
    std::string label{""}; // @note sign/door label
    std::array<int, 2> hits{0, 0}; // @note fg, bg
};

/* uid, id, count, pos*/
class ifloat 
{
    public:
    std::size_t uid{0};
    short id{0};
    short count{0};
    std::array<float, 2> pos;
};

#include "nlohmann/json.hpp" // @note nlohmann::json for writing/reading JSON format

class world 
{
public:
    world& read(std::string name);
    std::string name{};
    int owner{ 00 }; // @note owner of world using peer's user id.
    std::array<int, 6> admin{}; // @note admins (by user id). excluding owner. (6 is a experimental amount, if increase update me if any issue occur -leeendl)
    short visitors{0}; // -> stack object
    std::vector<block> blocks; /* all blocks, size of 1D meaning (6000) instead of (100, 60) */
    std::vector<ifloat> ifloats{}; /* (i)tem floating */
    ~world();
};
extern std::unordered_map<std::string, world> worlds;

void send_data(ENetPeer& peer, const std::vector<std::byte>& data);

void state_visuals(ENetEvent& event, state s);

void block_punched(ENetEvent& event, state s, block& b);

void drop_visuals(ENetEvent& event, const std::array<short, 2>& im, const std::array<float, 2>& pos);

void clothing_visuals(ENetEvent &event);
