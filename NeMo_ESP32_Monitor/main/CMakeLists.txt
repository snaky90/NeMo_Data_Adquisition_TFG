idf_component_register(SRCS "ota.c" "mqtt.c" "pdet.c" "pcnt.c" "meteo.c" "sntp.c" "wifi.c" "main.c" ""
                    INCLUDE_DIRS "." "include"
                    EMBED_TXTFILES ../certificate/github.cer
                    EMBED_TXTFILES ../certificate/hash)
