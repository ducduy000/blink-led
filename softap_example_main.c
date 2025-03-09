/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //Requires by memset
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <esp_http_server.h>

#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/api.h>
#include <lwip/netdb.h>
/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_WIFI_CHANNEL   CONFIG_ESP_WIFI_CHANNEL
#define EXAMPLE_MAX_STA_CONN       CONFIG_ESP_MAX_STA_CONN
//#define LED_PIN 13
#define LED_PIN1 13
#define LED_PIN2 12
#define LED_PIN3 14

//char on_resp[] = "<!DOCTYPE html><html><head><style type=\"text/css\">html {  font-family: Arial;  display: inline-block;  margin: 0px auto;  text-align: center;}h1{  color: #070812;  padding: 2vh;}.button {  display: inline-block;  background-color: #b30000; //red color  border: none;  border-radius: 4px;  color: white;  padding: 16px 40px;  text-decoration: none;  font-size: 30px;  margin: 2px;  cursor: pointer;}.button2 {  background-color: #364cf4; //blue color}.content {   padding: 50px;}.card-grid {  max-width: 800px;  margin: 0 auto;  display: grid;  grid-gap: 2rem;  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));}.card {  background-color: white;  box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);}.card-title {  font-size: 1.2rem;  font-weight: bold;  color: #034078}</style>  <title>ESP32 WEB SERVER</title>  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">  <link rel=\"icon\" href=\"data:,\">  <link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.7.2/css/all.css\"    integrity=\"sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr\" crossorigin=\"anonymous\">  <link rel=\"stylesheet\" type=\"text/css\" ></head><body>  <h2>ESP32 WEB SERVER</h2>  <div class=\"content\">    <div class=\"card-grid\">      <div class=\"card\">        <p><i class=\"fas fa-lightbulb fa-2x\" style=\"color:#c81919;\"></i>     <strong>GPIO2</strong></p>        <p>GPIO state: <strong> ON</strong></p>        <p>          <a href=\"/led2on\"><button class=\"button\">ON</button></a>          <a href=\"/led2off\"><button class=\"button button2\">OFF</button></a>        </p>      </div>    </div>  </div></body></html>";

//char off_resp[] = "<!DOCTYPE html><html><head><style type=\"text/css\">html {  font-family: Arial;  display: inline-block;  margin: 0px auto;  text-align: center;}h1{  color: #070812;  padding: 2vh;}.button {  display: inline-block;  background-color: #b30000; //red color  border: none;  border-radius: 4px;  color: white;  padding: 16px 40px;  text-decoration: none;  font-size: 30px;  margin: 2px;  cursor: pointer;}.button2 {  background-color: #364cf4; //blue color}.content {   padding: 50px;}.card-grid {  max-width: 800px;  margin: 0 auto;  display: grid;  grid-gap: 2rem;  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));}.card {  background-color: white;  box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);}.card-title {  font-size: 1.2rem;  font-weight: bold;  color: #034078}</style>  <title>ESP32 WEB SERVER</title>  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">  <link rel=\"icon\" href=\"data:,\">  <link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.7.2/css/all.css\"    integrity=\"sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr\" crossorigin=\"anonymous\">  <link rel=\"stylesheet\" type=\"text/css\"></head><body>  <h2>ESP32 WEB SERVER</h2>  <div class=\"content\">    <div class=\"card-grid\">      <div class=\"card\">        <p><i class=\"fas fa-lightbulb fa-2x\" style=\"color:#c81919;\"></i>     <strong>GPIO2</strong></p>        <p>GPIO state: <strong> OFF</strong></p>        <p>          <a href=\"/led2on\"><button class=\"button\">ON</button></a>          <a href=\"/led2off\"><button class=\"button button2\">OFF</button></a>        </p>      </div>    </div>  </div></body></html>";

char on_resp[]="<!DOCTYPE html>"
"<html>"
"<head>"
  "<style>"
    "html { font-family: Arial; text-align: center; }"
    "h2 { color: #070812; }"
    ".button { "
      "background-color: #b30000;"
      "border: none;"
      "border-radius: 4px;"
      "color: white;"
      "padding: 16px 40px;"
     " font-size: 30px;"
      "margin: 2px;"
      "cursor: pointer;"
    "}"
    ".button2 { background-color: #364cf4; }"
    ".content { padding: 50px; }"
    ".card { "
      "background-color: white;"
      "box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);"
   " }"
    ".card-title { "
     " font-size: 1.2rem;"
      "font-weight: bold;"
      "color: #034078;"
    "}"
  "</style>"
  "<title>ESP32 WEB SERVER</title>"
  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
"</head>"
"<body>"
  "<h2>ESP32 WEB SERVER</h2>"
 " <div class=\"content\">"
    "<div class=\"card\">"
      "<p><strong>LED 1</strong></p>"
      
      "<p>"
       " <a href=\"/led1on\"><button class=\"button\">ON</button></a>"
        "<a href=\"/led1off\"><button class=\"button button2\">OFF</button></a>"
      "</p>"
    "</div>"
    "<div class=\"card\">"

      "<p><strong>LED 2</strong></p>"
      
      "<p>"
        "<a href=\"/led2on\"><button class=\"button\">ON</button></a>"
        "<a href=\"/led2off\"><button class=\"button button2\">OFF</button></a>"
      "</p>"
    "</div>"
    "<div class=\"card\">"
      "<p><strong>LED 3</strong></p>"
      
      "<p>"
        "<a href=\"/led3on\"><button class=\"button\">ON</button></a>"
        "<a href=\"/led3off\"><button class=\"button button2\">OFF</button></a>"
      "</p>"
    "</div>"
    "<div class=\"card\">"
      "<p><strong>All led</strong></p>"
      
      "<p>"
        "<a href=\"/led4on\"><button class=\"button\">ON</button></a>"
        "<a href=\"/led4off\"><button class=\"button button2\">OFF</button></a>"
      "</p>"
    "</div>"
    "<div class=\"card\">"
      "<p><strong>Blink led</strong></p>"
      
      "<p>"
        "<a href=\"/led5on\"><button class=\"button\">ON</button></a>"
        "<a href=\"/led5off\"><button class=\"button button2\">OFF</button></a>"
      "</p>"
    "</div>"
  "</div>"
"</body>"
"</html>";
static const char *TAG = "espressif"; // TAG for debug
int led_state1 = 0;
int led_state2 = 0;
int led_state3 = 0;
int loop=0;
//int led_state = 0;
//static const char *TAG = "wifi softAP";
TaskHandle_t led_blink_task_handle = NULL;  // Task handle

void led_blink_task(void *pvParameter) {
    int ON = 0;
    while (1) {  // Chạy vô hạn, nhưng có thể bị xóa
        ON = !ON;
        gpio_set_level(LED_PIN1, ON);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN2, ON);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN3, ON);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
            .authmode = WIFI_AUTH_WPA3_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */
            .authmode = WIFI_AUTH_WPA2_PSK,
#endif
            .pmf_cfg = {
                    .required = true,
            },
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}

esp_err_t favicon_handler(httpd_req_t *req) {
    httpd_resp_send(req, "", 0); // Trả về phản hồi rỗng (204 No Content)
    return ESP_OK;
}

// Đăng ký handler trong server
httpd_uri_t uri_favicon = {
    .uri = "/favicon.ico",
    .method = HTTP_GET,
    .handler = favicon_handler,
    .user_ctx = NULL
};
esp_err_t send_web_page(httpd_req_t *req)
{
    int response;
    
        
    
        response = httpd_resp_send(req, on_resp, HTTPD_RESP_USE_STRLEN);
    return response;
}
esp_err_t get_req_handler(httpd_req_t *req)
{
    return send_web_page(req);
}

esp_err_t led1_on_handler(httpd_req_t *req)
{
    gpio_set_level(LED_PIN1, 1);
    led_state1 = 1;
    return send_web_page(req);
}

esp_err_t led1_off_handler(httpd_req_t *req)
{
    gpio_set_level(LED_PIN1, 0);
    led_state1 = 0;
    return send_web_page(req);
}

esp_err_t led2_on_handler(httpd_req_t *req)
{
    gpio_set_level(LED_PIN2, 1);
    led_state2 = 1;
    return send_web_page(req);
}

esp_err_t led2_off_handler(httpd_req_t *req)
{
    gpio_set_level(LED_PIN2, 0);
    led_state2 = 0;
    return send_web_page(req);
}

esp_err_t led3_on_handler(httpd_req_t *req)
{
    gpio_set_level(LED_PIN3, 1);
    led_state3 = 1;
    return send_web_page(req);
}

esp_err_t led3_off_handler(httpd_req_t *req)
{
    gpio_set_level(LED_PIN3, 0);
    led_state3 = 0;
    return send_web_page(req);
}

esp_err_t led4_on_handler(httpd_req_t *req)
{
	gpio_set_level(LED_PIN1, 1);
	gpio_set_level(LED_PIN2, 1);
    gpio_set_level(LED_PIN3, 1);
    led_state1 = 1;
    led_state2 = 1;
    led_state3 = 1;
    return send_web_page(req);
}

esp_err_t led4_off_handler(httpd_req_t *req)
{
    gpio_set_level(LED_PIN1, 0);
    gpio_set_level(LED_PIN2, 0);
    gpio_set_level(LED_PIN3, 0);
    led_state1 = 0;
    led_state2 = 0;
    led_state3 = 0;
    return send_web_page(req);
}

esp_err_t led5_on_handler(httpd_req_t *req)
{
    if (led_blink_task_handle == NULL) {  // Nếu chưa chạy task thì mới tạo
        xTaskCreate(led_blink_task, "led_blink_task", 2048, NULL, 5, &led_blink_task_handle);
    }
    return send_web_page(req);
}

esp_err_t led5_off_handler(httpd_req_t *req)
{
    if (led_blink_task_handle != NULL) {
        vTaskDelete(led_blink_task_handle);  // Xóa task
        led_blink_task_handle = NULL;
    }
    gpio_set_level(LED_PIN1, 0);
    gpio_set_level(LED_PIN2, 0);
    gpio_set_level(LED_PIN3, 0);

    return send_web_page(req);
    
    
}
httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_req_handler,
    .user_ctx = NULL};

httpd_uri_t uri1_on = {
    .uri = "/led1on",
    .method = HTTP_GET,
    .handler = led1_on_handler,
    .user_ctx = NULL};

httpd_uri_t uri1_off = {
    .uri = "/led1off",
    .method = HTTP_GET,
    .handler = led1_off_handler,
    .user_ctx = NULL};
    
httpd_uri_t uri2_on = {
    .uri = "/led2on",
    .method = HTTP_GET,
    .handler = led2_on_handler,
    .user_ctx = NULL};

httpd_uri_t uri2_off = {
    .uri = "/led2off",
    .method = HTTP_GET,
    .handler = led2_off_handler,
    .user_ctx = NULL};
    
httpd_uri_t uri3_on = {
    .uri = "/led3on",
    .method = HTTP_GET,
    .handler = led3_on_handler,
    .user_ctx = NULL};

httpd_uri_t uri3_off = {
    .uri = "/led3off",
    .method = HTTP_GET,
    .handler = led3_off_handler,
    .user_ctx = NULL};
    
httpd_uri_t uri4_on = {
    .uri = "/led4on",
    .method = HTTP_GET,
    .handler = led4_on_handler,
    .user_ctx = NULL};

httpd_uri_t uri4_off = {
    .uri = "/led4off",
    .method = HTTP_GET,
    .handler = led4_off_handler,
    .user_ctx = NULL}; 
    
httpd_uri_t uri5_on = {
    .uri = "/led5on",
    .method = HTTP_GET,
    .handler = led5_on_handler,
    .user_ctx = NULL};

httpd_uri_t uri5_off = {
    .uri = "/led5off",
    .method = HTTP_GET,
    .handler = led5_off_handler,
    .user_ctx = NULL};    
       

httpd_handle_t setup_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 16;
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri1_on);
        httpd_register_uri_handler(server, &uri1_off);
        httpd_register_uri_handler(server, &uri2_on);
        httpd_register_uri_handler(server, &uri2_off);
        httpd_register_uri_handler(server, &uri3_on);
        httpd_register_uri_handler(server, &uri3_off);
        httpd_register_uri_handler(server, &uri4_on);
        httpd_register_uri_handler(server, &uri4_off);
        httpd_register_uri_handler(server, &uri5_on);
        httpd_register_uri_handler(server, &uri5_off);
        httpd_register_uri_handler(server, &uri_favicon);
    }

    return server;
}
void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    wifi_init_softap();
    esp_rom_gpio_pad_select_gpio(LED_PIN1);
    gpio_set_direction(LED_PIN1, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(LED_PIN2);
    gpio_set_direction(LED_PIN2, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(LED_PIN3);
    gpio_set_direction(LED_PIN3, GPIO_MODE_OUTPUT);

    led_state1 = 0;
    led_state2 = 0;
    led_state3 = 0;
    ESP_LOGI(TAG, "LED Control Web Server is running ... ...\n");
    setup_server();
}
