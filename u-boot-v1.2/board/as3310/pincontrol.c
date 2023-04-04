/*
 * (C) Copyright 2001
 * He yong, Alpscale, Inc.heyong@alpscale.com
 *
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

#define HW_IOCON_PIO_BASE        0x80044000

#if 0
void set_pin_mux(int port,int pin,int mux_type){
int val,addr;

     val = (3<<((pin&0xf)<<1));
     addr = HW_PINCTRL_MUXSEL0 + (port*0x100) + (pin>>4)*0x10 + 8;
     outl( val ,addr );   // clear Pin mux
     
     val = (mux_type<<((pin&0xf)<<1));
     addr -= 4;
     outl( val ,addr );   // Set Pin mux

}
#endif

void set_pin_mux(int port,int pin,int mux_type)
{
    uint32_t val,addr;

    addr = HW_IOCON_PIO_BASE + (port*32) + (pin*4);
    val = inl( addr );   // read org val
    val &= 0xFFFFFFF8; // clear mux feild
    val |= mux_type; // set mux feild with new value

    outl( val,addr );
}

void set_GPIO(int port,int pin){
int val,addr;
    val = (1<<pin);
    addr = HW_PINCTRL_MUXSEL0 - 0x10 + (port*0x100) + 0x50 + 4;

    outl( val ,addr );   // Output data, set to 1
    addr += 0x20;
    outl( val ,addr );   // Output Enable
}


void clear_GPIO(int port,int pin){
int val,addr;
    val = (1<<pin);
    addr = HW_PINCTRL_MUXSEL0 - 0x10 + (port*0x100) + 0x50 + 8;

    outl( val ,addr );   // Output data, set to 0
    addr += (0x20 - 4);
    outl( val ,addr );   // Output Enable
}

void write_GPIO(int port,int pin,int value){
    if (value == 0) clear_GPIO(port,pin);
    else set_GPIO(port,pin);
}


int read_GPIO(int port,int pin){
int read_val,val,addr;

    val = (1<<pin);
    addr = HW_PINCTRL_MUXSEL0 - 0x10 + (port*0x100) + 0x70 + 8;  // direction input
    outl( val ,addr );   // input Enable
    addr -= 0x18;       // choose register DIN 
    read_val = inl(addr);   // read port
    if ((read_val&val) == 0) return 0;
    else return 1;
}
