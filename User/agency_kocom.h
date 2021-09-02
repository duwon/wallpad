/*------------------------------------------------------------------------------------
OP   						���� 			WP -> ISD		ISD -> WP	
------------------------------------------------------------------------------------
0x01 ����									0				0			������ ACK �ʿ�����Ϸ�
0x02 ���������� ��ȸ											0 
0x03 ����
	 OP_induc 2d		�δ���				0				0
	 OP_Gas   2c		�������			0				0			������ ACK �Ϸ�	
	 OP_Light 0e		�ϰ��ҵ�			0				0			������ ACK �Ϸ�
	 OP_Elv   44		����������ȣ��						0			������ ACK �Ϸ�	
0x04 �ù�˸�								0		
0x05 ��������								0
0x50 �����庸 ��							0
0x06 ����/�Ͱ�								0				0
0x07 ������ �� ��ȸ											0
0x08 �̼�����								0				0

0x3a �ð�����								0
0x4a ��������								0
0x6a �������������û											0
------------------------------------------------------------------------------------*/
// Big Endian

//---------------------------------------- �����
#define			USER_01
//#define		USER_02
//#define		USER_03
//#define		USER_04
//#define		USER_05
//#define		USER_06
//#define		USER_07

//------------------------------------- ���� ��ư ���� (��ǰ�� �������� LED5,4,3,2,1)
#ifdef	USER_01
	#define		Touch_Odd		LED1	// �ΰ����	(������)
	#define		Touch_Elv		LED2	// Elv
	#define		Touch_Gas		LED3	// ����	
	#define		Touch_Light		LED4	// ����
	#define		Touch_Out		LED5	// ����
	#define		Touch_None		0xff	
		
#endif

#ifdef	USER_02
	#define		Touch_Odd		LED1		
	#define		Touch_Out		LED2
	#define		Touch_Gas		LED3
	#define		Touch_Light		LED4
	#define		Touch_Elv		LED5
#endif


#define			Enargy_Use		0				// ������ ��뿩�� ������ ����ȭ������ �����, ������ ����ȭ����� �Ҹ�������



//------------------------------------ Value
#define		Stat_Gas_Open		0x01			// ��������
#define		Stat_Gas_Close		0x02			// ��������
#define		Stat_Light_Off		0x65			// ���� OFF
#define		Stat_Light_On		0x66			// ���� On
#define		Stat_Out_Goingout	0x11			// ����
#define		Stat_Out_Return		0x12			// ����
#define		Stat_Elv_Call		0x44			// ȣ��
#define		Stat_None			0x00			// ���


//------------------------------------ Packet
#define		Pkt_Size			21
#define		Version_H			0x01			
#define		Version_L			0x05
#define		Preamble_0			0xaa
#define		Preamble_1			0x55
#define		EOT					0x0d
#define		ISD 				0x82			// �Ž�
#define		WALLPAD				0x01			// ������

//   1001 1100		
//	  --    ---- ����ȸ��  00,01,10  
//	   +-------- 00=ack ����,  01=ACKȮ��,  10=ACK ��Ŷ

#define		Pkt_HD				2
#define		Pkt_CC				3
#define		Pkt_PCNT			4
#define		Pkt_ADH				5
#define		Pkt_ADL				6
#define		Pkt_ASH				7
#define		Pkt_ASL				8
#define		Pkt_OP				9
#define		Pkt_DATA			10
#define		Pkt_FCC				18




#define		CC_Ack_Request		0xbc			// ACK �䱸
#define		CC_Ack_None			0x9c			// ACK �ʿ����



//------------------------------------- Timer
#define		Uart_Rx_Timer		User_Timer_0	// 485 Ÿ�׾ƿ�
#define		Tc_Stay_Timer		User_Timer_1	// ��Űġ �����ð� 

#define		Ack_Request			1
#define		Ack_Response		2


//------------------------------------- Default Value
#define		LCD_Sleep_100ms			150			// 150 = 15sec
#define     Wait_Ack_100ms			4
#define		Wait_Ack_Limit			3
#define		Blink_Interval			5			// 500 msec
#define		Light_Blink_Counter		10			// 10ȸ    5��
#define		Gas_Blink_Counter		20			// 20ȸ   10��
#define		Elv_Blink_Counter		240			// 240ȸ 120��


#define	Rx_Buff					Uart.Rx_Buffer
#define	Tx_Buff					Uart.Tx_Buffer
#define	Rx_Len					Uart.Rx_Length


//--------------------------------------- OP code
#define	OP_Weather					0x01
#define	OP_Energy_Use				0x02
#define	OP_Delivery					0x04		// �ù�
#define OP_Finedust					0x08
#define OP_Enargy_List				0x07		
#define	OP_Out						0x06		// 0x11=����,  0x12=����
#define	OP_Park_Detail				0x50		// ��������
#define OP_Time						0x3a
#define OP_Version					0x4a	
#define OP_ServiceList				0x6a
#define	OP_Request_Park				0x05		// ����
//---------------------------------------------- ����̽� ����
#define OP_Control					0x03
#define Device_Gas					0x2c		// control �ڵ峻  D1 ��
#define Device_Induc				0x2d		
#define Device_Light				0x0e		// 0x65=�ϰ��ҵ�,  0x66=�ϰ�����
#define Device_Elv					0x44		// ȣ��


//---------------------------------------------- ������ Ÿ��
#define	Energy_Power				0x01
#define	Energy_Sudo					0x02
#define	Energy_Gas					0x03
#define	Energy_Onsu					0x04
#define	Energy_Warm					0x05


void changeLCDImage(void);
void Led_Display(int v);
void Led_All_Off(void);
void Set_Relay(char flag);
void Check_Touch ();
void Touch_Proc (uint8_t tc, uint16_t stay);
void Uart_Proc ();
uint8_t Get_Checksum(uint8_t *p);
void Parsing_Packet ();
void Check_Ack_Response (uint8_t code);
void Send_Packet (uint8_t cc, uint8_t op, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
void Remake_Pkt ();
void LCD_Display (uint8_t type, uint8_t D0, uint8_t D1, uint8_t D2, uint8_t D3, uint8_t D4, uint8_t D5, uint8_t D6, uint8_t D7);
uint8_t Get_Touch (void);
void Check_Request_Cycle ();
void Init_Start ();
void Display_Light ();
void LCD_Home_View ();
void Display_Date ();
void Display_Info (uint8_t val);
void Display_Out ();
void Display_Weather (uint8_t val);
void Display_Temp (uint8_t val);
void Display_ElvCall ();
void Display_Gas ();
void Touch_Parsing (uint8_t tc, uint16_t stay);
void Display_Msg_Parking (int8_t *p);
void Display_Msg_Dust ();
void Display_Time (uint8_t flag);
void Display_Msg_Elv (int8_t f);
void Sleep_LCD (int8_t flag);
void Response_Elv (uint8_t val, uint8_t *s);
void Display_Energy ();
void Get_Energy_Data (uint8_t *p);
void LCD_Out (int Screen, int x, int y, int image_no, int back_image_no, int xsise, int ysize);
