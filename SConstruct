env = Environment(CXXFLAGS="-std=c++11")

objs_getf0 = SConscript("GetF0/SConscript", 'env')
objs_viewer = SConscript("viewer/SConscript", 'env')

SConscript("tests/SConscript", "env")

env.Program('swpitcher', [Glob("*.cpp"), objs_getf0])

