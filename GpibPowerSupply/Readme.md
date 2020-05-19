Instructions to use the module:

- Requires linux-gpib ( https://linux-gpib.sourceforge.io/ )

- to be used in a project, include it as COSTUM_MODULE in the ProjectSettings.cmake, e.g. editing the following line like this:
  set(CUSTOM_SERVER_MODULES GpibPowerSupply)

- link the gpib library in the ProjectSettings.cmake, e.g. editing the following line like this:
  set(SERVER_LINK_LIBRARIES -lgpib )


