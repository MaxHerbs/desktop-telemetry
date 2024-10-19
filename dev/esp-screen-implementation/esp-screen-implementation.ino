#include <TFT_eSPI.h> // Include the graphics library (this will include the driver for your display)
#include <lvgl.h>     // Include LVGL library

TFT_eSPI tft = TFT_eSPI(); // Create TFT object

void setup() {
  // Initialize the display
  tft.begin();
  tft.setRotation(1); // Adjust rotation if needed
  tft.fillScreen(TFT_BLACK); // Clear the screen

  // Initialize LVGL
  lv_init();

  // Initialize the display driver for LVGL
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.flush_cb = my_disp_flush; // Set the display flush function
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  // Create a simple label
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello, World!");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0); // Center the label on the screen
}

void loop() {
  lv_task_handler(); // Handle LVGL tasks
  delay(5);          // Small delay
}

// Display flush function for LVGL
static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  // Call the TFT_eSPI library to flush the area
  tft.startWrite(); // Start the write operation
  tft.setAddrWindow(area->x1, area->y1, area->x2, area->y2); // Set the window
  tft.pushColors(&color_p->full, area->x2 - area->x1 + 1); // Push the colors to the display
  tft.endWrite(); // End the write operation

  lv_disp_flush_ready(disp); // Tell LVGL that flushing is done
}
