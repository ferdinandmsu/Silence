#include <Client.h>
#include <iostream>

int main()
{
    silence::Client client("http://localhost:3000/");
    client.connect();

    return 0;
}