env = Environment(CXXFLAGS="-std=c++11 -g -Wall")

env.ParseConfig('pkg-config libpulse-simple --libs --cflags')

env.MergeFlags(['!sdl-config --cflags --libs'])
env.MergeFlags('-lSDL_ttf')
env.MergeFlags('-lGL')

#env.Replace(CC="/home/sarah/pub/llvm-local/bin/clang")
#env.Replace(CXX="/home/sarah/pub/llvm-local/bin/clang++")

objs_getf0 = SConscript("GetF0/SConscript", 'env')
objs_viewer = SConscript("viewer/SConscript", 'env')

SConscript("tests/SConscript", "env")

env.Program('swpitcher', [Glob("*.cpp"), objs_getf0, objs_viewer])

