import os

def try_get_env(var):
    try:
        return os.environ[var].split()
    except:
        return []

AddOption('--dbg',
          dest='debug',
          action='store_true',
          help='Builds the system for debugging')

AddOption('--build-test',
          dest='build_test',
          action='store_true',
          help='Builds the unit tests')

cpppath = try_get_env('CPPPATH')
cxxflags = try_get_env('CXXFLAGS')
libpath = try_get_env('LD_LIBRARY_PATH')

cxxflags += "-g -std=c++17".split()
if GetOption('debug'):
    cxxflags += ["-O0"]
else:
    cxxflags += "-O3 -DNDEBUG".split()


env = Environment(CXXFLAGS=cxxflags, CPPPATH=cpppath, LIBPATH=libpath)

#We now need to use intercept-build instead of bear (thanks to osx 10.11 security measures)
if 'INTERCEPT_BUILD' in os.environ:
    env["CC"] = os.getenv("CC") or env["CC"]
    env["CXX"] = os.getenv("CXX") or env["CXX"]
    env["ENV"].update(x for x in os.environ.items() if x[0].startswith("CCC_"))
    env["ENV"].update(x for x in os.environ.items() if x[0].startswith("INTERCEPT"))

#Build the c libraries
grain_lib = env.SConscript(dirs=['grain'], exports = 'env')

# Build the unit tests
env.SConscript(['test/SConscript'], exports=['env', 'grain_lib'])

# Build the compositions
env.SConscript(['compositions/SConscript'], exports=['env', 'grain_lib'])
