import json
import os


json_file_name = '/data/atom_0761.json'

class kegman_conf():
  def __init__(self, CP=None):
    self.config = None
    self.init = { 
        "sR_KPH": [30,60],  # Speed  kph
        "sR_BPV": [[1.,30],     [1.,10]],  # steer
        "sR_steerRatioV": [[15.5,16.5],[15.5,16.5]],  # speed1[steer], speed2[steer]
        "lat_FACTORV": [[3.1,2.3],[2.7,2.4]],
        "lat_MAX_ACCELV": [[0.11,0.09],[0.1,0.095]],
         }


  def data_check(self, name, value ):
    if name not in self.config:
        self.config.update({name:value})
        self.element_updated = True


  def read_config(self):
    self.element_updated = False

    if os.path.isfile( json_file_name ):
      with open( json_file_name, 'r') as f:
        str_kegman = f.read()
        print( str_kegman )
        self.config = json.loads(str_kegman)

      for name in self.init:
        self.data_check( name, self.init[name] )

      if self.element_updated:
        print("updated")
        self.write_config(self.config)

    else:
      self.config = self.init      
      self.write_config(self.config)

    return self.config

  def write_config(self, config):
    try:
      with open( json_file_name, 'w') as f:
        json.dump(self.config, f, indent=2, sort_keys=True)
        os.chmod( json_file_name, 0o764)
    except IOError:
      os.mkdir('/data')
      with open( json_file_name, 'w') as f:
        json.dump(self.config, f, indent=2, sort_keys=True)
        os.chmod( json_file_name, 0o764)
