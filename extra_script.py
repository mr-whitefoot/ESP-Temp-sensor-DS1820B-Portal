Import("env")
env.Replace(PROGNAME="ESP_Sensor_v%s" % env.GetProjectOption("release_version"))