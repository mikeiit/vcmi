/*
 * LobbyDefines.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#pragma once

struct LobbyAccount
{
	std::string accountID;
	std::string displayName;
	//std::string status;
};

struct LobbyGameRoom
{
	std::string roomUUID;
	std::string roomStatus;
	uint32_t playersCount;
	uint32_t playersLimit;
};

struct LobbyChatMessage
{
	std::string sender;
	std::string messageText;
	std::chrono::seconds age;
};

enum class LobbyCookieStatus : int32_t
{
	INVALID,
	EXPIRED,
	VALID
};

enum class LobbyInviteStatus : int32_t
{
	NOT_INVITED,
	INVITED,
	DECLINED
};

enum class LobbyRoomState : int32_t
{
	IDLE,        // server is ready but no players are in the room
	PUBLIC,      // host has joined and allows anybody to join
	PRIVATE,     // host has joined but only allows those he invited to join
	//BUSY,        // match is ongoing
	//CANCELLED,   // game room was cancelled without starting the game
	//CLOSED,      // game room was closed after playing for some time
};
