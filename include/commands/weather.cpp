#include "pch.hpp"
#include "database/peer.hpp"
#include "database/world.hpp"
#include "network/packet.hpp"
#include "tools/string_view.hpp"
#include "weather.hpp"

void weather(ENetEvent& event, const std::string_view text) {
    if (_peer[event.peer]->lobby) {
        action(*event.peer, "log", "msg|`4You must be in a world to change weather!");
        return;
    }

    int weatherID = 0;
    std::string textStr(text);
    auto parts = readch(textStr, ' ');
    if (parts.size() > 1) {
        try {
            weatherID = std::stoi(parts[1]);
            if (weatherID < 0 || weatherID > 100) {
                weatherID = 0;
                action(*event.peer, "log", "msg|`4Invalid weather ID! Using default (0).");
            }
        } catch (...) {
            action(*event.peer, "log", "msg|`4Invalid weather ID! Using default (0).");
        }
    } else {
        action(*event.peer, "log", "msg|`2Usage: /weather <ID>");
        return;
    }

    std::string worldName = _peer[event.peer]->recent_worlds.back();

    peers(ENET_PEER_STATE_CONNECTED, [&](ENetPeer& p) {
        if (_peer[&p]->recent_worlds.empty()) return;
        if (_peer[&p]->recent_worlds.back() != worldName) return;

        gt_packet(p, false, {
            "OnSetCurrentWeather",
            weatherID
        });
    });

    action(*event.peer, "log", std::format("msg|`2Changed weather to ID {}", weatherID));
}
