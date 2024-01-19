#include <sys/time.h>
#include <sys/resource.h>

#include <android/log.h>
#include <log/logger.h>
#include <log/logprint.h>

#include "cereal/messaging/messaging.h"
#include "selfdrive/common/util.h"
#include "selfdrive/common/params.h"

#include "system/logcatd/traffic_sign.h"

typedef struct LiveNaviDataResult {
      float speedLimit;  // Float32;
      float speedLimitDistance;  // Float32;
      float remainTime;  // Float32;
      float roadCurvature;    // Float32;

      int   safetySign1;    // Camera
      int   safetySign2;    // Road

      int   turnInfo;    // Int32;
      int   distanceToTurn;    // Int32;      
      bool  mapValid;    // bool;
      int   mapEnable;    // bool;
      int   mapType;

      double  dArrivalDistance;    // unit:  M
      double  dArrivalTimeSec;    // unit: sec
      double  dEventSec;
      double  dHideTimeSec;

      long  tv_sec;
} LiveNaviDataResult;



// return 값이 1일 경우만 속도 제어.
int traffic_camera( LiveNaviDataResult *pEvet )
{
    int ret_code = 0;

    int mapType = pEvet->mapType;
    int nSignal1 = pEvet->safetySign1;
    int nSignal2 = pEvet->safetySign2;
    float fDistance = pEvet->speedLimitDistance;

    if( mapType == MAP_MAPPY )
    {
      switch( nSignal1 )
      {
        // mappy
        case  TS_CAMERA1:  // 단속(카메라, 신호위반) 
        case  TS_CAMERA2_BUS:
        case  TS_CAMERA3:
        case  TS_CAMERA4:  // 단속구간(고정형 이동식)
        case  TS_CAMERA5:  // 단속(카메라, 신호위반)
        case  TS_TRAFFIC_INFO:  // 교통정보수집
        case  TS_SCHOOL_ZONE1:
        case  TS_SCHOOL_ZONE2:
          ret_code = 1;
          break;

        case  TS_INTERVAL:  // 구간단속
          if(fDistance < 800)
              ret_code = 1;
          break;
      }
    }
    else if( mapType == MAP_iNAVI )
    {
      // iNavi Cam signtype
      switch( nSignal1 )
      {
        case  TC_CAMERA1:  
        case  TC_CAMERA2:
        case  TC_CAMERA3:
        case  TC_CAMERA6:  
        case  TC_CAMERA7:  
        case  TC_TRAFFIC_INFO: 
        case  TC_SCHOOL_ZONE1:
        case  TC_SCHOOL_ZONE2:
          ret_code = 1;
          break;

        case  TC_INTERVAL1:  // 구간단속
        case  TC_INTERVAL2:
          if(fDistance < 800)
              ret_code = 1;
          break;
      }

      //  iNavi Road signtype
      switch( nSignal2 )
      {
        case  TR_CAMERA6:
        case  TR_SPEED_BUMP:
        case  TR_EVENT:
        case  TR_JOIN_ZONE:
          ret_code = 1;
          break;
      }    
    }

    return ret_code;
}

// return sec
float arrival_time( float fDistance, float fSpeed_ms )
{
   float  farrivalTime = 0.0;

   if( fSpeed_ms )
    farrivalTime = fDistance / fSpeed_ms;
  else
    farrivalTime = fDistance;
   return farrivalTime;
}


void update_event(  LiveNaviDataResult *pEvet, float  dSpeed_ms )
{
    float  dEventDistance = pEvet->speedLimitDistance;
    float  dArrivalSec;

    if( dEventDistance > 10 ) {}
    else if(  pEvet->safetySign2 == TS_BUMP_ROAD ) // 과속방지턱
    {
        dEventDistance = 200;
    }

    if( dEventDistance > 10 )
    {
      dArrivalSec = arrival_time( dEventDistance, dSpeed_ms );

      pEvet->dHideTimeSec = pEvet->dEventSec + dArrivalSec;

      pEvet->dArrivalTimeSec =  dArrivalSec;
      pEvet->dArrivalDistance =  dEventDistance;
    }
    else
    {
      pEvet->dHideTimeSec =  pEvet->dEventSec + 5;
    }
}

int main() {
  setpriority(PRIO_PROCESS, 0, -15);
  long     nLastTime = 0;
  int      traffic_type;
  int      opkr =0;
  int      m_message = 0;


  //double  dEventLastSec, dEventHideSec;
  double  dCurrentSec;
  long    nCurPID = 0;
  long    nIdxID = 0;
      
      
  ExitHandler do_exit;
  PubMaster pm({"liveNaviData"});
  SubMaster sm({"carState"});
  LiveNaviDataResult  event;

  log_time last_log_time = {};
  logger_list *logger_list = android_logger_list_alloc(ANDROID_LOG_RDONLY | ANDROID_LOG_NONBLOCK, 0, 0);


  memset( &event, 0, sizeof(event) );

  event.mapType  =  std::atoi(Params().get("OpkrNaviSelect").c_str());   // 1. mappy  2.iNavi


  while (!do_exit) {
    // setup android logging
    if (!logger_list) {
      logger_list = android_logger_list_alloc_time(ANDROID_LOG_RDONLY | ANDROID_LOG_NONBLOCK, last_log_time, 0);
    }
    assert(logger_list);

    struct logger *main_logger = android_logger_open(logger_list, LOG_ID_MAIN);
    assert(main_logger);


    while (!do_exit) {
      //sm.update(0);
      const float dSpeed_ms = 10;// sm["carState"].getCarState().getVEgo();

      struct timeval t;
      gettimeofday(&t, NULL);
      dCurrentSec = t.tv_sec + 1.0e-6*t.tv_usec;
  
      log_msg log_msg;
      int err = android_logger_list_read(logger_list, &log_msg);
      if (err <= 0) break;


      AndroidLogEntry entry;
      err = android_log_processLogBuffer(&log_msg.entry_v1, &entry);
      if (err < 0) continue;
      last_log_time.tv_sec = entry.tv_sec;
      last_log_time.tv_nsec = entry.tv_nsec;

      dCurrentSec = entry.tv_sec + 1.0e-9*entry.tv_nsec;
      long nDelta2;
      nDelta2 = entry.tv_sec - nLastTime;
      if( nDelta2 >= 5 )
      {
        nLastTime = entry.tv_sec;
      }
      
      int  nUpdate = 0
      m_message = atoi( entry.message );
      // 2. MAP data Event.
      traffic_type = traffic_camera( &event );
      if( m_message == 0 ) 
      {
          if( strcmp( entry.tag, "opkrspddist" ) == 0 )  // 1
          {
             event.speedLimitDistance = m_message;
          }
         // update_event( &event, dSpeed_ms );
      }
      else if( strcmp( entry.tag, "opkrspddist" ) == 0 )  // 1
      {
        nCurPID = entry.tid;
        event.speedLimitDistance = m_message;
        if( m_message > 50 )
           opkr = 1;
      }
      else if( strcmp( entry.tag, "opkrspdlimit" ) == 0 ) // 2
      {
        event.speedLimit = m_message;
        opkr = 2;
      }
      else if( strcmp( entry.tag, "opkrcurvangle" ) == 0 )  // 3
      {
        event.roadCurvature = m_message;
        opkr = 3;
      }
      else if( strcmp( entry.tag, "opkrremaintime" ) == 0 )
      {
        event.remainTime = m_message;
        opkr = 4;
      }      
      else if( strcmp( entry.tag, "opkrsigntype" ) == 0 )  // 4.
      {
        nUpdate = 1
        event.safetySign1 = m_message;
        event.dEventSec = dCurrentSec;
        update_event( &event, dSpeed_ms );
        opkr = 5;        
      }
      else if( strcmp( entry.tag, "opkrroadsigntype" ) == 0 )
      {
        nUpdate = 1
        event.safetySign2 = m_message;
        event.dEventSec = dCurrentSec;
        update_event( &event, dSpeed_ms );
        opkr = 6;
       }      
      else if( strcmp( entry.tag, "opkrturninfo" ) == 0 )
      {
        event.turnInfo = m_message;
        opkr = 7;
      } 
      else if( strcmp( entry.tag, "opkrdistancetoturn" ) == 0 )
      {
        event.distanceToTurn = m_message;
        opkr = 8;
      }


      
      /*
      // 3. Message hide process.
      if( opkr )
      {
        if( dSpeed_ms > 2.0 )
        {
          dEventLastSec = dCurrentSec - event.dEventSec;  // 마지막 Event Time
          event.dArrivalTimeSec = event.dHideTimeSec - dCurrentSec;
          event.dArrivalDistance =  event.dArrivalTimeSec * dSpeed_ms;
         
          if( event.mapType == 2 )  // iNavi
          {
              dEventHideSec = 7;
          } 
          else
          {
            if( event.safetySign2 == TS_BUMP_ROAD ) dEventHideSec = 30; // 과속방지턱
            else if( dSpeed_ms < 10 )  dEventHideSec = 20;
            else if( dSpeed_ms < 20 )  dEventHideSec = 10;
            else dEventHideSec = 7;
          }

          if( dEventLastSec > dEventHideSec )   opkr = 0;          
          else if( event.dArrivalTimeSec < 1.5 )  opkr = 0;
        }
        else
        {
          event.dEventSec = dCurrentSec;
          update_event( &event, 2.0 );          
        }       
      }
      else
      {
//        if( event.mapType == 1 )  // mappy
        event.dHideTimeSec = dCurrentSec + 5;
      }
      */

      if ( opkr )
         event.mapValid = 1;
      else
         event.mapValid = 0;   

      MessageBuilder msg;
      auto framed = msg.initEvent().initLiveNaviData();


      nIdxID += nUpdate
      framed.setId( nIdxID );

      framed.setMapType( event.mapType  );
      framed.setTs( entry.tv_sec );
      framed.setSpeedLimit( event.speedLimit );  // Float32;
      framed.setSpeedLimitDistance( event.speedLimitDistance );  // raw_target_speed_map_dist Float32;

      framed.setSafetySign1( event.safetySign1 ); // map_sign Float32;
      framed.setSafetySign2( event.safetySign2 ); 

      framed.setRoadCurvature( event.roadCurvature ); // road_curvature Float32;
      framed.setRemainTime( event.remainTime ); // road_curvature Float32;

      // Turn Info
      framed.setTurnInfo( event.turnInfo );
      framed.setDistanceToTurn( event.distanceToTurn );

      framed.setMapEnable( event.mapEnable );
      framed.setMapValid( event.mapValid );
      framed.setTrafficType( traffic_type );

      framed.setArrivalSec(  event.dArrivalTimeSec );
      framed.setArrivalDistance(  event.dArrivalDistance );


      if( opkr  &&  nCurPID == entry.tid  )
      {
         printf("logcat - tag=%d.[%s] message=[%s] \n",  entry.tid, entry.tag, entry.message );
      }

      pm.send("liveNaviData", msg);
    }

    android_logger_list_free(logger_list);
    logger_list = NULL;
    util::sleep_for(500);
  }

  if (logger_list) {
    android_logger_list_free(logger_list);
  }

  return 0;
}

