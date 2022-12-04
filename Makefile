C_FLAGS_LINUX   = -D _DEBUG -ggdb3 -std=c++2a -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wstack-usage=8192 -pie -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
C_FLAGS_WINDOWS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

windows: differentiator_windows logging_windows construct_graphics_windows recursive_descent_windows latex_output_windows simplifying_windows taylor_dec in_and_out_windows
	g++ main.cpp Obj/Differentiator.o Obj/Logging.o Obj/ConstructGraphics.o Obj/LatexOutput.o Obj/RecursiveDescent.o Obj/Simplifying.o Obj/TaylorDecomposition.o Obj/InAndOut.o $(C_FLAGS_WINDOWS) -o Exe/Start.exe

construct_graphics_windows: latex_output_windows in_and_out_windows
	g++ -c Differentiator/Libs/ConstructGraphics/ConstructGraphics.cpp Obj/InAndOut.o Obj/LatexOutput.o $(C_FLAGS_WINDOWS) -o Obj/ConstructGraphics.o

in_and_out_windows: recursive_descent_windows latex_output_windows
	g++ -c Differentiator/Libs/InAndOut/InAndOut.cpp obj/RecursiveDescent.o obj/LatexOutput.o -o Obj/InAndOut.o

recursive_descent_windows:
	g++ -c Differentiator/Libs/ResursiveDescent/RecursiveDescent.cpp Obj/Differentiator.o Obj/Logging.o $(C_FLAGS_WINDOWS) -o Obj/RecursiveDescent.o

taylor_dec: differentiator_windows simplifying_windows latex_output_windows
	g++ -c Differentiator/Libs/TaylorDecomposition/TaylorDecomposition.cpp  $(C_FLAGS_WINDOWS) -o Obj/TaylorDecomposition.o

differentiator_windows: latex_output_windows simplifying_windows
	g++ -c Differentiator/Differentiator.cpp Obj/LatexOutput.o Obj/Simplifying.o $(C_FLAGS_WINDOWS) -o Obj/Differentiator.o

simplifying_windows: latex_output_windows
	g++ -c Differentiator/Libs/Simplifying/Simplifying.cpp Obj/LatexOutput.o $(C_FLAGS_WINDOWS) -o Obj/Simplifying.o

logging_windows:
	g++ -c Differentiator/Libs/Logging/Logging.cpp $(C_FLAGS_WINDOWS) -o Obj/Logging.o

latex_output_windows:
	g++ -c Differentiator/Libs/LatexOutput/LatexOutput.cpp $(C_FLAGS_WINDOWS) -o Obj/LatexOutput.o


linux: differentiator_linux logging_linux construct_graphics_linux recursive_descent_linux latex_output_linux simplifying_linux taylor_dec in_and_out_linux
	g++ main.cpp Obj/Differentiator.o Obj/Logging.o Obj/ConstructGraphics.o Obj/LatexOutput.o Obj/RecursiveDescent.o Obj/Simplifying.o Obj/TaylorDecomposition.o Obj/InAndOut.o $(C_FLAGS_LINUX) -o Exe/Start.exe

construct_graphics_linux: latex_output_linux in_and_out_linux
	g++ -c Differentiator/Libs/ConstructGraphics/ConstructGraphics.cpp Obj/InAndOut.o Obj/LatexOutput.o $(C_FLAGS_LINUX) -o Obj/ConstructGraphics.o

in_and_out_linux: recursive_descent_linux latex_output_linux
	g++ -c Differentiator/Libs/InAndOut/InAndOut.cpp obj/RecursiveDescent.o obj/LatexOutput.o -o Obj/InAndOut.o

recursive_descent_linux:
	g++ -c Differentiator/Libs/ResursiveDescent/RecursiveDescent.cpp Obj/Differentiator.o Obj/Logging.o $(C_FLAGS_LINUX) -o Obj/RecursiveDescent.o

taylor_dec: differentiator_linux simplifying_linux latex_output_linux
	g++ -c Differentiator/Libs/TaylorDecomposition/TaylorDecomposition.cpp  $(C_FLAGS_LINUX) -o Obj/TaylorDecomposition.o

differentiator_linux: latex_output_linux simplifying_linux
	g++ -c Differentiator/Differentiator.cpp Obj/LatexOutput.o Obj/Simplifying.o $(C_FLAGS_LINUX) -o Obj/Differentiator.o

simplifying_linux: latex_output_linux
	g++ -c Differentiator/Libs/Simplifying/Simplifying.cpp Obj/LatexOutput.o $(C_FLAGS_LINUX) -o Obj/Simplifying.o

logging_linux:
	g++ -c Differentiator/Libs/Logging/Logging.cpp $(C_FLAGS_LINUX) -o Obj/Logging.o

latex_output_linux:
	g++ -c Differentiator/Libs/LatexOutput/LatexOutput.cpp $(C_FLAGS_LINUX) -o Obj/LatexOutput.o