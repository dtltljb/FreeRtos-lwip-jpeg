
project: 基于FreeRTOS的矩阵压力数据高频2MHz采集速率,通过socket把数据传递给客户端。

@说明: 
	参考该工程文档下的"..\project\F407VGT-RTOSV1-lwip-jpeg\F407VGT-RTOSV1-lwip-jpeg.stmx"工程配置文件;
	使用STM32CubeMX（Version 5.2.0）应用软件打开 此工程配置文件,内部有详细配置参数,生产代码Keil v5 的IDE开发工程。

	第1条：
	(1). 	tcp/ip的信息配置,在netif.c文件line 163 行,程序初始化IPV4、GW、MASK的信息。
	(2).	FreeRTOS的缺省启动任务中,初始化LIBJPEG、LWIP两部分内容;
				
				
	 void StartDefaultTask(void const * argument)   	/*	FreeRTOS的缺省启动任务 */
		|	
		|-->| MX_LWIP_Init();					/* 以太网初始化路线图 */
		|
		|-->	 tcpip_init( NULL, NULL );			/* Initilialize the LwIP stack with RTOS */
		|
	    V---|-->	 netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);		/* add the network interface (IPv4/IPv6) with RTOS */
		|		|
		|		|-->	 netif_set_default(&gnetif);	  /* Registers the default network interface */
		|		|
		|		|-->		netif_set_up(&gnetif);		/* When the netif is fully configured this function must be called */
		|		|
		|		|-->	  dhcp_start(&gnetif);			/* Start DHCP negotiation for a network interface (IPv4) */
		|		
		|		/*   - initialize all sub modules, - start the tcpip_thread */
		|		void tcpip_init(tcpip_init_done_fn initfunc, void *arg)
		|		|
		|		|-->		 lwip_init();		/*   - initialize all sub modules */
		|		|
		|		|-->		 sys_thread_new(TCPIP_THREAD_NAME, tcpip_thread, NULL, TCPIP_THREAD_STACKSIZE, TCPIP_THREAD_PRIO);/*- start the tcpip_thread */
		|		
		|		
		|		void lwip_init(void)
		|		|
		|		|-->	  	stats_init();
		|		|
		|		|-->		  mem_init();
		|		|
		|		|-->		  memp_init();
		|		|
		|		|-->			pbuf_init();
		|		|
		|		|-->		  netif_init();
		|		|
		|		|-->			ip_init();
		|		|
		|		|-->			etharp_init();
		|		|
		|		|-->			udp_init();
		|		|
		|		|-->			tcp_init();
		|		|
		|		|-->			dns_init();
		|		|
		|		|-->			ppp_init();
		|		|
		|		|-->			sys_timeouts_init();
		|		
		|		
		|		
		|		
		|-->	netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
		|
		|--> err_t ethernetif_init(struct netif *netif)  /* This function should be passed as a parameter to netif_add() */
		|							
		|-->	low_level_init(netif);	/* the hardware should be initialized. * Called from ethernetif_init().	*/ /* Init ETH MAC Addr */
		|					
		|-->	hal_eth_init_status = HAL_ETH_Init(&heth);  /* @brief  Initializes the Ethernet MAC and DMA according to default   */         parameters.
		|				
		|-->	HAL_ETH_MspInit(heth);	/* Init the low level hardware : GPIO, CLOCK, NVIC. */
		|
		|-->	HAL_ETH_WritePHYRegister(heth, PHY_BCR, PHY_RESET);		  /* Put the PHY in reset mode */
		|
		|-->	ETH_MACDMAConfig(heth, err);    /* Config MAC and DMA */
		|
		|-->	HAL_ETH_ReadPHYRegister(heth, PHY_BSR, &phyreg);
		|
		|-->	        /* Config MAC and DMA */

第 2 条 ：
		(1).startup select mode 	
		_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  _  _  _  _  _  _  _  _  _  _  _ _ _  _  _  _  _  _  _  _  _  _ _ 
		| Boot mode selection pins  |									                          |
		---------------------------- -------------------------	---	------	---	---	---	---	---	---	---	
		| boot 1 			| boot	0			|  Boot mode       |     Aliasing    			|
                |   x				|		0		|Main Flash memory |  Main Flash memory is selected as the boot space	|	system selected ,repair sch. 
		|   0         |   1         |	System memory  	 |	System memory is selected as the boot space     |
		|   1         |   1         | Embedded SRAM    |  Embedded SRAM is selected as the boot space     |
		
第 3 条：
FreeRTOS 操作系统优点
(1).	任务调动时,上下文切换时,是有限度关闭中断使能限制,让系统其它中断具有更高的实时性。configKERNEL_INTERRUPT_PRIORITY 小于此中断号的中断时可以执行。
(2).  RTOS任务通知相当于直接向任务发送一个事件，接收到通知的任务可以解除任务的阻塞状态。相比于使用信号量解除任务阻塞，使用任务通知可以快45% 。configUSE_TASK_NOTIFICATIONS = 1 。
			
第 4 条: FreeRTOS的中断优先级配置
	osPriorityIdle					/* 空闲级别,最低优先级*/			
	osPriorityLow						/* 低优先级*/
	osPriorityUnderNormal		/* normal under */
	osPriorityNormal        /* normal */
	osPriorityAboveNormal		/* normal above */	
	osPriorityRealtime      /* Realtime 此级别程序不参与RTOS系统调动,梳理物理机直接中断调动机制*/
	本项目方案中DMA传递ADC采集数据,是此优先级的任务,操作系统是按照任务通知方式调动,效率提升40%。
				
第 5 条	：ADC DMA 参数配置
	 multimode.Mode = ADC_TRIPLEMODE_REGSIMULT;  /* 三通道规则转换,DMA   */
			
第 6 条 以太网参数配置
				
	6.1) 以太网芯片DP83848cvv 模式RMII方式,外部有源50MHz晶体.
				
