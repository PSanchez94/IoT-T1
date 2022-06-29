/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "addr_from_stdin.h"


#if defined(CONFIG_EXAMPLE_IPV4)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
#elif defined(CONFIG_EXAMPLE_IPV6)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV6_ADDR
#else
#define HOST_IP_ADDR ""
#endif

#define PORT CONFIG_EXAMPLE_PORT


static const char *TAG = "example";



#include "noise_packet.h"



/*
typedef struct Data4 {
    int32_t x_acc[1600];
    int32_t y_acc[1600];
    int32_t z_acc[1600];
};

typedef struct Packet4 {
    struct Header header;

    struct Data0 data0;
    struct Data1 data1;

    struct Data4 data4;
}
*/

struct Header header;
char payload[sizeof(header)];


static void udp_client_task(void *pvParameters) {

    // Received message
    char rx_buffer[4];


    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;


    header.id_device = INT16_MAX;
    header.mac = INT64_MAX;
    header.id_protocol = INT8_MAX;
    header.len = 19216;

    // Socket loop
    while (1) {


// idf.py menuconfig imports
#if defined(CONFIG_EXAMPLE_IPV4)
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
#elif defined(CONFIG_EXAMPLE_IPV6)
        struct sockaddr_in6 dest_addr = { 0 };
        inet6_aton(HOST_IP_ADDR, &dest_addr.sin6_addr);
        dest_addr.sin6_family = AF_INET6;
        dest_addr.sin6_port = htons(PORT);
        dest_addr.sin6_scope_id = esp_netif_get_netif_impl_index(EXAMPLE_INTERFACE);
        addr_family = AF_INET6;
        ip_protocol = IPPROTO_IPV6;
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
        struct sockaddr_storage dest_addr = { 0 };
        ESP_ERROR_CHECK(get_addr_from_stdin(PORT, SOCK_DGRAM, &ip_protocol, &addr_family, &dest_addr));
#endif


        // Socket definition
        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created, sending to %s:%d", HOST_IP_ADDR, PORT);


        // Packet loop
        while (1) {
            struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
            socklen_t socklen = sizeof(source_addr);


            memcpy ( &payload, &header, sizeof(header) );


            // Send packet
            int err = sendto(sock, payload, strlen(payload), 0, (struct sockaddr *) &dest_addr, sizeof(dest_addr));
            // Err handling
            if (err < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                break;
            }
            ESP_LOGI(TAG, "Message sent");


            // Receive  packet
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            // Err handling
            if (len < 0) {
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                rx_buffer[len] = 0; // Null-termination
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
                ESP_LOGI(TAG, "%s", rx_buffer);
                if (rx_buffer[0] == '1') {
                    ESP_LOGI(TAG, "Received expected message, reconnecting");
                    break;
                }
            }


            // Packet delay
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }


        // Shut down socket
        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }


    vTaskDelete(NULL);
}




void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    xTaskCreate(udp_client_task, "udp_client", 4096, NULL, 5, NULL);
}
