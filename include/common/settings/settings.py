import os.path
import shutil

real_file = "include/common/settings/settings.h"
tmpl_file = "include/common/settings/settings.h_template"

if not os.path.isfile(real_file):
    shutil.copy(tmpl_file, real_file)
