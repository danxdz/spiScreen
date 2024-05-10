#include "Arduino.h"
#include "oled.h"
#include "xbmStrings.h"
#include "uart.h"
#include "crsf.h"
#include "menus.h"
#include "rx_params.h"

#include "Adafruit_GC9A01A.h"

#define DEBUG


//U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE, 22, 21); // todo define pins
Adafruit_GC9A01A display(display_CS, display_DC);

Oled oled;

void Oled::PrintLoad(char *tmp)
{
    dbout.printf("PrintLoad\n");
    
    display.setCursor(0, 20);
    dbout.println(tmp);
    Oled::Println(tmp);
}


// with y , size default 1, color default white
void Oled::PrintCenter(char *tmp,uint8_t y, uint8_t size , uint16_t color , uint16_t bgcolor)
{
    display.setCursor(50, y);
    display.setTextColor(color, bgcolor);
    display.setTextSize(size);
    display.println(tmp);
}

void Oled::PrintRight(char *tmp)
{
    //uint8_t x = display.getDisplayWidth() - display.getStrWidth(tmp);
    //display.drawStr(x, display.getCursorY(), tmp);
}
// with y
void Oled::PrintRight(uint8_t y, char *tmp)
{
    //uint8_t x = display.getDisplayWidth() - (display.getStrWidth(tmp));
    //display.drawStr(x, y, tmp);
}

void Oled::PrintLeft(char *tmp)
{
    //display.drawStr(0, display.getCursorY(), tmp);
}
// with y
void Oled::PrintLeft(uint8_t y, char *tmp)
{
    //display.drawStr(0, y, tmp);
}

void Oled::Init()
{
#if defined(DEBUG)
    dbout.printf("starting screen...\n");
#endif

    dbout.println("GC9A01A Starting!");
  


    display.begin();
    oled.testFillScreen();

    #if defined(display_BL)
        pinMode(display_BL, OUTPUT);
        digitalWrite(display_BL, HIGH); // Backlight on
    #endif // end display_BL

    Oled::PrintCenter((char *)"simpleTX",7);

    //display.drawXBM(48, 15, 32, 32, elrs32);

    Oled::PrintCenter((char *)"ExpressLRS",58);
 
    delay(1000);
}


void Oled::testFillScreen() {
  unsigned long start = micros();
  display.fillScreen(GC9A01A_BLACK);
  delay(100);
  yield();
  display.fillScreen(GC9A01A_RED);
  delay(100);
  yield();
  display.fillScreen(GC9A01A_GREEN);
  delay(100);
  yield();
  display.fillScreen(GC9A01A_BLUE);
  delay(100);
  yield();
  display.fillScreen(GC9A01A_BLACK);
  delay(100);
  yield();
}
void ICACHE_RAM_ATTR Oled::printText(rc_input_t rcInput) {
    //display.fillScreen(GC9A01A_BLACK);
    unsigned long start = micros();
 
    display.setTextColor(GC9A01A_WHITE, GC9A01A_BLACK);
    //display.fillRoundRect(0, 0, 400, 150, 1, GC9A01A_BLACK);

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("      ");
    display.println("      ");
    display.println("      ");
    String spc = "        ";

    String str = spc + String(rcInput.elevator) + spc;    
    display.println(str);
    str = spc + String(rcInput.aileron) + spc;
    display.println(str);
    str = spc + String(rcInput.throttle) + spc;
    display.println(str);
    str = spc + String(rcInput.rudder) + spc;
    display.println(str);

}


void Oled::Clear()
{
display.fillScreen(GC9A01A_YELLOW);
delay(100);
display.fillScreen(GC9A01A_BLACK);

if (!debugEnabled) dbout.printf("clear screen\n");

   
}

void Oled::setFont(const uint8_t font[])
{
        //display.setFont(font);

}


void ICACHE_RAM_ATTR Oled::clearProgressBar(int x, int y, int width, int height) {
   
    // clear progress bar
    //display.setDrawColor(0);
    //display.drawBox(x+1, y+1, width-2, height-2);
    //display.sendBuffer();

}


void ICACHE_RAM_ATTR Oled::drawProgressBarFrames(int x, int y, int width, int height) {

    //display.setDrawColor(1);
    //display.drawFrame(x, y, width, height);
    //display.sendBuffer();
       
}


void ICACHE_RAM_ATTR Oled::drawProgressBar(int x, int y, int width, int height, int value, int maxValue, int minValue, int centerValue) {

    //display.setDrawColor(1);
    int barFillHeight = map(value, minValue, maxValue, 0, height); 
    dbout.printf("bssdfsdr: x%i y%i w%i h%i v%i mv%i mi%i cv%i bFH%i\n", x, y, width, height, value, maxValue, minValue, centerValue, barFillHeight);
    

 
    display.fillRect(x-2, y-3, width+4, height+56, GC9A01A_BLACK);

    display.setTextColor(GC9A01A_DARKGREEN);

    display.setCursor(x-50, 175);
    //print value / const char 
    String spc = "        ";
    display.println(spc + value);

    display.drawLine(x-3,y-3,x+width+6,y-3, GC9A01A_WHITE);  // top line
    display.drawLine(x, y+ height/2, x + width-1, y+ height/2, GC9A01A_RED); // center line
    display.drawLine(x-3,y+height+9,x+width+6,y+height+9, GC9A01A_WHITE);  // bottom line
      
    

    
    display.drawLine(x, y + height - barFillHeight, x + width, y + height - barFillHeight,GC9A01A_WHITE);
    display.drawLine(x, y + height - barFillHeight+1, x + width, y + height - barFillHeight+1,GC9A01A_WHITE);
    display.drawLine(x, y + height - barFillHeight+2, x + width, y + height - barFillHeight+2,GC9A01A_WHITE);
    
       
}



void Oled::setMainScreen(char *name, crsfLinkStatistics_t LinkStatistics, uint8_t bpkts, uint8_t gpkts)
{
    //dbout.printf("Mainsreen %s:%u:%u\n", name, bpkts, gpkts); 

    char output[400];

    display.setCursor(0, 7);

    if (crsf_devices[0].address != 0)
    {

        Oled::PrintLeft(7, name);

        snprintf(output, sizeof output, "%u:%u", bpkts, gpkts);
        Oled::PrintRight(output);

        if (crsf_devices[1].address != 0)
        {

            snprintf(output, sizeof output, "%u:%u | %i dBm  \n",
                     LinkStatistics.rf_Mode,
                     LinkStatistics.uplink_Link_quality,
                     LinkStatistics.uplink_RSSI_1);
            Oled::Println(output);

            //display.setFont(u8g2_font_10x20_mr);
            // display.setCursor(0,32);
            float vBat = 5; // (float)batteryVoltage.voltage/10;
            char bat[64];

            int ret = snprintf(bat, sizeof bat, "%.2f", vBat);
            Oled::Println((char *)"");
            Oled::Println((char *)"");
            Oled::Println(bat);

            //display.setFont(u8g2_font_chikita_tr);

            Oled::Println(crsf_devices[1].name);

            snprintf(output, sizeof output, "%u:%u | %i dBm",
                     LinkStatistics.rf_Mode,
                     LinkStatistics.downlink_Link_quality,
                     LinkStatistics.downlink_RSSI);
            Oled::Println(output);

            snprintf(output, sizeof output, "%u mW", LinkStatistics.uplink_TX_Power);
            Oled::PrintRight(output);
        }
        else
        {
            //display.setFont(u8g2_font_10x20_mr);
            Oled::PrintCenter((char *)"no rx",45);
        }
    } else
        {
            //display.setFont(u8g2_font_10x20_mr);
            Oled::PrintCenter((char *)"no tx module",45);
        }
    //display.sendBuffer();
}

void Oled::setSubMenuItems()
{
    // dbout.printf("Subscreen\n");
    //display.clearBuffer();
    //display.setFont(u8g2_font_profont11_tr);
    display.setCursor(0, 7);
    display.print(crsf_devices[0].name);
    display.setCursor(0, 15);
    display.print(menuItems[selected].name);

    // debug submenus options
#if !defined(DEBUG)
    display.print("CRSF settings");
#else
    display.print(":");
    display.print(selected);
    display.print(":");
    display.print(subSelected);
#endif

    int submenu_item_num = 0;
    for (size_t i = 0; menuItems[i].name; i++)
    {
        if (menuItems[i].parent == menuItems[selected].id)
            submenu_item_num++;
    }

    if (subSelected > submenu_item_num + selected)
        subSelected = selected + 1;
    if (subSelected < selected + 1)
        subSelected = selected + submenu_item_num;

    for (size_t i = 0; i < crsf_devices[0].number_of_params; i++)
    {
        if (menuItems[i].parent == menuItems[selected].id)
        {
            // dbout.printf("%s:%u : \n ",menuItems[i].name,menuItems[i].id);

            display.setCursor(0, display.getCursorY() + 9);
            if (i == subSelected)
                display.print("> ");

            display.print(menuItems[i].name);
            // dbout.printf("len! %u;%u\n",len,  display.getDisplayWidth());
            // display.setFont(u8g2_font_profont10_mr);
            if (menuItems[i].optionsMainMenu[menuItems[i].status])
                Oled::PrintRight(menuItems[i].optionsMainMenu[menuItems[i].status]);
        }
    }

    //display.sendBuffer();
}

void Oled::setMainMenuItems()
{
    /*  dbout.printf("Mainsreen: %s:%i:%s\n",
        menuItems[selected].name,
        menuItems[selected].status,
        menuItems[selected].getMainMenuItem_StatusText());
  */
    //display.clearBuffer();
    //display.setFont(u8g2_font_profont11_tr);
    //display.drawStr(0, 7, crsf_devices[0].name);

    display.setCursor(0, 15);

    int menu_item_num = 0;
    int submenu_item_num = 0;
    for (size_t i = 0; i < crsf_devices[0].number_of_params - 2; i++)
    {
        if (menuItems[i].parent == 0)
        {
            menu_item_num++;
        }
        else
            submenu_item_num++;
        /*  dbout.printf("menu_item_num:%u:%u:%u\n",
                         menu_item_num,
                         submenu_item_num,
                         selected); */
    }

    // set line number that fits oled 128x64
    int num_lines = 5;//display.getRows() - 3;

    int start = (menu_item_num / num_lines) * num_lines;
    if (start > selected)
        start = start - num_lines;

// debug main menu options
#if !defined(DEBUG)
    display.print("CRSF settings");
#else
    display.print(selected);
    display.print(":");
    display.print(start);
    display.print(":");
    display.print(menu_item_num);
    // dbout.printf("selected:%u:s:%u:num:%u\n",
    // selected,start,menu_item_num);
#endif

    int i = start;
    int count = 0;
    do
    {
        //display.setFont(u8g2_font_profont11_tr);

        if ((menuItems[i].parent == 0) && (menuItems[i].p_type != 12))
        {
            count++;
            char *tmp = menuItems[i].name;
            display.setCursor(0, display.getCursorY() + 9);
            if (i == selected)
                display.print("> ");

            display.print(tmp);

            // text selection - items w/ menu
            if (menuItems[i].p_type == 9)
            {
                // int len = display.getDisplayWidth() - (strlen(menuItems[i].getMainMenuItem_StatusText())*6);
                // dbout.printf("len! %u;%u\n",len,  display.getDisplayWidth());
                Oled::PrintRight(menuItems[i].getMainMenuItem_StatusText());
            }
            display.print("\n");
        }
        i++;
    } while (count < num_lines);
    //display.sendBuffer();
}

void Oled::Println(char *tmp)
{
    display.setCursor(0, display.getCursorY() + 8);
    display.print(tmp);
}

void Oled::selectOptionMainMenu()
{
    // db_out.printf("selectOptionMainMenu\n");
    /* db_out.printf("Mainsreen: %s:%i:%s\n",
      menuItems[selected].name,
      menuItems[selected].status,
      menuItems[selected].getMainMenuItem_StatusText());
*/
    ///display.clearBuffer();
    //display.setFont(u8g2_font_profont11_tr);
    //display.drawStr(0, 7, crsf_devices[0].name);

    display.setCursor(0, 15);

    int menu_item_num = 0;
    int submenu_item_num = 0;
    for (size_t i = 0; i < crsf_devices[0].number_of_params - 2; i++)
    {
        if (menuItems[i].parent == 0)
        {
            menu_item_num++;
        }
        else
            submenu_item_num++;
        /*  db_out.printf("menu_item_num:%u:%u:%u\n",
                         menu_item_num,
                         submenu_item_num,
                         selected); */
    }

    // set line number that fits oled 128x64
    int num_lines = 5;//display.getRows() - 3;

    int start = (menu_item_num / num_lines) * num_lines;
    if (start > selected)
        start = start - num_lines;

// debug main menu options
#if !defined(DEBUG)
    display.print("CRSF settings");
#else
    display.print(selected);
    display.print(":");
    display.print(start);
    display.print(":");
    display.print(menu_item_num);
    // db_out.printf("selected:%u:s:%u:num:%u\n",
    // selected,start,menu_item_num);
#endif

    int i = start;
    int count = 0;
    do
    {
        //display.setFont(u8g2_font_profont11_tr);

        if ((menuItems[i].parent == 0) && (menuItems[i].p_type != 12))
        {
            count++;
            char *tmp = menuItems[i].name;
            display.setCursor(0, display.getCursorY() + 9);

            display.print(tmp);

            // text selection - items w/ menu
            if (menuItems[i].p_type == 9)
            {
                // int len = display.getDisplayWidth() - (strlen(menuItems[i].getMainMenuItem_StatusText())*6);
                // db_out.printf("selected: %i;%i\n",selected,i);
                if (i == selected)
                {
                    char output[20];

                    if (menuItems[i].optionsMainMenu[mmOptionSelected])
                    {
                        snprintf(output, sizeof output, "<%s>",
                                 menuItems[i].optionsMainMenu[mmOptionSelected]);
                        //  menuItems[i].getMainMenuItem_StatusText());
                        Oled::PrintRight(output);
                    }
                }
                else
                    Oled::PrintRight(menuItems[i].getMainMenuItem_StatusText());
            }
            display.print("\n");
        }
        i++;
    } while (count < num_lines);
    //display.sendBuffer();
}
