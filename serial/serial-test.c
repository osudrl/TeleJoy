#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <stdbool.h>

#include "cpTime.h"
#include <pthread.h>
#include "../joy/jt-constants.h"

// #define SERIAL_SHOULD_READ

// #define SERIAL_TEST_COUNTING
// #define SERIAL_TEST_ESCAPING
// #define SERIAL_TEST_SQAURES
#define SERIAL_TEST_SDLSTATES

FILE* output;

#if defined(SERIAL_TEST_SDLSTATES)
    enum _relative_positions_
    {
        RP_NONE, RP_ZERO, RP_RR, RP_R, RP_L, RP_LL
    };
    typedef enum _relative_positions_ rp_t;

    struct _aaxis_t
    {
        uint8_t id;
        rp_t st[5] /*= {RP_NONE}*/;
        double raw;
        double percent;
        uint8_t tdata;
    };
    typedef struct _aaxis_t aaxis_t;

    aaxis_t axes[16];

    SDL_Joystick* get_joystick()
    {

        const int n = SDL_NumJoysticks();
        char name[] = "Teensyduino Serial/Keyboard/Mouse/Joystick";
        for (int i = 0; i < n; ++i) {
            if (strncmp(SDL_JoystickNameForIndex(i), name, sizeof name) == 0)
                return SDL_JoystickOpen(i);
        }
        return NULL;
    }

    void* joy_read()
    {
        printf("About to init JOY \n");
        SDL_Init(SDL_INIT_JOYSTICK);
        printf("JOY was properly init\n");
        SDL_Joystick* ctrl = get_joystick();
        
        //double radio_input[16] = { 0 };

        for (int i = 0; i < 16; i++)    
            for (int s = 0; s < 5; s++)
                axes[i].st[s] = RP_NONE;

        while (ctrl) 
        {
            SDL_JoystickUpdate();
            for (int i = 0; i < 16; i++) 
            {
                axes[i].raw = SDL_JoystickGetAxis(ctrl, i);
                axes[i].percent = axes[i].raw  / 32768.0;
                axes[i].id = i;

                rp_t addTo = RP_NONE;
                if(axes[i].percent < 0.2 && axes[i].percent > -0.2)
                    addTo = RP_ZERO;
                else if(axes[i].percent > 0.8)
                    addTo = RP_RR;
                else if(axes[i].percent > 0)
                    addTo = RP_R;
                else if(axes[i].percent < -0.8)
                    addTo = RP_LL;
                else if(axes[i].percent < 0)
                    addTo = RP_L;
    
                bool foundCurrSymbol = false;
                bool broke = false;
                for (int s = 0; s < 5; s++)
                {   
                    if(axes[i].st[s] == addTo)
                        foundCurrSymbol = true;
                    if(axes[i].st[s] == RP_NONE)
                    {
                        axes[i].tdata = s;
                        broke = true;
                        if(!foundCurrSymbol)
                            axes[i].st[s] = addTo;
                        break;
                    }
                }
                if(!broke)
                    axes[i].tdata = 5;
            }
        }
        SDL_Quit();
        printf("k done\n");
        return (void*) 0;
    }

#endif
#if defined(SERIAL_SHOULD_READ)
void* serial_read()
{
    FILE* input;
    char text[10000];
    input = fopen("/dev/ttyACM1", "r");
    int index = 0;

    while (true || cpMillis() < 60000) {
        char ch = getc(input);

        if (ch == EOF)
            continue;
        if (ch != '\n')
            text[index++] = ch;
        else {
            text[index] = '\0';
            index = 0;

            printf("%s\n", text);
        }
    }
}
#endif
#if defined(SERIAL_TEST_COUNTING) || defined (SERIAL_TEST_SDLSTATES)
int16_t sourceInts[tele_DATA_COUNT];
uint8_t printBuffer[tele_MAX_BUF];
int build_escaped_buffer(int16_t* source, uint8_t* result)
{
    int buildIndex = 0;
    result[buildIndex++] = USB_ESCAPE_BYTE /*0xFE*/;
    result[buildIndex++] = USB_HEADER_BYTE /*0x88*/;
    for (int i = 0; i < tele_DATA_COUNT; i++) {
        uint8_t lsb = (uint8_t)(source[i] & 0x00ff);
        uint8_t msb = (uint8_t)((source[i] & 0xff00) >> 8);
        if (lsb == USB_ESCAPE_BYTE) {
            result[buildIndex++] = USB_ESCAPE_BYTE /*0xFE*/;
            result[buildIndex++] = USB_ESCAPE_BYTE /*0xFE*/;
        }
        else
            result[buildIndex++] = lsb;

        if (msb == USB_ESCAPE_BYTE) {
            result[buildIndex++] = USB_ESCAPE_BYTE /*0xFE*/;
            result[buildIndex++] = USB_ESCAPE_BYTE /*0xFE*/;
        }
        else
            result[buildIndex++] = msb;
    }
    return buildIndex;
}

void sendBuffer(uint8_t* buf, int filled)
{
    for (int i = 0; i < filled; i++)
        fprintf(output, "%c", buf[i]);
    fflush(output);
}
#endif
#if defined(SERIAL_TEST_COUNTING)
void* serial_write()
{
    printf("The Counting Serial Test\n");
    #warning "The Counting Serial Test"

    output = fopen("/dev/ttyACM1", "w"); //open the terminal screen
    if (output == NULL) {
        printf("ACM1 is NULL, trying ACM0.\n");
        output = fopen("/dev/ttyACM0", "w");
        if (output == NULL) {
            printf("ACM0 is also NULL. Exiting.\n");
            return (void*)1;
        }
    }
    cpSleep(100);
    while (true) {
        for (int i = 0; i < tele_DATA_COUNT; i += 1) {
            int currMod = sourceInts[i];
            if (i == 7)
                currMod = 1010;
            else if (i == 9)
                currMod = (cpMillis() + 1500) / 3000;
            else if (i == 11)
                currMod = cpMillis() / 10000;
            else
                currMod = ((cpMillis() + (i * 100)) / 1000);

            if (currMod == 2313)
                printf("im currently doing a bad");

            sourceInts[i] = currMod;
        }
        int howMany = build_escaped_buffer(sourceInts, printBuffer);
        sendBuffer(printBuffer, howMany);
        cpSleep(15);
    }
    fclose(output);
}
#elif defined(SERIAL_TEST_ESCAPING)
void* serial_write()
{
    printf("Escaped Serial Test\n");
    #warning "Escaped Serial Test"
    output = fopen("/dev/ttyACM1", "w"); //open the terminal screen
    if (output == NULL) {
        printf("ACM1 is NULL, trying ACM0.\n");
        output = fopen("/dev/ttyACM0", "w");
        if (output == NULL) {
            printf("ACM0 is also NULL. Exiting.\n");
            return (void*)1;
        }
    }

    cpSleep(100);
    fprintf(output, "%c", 0xfe);
    fprintf(output, "%c", 0x88);

    //the rest are numbers
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x01);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x04);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x09);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x10);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x19);
    fprintf(output, "%c", 0x00);

    //modified value 6
    //fprintf(output,"%c",0x24);
    fprintf(output, "%c", 0xFE); // escape byte
    fprintf(output, "%c", 0xFE); // tells teensy the value actually is 254
    fprintf(output, "%c", 0x00);

    fprintf(output, "%c", 0x31);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x40);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x51);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x64);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x79);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x90);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0xA9);
    fprintf(output, "%c", 0x00);

    fflush(output);
    fclose(output);
}
#elif defined(SERIAL_TEST_SQAURES)
void* serial_write()
{
    printf("Squares Serial Test\n");
    #warning "Squares Serial Test"
    output = fopen("/dev/ttyACM1", "w"); //open the terminal screen
    if (output == NULL) {
        printf("ACM1 is NULL, trying ACM0.\n");
        output = fopen("/dev/ttyACM0", "w");
        if (output == NULL) {
            printf("ACM0 is also NULL. Exiting.\n");
            return (void*)1;
        }
    }

    cpSleep(100);

    //header bytes
    fprintf(output, "%c", 0xfe);
    fprintf(output, "%c", 0x88);
    //the rest are numbers
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x01);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x04);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x09);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x10);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x19);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x24);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x31);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x40);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x51);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x64);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x79);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0x90);
    fprintf(output, "%c", 0x00);
    fprintf(output, "%c", 0xA9);
    fprintf(output, "%c", 0x00);
    fflush(output);
    fclose(output);
}
#elif defined(SERIAL_TEST_SDLSTATES)
void* serial_write()
{
    printf("SDL States Serial Test\n");
    #warning "SDL States Serial Test"
    output = fopen("/dev/ttyACM1", "w"); //open the terminal screen
    if (output == NULL) {
        printf("ACM1 is NULL, trying ACM0.\n");
        output = fopen("/dev/ttyACM0", "w");
        if (output == NULL) {
            printf("ACM0 is also NULL. Exiting.\n");
            return (void*)1;
        }
    }

    cpSleep(100);
    while (true) {
        for (int i = 0; i < tele_DATA_COUNT; i ++) 
        {
            sourceInts[i] = axes[((i-5)+16)%16].tdata;
        }
        int howMany = build_escaped_buffer(sourceInts, printBuffer);
        sendBuffer(printBuffer, howMany);
        cpSleep(15);
    }
    fclose(output);
}
#endif

int main(void)
{
    void* result;

    pthread_t swriter;
    if (pthread_create(&swriter, NULL, serial_write, NULL) == -1)
        printf("Can't create thread serial_write");

    #ifdef SERIAL_SHOULD_READ
        pthread_t sreader;
        if (pthread_create(&sreader, NULL, serial_read, NULL) == -1)
            printf("Can't create thread serial_read");
    #endif

    #ifdef SERIAL_TEST_SDLSTATES
        pthread_t joy;
        if (pthread_create(&joy, NULL, joy_read, NULL) == -1)
            printf("Can't create thread joy");
    #endif

    if (pthread_join(swriter, &result) == -1)
        printf("Can't join thread serial_write");
    #ifdef SERIAL_SHOULD_READ
        if (pthread_join(sreader, &result) == -1)
            printf("Can't join thread serial_read");
    #endif
    #ifdef SERIAL_TEST_SDLSTATES
        if (pthread_join(joy, &result) == -1)
            printf("Can't join thread joy");
    #endif

    printf("About to exit??");
    
    return 0;
}
