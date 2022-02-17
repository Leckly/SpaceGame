/** @file
 *
 * @author Marek Mierzwinski Marcin Marciniak Jakub Marcin Mieczkowski
 * @copyright Politechnika Bialostocka
 * @date 31.01.2022
 */
#include <msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"
#include <stdlib.h>
#include <time.h>
#define BUTTON1 (P4IN & BIT4)
#define BUTTON2 (P4IN & BIT5)

int screen[2][16];
int pos, count, offset, gap, lvlupcount, score, hscore = 0;

/** @brief Ustawia przyciski
 */
void setButtons()
{
    P4DIR &= ~BIT4;
    P4DIR &= ~BIT5;
}
/** @brief Wyswietla uzyskany wynik
 *@param liczba do wyswietlenia
 */
void printScore(int x)
{
    if(x >= 10)
    {
        printScore(x / 10);
    }
    int l = x % 10 + 48 ;
    SEND_CHAR(l);
}
/** @brief Inicjalizuje znaki specjalne(samolot, przeszkody)
 */
void InitSpecial()
{
    SEND_CMD(CG_RAM_ADDR);
    int spec[2][8] = {{24, 28, 30, 7, 7, 30, 28, 24},
                    {17, 17, 17, 31, 31, 17, 17, 17}
    };
    for(int i=0; i<2 ; i++)
    {
        for(int j=0; j<8; j++)
        {
            SEND_CHAR(spec[i][j]);
        }
    }
}
/** @brief Generuje przeszkody
 */
void generateMeteor()
{
    int i = rand() % 2;
    screen[i][15] = 1;
}
/** @brief Aktualizuje stan tablicy przechowujacej plansze
 */
void shuffle()
{
    for(int i=0; i<15; i++)
    {
        screen[0][i] = screen[0][i+1];
        screen[1][i] = screen[1][i+1];
    }
    screen[0][15] = screen[1][15] = 0;
}
/** @brief Uzywany na poczatku gry aby wyswietlic ekran
 */
void showScreen()
{
    SEND_CMD(DD_RAM_ADDR);
    if(pos==0) SEND_CHAR(8);
    else
    {
        if(screen[0][0]==0) SEND_CHAR(' ');
        else SEND_CHAR(9);
    }
    for(int i=1; i<15; i++)
    {
        if(screen[0][i]==0) SEND_CHAR(' ');
        else SEND_CHAR(9);
    }

    SEND_CMD(DD_RAM_ADDR2);
    if(pos==1) SEND_CHAR(8);
    else
    {
        if(screen[1][0]==0) SEND_CHAR(' ');
        else SEND_CHAR(9);
    }
    for(int i=1; i<15; i++)
    {
        if(screen[1][i]==0) SEND_CHAR(' ');
        else SEND_CHAR(9);
    }
}
/** @brief Przesuwa przeszkody na ekranie w lewo i generuje nowe z tablicy ekran
 */
void updateScreen()
{
    SEND_CMD(DATA_ROL_LEFT);
    if(pos==0)
    {
        SEND_CMD(DD_RAM_ADDR+(offset)%40);
        SEND_CHAR(8);
    }
    else
    {
        SEND_CMD(DD_RAM_ADDR2+(offset)%40);
        SEND_CHAR(8);
    }
    SEND_CMD(DD_RAM_ADDR+(15+offset)%40);
    if(screen[0][15]==0)
    {
        SEND_CHAR(' ');
        SEND_CHAR(' ');
    }
    else
    {
        SEND_CHAR(9);
        SEND_CHAR(' ');
    }
    SEND_CMD(DD_RAM_ADDR2+(15+offset)%40);
    if(screen[1][15]==0)
    {
        SEND_CHAR(' ');
        SEND_CHAR(' ');
    }
    else
    {
        SEND_CHAR(9);
        SEND_CHAR(' ');
    }

    offset = (offset+1)%40;
}
/** @brief Czysci ekran
 */
void clearScreen()
{
    for(int i=0; i<16; i++)
    {
        screen[0][i] = screen[1][i]=0;
    }
}
/** @brief Funkcja main odpowiedzialna za logike gry
 */
void main(void)
{
    WDTCTL=WDTPW+ WDTHOLD;
    InitPortsLcd();
    InitLCD();
    clearDisplay();
    InitSpecial();
    setButtons();
    srand(time(NULL));
    while(1)
    {
        SEND_CMD(DD_RAM_ADDR);
        SEND_CHAR('P');
        SEND_CHAR('r');
        SEND_CHAR('e');
        SEND_CHAR('s');
        SEND_CHAR('s');
        SEND_CHAR(' ');
        SEND_CHAR('1');
        SEND_CHAR(' ');
        SEND_CHAR('t');
        SEND_CHAR('o');
        SEND_CHAR(' ');
        SEND_CHAR('s');
        SEND_CHAR('t');
        SEND_CHAR('a');
        SEND_CHAR('r');
        SEND_CHAR('t');
        SEND_CMD(DD_RAM_ADDR2);
        SEND_CHAR('U');
        SEND_CHAR('s');
        SEND_CHAR('e');
        SEND_CHAR(' ');
        SEND_CHAR('2');
        SEND_CHAR(' ');
        SEND_CHAR('t');
        SEND_CHAR('o');
        SEND_CHAR(' ');
        SEND_CHAR('m');
        SEND_CHAR('o');
        SEND_CHAR('v');
        SEND_CHAR('e');
        while(BUTTON1);

        pos = 0;
        count = 0;
        offset = 1;
        gap = 3;
        lvlupcount = 0;
        score = 0;
        clearDisplay();
        clearScreen();
        showScreen();

        while(screen[pos][0]==0)
        {
            if(screen[0][0]==1 || screen[1][0]==1)
            {
                score++;
            }
            if(count==0)
            {
                generateMeteor();
                lvlupcount++;
            }
            if(!BUTTON2) pos = (pos+1)%2;
            updateScreen();
            Delayx100us(120);
            shuffle();
            count = (count+1)%(gap+2);
            if(lvlupcount==10)
            {
                gap = (gap-1)%4;
                lvlupcount = 0;
            }
        }
        if(score>hscore) hscore = score;

        clearDisplay();
        SEND_CMD(DD_RAM_ADDR);
        SEND_CHAR('S');
        SEND_CHAR('c');
        SEND_CHAR('o');
        SEND_CHAR('r');
        SEND_CHAR('e');
        SEND_CHAR(' ');
        printScore(score);
        SEND_CMD(DD_RAM_ADDR2);
        SEND_CHAR('H');
        SEND_CHAR('i');
        SEND_CHAR('g');
        SEND_CHAR('h');
        SEND_CHAR('e');
        SEND_CHAR('s');
        SEND_CHAR('t');
        SEND_CHAR(' ');
        SEND_CHAR('S');
        SEND_CHAR('c');
        SEND_CHAR('o');
        SEND_CHAR('r');
        SEND_CHAR('e');
        SEND_CHAR(' ');
        printScore(hscore);
        while(BUTTON1);
    }
}
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
    ++clock;
    _BIC_SR_IRQ(LPM3_bits);
}
