

#define UDMA_HYPERBUS_OFFSET 0x1a102000+128*9
#define HYPERBUS_DEVICE_NUM 8
#define CONFIG_REG_OFFSET 0x80


static inline void udma_hyper_setup(){
  pulp_write32(0x1a102000, 1 << HYPERBUS_DEVICE_NUM); // clock for the hyper bus module is activated
  //pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x24, 0x00 ); // 2D TRAN is deactivated
  pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x08, 0x01 ); // REG_T_EN_LATENCY_ADD
  pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x0C, 0xffffffff ); // REG_T_CS_MAX

  pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x20, 0x00); // DRAM memory is selected
  pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x04, 0x06); // T latench access is set to 6 cycles

}

static inline void udma_hyper_flash_setup(){
  int i=0;
  pulp_write32(0x1a102000, 1 << HYPERBUS_DEVICE_NUM); // clock for the hyper bus module is activated
  for(i=0; i< 8; i++){
      pulp_write32(UDMA_HYPERBUS_OFFSET + i*CONFIG_REG_OFFSET + 0x28, 0x00 ); // 2D TRAN is deactivated
      pulp_write32(UDMA_HYPERBUS_OFFSET + i*CONFIG_REG_OFFSET + 0x2C, 0x00 ); // 2D COUNT for ext mem
      pulp_write32(UDMA_HYPERBUS_OFFSET + i*CONFIG_REG_OFFSET + 0x30, 0x00 ); // 2D STRIDE for ext mem
      pulp_write32(UDMA_HYPERBUS_OFFSET + i*CONFIG_REG_OFFSET + 0x34, 0x00 ); // 2D TRAN is deactivated
      pulp_write32(UDMA_HYPERBUS_OFFSET + i*CONFIG_REG_OFFSET + 0x38, 0x00 ); // 2D COUNT for l2
      pulp_write32(UDMA_HYPERBUS_OFFSET + i*CONFIG_REG_OFFSET + 0x3C, 0x00 ); // 2D STRIDE for l2
  }
  pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x24, 0x00 ); // 2D TRAN is deactivated
  pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x08, 0x00 ); // REG_T_EN_LATENCY_ADD
  pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x0C, 0xffffffff ); // REG_T_CS_MAX
  pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x20, 0x01); // Flash memory is selected
  pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x04, 0x0f); // T latench access is set to 16 cycles
}

static inline void udma_hyper_sleep(){
  int a;
  a = pulp_read32(0x1a102000);
  pulp_write32(0x1a102000, (1 << HYPERBUS_DEVICE_NUM)^a ); // Clock gating is activated
}

// Burst write is conducted for the hyper flash. len <- burst length in bytes, ext_addr <- start address of the external memory, l2_addr <- start_address of the L2 memory, page_bound <- page boundary in the external memory
static inline void udma_hyper_dwrite(unsigned int len, unsigned int ext_addr, unsigned int l2_addr, unsigned int page_bound, unsigned int tran_id){

  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x28, 0x00 ); // 2D TRAN is deactivated
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x2C, 0x00 ); // 2D COUNT for ext mem
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x30, 0x00 ); // 2D STRIDE for ext mem
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x34, 0x00 ); // 2D TRAN is deactivated
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x38, 0x00 ); // 2D COUNT for l2
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x3C, 0x00 ); // 2D STRIDE for l2

  switch(page_bound){
     case 128:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x00 ); // page boundary is set to every 128 bytes
        break;
     case 256:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x01 ); // page boundary is set to every 256 bytes
        break;
     case 512:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x02 ); // page boundary is set to every 128 bytes
        break;
     case 1024:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x03 ); // page boundary is set to every 256 bytes
        break;
     default:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x04 ); // page boundary is not considered
  }


  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x0C, l2_addr ); // Data address 
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x10, len );     // Data size to be sent
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x1C, ext_addr );     // Data size to be sent
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x18, 0x01);     // Write is declared for the external mem
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x14, 0x14);     // Write transaction is kicked
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x14, 0x00);     // Write transaction information is reset
}

static inline void udma_hyper_twd_dwrite(unsigned int len, unsigned int ext_addr, unsigned int l2_addr, unsigned int page_bound, unsigned int twd_act_ext, unsigned int twd_count_ext,unsigned int twd_stride_ext, unsigned int twd_act_l2, unsigned int twd_count_l2, unsigned int twd_stride_l2, unsigned int tran_id){

  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x28, twd_act_ext    ); // 2D TRAN is deactivated
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x2C, twd_count_ext  ); // 2D COUNT for ext mem
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x30, twd_stride_ext ); // 2D STRIDE for ext mem
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x34, twd_act_l2     ); // 2D TRAN is deactivated
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x38, twd_count_ext  ); // 2D COUNT for l2
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x3C, twd_stride_ext ); // 2D STRIDE for l2

  switch(page_bound){
     case 128:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x00 ); // page boundary is set to every 128 bytes
        break;
     case 256:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x01 ); // page boundary is set to every 256 bytes
        break;
     case 512:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x02 ); // page boundary is set to every 128 bytes
        break;
     case 1024:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x03 ); // page boundary is set to every 256 bytes
        break;
     default:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x04 ); // page boundary is not considered
  }


  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x0C, l2_addr ); // Data address 
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x10, len );     // Data size to be sent
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x1C, ext_addr );     // Data size to be sent
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x18, 0x01);     // Write is declared for the external mem
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x14, 0x14);     // Write transaction is kicked
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x14, 0x00);     // Write transaction information is reset
}


// Word write for Hyper flash
static inline void udma_hyperflash_wwrite(unsigned int ext_addr, short int data, unsigned int tran_id){
  pulp_write32(UDMA_HYPERBUS_OFFSET + 8*CONFIG_REG_OFFSET + 0x00, 0x04 ); // page boundary is not considered
  pulp_write32(UDMA_HYPERBUS_OFFSET + 8*CONFIG_REG_OFFSET + 0x24, 0x00 ); // 2D TRAN is deactivated
  pulp_write32(UDMA_HYPERBUS_OFFSET + 8*CONFIG_REG_OFFSET + 0x08, 0x00 ); // REG_T_EN_LATENCY_ADD
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x1C, ext_addr<<1); // Address for the external memory
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x20, data); // Data to be written
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x18, 0x00); // Write is declared for the external mem
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x14, 0x14); // Write transaction is kicked
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x14, 0x00); // Write transaction information is reset
  
}
static inline int udma_hyper_nb_tran(unsigned int tran_id){
  return pulp_read32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x24) >> 1;
}


// Burst write for Hyper flash. Page boundary consideration and byte addressing mode are NOT supported. nb_word <- burst length in words, hyper_waddr <- start address of the hyperflash (word addressing), l2_addr <- start_address for L2
static inline void udma_hyperflash_bwrite(unsigned int nb_word, unsigned int hyper_waddr, unsigned int l2_addr, unsigned int tran_id){

    udma_hyperflash_wwrite(0x555, 0x00aa, tran_id);
    udma_hyperflash_wwrite(0x2aa, 0x0055, tran_id);
    udma_hyperflash_wwrite(hyper_waddr, 0x0025, tran_id);
    udma_hyperflash_wwrite(hyper_waddr, nb_word-1, tran_id);
    for(int i=0; i< nb_word; i++ ){
       while(udma_hyper_nb_tran(tran_id)>7){}
       udma_hyperflash_wwrite(hyper_waddr+i, *((short int *) l2_addr+i), tran_id);
       //printf("%d th data= %x \n", hyper_waddr+i,  *((short int *) l2_addr+i));
    }
    udma_hyperflash_wwrite(hyper_waddr, 0x0029, tran_id);
    rt_time_wait_us(1000);
    //for( volatile int i=0; i<10000; i++){}

}

static inline void udma_hyperflash_erase(unsigned int sector_addr, unsigned int tran_id){
    udma_hyperflash_wwrite(0x555, 0x00aa, tran_id);
    udma_hyperflash_wwrite(0x2aa, 0x0055, tran_id);
    udma_hyperflash_wwrite(0x555, 0x0080, tran_id);
    udma_hyperflash_wwrite(0x555, 0x00aa, tran_id);
    udma_hyperflash_wwrite(0x2aa, 0x0055, tran_id);
    udma_hyperflash_wwrite(sector_addr, 0x0030, tran_id);
    rt_time_wait_us(1000); 
    //for( volatile int i=0; i<10000; i++){}
}

/*static inline void udma_hyperflash_cperase(){
    udma_hyperflash_wwrite(0x555, 0x00aa);
    udma_hyperflash_wwrite(0x2aa, 0x0055);
    udma_hyperflash_wwrite(0x555, 0x0080);
    udma_hyperflash_wwrite(0x555, 0x00aa);
    udma_hyperflash_wwrite(0x2aa, 0x0055);
    udma_hyperflash_wwrite(0x555, 0x0010);
    for( volatile int i=0; i<10000; i++){}
}*/


// Linear read is conducted. len <- burst length in bytes, ext_addr <- start address of the external memory, l2_addr <- start_address of the L2 memory, page_bound <- page boundary in the external memory
//

static inline void udma_hyper_dread(unsigned int len, unsigned int ext_addr, unsigned int l2_addr, unsigned int page_bound, unsigned int tran_id){

  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x28, 0x00 ); // 2D TRAN is deactivated
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x2C, 0x00 ); // 2D COUNT for ext mem
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x30, 0x00 ); // 2D STRIDE for ext mem
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x34, 0x00 ); // 2D TRAN is deactivated
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x38, 0x00 ); // 2D COUNT for l2
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x3C, 0x00 ); // 2D STRIDE for l2

  switch(page_bound){
     case 128:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x00 ); // page boundary is set to every 128 bytes
        break;
     case 256:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x01 ); // page boundary is set to every 256 bytes
        break;
     case 512:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x02 ); // page boundary is set to every 128 bytes
        break;
     case 1024:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x03 ); // page boundary is set to every 256 bytes
        break;
     default:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x04 ); // page boundary is not considered
  }

  
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x00, l2_addr ); // Data address 
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x04, len );     // Data size to be sent
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x1C, ext_addr );     // Data size to be sent
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x18, 0x05);     // Read is declared for the external mem
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x08, 0x14);     // Read transaction is kicked
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x08, 0x00);     // Write transaction information is reset
}

static inline void udma_hyper_twd_dread(unsigned int len, unsigned int ext_addr, unsigned int l2_addr, unsigned int page_bound, unsigned int twd_act_ext, unsigned int twd_count_ext,unsigned int twd_stride_ext, unsigned int twd_act_l2, unsigned int twd_count_l2, unsigned int twd_stride_l2, unsigned int tran_id){

  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x28, twd_act_ext    ); // 2D TRAN is deactivated
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x2C, twd_count_ext  ); // 2D COUNT for ext mem
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x30, twd_stride_ext ); // 2D STRIDE for ext mem
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x34, twd_act_l2     ); // 2D TRAN is deactivated
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x38, twd_count_ext  ); // 2D COUNT for l2
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x3C, twd_stride_ext ); // 2D STRIDE for l2


  switch(page_bound){
     case 128:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x00 ); // page boundary is set to every 128 bytes
        break;
     case 256:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x01 ); // page boundary is set to every 256 bytes
        break;
     case 512:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x02 ); // page boundary is set to every 128 bytes
        break;
     case 1024:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x03 ); // page boundary is set to every 256 bytes
        break;
     default:
        pulp_write32(UDMA_HYPERBUS_OFFSET + CONFIG_REG_OFFSET*8 + 0x00, 0x04 ); // page boundary is not considered
  }


  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x00, l2_addr ); // Data address 
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x04, len );     // Data size to be sent
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x1C, ext_addr );     // Data size to be sent
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x18, 0x05);     // Read is declared for the external mem
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x08, 0x14);     // Read transaction is kicked
  pulp_write32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x08, 0x00);     // Write transaction information is reset
}

// Outputs the number of transactions which is remaining the FIFO
//unsigned int udma_hyper_nb_tran(){
//  return pulp_read32(UDMA_HYPERBUS_OFFSET + 0x58) >> 1;
//}

// If the hyperbus module is doing something.
static inline int udma_hyper_busy(unsigned int tran_id){
  return pulp_read32(UDMA_HYPERBUS_OFFSET + tran_id*CONFIG_REG_OFFSET + 0x24) & 0x00000001;
}

static inline void udma_hyper_wait(unsigned int tran_id){
   while(udma_hyper_busy(tran_id)){
   }
}

static inline int udma_hyper_id_alloc(){
  return pulp_read32(UDMA_HYPERBUS_OFFSET + 8*CONFIG_REG_OFFSET + 0x24);
}

