#include "ESP8266WiFi.h"
extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "smartconfig.h"
#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/dns.h"
    //#include "user_config.h"
}

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1
#define CHANNEL_HOP_INTERVAL   5000
#define MAC_EXPIRY_TIME   60000
#define NUM_MACS 100

#define DEBUG true
#define PRINT_ON_DISCOVERY true
#define PRINT_IN_TABLE true

//os_event_t    user_procTaskQueue[user_procTaskQueueLen];
static os_timer_t channelHop_timer;

//static void loop(os_event_t *events);
static void promisc_cb(uint8 *buf, uint16 len);

#define printmac(buf, i) Serial.print(buf[i+0],HEX); Serial.print(":"); Serial.print(buf[i+1],HEX); Serial.print(":"); Serial.print(buf[i+2],HEX); Serial.print(":"); Serial.print(buf[i+3],HEX); Serial.print(":"); Serial.print(":"); Serial.print(buf[i+4],HEX); Serial.print(":"); Serial.print(buf[i+5],HEX);

const char* hex ="0123456789ABCDEF";
const char* search_string="c48e";

byte mac_table[NUM_MACS][6];
unsigned long last_seen[NUM_MACS];

int lastTime=0;
const byte zeros[6]={0x00,0x00,0x00,0x00,0x00,0x00}; 
const byte fs[6]=   {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}; 

bool ArrayCompare(byte *arr1, byte *arr2, int len){
    for(int i=0; i<len; i++){
        if(arr1[i]!=arr2[i]){
            return false;
        }
    } 
    return true;
}

void PrintHex83(uint8_t *data, uint8_t start, uint8_t length) // prints 8-bit data in hex
{
    char tmp[length*2+1];
    byte first;
    int j=0;
    bool string_found=false;
    int k=0;
    for (uint8_t i=0; i<length; i++) 
    {
        tmp[j]=hex[data[i+start]>>4];
        j++;
        tmp[j]=hex[(data[i+start]&0x0F)];
        j++;
    }
    tmp[length*2] = 0;
    Serial.print(tmp);
}
void PrintMac(uint8_t *data, uint8_t start, uint8_t length) // prints 8-bit data in hex
{
    char tmp[length*3];
    byte first;
    int j=0;
    for (uint8_t i=0; i<length; i++) 
    {
        tmp[j]=hex[data[i+start]>>4];
        j++;
        tmp[j]=hex[(data[i+start]&0x0F)];
        j++;
        tmp[j]=':';
        j++;
    }
    tmp[length*3-1] = 0;
    Serial.print(tmp);
}

void channelHop()
{
    // 1 - 13 channel hopping
    uint8 new_channel = wifi_get_channel() % 12 + 1;
    wifi_set_channel(new_channel);
    Serial.print("jumping to channel: ");
    Serial.println(new_channel);
}

//look for 802.11 frames with 
static void ICACHE_FLASH_ATTR promisc_cb(uint8 *buf, uint16 len)
{
    
#define ADDR1 16
#define ADDR2 22
#define ADDR3 28
#define ADDR4 36
    uint8_t addresses[3]={ADDR1,ADDR2,ADDR3};
    for(int i=0; i<3;i++){ 
        bool notZeroes= (1-ArrayCompare((buf+addresses[i]), (byte*) zeros, 6)); 
        bool notFs=     (1-ArrayCompare((buf+addresses[i]), (byte*) fs, 6));
        if(notZeroes && notFs){ 
            //Serial.print("\t");
            //PrintMac(buf, addresses[i], 6);
            //Serial.print(" ");
            //Serial.print(notZeroes);
            //Serial.print(" ");
            //Serial.print(notFs);
            //Serial.print("\n");
            bool inTable=false;
            uint8_t toReplace=0;
            int time;
            int maxTime=0;
            byte max_index=0;
            //find a MAC which has not been seen recently to evict, or a space in the table 
            for(int j=0; j<NUM_MACS;j++){
                //mac_table[j]==[current mac]
                if( ArrayCompare((buf+addresses[i]), mac_table[j], 6) ){
                    //update the last seen time if the mac is already in the table
                    inTable=true;
                    last_seen[j]=millis();
                }
                //the least recently seen mac
                if(millis()-last_seen[j]>maxTime){
                    maxTime=(millis()-last_seen[j]);
                    toReplace=j;
                }
            }
            //if(inTable){Serial.println("already in table");}
            if(!inTable){
                memcpy( (&mac_table[toReplace]), (buf+addresses[i]), 6);
                last_seen[toReplace]=millis();
                //Serial.print("inserting new mac:");
                #ifdef PRINT_ON_DISCOVERY
                PrintMac(buf, addresses[i], 6);
                Serial.print("\n");
                #endif 
                ////copy the mac address into the mac_table
                //Serial.print("inserted ");
                //PrintMac( (byte*) (&mac_table[toReplace]) ,0, 6);
                //Serial.print(" at ");
                //Serial.print(last_seen[toReplace]);
                //Serial.print(" time now is ");
                //Serial.print(millis());
                //Serial.print(" inserted at index ");
                //Serial.print(toReplace);
                //Serial.print("\n");
            }
        }
    }
    if((millis()-lastTime) > 5000){
        channelHop();
        lastTime=millis();
        Serial.println("macs:");
        for(int i=0; i<NUM_MACS;i++){
            //if [the value has not expired] and [the value has been entered]
            if((millis() - (last_seen[i])) > MAC_EXPIRY_TIME){
                last_seen[i]=0;
            }
            #ifdef PRINT_IN_TABLE
            if( (last_seen[i] != 0) ){
                Serial.print("\t");
                PrintMac(mac_table[i], 0, 6);
                Serial.print("\t");
                Serial.print("last seen: ");
                Serial.print((millis()-last_seen[i])/1000);
                Serial.print("\t seconds ago\n");
                Serial.print("\tat");
                Serial.println(last_seen[i]);
            }
            #endif
        }
    }
}

//Init function 
void setup ()
{
    Serial.begin(115200);
    delayMicroseconds(100);

    Serial.println("*** Monitor mode test ***");

    Serial.print(" -> Promisc mode setup ... ");
    wifi_set_promiscuous_rx_cb(promisc_cb);
    wifi_promiscuous_enable(1);
    Serial.println("done.");

    //Serial.print(" -> Timer setup ... ");
    //os_timer_disarm(&channelHop_timer);
    //os_timer_setfn(&channelHop_timer, (os_timer_func_t *) channelHop, NULL);
    //os_timer_arm(&channelHop_timer, CHANNEL_HOP_INTERVAL, 1);
    //Serial.println("done.\n");

    Serial.print(" -> Set opmode ... ");
    wifi_set_opmode( 0x1 );
    Serial.println("done.");

    //Start os task
    //system_os_task(loop, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen);

    Serial.println(" -> Init finished!");

    //make sure the table of macs is cleared
    for(int i=0;i<NUM_MACS;i++){
        last_seen[i]=0;
    }
}

void loop() {
    delay(10);
}
