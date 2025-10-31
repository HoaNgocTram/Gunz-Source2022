#ifndef _ZCOLORTABLE_H
#define _ZCOLORTABLE_H


// 채팅
#define ZCOLOR_CHAT_LOBBY_DEFALT	0xFFE0E0E0			///< 로비에서의 기본색
#define ZCOLOR_CHAT_STAGE_DEFAULT	0xFFE0E0E0			///< 스테이지에서의 기본색

#define ZCOLOR_CHAT_SYSTEM			0xFF00FF00			///< 채팅창의 시스템 말
#define ZCOLOR_CHAT_SYSTEM_GAME		0xFF00FF00			///< 게임 채팅창의 시스템 말
#define ZCOLOR_CHAT_SYSTEM_LOBBY	0xFF00FF00			///< 로비 채팅창의 시스템 말
#define ZCOLOR_CHAT_SYSTEM_STAGE	0xFF00FF00			///< 스테이지 채팅창의 시스템 말

#define ZCOLOR_CHAT_BROADCAST		0xFFff9b25			///< 채팅창의 방송 말
#define ZCOLOR_CHAT_BROADCAST_GAME	0xFFff9b25			///< 게임 채팅창의 방송 말
#define ZCOLOR_CHAT_BROADCAST_LOBBY	0xFFff9b25			///< 로비 채팅창의 방송 말
#define ZCOLOR_CHAT_BROADCAST_STAGE	0xFFff9b25			///< 스테이지 채팅창의 방송 말


#define ZCOLOR_CHAT_WHISPER			0xFF6060A8			///< 채팅창의 귓속말
#define ZCOLOR_CHAT_CLANMSG			0xFF69BAFF			///< 채팅창의 클랜메세지
#define ZCOLOR_CHAT_ROOMCHAT		0xFFF7941D			///< 채팅창의 채팅방 채팅글

#define ZCOLOR_GAME_INFO			0xFF808080			///< 게임안에서의 정보 (레벨업/다운/출입)

#define ZCOLOR_ADMIN_NAME			0xFFFF8040			///< 운영자 이름 색깔

#ifdef _VIPGRADES
#define ZCOLOR_VIP1_NAME		    0xFF00FFF6          //CYAN
#define ZCOLOR_VIP2_NAME		    0xFF0072FF          //BLUE
#define ZCOLOR_VIP3_NAME		    0xFFFFD800          //YELLOW
#define ZCOLOR_VIP4_NAME		    0xFFFF54f9          //ROSA
#define ZCOLOR_VIP5_NAME		    0xFF404040          //GRIS
#define ZCOLOR_VIP6_NAME		    0xFFBD00FC          //MORADADO
#define ZCOLOR_VIP7_NAME		    0xFFFF7300          //Super Donor
#endif

#ifdef _EVENTGRD
#define ZCOLOR_EVENT1_NAME		    0xFFb5006d          //Fiucha
#define ZCOLOR_EVENT2_NAME		    0xFF009db5          //Agua
#define ZCOLOR_EVENT3_NAME		    0xFFb57600          //Kake
#define ZCOLOR_EVENT4_NAME		    0xFF0d0d0d         //Black

#define ZCOLOR_EVENT5_NAME		    0xFF750021          //booster
#define ZCOLOR_EVENT6_NAME		    0xFF915C00         //winner tour

#endif

#define ZCOLOR_CHAT_CLANVICTORY1	0xFF009b25			///< 클랜전 연승 색깔 1
#define ZCOLOR_CHAT_CLANVICTORY2	0xFF669b25			///< 클랜전 연승 색깔 2
#define ZCOLOR_CHAT_CLANVICTORY3	0xFFAA9b25			///< 클랜전 연승 색깔 3
#define ZCOLOR_CHAT_CLANVICTORY4	0xFFFF9b25			///< 클랜전 연승 색깔 4
#define ZCOLOR_CHAT_CLANVICTORY5	0xFFFFFFFF			///< 클랜전 연승 색깔 5


#endif