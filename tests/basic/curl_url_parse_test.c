#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

int main() {
    CURLU *handle = curl_url();
    curl_url_set(handle, CURLUPART_URL, "wss://gateway.discord.gg/", 
    CURLU_NON_SUPPORT_SCHEME);
    char *url, *host, *path;
    curl_url_get(handle, CURLUPART_URL, &url, 0);
    curl_url_get(handle, CURLUPART_HOST, &host, 0);
    curl_url_get(handle, CURLUPART_PATH, &path, 0);
    printf("%s\n%s\n%s\n", url, host, path);

    curl_free(host);
    curl_free(path);

    curl_url_cleanup(handle);

    return 0;
}