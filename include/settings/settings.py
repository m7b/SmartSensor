import os.path
import shutil

real_file = "include/settings/smartsensor_settings.h"
tmpl_file = "include/settings/smartsensor_settings.h_template"

if not os.path.isfile(real_file):
    shutil.copy(tmpl_file, real_file)
