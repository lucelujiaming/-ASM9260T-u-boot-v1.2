#include <common.h>
#include <command.h>
#include <hardware.h>
#include <net.h>
#include <malloc.h>

#include "mac9260.h"

struct board_info *db;

uint8_t	sendbuf[] = {0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x01,0x01,0x08,0x06,0x00,0x01,0x08,0x00,0x06,0x04,			
		     0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0xc0,0xa8,0x64,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xa8,
		     0x64,0x3f};

#if 1     
void set_pin(void)
{
      set_pin_mux(9,1,5);                 //mdio
      set_pin_mux(9,0,5);                 //mdc
      set_pin_mux(9,7,5);                 //txd1
      set_pin_mux(9,6,5);                 //txd0
      set_pin_mux(9,5,5);                 //tx_en
      set_pin_mux(9,2,5);                 //rmii_crs_dv
      set_pin_mux(9,3,5);                 //rxd0
      set_pin_mux(9,4,5);                 //rxd1
      set_pin_mux(10,7,5);                 //rmii_ref_clk
}
#endif

#if 0
void set_pin(void)
{
	set_pin_mux(5,1,5);	 
	set_pin_mux(5,0,5);	 
	set_pin_mux(4,3,5);	 
	set_pin_mux(4,2,5);	 
	set_pin_mux(5,7,5);	 
	set_pin_mux(5,6,5);	 
	set_pin_mux(5,5,5);	 
	set_pin_mux(4,7,5);	 
	set_pin_mux(4,5,5);	 
	set_pin_mux(4,6,5);	 
	set_pin_mux(5,2,5);	 
	set_pin_mux(5,3,5);	 
	set_pin_mux(5,4,5);	 
	set_pin_mux(4,0,5);	 
	set_pin_mux(4,1,5);	 
	set_pin_mux(4,4,5);	 
	set_pin_mux(10,5,5);	 
}
#endif

void __inline delayms(u32 nMs)
{
	u32 i,j;
	for(i = 0; i < nMs; i++)
	{
		for(j = 0; j < 20; j++)
		{
		}
	}
}

/******************************** 
      ensure 32 bytes align 
********************************/
void *alpmalloc(unsigned int n)
{
	n=n+(32-n%32)+24;	
	return (malloc(n));
}


int alp_dma_map(void)
{	
      
        db->tx_desc=(struct desc_type *)alpmalloc(sizeof(struct desc_type)*ETH_TXBUFNB);
        if(!db->tx_desc)
        {
                   printf("malloc tx_desc failed!\n");
                   goto out;
        }
        
        db->rx_desc=(struct desc_type *)alpmalloc(sizeof(struct desc_type)*ETH_RXBUFNB);
        if(!db->rx_desc)
        {
                printf("dma_recv_map failed!\n");
                goto out;
        }
        
	
	db->p_tx=(uint8_t *)alpmalloc(sizeof(uint8_t)*ETH_TXBUFNB*ETH_MAX_PACKET_SIZE);
	if(!db->p_tx)
	{
		printf("buf_send_map failed!\n");
		goto out;
	}

	db->p_rx=(uint8_t *)alpmalloc((sizeof(uint8_t)*ETH_RXBUFNB*ETH_MAX_PACKET_SIZE));
	if(!db->p_rx)
	{
		printf("buf_recv_map failed!\n");
		goto out;
        }

	return 0;
out:
	alp_dma_unmap();
	return -1;
}

void alp_dma_unmap(void)
{
	free(db->tx_desc);	
	free(db->rx_desc);
	free(db->p_tx);
	free(db->p_rx);
}

void set_phy_mode(int phy)
{
	switch(phy){
		case 0:
			/*100_FD*/
                        mac9260_phy_write(PHY_BCR,0x2100);
			break;
		case 1:
			/*100_HD*/
                        mac9260_phy_write(PHY_BCR,0x2000);
			break;
		case 2:
			/*10_FD*/
                        mac9260_phy_write(PHY_BCR,0x0100);
			break;
		case 3:
			/*10_HD*/
                        mac9260_phy_write(PHY_BCR,0x0000);
			break;	
		default:
			break;
	}	
}

void set_mac_mode(int mac)
{
	switch(mac){
		case 0:
			outl((0x0000CA00)|inl(HW_ETH_MACCR),HW_ETH_MACCR);		/*mii,100mbps,full duplex*/
			break;
		case 1:
			outl((0x0000C200)|inl(HW_ETH_MACCR),HW_ETH_MACCR);              /*mii,100mbps,half duplex*/
			break;
		case 2:
			outl((0x00008A00)|inl(HW_ETH_MACCR),HW_ETH_MACCR);		/*mii,10mbps,full duplex*/
			break;
		case 3:
			outl((0x00008200)|inl(HW_ETH_MACCR),HW_ETH_MACCR);		/*mii,10mbps,half duplex*/
			break;	
		default:
			break;	
	}	
}

void set_work_mode(int mode)
{
	switch(mode){
		case 0:
			outl(0x00000000,HW_MACPHY_SEL);    	/* Mode MII */
			break;
		case 1:
			outl(0x00000004,HW_MACPHY_SEL);  	/* Mode RMII_OUT_CLK */
			break;
		case 2:
			outl(0x0000000c,HW_MACPHY_SEL);  	/* Mode RMII_IN_CLK */
   			outl(0x00000008,HW_MACCLKDIV);		/* RMII CLK */
			break;	
		default:
			break;
	}	
}


void set_mac_address(unsigned int high,unsigned int low)
{
	outl(high,HW_ETH_MACA0HR);
        outl(low,HW_ETH_MACA0LR);
}

void mac_dma_enable(void)
{
	outl((0x00000008)|inl(HW_ETH_MACCR),HW_ETH_MACCR);
   	outl((0x00100000)|inl(HW_ETH_DMAOMR),HW_ETH_DMAOMR);
   	outl((0x00000004)|inl(HW_ETH_MACCR),HW_ETH_MACCR);
   	outl((0x00002002)|inl(HW_ETH_DMAOMR),HW_ETH_DMAOMR);
}

void mac_clk_enable(void)
{
	outl(0x1<<5,HW_AHBCLKCTRL0+4);		
}

void mac_clk_disable(void)
{
	outl(0x1<<5,HW_AHBCLKCTRL0+8);		
}

void eth_reset(void)
{
	outl(0x1<<5,HW_PRESETCTRL0+8);    
        outl(0x1<<5,HW_PRESETCTRL0+4);
}

void software_reset(void)
{
	outl((0x1<<0)|inl(HW_ETH_DMABMR),HW_ETH_DMABMR);     	/* Software reset */
        while((inl(HW_ETH_DMABMR)&0x1) != 0)
        {
        }
}

void mac_loopback(void)
{
	outl((inl(0x80500000)|0x1000),0x80500000); 		/*mac loop back*/
}

void set_csr_clock(void)
{
	outl((0x4<<2)|inl(HW_ETH_MACMIIAR),HW_ETH_MACMIIAR);    /* CSR Clock Range between 35-60 MHz */
}

void set_interrupt(void)
{
	outl((0x00000040)|inl(HW_ETH_MACFFR),HW_ETH_MACFFR);
   	outl((0x00000000),HW_ETH_MACHTHR);
   	outl((0x00000000),HW_ETH_MACHTLR);
   	outl((0x00000080)|inl(HW_ETH_MACFCR),HW_ETH_MACFCR);
   	outl((0x00000000),HW_ETH_MACVLANTR);
   	outl((0x03200004)|inl(HW_ETH_DMAOMR),HW_ETH_DMAOMR);
   	outl((0x02C16000),HW_ETH_DMABMR);
   	outl((0x00010040)|inl(HW_ETH_DMAIER),HW_ETH_DMAIER);
}

void eth_send_init(struct desc_type *txtab, uint8_t *tbuf, uint32_t tbufcount)
{
  	uint32_t i = 0;
	struct desc_type *dmatxdesc;
  
  	for(i=0; i < tbufcount; i++)
  	{
                dmatxdesc = txtab + i;
		dmatxdesc->conbufsize = ETH_DMATxDesc_TCH;   
                dmatxdesc->buf1addr = (uint32_t)(&tbuf[i*ETH_MAX_PACKET_SIZE]);
                if(i < (tbufcount-1))
                {
                        dmatxdesc->buf2nextdescaddr = (uint32_t)((struct desc_type *)db->tx_desc+i+1);
                }
                else
                { 
                        dmatxdesc->buf2nextdescaddr = (uint32_t)((struct desc_type *)db->tx_desc);  
                }
  	}
  	outl((uint32_t)db->tx_desc,HW_ETH_DMATDLAR);
}

void eth_recv_init(struct desc_type *rxtab, uint8_t *rbuf, uint32_t rbufcount)
{
  	uint i = 0;
	struct desc_type *dmarxdesc;
  
  	for(i=0; i < rbufcount; i++)
  	{
    		dmarxdesc = rxtab+i;
    		dmarxdesc->status = ETH_DMARxDesc_OWN;
    		dmarxdesc->conbufsize = ETH_DMARxDesc_RCH | (uint32_t)ETH_MAX_PACKET_SIZE;  
    		dmarxdesc->buf1addr = (uint32_t)(&rbuf[i*ETH_MAX_PACKET_SIZE]);
    
    		if(i < (rbufcount-1))
    		{
      			dmarxdesc->buf2nextdescaddr = (uint32_t)((struct desc_type *)db->rx_desc+i+1); 
    		}
    		else
    		{ 
      			dmarxdesc->buf2nextdescaddr = (uint32_t)((struct desc_type *)db->rx_desc); 
    		}
  	}
  	outl((uint32_t)db->rx_desc,HW_ETH_DMARDLAR);
}

void eth_it_config(struct desc_type *dmarxdesc)
{
    	dmarxdesc->conbufsize &=(~(uint32_t)0x80000000);		
}


void eth_insert_config(struct desc_type *dmatxdesc, uint32_t DMATxDesc_Checksum)
{
        dmatxdesc->conbufsize |= 0x04000000;		//tcpudpicmpfull
}


uint32_t eth_get_size(struct desc_type *rxtab)
{
  	uint32_t framlen = 0;
  	if(((rxtab->status & ETH_DMARxDesc_OWN) == 0) &&
     	((rxtab->status & ETH_DMARxDesc_ES) == 0) &&
     	((rxtab->status & ETH_DMARxDesc_LS) != 0) &&
     	((rxtab->status & ETH_DMARxDesc_FS) != 0))
  	{
    		framlen = (rxtab->status & 0x3FFF0000) >> 16;
                if (framlen != 0)
                {
                        db->recv_len = framlen;
                }
  	}
  
 	return framlen;
}


static void mac9260_phy_write(int reg, uint32_t value)
{
	uint32_t tmpreg=0;
	uint32_t timeout=0;

	tmpreg = inl(HW_ETH_MACMIIAR)&0x1C;
   	tmpreg |=(((uint32_t)PHY_ADDRESS<<11) & ETH_MACMIIAR_PA); /* Set the PHY device address */
   	tmpreg |=((reg<<6) & ETH_MACMIIAR_MR);      /* Set the PHY register address */
   	tmpreg |= ETH_MACMIIAR_MW;                               /* Set the write mode */
   	tmpreg |= ETH_MACMIIAR_MB;                               /* Set the MII Busy bit */
   
   	outl(value,HW_ETH_MACMIIDR);
   	outl(tmpreg,HW_ETH_MACMIIAR);
   	do
   	{
     		timeout++;
     		tmpreg = inl(HW_ETH_MACMIIAR);
   	}while ((tmpreg & ETH_MACMIIAR_MB) && (timeout < (uint32_t)PHY_WRITE_TO));

   	if(timeout == PHY_WRITE_TO)
   	{
   	}

}

static unsigned long mac9260_phy_read(int reg)
{
	uint32_t tmpreg=0;
	uint32_t timeout=0;
	uint32_t value=0;
        
	tmpreg = inl(HW_ETH_MACMIIAR)&0x1C;
   	tmpreg |=(((uint32_t)PHY_ADDRESS<<11) & ETH_MACMIIAR_PA); /* Set the PHY device address */
   	tmpreg |=((reg<<6) & ETH_MACMIIAR_MR);      /* Set the PHY register address */
   	tmpreg &= ~ETH_MACMIIAR_MW;                              /* Set the read mode */
   	tmpreg |= ETH_MACMIIAR_MB;                               /* Set the MII Busy bit */
   	outl(tmpreg,HW_ETH_MACMIIAR);
   	timeout = 0;
   	do
   	{
     		timeout++;
     		tmpreg = inl(HW_ETH_MACMIIAR);
   	} while ((tmpreg & ETH_MACMIIAR_MB) && (timeout < (uint32_t)PHY_READ_TO));
   	if(timeout == PHY_READ_TO)
   	{
   	}
   	value=inl(HW_ETH_MACMIIDR);
        
	return value;
}

uint32_t eth_chain_mode(uint16_t FrameLength)
{   
  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  if((db->tx_desc->status & ETH_DMATxDesc_OWN) != 0)
  {  
     printf("eth_chain_mode\n");
     /* Return ERROR: OWN bit set */
    return -1;
  }

  //db->tx_desc->conbufsize &=0xFFFFF800;
  /* Setting the Frame Length: bits[12:0] */
  db->tx_desc->conbufsize |= (FrameLength & 0x7ff);

  /* Setting the last segment and first segment bits (in this case a frame is transmitted in one descriptor) */    
  db->tx_desc->conbufsize |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;

  /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
  db->tx_desc->status |= ETH_DMATxDesc_OWN;

  /* When Tx Buffer unavailable flag is set: clear it and resume transmission */
  if ((inl(HW_ETH_DMASR)& 0x00000004) != 0)
  {
    /* Clear TBUS ETHERNET DMA flag */
	outl((0x00000004),HW_ETH_DMASR);
    /* Resume DMA transmission*/
	outl(0x00000001,HW_ETH_DMATPDR);
  }	
  	
  
  /* Update the ETHERNET DMA global Tx descriptor with next Tx decriptor */  
  /* Chained Mode */
  /* Selects the next DMA Tx descriptor list for next buffer to send */ 
  db->tx_desc = (struct desc_type *) (db->tx_desc->buf2nextdescaddr);    


  /* Return SUCCESS */
  return 0;   
}

void mac_init(void)
{
        int i;
	alp_dma_map();
	mac_clk_enable();
	set_pin();
	  
	set_work_mode(1);					
	
	eth_reset();
	software_reset();
	set_csr_clock();

	/*   Reset PHY   */
        mac9260_phy_write(PHY_BCR,0x8000);
	delayms(50000);

	set_mac_mode(0);
	set_phy_mode(0);		/*100_FD*/

        //dp83848_AutoNegotiate(ndev);
	set_interrupt();

	/*   SET MAC ADDRESS    */
	set_mac_address(0x6798,0xa4a3e000);


	eth_send_init(db->tx_desc, (uint8_t *)db->p_tx, ETH_TXBUFNB);
	eth_recv_init(db->rx_desc, (uint8_t *)db->p_rx, ETH_RXBUFNB);	

	/* Enable Ethernet Rx interrrupt */
	for(i=0; i<ETH_RXBUFNB; i++)
	{
		eth_it_config(db->rx_desc+i);
	}
   
	/* Enable MAC and DMA transmission and reception */
	mac_dma_enable();	
}

void mac_stop(void)
{
      alp_dma_unmap();
      mac_clk_disable();
}

static int mac9260_tx(struct eth_device *ndev, void *buf, int len)
{ 
    int buf_len;
    buf_len=(len<ETH_MAX_PACKET_SIZE)?len:ETH_MAX_PACKET_SIZE;
    memcpy((uint8_t *)(db->tx_desc->buf1addr),(uint8_t *)buf,buf_len);  

    if((db->tx_desc->status & ETH_DMATxDesc_OWN) != 0)
    {
    }
            
    db->tx_desc->conbufsize &=0xFFFFF800;  
    db->tx_desc->conbufsize |= (buf_len & 0x7ff);    
    db->tx_desc->conbufsize |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;
    db->tx_desc->status |= ETH_DMATxDesc_OWN;


    if ((inl(HW_ETH_DMASR)& 0x00000004) != 0)
    {
    	outl(0x00000004,HW_ETH_DMASR);
        outl(0x00000001,HW_ETH_DMATPDR);     
    }                  		     
    
    return 0;
   
}

static void mac9260_rx(struct eth_device *ndev,int len)
{
        int i;
}


int mac9260_init(struct eth_device *ndev,bd_t *bis)
{
      return 0;
}

int mac9260_halt(struct eth_device *ndev)
{
      //mac_stop();
      return 0;
}

int mac9260_send(struct eth_device *ndev, void *buf, int len)
{
      mac9260_tx(ndev,buf,len);
      db->tx_desc = (struct desc_type *)(db->tx_desc->buf2nextdescaddr);
               
      return 0;
}

int mac9260_recv(struct eth_device *ndev)
{     
      int i;
      uint8_t *p;
      while(eth_get_size(db->rx_desc) != 0) 
      {		 	
            if((db->rx_desc->status & ETH_DMARxDesc_OWN) == 0)
            {
		db->rx_desc->status = ETH_DMARxDesc_OWN; 	
		/* When Rx Buffer unavailable flag is set: clear it and resume reception */
		if ((inl(HW_ETH_DMASR)&0x00000080) != 0)  
		{
		  /* Clear RBUS ETHERNET DMA flag */
		  outl((0x00000080),HW_ETH_DMASR);
		  /* Resume DMA reception */
		  outl(0x00000001,HW_ETH_DMARPDR);
                }
            }
            if (db->recv_len > 0) 
            {
               /* Pass the packet up to the protocol layers. */
               NetReceive (db->rx_desc->buf1addr, (db->recv_len-4));
               db->recv_len = 0;
            }
            db->rx_desc = (struct desc_type *)(db->rx_desc->buf2nextdescaddr);
      }

      return 0;
}

int mac9260_eth_initialize(bd_t *bis)
{
        int len;
	struct eth_device *ndev;

	db = (struct board_info *)malloc(sizeof(struct board_info));
	if (!db) {
		printf("Error: Failed to allocate memory for mac\n");
		return -1;
	}
	memset(db, 0, sizeof(struct board_info));

	ndev = &db->ndev;
        len = strlen("mac9260");
        memcpy(ndev->name,"mac9260",len);
        ndev->enetaddr[0]=0x00;
        ndev->enetaddr[1]=0xe0;
        ndev->enetaddr[2]=0xa3;
        ndev->enetaddr[3]=0xa4;
        ndev->enetaddr[4]=0x98;
        ndev->enetaddr[5]=0x67;

	ndev->init = mac9260_init;
        ndev->halt = mac9260_halt;
	ndev->send = mac9260_send;
	ndev->recv = mac9260_recv;

	eth_register(ndev);
        mac_init();

	return 0;
}
