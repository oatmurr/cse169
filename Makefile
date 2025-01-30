CC = g++

BREW = $(shell brew --prefix)

# obj directory to keep root clean
OBJDIR = obj

CFLAGS = -g -std=c++11 -Wno-deprecated-declarations
INCFLAGS = -Iinclude -I$(BREW)/include -Iinclude/imgui -Iinclude/backends
LDFLAGS = -framework OpenGL -L$(BREW)/lib -lglfw

RM = /bin/rm -f

$(OBJDIR):
	mkdir -p $(OBJDIR)

.DEFAULT_GOAL := all
all: menv
menv: $(OBJDIR)/main.o $(OBJDIR)/Camera.o $(OBJDIR)/Cube.o $(OBJDIR)/Shader.o $(OBJDIR)/Tokenizer.o $(OBJDIR)/Window.o $(OBJDIR)/DOF.o $(OBJDIR)/Joint.o $(OBJDIR)/Skeleton.o $(OBJDIR)/imgui.o $(OBJDIR)/imgui_demo.o $(OBJDIR)/imgui_draw.o $(OBJDIR)/imgui_tables.o $(OBJDIR)/imgui_widgets.o $(OBJDIR)/imgui_impl_glfw.o $(OBJDIR)/imgui_impl_opengl3.o $(OBJDIR)/Vertex.o $(OBJDIR)/Triangle.o $(OBJDIR)/Skin.o
	$(CC) -o menv $(OBJDIR)/*.o $(LDFLAGS)

# project 1 - skeleton
$(OBJDIR)/main.o: main.cpp include/Window.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c main.cpp -o $(OBJDIR)/main.o

$(OBJDIR)/Camera.o: src/Camera.cpp include/Camera.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c src/Camera.cpp -o $(OBJDIR)/Camera.o

$(OBJDIR)/Cube.o: src/Cube.cpp include/Cube.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c src/Cube.cpp -o $(OBJDIR)/Cube.o

$(OBJDIR)/Shader.o: src/Shader.cpp include/Shader.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c src/Shader.cpp -o $(OBJDIR)/Shader.o

$(OBJDIR)/Tokenizer.o: src/Tokenizer.cpp include/Tokenizer.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c src/Tokenizer.cpp -o $(OBJDIR)/Tokenizer.o

$(OBJDIR)/Window.o: src/Window.cpp include/Window.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c src/Window.cpp -o $(OBJDIR)/Window.o

$(OBJDIR)/DOF.o: src/DOF.cpp include/DOF.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c src/DOF.cpp -o $(OBJDIR)/DOF.o

$(OBJDIR)/Joint.o: src/Joint.cpp include/Joint.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c src/Joint.cpp -o $(OBJDIR)/Joint.o

$(OBJDIR)/Skeleton.o: src/Skeleton.cpp include/Skeleton.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c src/Skeleton.cpp -o $(OBJDIR)/Skeleton.o

# imgui
$(OBJDIR)/imgui.o: include/imgui/imgui.cpp include/imgui/imgui.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c include/imgui/imgui.cpp -o $(OBJDIR)/imgui.o

$(OBJDIR)/imgui_demo.o: include/imgui/imgui_demo.cpp include/imgui/imgui.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c include/imgui/imgui_demo.cpp -o $(OBJDIR)/imgui_demo.o

$(OBJDIR)/imgui_draw.o: include/imgui/imgui_draw.cpp include/imgui/imgui.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c include/imgui/imgui_draw.cpp -o $(OBJDIR)/imgui_draw.o

$(OBJDIR)/imgui_tables.o: include/imgui/imgui_tables.cpp include/imgui/imgui.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c include/imgui/imgui_tables.cpp -o $(OBJDIR)/imgui_tables.o

$(OBJDIR)/imgui_widgets.o: include/imgui/imgui_widgets.cpp include/imgui/imgui.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c include/imgui/imgui_widgets.cpp -o $(OBJDIR)/imgui_widgets.o

$(OBJDIR)/imgui_impl_glfw.o: include/backends/imgui_impl_glfw.cpp include/backends/imgui_impl_glfw.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c include/backends/imgui_impl_glfw.cpp -o $(OBJDIR)/imgui_impl_glfw.o

$(OBJDIR)/imgui_impl_opengl3.o: include/backends/imgui_impl_opengl3.cpp include/backends/imgui_impl_opengl3.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c include/backends/imgui_impl_opengl3.cpp -o $(OBJDIR)/imgui_impl_opengl3.o

# project 2 - skin
$(OBJDIR)/Vertex.o: src/Vertex.cpp include/Vertex.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c src/Vertex.cpp -o $(OBJDIR)/Vertex.o

$(OBJDIR)/Triangle.o: src/Triangle.cpp include/Triangle.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c src/Triangle.cpp -o $(OBJDIR)/Triangle.o

$(OBJDIR)/Skin.o: src/Skin.cpp include/Skin.h | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c src/Skin.cpp -o $(OBJDIR)/Skin.o

clean:
	$(RM) $(OBJDIR)/*.o menv
	rmdir $(OBJDIR)
