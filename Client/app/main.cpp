#include <Client.h>

int main() {
    silence::Client client("http://127.0.0.1:3000"); // server url
    client.connect();

    return 0;
}