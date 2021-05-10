#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include <libwebsockets.h>
#include <curl/curl.h>
#include <json-c/json.h>

#include "const.h"
#include "../const.h"

// Alright, so here's the part where we try and get the
// gateway uri from the API

struct string
{
    char *ptr;
    size_t len;
};

int init_string(struct string *s)
{
    s->len = 0;
    s->ptr = malloc(s->len + 1);
    if (s->ptr == NULL)
    {
        return -1;
    }
    s->ptr[0] = '\0';
    return 0;
}

void deinit_string(struct string *s)
{
    free(s->ptr);
    s->ptr = NULL;

    s->len = 0;
}
/*
size_t guri_wfunc(void *ptr, size_t size, size_t nmemb,
                             struct string *s)
{
    size_t new_len = s->len + size * nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (s->ptr == NULL) {
        return -1;
    }
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    return size * nmemb;
}

static struct string gateway_uri;
static int gateway_uri_has_been_init = 0;

static int get_gateway_uri(CURL * curl_handle) {
    CURLcode curl_ret;
    struct string s;

    init_string(&s);

    curl_easy_setopt(curl_handle, CURLOPT_URL, DISCORD_HTTP_GET_GATEWAY);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, guri_wfunc);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &s);

    curl_ret = curl_easy_perform(curl_handle);

    if(curl_ret != CURLE_OK) {
        deinit_string(&s);
        return curl_ret;
    }

    struct json_object *jobj = json_tokener_parse(s.ptr);
    struct json_object *urlthing = NULL;

    if(json_object_object_get_ex(jobj, "url", &urlthing)) {
        if(!gateway_uri_has_been_init) {
            gateway_uri.ptr = NULL;
            gateway_uri.len = 0;
        }
        size_t url_len = json_object_get_string_len(urlthing);
        gateway_uri.ptr = realloc(gateway_uri.ptr, url_len + 1);
        if(gateway_uri.ptr == NULL) {
            deinit_string(&s);
            return -998;
        }
        gateway_uri_has_been_init = 1;
        memcpy(gateway_uri.ptr, json_object_get_string(urlthing), url_len);
        gateway_uri.len = url_len;
    } else {
        deinit_string(&s);
        return -999;
    }

    deinit_string(&s);
	return 0;
}

static CURLU uri_handle;
*/
// represents object containing client connection

static struct discord_gateway_connection
{
    lws_sorted_usec_list_t sul; /* schedule connection retry */
    struct lws *wsi;            /* related wsi if any */
    uint16_t retry_count;       /* counter for consequetive retries */
} dgc;

static struct lws_context *context;

static int interrupted, port = 443, ssl_connection = LCCSCF_USE_SSL,
                        do_identify;
static const char *pro = "discord-gateway-protocol";
static int heartbeat_interval;
static int sequence_num;

static char *token;

static void connect_client(lws_sorted_usec_list_t *sul)
{
    struct discord_gateway_connection *dgc =
        lws_container_of(sul, struct discord_gateway_connection, sul);
    struct lws_client_connect_info i;

    memset(&i, 0, sizeof(i));

    i.context = context;
    i.port = port;
    i.address = "gateway.discord.gg";
    i.path = "/?v=8&encoding=json";
    i.host = i.address;
    i.origin = i.address;
    i.ssl_connection = ssl_connection;
    i.protocol = pro;
    i.local_protocol_name = "discord-gateway";
    i.pwsi = &dgc->wsi;
    i.userdata = dgc;

    if (!lws_client_connect_via_info(&i))
    {
        lwsl_err("%s: connection failed\n", __func__);
        interrupted = 1;
    }
}
static int intents = 513;

static const char * os = "linux";
static const char * libname = "libdiscord";

static inline void discord_gateway_identify(struct lws *wsi) {
    struct json_object *payload = NULL;
    struct json_object *payload_data = NULL;
    struct json_object *payload_data_props = NULL;
    payload = json_object_new_object();
    payload_data = json_object_new_object();
    payload_data_props = json_object_new_object();

    json_object_object_add(payload_data_props, "$os", json_object_new_string(os));
    json_object_object_add(payload_data_props, "$browser", json_object_new_string(libname));
    json_object_object_add(payload_data_props, "$device", json_object_new_string(libname));

    json_object_object_add(payload_data, "token", json_object_new_string_len(token, 59));
    json_object_object_add(payload_data, "intents", json_object_new_int(intents));
    json_object_object_add(payload_data, "properties", payload_data_props);

    json_object_object_add(payload, "op", json_object_new_int(DISCORD_GATEWAY_IDENTIFY));
    json_object_object_add(payload, "d", payload_data);
lwsl_user("1\n");
    char buf[LWS_PRE + 4096];

    memset(&buf[LWS_PRE], 0, 4096);
    strcpy(&buf[LWS_PRE], json_object_to_json_string(payload));
    lwsl_user("%s: sending payload %s", __func__, json_object_to_json_string(payload));
    lws_write(wsi, &buf[LWS_PRE], strlen(&buf[LWS_PRE]), LWS_WRITE_TEXT);
}

static inline void discord_gateway_receive(struct lws *wsi, void *user, void *in, size_t len)
{
    struct json_object *json_payload = json_tokener_parse((char *)in);
    struct json_object *json_op = NULL;
    if (!json_pointer_get(json_payload, "/op", &json_op))
    {
        int op = json_object_get_int(json_op);
        lwsl_user("%i\n", op);
        switch (op)
        {
        case DISCORD_GATEWAY_DISPATCH:
            lwsl_user("DISPATCH received\n");
            struct json_object *json_sequence_number = NULL;
            if (!json_pointer_get(json_payload, "/s", &json_sequence_number))
            {
                sequence_num = json_object_get_int(json_sequence_number);
            }
            break;

        case DISCORD_GATEWAY_RECONNECT:
            interrupted = 1;
            break;

        case DISCORD_GATEWAY_INVALID_SESSION:
            interrupted = 1;
            break;

        case DISCORD_GATEWAY_HELLO:
            lwsl_user("HELLO received\n");
            struct json_object *json_payload_data_heartbeat_interval = NULL;
            if (!json_pointer_get(json_payload,
                                  "/d/heartbeat_interval",
                                  &json_payload_data_heartbeat_interval))
            {
                heartbeat_interval = json_object_get_int(json_payload_data_heartbeat_interval) * 1000;
                lws_set_timer_usecs(wsi, heartbeat_interval);
                if(do_identify) {
                    lwsl_user("Sending IDENTIFY\n");
                    discord_gateway_identify(wsi);
                    do_identify = 0;
                }
            }
            break;
        case DISCORD_GATEWAY_HEARTBEAT_ACK:
            lwsl_user("HEARTBEAT ACK\n");
            break;
        default:
            break;
        }
    }
}

static struct json_object *json_heartbeat_payload;



static int callback_discord_gateway(struct lws *wsi,
                                    enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
    struct discord_gateway_connection *dgc =
        (struct discord_gateway_connection *)user;
    switch (reason)
    {
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        lwsl_err("CLIENT_CONNECTION_ERROR: %s\n",
                 in ? (char *)in : "(null)");
        interrupted = 1;
        break;
    case LWS_CALLBACK_CLIENT_RECEIVE:
        lwsl_user("%s: %s\n", __func__, (char *)in);
        discord_gateway_receive(wsi, user, in, len);
        break;
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        lwsl_user("%s: established\n", __func__);
        break;
    case LWS_CALLBACK_CLIENT_CLOSED:
        interrupted = 1;
        break;
    case LWS_CALLBACK_TIMER:;
        struct json_object *payload = NULL;
        if (json_object_deep_copy(json_heartbeat_payload, &payload,
                                  json_c_shallow_copy_default) == 0)
        {
            if (sequence_num == NULL)
            {
                json_object_object_add(payload, "d",
                                       json_object_new_null());
            }
            else
            {
                json_object_object_add(payload, "d",
                                       json_object_new_int(sequence_num));
            }

            char buf[LWS_PRE + 4096];

            memset(&buf[LWS_PRE], 0, 4096);

            strcpy(&buf[LWS_PRE], json_object_to_json_string(payload));

            lws_write(wsi, &buf[LWS_PRE], strlen(&buf[LWS_PRE]), LWS_WRITE_TEXT);

            lws_set_timer_usecs(wsi, heartbeat_interval);
        }
        else
        {
            interrupted = 1;
        }
        break;
    LWS_CALLBACK_CLIENT_WRITEABLE:
        break;
    default:
        break;
    }

    return lws_callback_http_dummy(wsi, reason, user, in, len);
}

static const struct lws_protocols protocols[] = {
    {
        "discord-gateway",
        callback_discord_gateway,
        0,
        0,
    },
    {NULL, NULL, 0, 0}};

static void sigint_handler(int sig)
{
    interrupted = 1;
}

int main(int argc, const char **argv)
{
    do_identify = 0;
    token = NULL;
    sequence_num = NULL;
    json_heartbeat_payload = json_object_new_object();
    json_object_object_add(json_heartbeat_payload, "op",
                           json_object_new_int(DISCORD_GATEWAY_HEARTBEAT));
    struct lws_context_creation_info info;
    const char *p;
    int n = 0;

    signal(SIGINT, sigint_handler);
    memset(&info, 0, sizeof info);

    lws_cmdline_option_handle_builtin(argc, argv, &info);

    info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
    info.port = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
    info.protocols = protocols;

    if ((p = lws_cmdline_option(argc, argv, "--protocol")))
        pro = p;

    if ((p = lws_cmdline_option(argc, argv, "-t")))
    {
        FILE *fp;
        fp = fopen(p, "r");
        if (fp != NULL)
        {
            if (fseek(fp, 0L, SEEK_END) != 0)
            {
                perror("fseek");
                exit(1);
            }
            do_identify = 1;
            long bufsz = ftell(fp);
            if (bufsz == -1)
            {
                perror("ftell");
                exit(1);
            }

            token = malloc(sizeof(char) * (bufsz + 1));

            if (fseek(fp, 0L, SEEK_SET) != 0)
            {
                perror("fseek");
                exit(1);
            }

            size_t new_length = fread(token, sizeof(char),
                                      bufsz, fp);

            if (ferror(fp))
            {
                perror("fread");
                exit(1);
            }

            token[new_length++] = '\0';
        }
        else
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
        fclose(fp);
    }

    if ((p = lws_cmdline_option(argc, argv, "-p")))
        port = atoi(p);

    if (lws_cmdline_option(argc, argv, "-n"))
        ssl_connection &= ~LCCSCF_USE_SSL;

    if (lws_cmdline_option(argc, argv, "-j"))
        ssl_connection |= LCCSCF_ALLOW_SELFSIGNED;

    if (lws_cmdline_option(argc, argv, "-k"))
        ssl_connection |= LCCSCF_ALLOW_INSECURE;

    if (lws_cmdline_option(argc, argv, "-m"))
        ssl_connection |= LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;

    if (lws_cmdline_option(argc, argv, "-e"))
        ssl_connection |= LCCSCF_ALLOW_EXPIRED;

    info.fd_limit_per_thread = 1 + 1 + 1;

    context = lws_create_context(&info);
    if (!context)
    {
        lwsl_err("lws init failed\n");
        return 1;
    }
    lwsl_user("doIdentify: %d\n", do_identify);
    lws_sul_schedule(context, 0, &dgc.sul, connect_client, 1);

    while (n >= 0 && !interrupted)
        n = lws_service(context, 0);

    lws_context_destroy(context);
    lwsl_user("Completed\n");

    return 0;
}