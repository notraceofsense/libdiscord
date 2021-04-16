#pragma once

// The Discord API gateway limits payloads to a maximum of 4096 bytes.
// https://discord.com/developers/docs/topics/gateway#payloads
#define DISCORD_GATEWAY_PAYLOAD_MAXSIZE 4096

#define DISCORD_HTTP_GET_GATEWAY "https://discord.com/api/gateway"

enum discord_gateway_opcode {
    DISCORD_GATEWAY_DISPATCH,
    DISCORD_GATEWAY_HEARTBEAT,
    DISCORD_GATEWAY_IDENTIFY,
    DISCORD_GATEWAY_PRESENCE_UPDATE,
    DISCORD_GATEWAY_VOICE_STATE_UPDATE,
    DISCORD_GATEWAY_RESUME,
    DISCORD_GATEWAY_RECONNECT,
    DISCORD_GATEWAY_REQUEST_GUILD_MEMBERS,
    DISCORD_GATEWAY_INVALID_SESSION,
    DISCORD_GATEWAY_HELLO,
    DISCORD_GATEWAY_HEARTBEAT_ACK
};

enum discord_gateway_close_event_code {
    DISCORD_GATEWAY_CLOSE_UNKNOWN_ERROR = 4000,
    DISCORD_GATEWAY_CLOSE_UNKNOWN_OPCODE = 4001,
    DISCORD_GATEWAY_CLOSE_DECODE_ERROR = 4002,
    DISCORD_GATEWAY_CLOSE_NOT_AUTHENTICATED = 4003,
    DISCORD_GATEWAY_CLOSE_AUTHENTICATION_FAILED = 4004,
    DISCORD_GATEWAY_CLOSE_ALREADY_AUTHENTICATED = 4005,
    DISCORD_GATEWAY_CLOSE_INVALID_SEQUENCE = 4007,
    DISCORD_GATEWAY_CLOSE_RATE_LIMITED = 4008,
    DISCORD_GATEWAY_CLOSE_SESSION_TIMED_OUT = 4009,
    DISCORD_GATEWAY_CLOSE_INVALID_SHARD = 4010,
    DISCORD_GATEWAY_CLOSE_SHARDING_REQUIRED = 4011,
    DISCORD_GATEWAY_CLOSE_INVALID_API_VERSION = 4012,
    DISCORD_GATEWAY_CLOSE_INVALID_INTENTS = 4013,
    DISCORD_GATEWAY_CLOSE_DISALLOWED_INTENTS = 4014
};