from  selfdrive.kegman_conf import kegman_conf

class atom_params():
  def __init__(self, CP=None):
    self.kegman = kegman_conf()

    self.sR_KPHV         = [30, 80]   # Speed  kph
    self.sR_BPV         = [[1.,30],     [1.,10]     ]  # steer
    self.sR_steerRatioV = [[15.5,16.5],[15.5,16.5]]

    self.lat_friction    = [[3.1,2.3],[2.7,2.4]]
    self.lat_latAccelFactor = [[0.11,0.09],[0.1,0.095]]

    self.read_tune()


  def read_tune(self):
    conf = self.kegman.read_config()
    self.sR_KPHV = conf['sR_KPHV']
    self.sR_BPV = conf['sR_BPV']
    self.sR_steerRatioV = conf['sR_steerRatioV']

    self.lat_friction = conf['lat_FACTORV']
    self.lat_latAccelFactor = conf['lat_MAX_ACCELV']
