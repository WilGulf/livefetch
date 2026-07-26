#ifndef PATHS_H
#define PATHS_H

#ifndef DATA_DIR
#define DATA_DIR "."
#endif

#define DATA_PATH DATA_DIR

const char *get_config_path(void);
const char *get_logo_path(void);

//#define LOGO_PATH DATA_DIR "/share/livefetch/logos"
//#define CONFIG_PATH DATA_DIR "/share/livefetch/default.conf"

#endif