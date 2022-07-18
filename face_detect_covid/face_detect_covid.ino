#include <TridentTD_LineNotify.h>
#include "esp_camera.h"
#include "fd_forward.h"

#define LINE_TOKEN  "YOUR TOKEN HERE"
#define ssid        "YOUR SSID HERE"
#define password    "YOUR WIFI PASSWORD HERE"

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

static inline mtmn_config_t app_mtmn_config()
{
  mtmn_config.t mtmn_config = {0};
  mtmn_config.type = FAST;
  mtmn_config.min_face = 80;
  mtmn_config.pyramid = 0.707;
  mtmn_config.pyramid_times = 4;
  mtmn_config.p_threshold.score = 0.6;
  mtmn_config.p_threshold.nms = 0.7;
  mtmn_config.p_threshold.candidate_number = 20;
  mtmn_config.r_threshold.score = 0.7;
  mtmn_config.r_threshold.nms = 0.7;
  mtmn_config.r_threshold.candidate_number = 10;
  mtmn_config.o_threshold.score = 0.7;
  mtmn_config.o_threshold.nms = 0.7;
  mtmn_config.o_threshold.candidate_number = 1;
  return mtmn_config;
}
mtmn_config_t mtmn_config = app_mtmn_config();

unsigned long currentTime = millis();
unsigned long oldTime = currentTime;
#define IntIR1Pin     13
bool statusSend = false;

void setup()
{
  pinMode(IntIR1Pin,INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println();
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);
  int connectCount = 0;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    connectCount++;
    delay(500);
    Serial.print(".");
    if (connectCount > 10) ESP.restart();
  }
  Serial.println(""); Serial.println("WiFi connected"); Serial.print("ESP32 CAM Ready! Use IP: ");
  Serial.print(WiFi.localIP()); Serial.println("' to connect");
  LINE.setToken(LINE_TOKEN);
  
}

void loop()
{
  currentTime = millis();
  if((currentTime - oldTime) > 100){
    Serial.println(digitalRead(IntIR1Pin));
    if (digitalRead(IntIR1Pin) == 0){
      detection();
    }
    oldTime = currentTime;
  }
}

void detection(){
  camera_fb_t * fb = NULL;
  dl_matrix3du_t *image_matrix = NULL;
  while (true) {
      fb = esp_camera_fb_get();
      if (!fb) {
          Serial.println("Camera capture failed");
        } else {
          image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
          if (!image_matrix) {
            Serial.println("dl_matrix3du_alloc failed");
          } else {
              if(!fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item)){
                Serial.println("fmt2rgb888 failed");
              } else {
                box_array_t *net_boxes = NULL;
                net_boxes = face_detect(image_matrix, &mtmn_config);
                if(net_boxes){
                  if((float)net_boxes->score[0] > 0.99){
                    Serial.println("Face Detected!!!");
                    LINE.notifyPicture("ไม่ใส่หน้ากากอนามัย!",fb->buf, fb->len);
                    Serial.println("Send Picture To Line...");
                    delay(1000);
                    statusSend = true;
                  }
                  free(net_boxes->score);
                  free(net_boxes->box);
                  free(net_boxes->landmark);
                  free(net_boxes);
                }
                esp_camera_fb_return(fb);
                fb = NULL;
              }
              dl_matrix3du_free(image_matrix);
          }
      }
      if (statusSend == true || digitalRead(IntIR1Pin) == 1){
        statusSend = false;
        break;
      }
  }
}
