import donkeycar as dk

from obs import drive

cfg = dk.load_config()
drive(cfg, use_joystick = True)
# drive(cfg, model_path = 'models/pilot1.h5')