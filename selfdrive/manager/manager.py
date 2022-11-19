#!/usr/bin/env python3
import datetime
import os
import signal
import subprocess
import sys
import traceback
from typing import List, Tuple, Union

import cereal.messaging as messaging
import selfdrive.sentry as sentry
from common.basedir import BASEDIR
from common.spinner import Spinner
from common.params import Params, ParamKeyType
from common.text_window import TextWindow
from selfdrive.boardd.set_time import set_time
from system.hardware import HARDWARE, PC
from selfdrive.manager.helpers import unblock_stdout
from selfdrive.manager.process import ensure_running
from selfdrive.manager.process_config import managed_processes
from selfdrive.athena.registration import register, UNREGISTERED_DONGLE_ID
from system.swaglog import cloudlog, add_file_handler
from system.version import is_dirty, get_commit, get_version, get_origin, get_short_branch, \
                              terms_version, training_version, is_tested_branch


sys.path.append(os.path.join(BASEDIR, "pyextra"))


def manager_init() -> None:
  # update system time from panda
  set_time(cloudlog)


  params = Params()
  enableLogger = params.get_bool("UploadRaw")
  if enableLogger:
    # save boot log
    subprocess.call("./bootlog", cwd=os.path.join(BASEDIR, "selfdrive/loggerd"))

  params.clear_all(ParamKeyType.CLEAR_ON_MANAGER_START)

  default_params: List[Tuple[str, Union[str, bytes]]] = [
    ("CompletedTrainingVersion", "0"),
    ("DisengageOnAccelerator", "0"),
    ("HasAcceptedTerms", "0"),
    ("OpenpilotEnabledToggle", "1"),

    ("IsOpenpilotViewEnabled", "0"),
    ("OpkrAutoResume", "0"),
    ("OpkrLiveSteerRatio", "0"),
    ("OpkrTurnSteeringDisable", "0"),
    ("OpkrPrebuiltOn", "0"),
    ("OpkrAutoScreenOff", "0"),
    ("OpkrAutoFocus", "0"),
    ("OpkrUIBrightness", "0"),
    ("OpkrUIVolumeBoost", "0"),    
    ("OpkrPandaFirmwareCk", "1"),
    ("OpkrPowerShutdown", "0"),
    ("OpkrRunNaviOnBoot", "0"),
    ("OpkrSSHLegacy", "0"),
    ("OpkratomLongitudinal", "0"), 


    ("OpkrMaxAngleLimit", "90"),
    ("OpkrSteerMethod", "0"),
    ("OpkrMaxSteeringAngle", "85"),
    ("OpkrMaxDriverAngleWait", "0.002"),
    ("OpkrMaxSteerAngleWait", "0.001"),
    ("OpkrDriverAngleWait", "0.001"),
    
    # Tunning
    ("OpkrLateralControlMethod", "3"),

    # 0.PID
    ("PidKp", "0.25"),
    ("PidKi", "0.05"),
    ("PidKf", "0.00005"),

    # 1.INDI

    # 2.LQR
    ("LqrScale", "2000"),
    ("LqrKi", "0.01"),
    ("LqrDcGain","0.0030"),    

    # 3.Torque
    ("TorqueMaxLatAccel", "3"),
    ("TorqueHybridSpeed", "50"),
    ("Torquedeadzone", "0"),     
    ("TorqueKp", "1.0"),
    ("TorqueKf", "1.0"),
    ("TorqueKi", "0.1"),
    ("TorqueFriction","0"),    
    ("TorqueUseAngle", "1"), 
    ("TorqueLiveTuning", "1"), 
    

   # lane
    ("OpkrCameraOffsetAdj", "0"), 
    ("OpkrPathOffsetAdj", "0"), 
    ("OpkrLeftLaneOffset", "0"), 
    ("OpkrRightLaneOffset", "0"), 
    ("OpkrSteerRatio", "16.5"), 
  ]
  if not PC:
    default_params.append(("LastUpdateTime", datetime.datetime.utcnow().isoformat().encode('utf8')))

  if params.get_bool("RecordFrontLock"):
    params.put_bool("RecordFront", True)

  #if not params.get_bool("DisableRadar_Allow"):
  params.remove("DisableRadar")

  # set unset params
  for k, v in default_params:
    try:      
      if params.get(k) is None:
        params.put(k, v)
    except:  # Not on a branch, fallback
      print("default_params:{}  {}".format( k, v) )
      pass
    finally:  # try end 
      pass 
    
  # is this dashcam?
  if os.getenv("PASSIVE") is not None:
    params.put_bool("Passive", bool(int(os.getenv("PASSIVE", "0"))))

  if params.get("Passive") is None:
    raise Exception("Passive must be set to continue")

  # Create folders needed for msgq
  try:
    os.mkdir("/dev/shm")
  except FileExistsError:
    pass
  except PermissionError:
    print("WARNING: failed to make /dev/shm")

  # set version params
  params.put("Version", get_version())
  params.put("TermsVersion", terms_version)
  params.put("TrainingVersion", training_version)
  params.put("GitCommit", get_commit(default=""))
  params.put("GitBranch", get_short_branch(default=""))
  params.put("GitRemote", get_origin(default=""))
  params.put_bool("IsTestedBranch", is_tested_branch())
  
  # set dongle id
  reg_res = register(show_spinner=True)
  if reg_res:
    dongle_id = reg_res
  else:
    serial = params.get("HardwareSerial")
    raise Exception(f"Registration failed for device {serial}")
  os.environ['DONGLE_ID'] = dongle_id  # Needed for swaglog

  if not is_dirty():
    os.environ['CLEAN'] = '1'

  # init logging
  sentry.init(sentry.SentryProject.SELFDRIVE)
  cloudlog.bind_global(dongle_id=dongle_id, version=get_version(), dirty=is_dirty(),
                       device=HARDWARE.get_device_type())


def manager_prepare() -> None:
  for p in managed_processes.values():
    p.prepare()


def manager_cleanup() -> None:
  # send signals to kill all procs
  for p in managed_processes.values():
    p.stop(block=False)

  # ensure all are killed
  for p in managed_processes.values():
    p.stop(block=True)

  cloudlog.info("everything is dead")


def manager_thread() -> None:
  cloudlog.bind(daemon="manager")
  cloudlog.info("manager start")
  cloudlog.info({"environ": os.environ})

  params = Params()

  ignore: List[str] = []
  enableLogger = params.get_bool("UploadRaw")
  if not enableLogger:
    ignore += ["loggerd","logmessaged","deleter","tombstoned","uploader","statsd"]

  if params.get("DongleId", encoding='utf8') in (None, UNREGISTERED_DONGLE_ID):
    ignore += ["manage_athenad", "uploader"]
  if os.getenv("NOBOARD") is not None:
    ignore.append("pandad")
  ignore += [x for x in os.getenv("BLOCK", "").split(",") if len(x) > 0]

  ensure_running(managed_processes.values(), started=False, not_run=ignore)

  started_prev = False
  sm = messaging.SubMaster(['deviceState'])
  pm = messaging.PubMaster(['managerState'])

  while True:
    sm.update()
    not_run = ignore[:]

    started = sm['deviceState'].started
    driverview = params.get_bool("IsDriverViewEnabled")
    ensure_running(managed_processes.values(), started, driverview, not_run)

    # trigger an update after going offroad
    if started_prev and not started and 'updated' in managed_processes:
      os.sync()
      managed_processes['updated'].signal(signal.SIGHUP)

    started_prev = started

    running = ' '.join("%s%s\u001b[0m" % ("\u001b[32m" if p.proc.is_alive() else "\u001b[31m", p.name)
                       for p in managed_processes.values() if p.proc)
    print(running)
    cloudlog.debug(running)

    # send managerState
    msg = messaging.new_message('managerState')
    msg.managerState.processes = [p.get_process_state_msg() for p in managed_processes.values()]
    pm.send('managerState', msg)

    # Exit main loop when uninstall/shutdown/reboot is needed
    shutdown = False
    for param in ("DoUninstall", "DoShutdown", "DoReboot"):
      if params.get_bool(param):
        shutdown = True
        params.put("LastManagerExitReason", param)
        cloudlog.warning(f"Shutting down manager - {param} set")

    if shutdown:
      break

def map_select():
  param_navi_sel = Params().get("OpkrNaviSelect")
  if param_navi_sel  is not None:
    navi_select = int(param_navi_sel)
  else:
    navi_select = 1

 # navi_select = 0
  return  navi_select

def map_exec( map_sel ):
  if map_sel == 1:
    os.system("am start com.mnsoft.mappyobn/com.mnsoft.mappy.MainActivity &") 
  elif map_sel == 2:
    os.system("am start com.thinkware.inaviair/com.thinkware.inaviair.UIActivity &") 


def map_hide():
  os.system("am start --activity-task-on-home com.opkr.maphack/com.opkr.maphack.MainActivity") 



def map_return( map_sel ):
  if map_sel == 1:
    os.system("am start --activity-task-on-home com.mnsoft.mappyobn/com.mnsoft.mappy.MainActivity")
  elif map_sel == 2:
    os.system("am start --activity-task-on-home com.thinkware.inaviair/com.thinkware.inaviair.UIActivity")

def main() -> None:
  spinner = Spinner()
  spinner.update_progress(0, 100)


  map_sel = map_select()



  spinner.update_progress( 50, 100.)
  if map_sel:
    map_exec( map_sel )

  prepare_only = os.getenv("PREPAREONLY") is not None

  manager_init()

  spinner.close()  
  # Start UI early so prepare can happen in the background
  if not prepare_only:
    managed_processes['ui'].start()

  manager_prepare()
  if map_sel:
    map_hide()


  if prepare_only:
    return

  # SystemExit on sigterm
  signal.signal(signal.SIGTERM, lambda signum, frame: sys.exit(1))

  try:
    manager_thread()
  except Exception:
    traceback.print_exc()
    sentry.capture_exception()
  finally:
    manager_cleanup()

  params = Params()
  if params.get_bool("DoUninstall"):
    cloudlog.warning("uninstalling")
    HARDWARE.uninstall()
  elif params.get_bool("DoReboot"):
    cloudlog.warning("reboot")
    HARDWARE.reboot()
  elif params.get_bool("DoShutdown"):
    updateAvailable = params.get_bool("UpdateAvailable");    
    pre_built_on = params.get_bool("OpkrPrebuiltOn")
    if updateAvailable:
       os.system( "cd /data/openpilot; rm -f prebuilt" )
    elif pre_built_on:
      PREBUILT = os.path.exists(os.path.join(BASEDIR, 'prebuilt'))
      cloudlog.warning("check PREBUILT")
      if not PREBUILT:
        os.system( "cd /data/openpilot; touch prebuilt" )

    cloudlog.warning("shutdown")
    HARDWARE.shutdown()


if __name__ == "__main__":
  unblock_stdout()

  try:
    main()
  except Exception:
    add_file_handler(cloudlog)
    cloudlog.exception("Manager failed to start")

    try:
      managed_processes['ui'].stop()
    except Exception:
      pass

    # Show last 3 lines of traceback
    error = traceback.format_exc(-3)
    error = "Manager failed to start\n\n" + error
    with TextWindow(error) as t:
      t.wait_for_exit()

    raise

  # manual exit because we are forked
  sys.exit(0)
