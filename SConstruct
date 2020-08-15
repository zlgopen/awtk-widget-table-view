import os
import scripts.app_helper as app

def initArgument(name, defVal):
    val = ARGUMENTS.get(name, '')
    if len(val) == 0:
        ARGUMENTS[name] = defVal

initArgument('LCD', '800_480')
helper = app.Helper(ARGUMENTS);
helper.set_dll_def('src/table_view.def').set_libs(['table_view']).call(DefaultEnvironment)

CustomWidgetSConscriptFiles = []
SConscriptFiles = CustomWidgetSConscriptFiles + ['src/SConscript', 'demos/SConscript', 'tests/SConscript']
SConscript(SConscriptFiles)
