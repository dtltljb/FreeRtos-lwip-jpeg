
project: ����ѹ��������ϵͳ

@˵��: 
	�ο��ù����ĵ��µ�"..\project\F407VGT-RTOSV1-lwip-jpeg\F407VGT-RTOSV1-lwip-jpeg.stmx"���������ļ�;
	ʹ��STM32CubeMX��Version 5.2.0��Ӧ�������� �˹��������ļ�,�ڲ�����ϸ���ò���,��������Keil v5 ��IDE�������̡�

	��1����
	(1). 	tcp/ip����Ϣ����,��netif.c�ļ�line 163 ��,�����ʼ��IPV4��GW��MASK����Ϣ��
	(2).	FreeRTOS��ȱʡ����������,��ʼ��LIBJPEG��LWIP����������;
				
				
				 void StartDefaultTask(void const * argument)   	/*	FreeRTOS��ȱʡ�������� */
				 			|	
							|-->| MX_LWIP_Init();					/* ��̫����ʼ��·��ͼ */
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

			�� 2 �� ��
				(1).startup select mode 	
				_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  _  _  _  _  _  _  _  _  _  _  _ _ _  _  _  _  _  _  _  _  _  _ _ 
				| Boot mode selection pins  |																																			|
				---------------------------- -------------------	---	---	---	---	---	---	---	---	---	---	---	---	
				| boot 1 			| boot	0			|  Boot mode       |     Aliasing      																|
				|   x					|		0					|Main Flash memory |  Main Flash memory is selected as the boot space	|	system selected ,repair sch. 
				|   0         |   1         |	System memory  	 |	System memory is selected as the boot space     |
				|   1         |   1         | Embedded SRAM    |  Embedded SRAM is selected as the boot space     |
				-		-		-		-		-		-		-		-		-		-		-		-		-		-		-		-		-		-		-		-		-		-		-		-		-	-	
				
			�� 3 ����
			FreeRTOS ����ϵͳ�ŵ�
			(1).	�������ʱ,�������л�ʱ,�����޶ȹر��ж�ʹ������,��ϵͳ�����жϾ��и��ߵ�ʵʱ�ԡ�configKERNEL_INTERRUPT_PRIORITY С�ڴ��жϺŵ��ж�ʱ����ִ�С�
			(2).  RTOS����֪ͨ�൱��ֱ����������һ���¼������յ�֪ͨ��������Խ�����������״̬�������ʹ���ź����������������ʹ������֪ͨ���Կ�45% ��configUSE_TASK_NOTIFICATIONS = 1 ��
			
			�� 4 ��: FreeRTOS���ж����ȼ�����
				osPriorityIdle					/* ���м���,������ȼ�*/			
				osPriorityLow						/* �����ȼ�*/
				osPriorityUnderNormal		/* normal under */
				osPriorityNormal        /* normal */
				osPriorityAboveNormal		/* normal above */	
				osPriorityRealtime      /* Realtime �˼�����򲻲���RTOSϵͳ����,����������ֱ���жϵ�������*/
				����Ŀ������DMA����ADC�ɼ�����,�Ǵ����ȼ�������,����ϵͳ�ǰ�������֪ͨ��ʽ����,Ч������40%��
				
			�� 5 ��	��ADC DMA ��������
			
			  multimode.Mode = ADC_TRIPLEMODE_REGSIMULT;  /* ��ͨ������ת��,DMA   */
			
			�� 6 �� ��̫����������
				
				6.1) ��̫��оƬDP83848cvv ģʽRMII��ʽ,�ⲿ��Դ50MHz����.
				