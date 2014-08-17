env = Environment(CXXFLAGS="-std=c++11")

objs_getf0 = SConscript("GetF0/SConscript", 'env')

env.Program('swpitcher', ["swpitcher.cpp", objs_getf0])

