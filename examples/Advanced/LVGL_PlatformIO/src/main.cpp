

#include <lv_demo.h>
#include <lvgl.h>
#include <SD.h>

#ifdef LILYGO_S3CAP
#include "lgfx_lilygo_t_display_s3_cap.h"
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 170
#endif

#ifdef SUNTON7IN
#include "lgfx_user/LGFX_Sunton_ESP32-8048S070.h"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480
#endif

#ifdef CORES3
#define LGFX_AUTODETECT
#include <LGFX_AUTODETECT.hpp>
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#endif

#ifdef CORE2
#define LGFX_USE_V1
#define LGFX_AUTODETECT
#include <M5Core2.h>
#include <LovyanGFX.hpp>      
#include <LGFX_AUTODETECT.hpp> // クラス"LGFX"を準備
// #include <LovyanGFX.hpp>
// // #define LGFX_USE_V0
// #define LGFX_DEFAULT_BOARD board_t::board_M5StackCore2
// // #include <LGFX_AUTODETECT.hpp>
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#endif

#ifdef ATOMS3
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#endif

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[2][SCREEN_WIDTH * 10];

LGFX gfx;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    if (gfx.getStartCount() == 0)
    { // Processing if not yet started
        gfx.startWrite();
    }
    // gfx.pushImage(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::swap565_t *)&color_p->full);
    gfx.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::swap565_t *)&color_p->full);
    lv_disp_flush_ready(disp);
}

void sd_access_sample(void)
{
    if (gfx.getStartCount() > 0)
    { // Free the bus before accessing the SD card
        gfx.endWrite();
    }

    // Something to manipulate the SD card.
    auto file = SD.open("/file");
    file.close();
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;

    data->state = LV_INDEV_STATE_REL;

#if defined(CORES3) || defined(CORE2) || defined(SUNTON7IN)
    if (gfx.getTouch(&touchX, &touchY))
    {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
        Serial.printf("xy %d %d\n", data->point.x, data->point.y);
    }
#endif

#ifdef LILYGO_S3CAP
    lgfx::v1::touch_point_t tp;

    if (gfx.getTouchRaw(&tp, 0))
    {
        data->state = LV_INDEV_STATE_PR;
        data->point.y = tp.x;
        data->point.x = SCREEN_WIDTH - tp.y;
        Serial.printf("xy %d %d\n", data->point.x, data->point.y);
    }
#endif
}

void setup()
{
    Serial.begin(115200);
    gfx.begin();
#ifdef LILYGO_S3CAP
    gfx.setRotation(3);
#endif
    gfx.setBrightness(255);
    Serial.printf("setup\n");

    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf[0], buf[1], SCREEN_WIDTH * 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    lv_demo_benchmark();
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(1);
}
