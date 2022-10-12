import os
import subprocess
import glob
import hashlib
import shutil
import time

from common.basedir import BASEDIR
from selfdrive.swaglog import cloudlog


# OPKR 

#android_packages = ("com.mixplorer", "com.opkr.maphack",  "com.gmd.hidesoftkeys", "com.google.android.inputmethod.korean", "com.skt.tmap.ku",)
android_packages = ("com.mixplorer","com.gmd.hidesoftkeys", "com.opkr.maphack", "com.mnsoft.mappyobn", "com.thinkware.inaviair","com.phillit.akeyboard","com.goodappsoftware.laserlevel","net.androgames.level","com.moon.android.level" )   # "com.skt.tmap.ku",

def get_installed_apks():
  dat = subprocess.check_output(["pm", "list", "packages", "-f"], encoding='utf8').strip().split("\n")
  ret = {}
  for x in dat:
    if x.startswith("package:"):
      v, k = x.split("package:")[1].split("=")
      ret[k] = v
  return ret

def install_apk(path):
  # can only install from world readable path
  install_path = "/sdcard/%s" % os.path.basename(path)
  shutil.copyfile(path, install_path)

  ret = subprocess.call(["pm", "install", "-r", install_path])
  os.remove(install_path)
  return ret == 0

def appops_set(package, op, mode):
  system(f"LD_LIBRARY_PATH= appops set {package} {op} {mode}")

def pm_grant(package, permission):
  system(f"pm grant {package} {permission}")

def system(cmd):
  try:
    cloudlog.info("running %s" % cmd)
    subprocess.check_output(cmd, stderr=subprocess.STDOUT, shell=True)
  except subprocess.CalledProcessError as e:
    cloudlog.event("running failed",
      cmd=e.cmd,
      output=e.output[-1024:],
      returncode=e.returncode)

# *** external functions ***

def update_apks():
  # install apks
  installed = get_installed_apks()

  install_apks = glob.glob(os.path.join(BASEDIR, "selfdrive/assets/addon/apk/*.apk"))
  for apk in install_apks:
    app = os.path.basename(apk)[:-4]
    if app not in installed:
      installed[app] = None

  cloudlog.info("installed apks %s" % (str(installed), ))

  for app in installed.keys():
    apk_path = os.path.join(BASEDIR, "selfdrive/assets/addon/apk/"+app+".apk")
    if not os.path.exists(apk_path):
      continue

    h1 = hashlib.sha1(open(apk_path, 'rb').read()).hexdigest()
    h2 = None
    if installed[app] is not None:
      h2 = hashlib.sha1(open(installed[app], 'rb').read()).hexdigest()
      cloudlog.info("comparing version of %s  %s vs %s" % (app, h1, h2))

    if h2 is None or h1 != h2:
      cloudlog.info("installing %s" % app)

      success = install_apk(apk_path)
      if not success:
        cloudlog.info("needing to uninstall %s" % app)
        system("pm uninstall %s" % app)
        success = install_apk(apk_path)

      if app == "com.thinkware.inaviair":
        wanted_permissions = ["ACCESS_FINE_LOCATION", "READ_PHONE_STATE", "READ_EXTERNAL_STORAGE", "SYSTEM_ALERT_WINDOW"]
        for permission in wanted_permissions:
          pm_grant("com.thinkware.inaviair", "android.permission."+permission)




      assert success

def pm_apply_packages(cmd):
  for p in android_packages:
    system("pm %s %s" % (cmd, p))

if __name__ == "__main__":
  update_apks()
