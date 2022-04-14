Import("env")
env.AddCustomTarget(
    "buildandupload", 
    None, 
    'pio run -e %s -t upload' %
        env["PIOENV"], 
    title="Builds and uploads"
)