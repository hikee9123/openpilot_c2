#pragma once

/*
MAPPY
    signtype
    111 오른쪽 급커브
    112 왼쪽 급커브
    113 굽은도로
    118, 127 어린이보호구역
    122 : 좁아지는 도로
    124 : 과속방지턱
    129 : 주정차
    131 : 단속카메라(신호위반카메라)  
    135 : 고정식(버스단속구간)  - 호야
    150 : 경찰차(이동식단속구간)  - 호야
    165 : 구간단속   
    197 : 구간단속 
    198 차선변경금지시작
    199 차선변경금지종료
    129 주정차금지구간
    123 철길건널목
    200 : 단속구간(고정형 이동식)
    231 : 단속(카메라, 신호위반)    - 어린이.
    246 버스전용차로단속
    247 과적단속
    248 교통정보수집
    249 추월금지구간
    250 갓길단속
    251 적재불량단속
*/


typedef enum TrafficSign {
  TS_CURVE_RIGHT = 111,  // 오른쪽 급커브
  TS_CURVE_LEFT = 112,   // 왼쪽 급커브
  TS_BEND_ROAD = 113,    // 굽은도로
  TS_SCHOOL_ZONE1 = 118,  // 어린이보호구역
  TS_SCHOOL_ZONE2 = 127,  // 어린이보호구역
  TS_NARROW_ROAD = 122,   // 좁아지는 도로
  TS_RAIL_ROAD = 123,     // 철길건널목
  TS_BUMP_ROAD =  124,  // 과속방지턱
  TS_PARK_CRACKDOWN  = 129,  // 주정차단속
  TS_CAMERA1  = 131,  // 단속카메라(신호위반카메라)  
  TS_CAMERA2_BUS  = 135,  // 고정식  - 호야
  TS_CAMERA3  = 150,  // 경찰차(이동식)  - 호야
  TS_INTERVAL  = 165,  // 구간 단속
  TS_VARIABLE  = 195,  // 가변구간
  TS_INTERVAL2  = 197,  // 구간 단속2
  TS_LANE_CHANGE1  = 198,  // 차선변경금지시작
  TS_ANE_CHANGE2  = 199,  // 차선변경금지종료
  TS_CAMERA4  = 200,  // 단속구간(고정형 이동식)
  TS_CAMERA5  = 231,  // 단속(카메라, 신호위반)    
  TS_BUS_ONLY  = 246,  // 버스전용차로단속
  TS_LOAD_OVER  = 247,  // 과적단속
  TS_TRAFFIC_INFO  = 248,  // 교통정보수집
  TS_OVERTRAK  = 249,  // 추월금지구간
  TS_SHOULDER  = 250,  // 갓길단속
  TS_LOAD_POOR  = 251,  // 적재불량단속  
} TrafficSign;



// TMAP
/*
    iNavi Road signtype
    5 이동식
    9 과적단속
    10 합류
    15 고정식    
    18 이벤트 발생
    101 연속 커브
    102 추돌주의
    105 낙석주의
    107 과속방지턱
    111 철길건널목
    203 녹색교통
       

    iNavi Cam signtype
    1 안전속도    
    2 신호및속도단속
    3 신호위반단속
    4, 7 버스전용차로 단속
    5 교통량 측정
    6 이동식단속

    8 주차위반 단속
    11, 12 구간단속
    
    15 박스형카메라
    16 스쿨존
    18 실버존
    20 차선변경금지

    101 연속 커브
    118 야생동몰
    203 녹색교통
    204 미끄럼주의
*/  

typedef enum TrafficSign_TMAP {
  // 1.iNavi Cam signtype
  TC_CAMERA1  = 1,  // 1 안전속도
  TC_CAMERA2  = 2,  // 신호및속도단속
  TC_CAMERA3  = 3,  // 신호위반단속
  TC_BUS_ONLY1  = 4,  // 버스전용차로 단속
  TC_BUS_ONLY2  = 7,  // 버스전용차로 단속
  TC_TRAFFIC_INFO = 5,  // 교통량 측정
  TC_CAMERA6 = 6,  // 이동식단속
  TC_PARK_CRACKDOWN  = 8,  // 주차위반 단속

  TC_INTERVAL1 = 11,
  TC_INTERVAL2 = 12,

  TC_CAMERA7 = 15,  // 박스형카메라

  TC_SCHOOL_ZONE1 = 16,  // 스쿨존
  TC_SCHOOL_ZONE2 = 18,  //실버존

  TC_NO_LANE_CHANGE =   20,  // 차선변경금지

  TC_CURVE_CONT =   101,  // 연속 커브
  TC_WILD_ANIMAL =   118,  // 야생동몰
  TC_GREEN_TRANSP =   203,  // 녹색교통
  TC_BEWARE_SLIPP =   204,  // 미끄럼주의  


  //  2.iNavi Road signtype
  TR_CAMERA6 =   5, // 이동식
  TR_OVER_LOAD =   9, // 과적단속
  TR_JOIN_ZONE =   10, // 합류
  TR_CAMERA4 =   15, // 고정식    
  TR_EVENT =   18, // 이벤트 발생
  TR_CURVE_CONT =   101, // 연속 커브
  TR_COLL_WARN =   102, // 추돌주의
  TR_ROCKFALLISM =   105, // 낙석주의
  TR_SPEED_BUMP =   107, // 과속방지턱
  TR_RAIL_CROSS =   111, // 철길건널목
  TR_GREEN_TRANSP =   203, // 녹색교통
} TrafficSign_TMAP;


typedef enum MapType
{
  MAP_NONE = 0,
  MAP_MAPPY = 1,
  MAP_iNAVI = 2,
} MapTypel;