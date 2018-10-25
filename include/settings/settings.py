import os.path
import shutil

real_file = "include/settings/wifi_access_datas.h"
tmpl_file = "include/settings/wifi_access_datas.h_template"



if not os.path.isfile(real_file):
    shutil.copy(tmpl_file, real_file)
