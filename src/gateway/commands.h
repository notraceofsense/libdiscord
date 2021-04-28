struct discord_gateway_status_update {
    int since;

};

enum discord_gateway_activity_type {
    DISCORD_GATEWAY_ACTIVITY_GAME,
    DISCORD_GATEWAY_ACTIVITY_STREAMING,
    DISCORD_GATEWAY_ACTIVITY_LISTENING,
    DISCORD_GATEWAY_ACTIVITY_CUSTOM,
    DISCORD_GATEWAY_ACTIVITY_COMPETING
};

struct discord_gateway_activity {
    char * name;
    discord_gateway_activity_type type;
    char * url;
    int created_at;

};

