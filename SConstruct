import os
import scripts.awtk_locator as locator

def getAppHelper(ARGUMENTS):
    locator.init(ARGUMENTS)

    from app_helper_base import AppHelperBase
    return AppHelperBase(ARGUMENTS)

helper = getAppHelper(ARGUMENTS)
helper.set_dll_def('src/table_view.def').set_libs(['table_view']).call(DefaultEnvironment)

SConscriptFiles = ['src/SConscript', 'demos/SConscript', 'tests/SConscript']
SConscript(SConscriptFiles)
